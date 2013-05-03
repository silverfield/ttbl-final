/*****************************************************************/
/* Includes
/*****************************************************************/

#include <fstream>
#include <iostream>
#include <algorithm>

#include "../central.h"
#include "../algorithms.h"

#include "timedep.h"
#include "undergr.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Cost function - constructors
/*****************************************************************/

CostFunc::CostFunc() {
	cNeedRebuild = false;
	cMaxRange = -1;
	cMinRange = -1;
	cFromDay = -1;
	cToDay = -1;
	cDayPeriod = -1;
	cMinutePeriod = -1;
}

/*****************************************************************/
/* Cost Function - Interface
/*****************************************************************/

void CostFunc::fAddIntPoint(int iDep, int iTime) {
	IntPoint lIntPoint;
	lIntPoint.cDep = iDep;
	lIntPoint.cTravel = iTime;
	cIntPoints.push_back(lIntPoint);
	cNeedRebuild = true;
}

void CostFunc::fRemoveIntPoint(int iIndex) {
	cIntPoints.erase(cIntPoints.begin() + iIndex);
}

void CostFunc::fRebuildSearch(int iFromDay, int iToDay) {
	if (cIntPoints.size() == 0) {
		return;
	}

	cFromDay = iFromDay;
	cToDay = iToDay;
	cDayPeriod = cToDay - cFromDay + 1;
	cMinutePeriod = cDayPeriod * 60 * 24;

	//wrap interpolation points not in the range
	int lFromMinutes = cFromDay * 1440;
	for (size_t i = 0; i < cIntPoints.size(); i++) {
		cIntPoints[i].cDep = (cIntPoints[i].cDep - lFromMinutes) % cMinutePeriod + lFromMinutes;
	}

	//sort the interpolation points
	sort(cIntPoints.begin(), cIntPoints.end(), IntPoint::fCompare);

	//remove slower interpolation points
	vector<IntPoint> lNewIntPoints;
	for (size_t i = 0; i < cIntPoints.size(); i++) {
		int lMinPoint = i;
		i++;
		while (i < cIntPoints.size() && cIntPoints[i].cDep == cIntPoints[lMinPoint].cDep) {
			if (cIntPoints[i].cTravel < cIntPoints[lMinPoint].cTravel) {
				lMinPoint = i;
			}
			i++;
		}

		lNewIntPoints.push_back(cIntPoints[lMinPoint]);
		i--; //cause for loop will move it
	}
	cIntPoints = lNewIntPoints;

	cMaxRange = cIntPoints[cIntPoints.size() - 1].cDep;
	cMinRange = cIntPoints[0].cDep;

	//build the data structure for locating correct interval
	cSearchStruct = vector<vector<int64_t> >();
	int64_t lHeight = fGetHeight();
	int lPointPos = 0;
	for (int64_t i = 0; i < lHeight; i++) {
		vector<int64_t> lIntPoints;
		while (lPointPos < cIntPoints.size() &&
				(cIntPoints[lPointPos].cDep - cMinRange) * lHeight <= (cMaxRange - cMinRange) * (i + 1)) {
			lIntPoints.push_back(lPointPos);
			lPointPos++;
		}

		cSearchStruct.push_back(lIntPoints);
	}

	cNeedRebuild = false;
}

int CostFunc::fGetHeight() {
	return cIntPoints.size();
}

int CostFunc::fGetLength(int iTime) {
	IntPoint lIntPoint = fGetIntPoint(iTime);

	if (lIntPoint.cDep == -1) {
		DEB(LVL_TIMEDEP, "CostFunc::fGetLength: Unable to retrieve an interpolation point" <<
				endl);
		return -1;
	}

	//the total time is the sum of waiting and travel time
	int lWaitTime = lIntPoint.cDep - iTime;
	int lTravelTime = lIntPoint.cTravel;

	return lWaitTime + lTravelTime;
}

string CostFunc::fGetSearchStructString() {
	ostringstream lResult;

	lResult << "Min Range: " << fMinutesToString(cMinRange) << " (" << cMinRange << ")" << endl;
	lResult << "Max Range: " << fMinutesToString(cMaxRange) << " (" << cMaxRange << ")" << endl;
	double lIntervalSize = ((double)cMaxRange - (double)cMinRange + 1) / (double)fGetHeight();
	lResult << "Interval size: " << fMinutesToString(lIntervalSize) << " (" << lIntervalSize << ")" << endl;

	for (size_t i = 0; i < cSearchStruct.size(); i++) {
		int lFromTime = cMinRange + fRound(i * lIntervalSize);
		int lToTime = cMinRange + fRound((i + 1) * lIntervalSize) - 1;
		lResult << "	Int. " << i << " (" << fTimeFormatToString(lFromTime) << " - " <<
				fTimeFormatToString(lToTime) << ")" << ": "
				<< fGetVectorString(cSearchStruct[i]) << endl;

	}

	return lResult.str();
}

string CostFunc::fGetCostString(TimeFormat iTimeFormat) {
	if (cIntPoints.size() == 0) {
		return NULL_STR;
	}

	string lString = "";
	for (size_t i = 0; i < cIntPoints.size(); i++) {
		if (iTimeFormat == TFOn) {
			lString += "(" + fMinutesToString(cIntPoints[i].cDep) + " "
					+ fIntToStr(cIntPoints[i].cTravel) + ")";
		}
		else {
			lString += "(" + fIntToStr(cIntPoints[i].cDep) + " " + fIntToStr(cIntPoints[i].cTravel) + ")";
		}
		lString += " ";
	}

	return lString;
}

int CostFunc::fGetSpace() const {
	int lBytes = 0;

	lBytes += fSizeOf(cIntPoints);
	lBytes += fSizeOf(cSearchStruct);
	for (vector<vector<int64_t> >::const_iterator i = cSearchStruct.begin(); i != cSearchStruct.end(); i++) {
		lBytes += fSizeOf(*i);
	}

	return lBytes;
}

const std::vector<IntPoint>* CostFunc::fGetIntPoints() {
	return &cIntPoints;
}

int CostFunc::fGetFirstDeparture(int iTime) {
	IntPoint lIntPoint = fGetIntPoint(iTime);

	if (lIntPoint.cDep == -1) {
		DEB(LVL_TIMEDEP, "CostFunc::fGetLength: Unable to retrieve an interpolation point" <<
				endl);
		return -1;
	}

	return lIntPoint.cDep;
}

IntPoint* CostFunc::fGetExactIntPointPtr(int iTime) {
	if (iTime > cMaxRange) {
		return NULL;
	}

	iTime = max(iTime, (int)cMinRange);

	//i-th interval has such departures that satisfy:
	//(i/h) * range < dep - minr <= ((i + 1)/h) * range
	//so let number = (dep - minr) * h / range
	//then i < number <= i + 1
	int64_t lIntervalIndex = 0;
	if (cMaxRange != cMinRange) {
		lIntervalIndex = ((int64_t)(iTime - cMinRange) * (int64_t)fGetHeight()) /
				(int64_t)(cMaxRange - cMinRange);
		if (((int64_t)(iTime - cMinRange) * (int64_t)fGetHeight()) %
				(int64_t)(cMaxRange - cMinRange) == 0) {
			lIntervalIndex--;
		}
	}
	lIntervalIndex = max((int)lIntervalIndex, 0);

	//find non-empty interval
	while (lIntervalIndex < cSearchStruct.size() && cSearchStruct[lIntervalIndex].size() == 0) {
		lIntervalIndex++;
	}

	if (lIntervalIndex == cSearchStruct.size()) {
		ERR("CostFunc::fGetExactIntPointPtr: Interval not found" << endl);
		return NULL;
	}

	//find correct interpolation point (first with departure time ahead of iTime)
	int lPointPos = 0;
	while (lPointPos < cSearchStruct[lIntervalIndex].size() &&
		   cSearchStruct[lIntervalIndex][lPointPos] < cIntPoints.size() &&
		   cIntPoints[cSearchStruct[lIntervalIndex][lPointPos]].cDep < iTime) {
		lPointPos++;
		if (lPointPos == cSearchStruct[lIntervalIndex].size()) {
			lPointPos = 0;
			lIntervalIndex++;
			while (lIntervalIndex < cSearchStruct.size() &&
				   cSearchStruct[lIntervalIndex].size() == 0) {
				lIntervalIndex++;
			}

			if (lIntervalIndex == cSearchStruct.size()) {
				ERR("CostFunc::fGetExactIntPointPtr: Interval not found" << endl);
				return NULL;
			}
		}
	}

	if (lPointPos == cSearchStruct[lIntervalIndex].size() ||
		cSearchStruct[lIntervalIndex][lPointPos] == cIntPoints.size()) {
		ERR("CostFunc::fGetExactIntPointPtr: Could not find point in the interval" << endl);
		return NULL;
	}

	return &(cIntPoints[cSearchStruct[lIntervalIndex][lPointPos]]);
}

IntPoint CostFunc::fGetIntPoint(int iTime) {
	if (cNeedRebuild) {
		ERR("CostFunc::fGetIntPoint: Need rebuild" << endl);
		return IntPoint();
	}

	//return fGetExactIntPoint(iTime);

	int lToDecrease = std::max(iTime - (int)cMaxRange, 0);
	int lTime = iTime;
	int lDecMins = 0;
	if (lToDecrease != 0) {
		int lDecTimes = (lToDecrease - 1) / cMinutePeriod + 1;
		lDecMins = lDecTimes * cMinutePeriod;
		lTime = iTime - lDecMins;
	}

	IntPoint lIntPoint;
	if (lTime < cMinRange) {
		lIntPoint = fGetExactIntPoint(cMinRange);
	}
	lIntPoint = fGetExactIntPoint(lTime);

	if (lIntPoint.cDep == -1) {
		return IntPoint();
	}

	lIntPoint.cDep += lDecMins;
	return lIntPoint;
}

IntPoint CostFunc::fGetLatestIntPoint() {
	IntPoint lResult;

	for (size_t i = 0; i < cIntPoints.size(); i++) {
		if (lResult.cDep == -1 || lResult.cDep < cIntPoints[i].cDep) {
			lResult = cIntPoints[i];
		}
	}

	return lResult;
}

IntPoint CostFunc::fGetEarliestIntPoint() {
	IntPoint lResult;

	for (size_t i = 0; i < cIntPoints.size(); i++) {
		if (lResult.cDep == -1 || lResult.cDep > cIntPoints[i].cDep) {
			lResult = cIntPoints[i];
		}
	}

	return lResult;
}

/*****************************************************************/
/* TdGraph - constructors
/*****************************************************************/

TdGraph::TdGraph() {
	cGraph = Graph<TDNodeData, TDArcData>();

	cFromDay = -1;
	cToDay = -1;
	cPeriod = -1;
	cSetPeriod = -1;

	cIdIndexMapValid = false;
}
/*****************************************************************/
/* TdGraph - Interface
/*****************************************************************/

string TdGraph::fGetObjectString(bool iForSave) {
	stringstream lResult (stringstream::out);

	DEB(LVL_TIMEDEP, "TdGraph::fGetObjectString: going to get the graph string" << endl);

	fWriteObject(&lResult, iForSave);

	return lResult.str();
}

Ret TdGraph::fSaveObject(const char *iFileName) {
	ofstream lFile;
	lFile.open(iFileName);

	if (!lFile) {
		ERR("TdGraph::fSaveObject: failed to open file " << iFileName << endl);
		return NOK;
	}

	fWriteObject(&lFile, true);
	lFile.close();

	return OK;
}

Ret TdGraph::fLoadObject(const char *iFileName) {
	ifstream lFile;
	lFile.open(iFileName);

	if (!lFile) {
		ERR("TdGraph::fLoadObject: failed to open file " << iFileName << endl);
		return NOK;
	}

	int lN = 0;
	int lM = 0;

	lFile >> lN;
	lFile >> lM;
	string lLine;
	getline(lFile, lLine);

	//nodes
	unordered_map<string, int> lNameToIndex;
	for (int i = 0; i < lN; i++) {
		//read
		getline(lFile, lLine);
		vector<string> lItems = fSplitString(lLine, " ");

		//check format
		if (lItems.size() < 2) {
			ERR("TdGraph::fLoadObject: Wrong format for arc" << endl);
			return NOK;
		}

		//add node
		DEBF(LVL_TIMEDEP, "TdGraph::fLoadObject: adding node %s\n", lItems[0].c_str());
		int lNodeIndex = fAddNode();

		//add node data
		TDNodeData *lData = fGetNodeData(lNodeIndex);
		lData->cName = lItems[0];
		if (lItems[1] != NULL_STR && lItems[2] != NULL_STR) {
			//we have coordinates
			lData->cCoor = true;
			if (fIsDouble(lItems[1]) == false || fIsDouble(lItems[2]) == false) {
				ERR("TdGraph::fLoadObject: Wrong format for coordinations" << endl);
				return NOK;
			}
			lData->cCoorX = fStrToDouble(lItems[1]);
			lData->cCoorY = fStrToDouble(lItems[2]);
		}

		lNameToIndex.insert(pair<string, int>(lData->cName, lNodeIndex));
	}

	DEB(LVL_TIMEDEP, "TdGraph::fLoadObject: nodes loaded" << endl);
	DEB(LVL_TIMEDEP, "TdGraph::fLoadObject: name-to-index " << fGetMapString(lNameToIndex) << endl);

	//edges
	for (int i = 0; i < lM; i++) {
		//read
		getline(lFile, lLine);
		vector<string> lItems = fSplitString(lLine, " ");

		//check format
		if (lItems.size() < 3) {
			ERR("TdGraph::fLoadObject: Wrong format for arc" << endl);
			return NOK;
		}

		//add arc
		int lFromI = lNameToIndex[lItems[0]];
		int lToI = lNameToIndex[lItems[1]];
		DEBF(LVL_TIMEDEP, "TdGraph::fLoadObject: adding arc %d->%d\n", lFromI, lToI);
		fAddArc(lFromI, lToI);

		//add arc data
		TDArcData *lArcData = fGetArcData(lFromI, lToI);
		if (lItems[2] != NULL_STR) {
			for (int i = 2; i < lItems.size(); i++) {
				lItems[i] = fCharRemove(lItems[i], ')');
				lItems[i] = fCharRemove(lItems[i], '(');
			}
			if (cTimeFormat == TFOn) {
				int i = 0;
				for (i = 2; i < lItems.size(); i += 3) {
					if (lItems[i] == "") {
						continue;
					}
					if (i + 2 >= lItems.size()) {
						ERR("TdGraph::fLoadObject: Wrong format of interpolation points " << endl);
						return NOK;
					}
					string lDep = lItems[i] + " " + lItems[i + 1];
					string lTravel = lItems[i + 2];

					int lDepTime = fStringToMinutesNoCheck(lDep);
					int lTravelTime = fStrToInt(lTravel);
					if (lDepTime < 0 || lTravelTime < 0) {
						ERR("TdGraph::fLoadObject: Wrong format of interpolation points" << endl);
						return NOK;
					}

					lArcData->cCostF.fAddIntPoint(lDepTime, lTravelTime);
				}
			}
			else {
				int i = 0;
				for (i = 2; i < lItems.size(); i += 2) {
					if (lItems[i] == "") {
						continue;
					}
					if (i + 1 >= lItems.size()) {
						ERR("TdGraph::fLoadObject: Wrong format of interpolation points" << endl);
						return NOK;
					}
					string lDep = lItems[i];
					string lTravel = lItems[i + 1];

					int lDepTime = fStrToInt(lDep);
					int lTravelTime = fStrToInt(lTravel);
					if (lDepTime < 0 || lTravelTime < 0) {
						ERR("TdGraph::fLoadObject: Wrong format of interpolation points" << endl);
						return NOK;
					}

					lArcData->cCostF.fAddIntPoint(lDepTime, lTravelTime);

					i += 2;
				}
			}
		}
	}

	fRebuildCostFunctions();

	DEB(LVL_TIMEDEP, "TdGraph::fLoadObject: edges loaded" << endl);

	cIdIndexMapValid = false;

	lFile.close();

	return OK;
}

std::string TdGraph::fGetBasicFactsString() {
	stringstream lResult (stringstream::out);

	lResult << "Nodes: " << fGetN() << endl;
	lResult << "Edges: " << fGetM() << endl;
	lResult << "II map valid: " << gBoolStr[cIdIndexMapValid] << endl;
	lResult << "Size: " << fGetSizeString(sizeof(*this) + fGetSpace()) << endl;

	return lResult.str();
}

int TdGraph::fGetSpace() {
	int lBytes = 0;

	lBytes += GraphObject::fGetSpace();
	lBytes += cGraph.fGetSpace();

	return lBytes;
}

string TdGraph::fGetNodeString(int iIndex) {
	if (cGraph.fNodeIndexExists(iIndex) == false) {
		ERR("TdGraph::fGetNodeString: Node with specified index does not exist" << endl);
		return "";
	}

	stringstream lSs (stringstream::out);
	lSs << iIndex << ": " << cGraph.fGetNodeData(iIndex)->fGetId() << endl;
	lSs << "	Coor: " << cGraph.fGetNodeData(iIndex)->fGetCoorString() << endl;
	lSs << "	Deg: " << cGraph.fGetDeg(iIndex) << endl;
	lSs << "	Neighs: " << fGetArcsString(iIndex) << endl;

	return lSs.str();
}

string TdGraph::fGetArcString(int iFrom, int iTo) {
	if (cGraph.fArcExists(iFrom, iTo) == false) {
		ERR("TdGraph::fGetArcString: Arc does not exist" << endl);
		return "";
	}

	TDArcData *lArcData = cGraph.fGetArcData(iFrom, iTo);

	string lArcString = "";
	lArcString += cGraph.fGetNodeData(iFrom)->fGetId() + " ";
	lArcString += cGraph.fGetNodeData(iTo)->fGetId() + " ";
	lArcString += lArcData->cCostF.fGetCostString(cTimeFormat);

	return lArcString;
}

std::string TdGraph::fGetNodeName(int iX) {
	if (cGraph.fNodeIndexExists(iX) == false) {
		ERR("TdGraph::fGetNodeName: Node with specified index does not exist" << endl);
		return "";
	}

	return fGetNodeData(iX)->cName;
}

string TdGraph::fGetArcsString(int iX) {
	if (cGraph.fNodeIndexExists(iX) == false) {
		ERR("TdGraph::fGetToString: Node with specified index does not exist" << endl);
		return "";
	}

	unordered_map<int, Arc<TDArcData> > lArcs = fGetArcs(iX);
	string lResult = "";
	for (unordered_map<int, Arc<TDArcData> >::iterator i = lArcs.begin(); i != lArcs.end(); i++) {
		if (i != lArcs.begin()) {
			lResult += ", ";
		}
		lResult += fIntToStr(i->first) + " [" + fGetNodeId(i->first) + "]";
	}

	return lResult;
}

bool TdGraph::fCheckPath(Path iPath) {
	for (int i = 0; i < iPath.fGetSize() - 1; i++) {
		if (fArcExists(iPath.fGet(i), iPath.fGet(i + 1)) == false) {
			return false;
		}
	}

	return true;
}

int TdGraph::fGetPathLength(Path iPath) {
	if (iPath.fGetSize() == 0) {
		return -1;
	}
	return iPath.fGetSize() - 1;
}

string TdGraph::fGetPathString(Path iPath, std::string iSep = ", ") {
	string lPathString = "";
	for (int i = 0; i < iPath.fGetSize(); i++) {
		string lNodeString  = fGetNodeId(iPath.fGet(i));

		lPathString += lNodeString;
		if (i != iPath.fGetSize() - 1) {
			lPathString += iSep;
		}
	}

	return lPathString;
}

vector<SPQuery> TdGraph::fGetSPInstances(int iCount) {
	vector<SPQuery> lResult;

	while (lResult.size() != iCount) {
		SPQuery lQuery;
		lQuery.cFrom = rand() % fGetN();
		lQuery.cTo = rand() % fGetN();
		if (lQuery.cFrom == lQuery.cTo) {
			continue;
		}

		lResult.push_back(lQuery);
	}

	return lResult;
}

int TdGraph::fGetLength(int iFrom, int iTo, int iTime = -1) {
	if (fArcExists(iFrom, iTo) == false) {
		ERR("TdGraph::fGetLength: Arc does not exist" << endl);
		return -1;
	}

	if (iTime == -1) {
		return 1;
	}

	return fGetArcData(iFrom, iTo)->cCostF.fGetLength(iTime);
}

GraphObject* TdGraph::fSubGraph(vector<int> iIndices) {
	TdGraph *lSubTdGraph = new TdGraph();

	Graph<TDNodeData, TDArcData> lSubGraph = cGraph.fSubGraph(iIndices);
	lSubTdGraph->fSetGraph(&lSubGraph);

	return lSubTdGraph;
}

void TdGraph::fChangeTimeFormat(TimeFormat iTF) {
	cTimeFormat = iTF;

	fInvalidateIdIndexMap();
}

vector<EAQuery> TdGraph::fGetEAInstances(int iCount, double iExistRatio) {
	vector<EAQuery> lResult;

	if (iExistRatio < 0 || iExistRatio > 1) {
		iExistRatio = 0;
	}

	int lExistCount = 0;
	int lExistNeeded = (double)iCount * iExistRatio;

	int lMinTime = fGetMinTime();
	int lMaxTime = fGetMaxTime();
	int lTimeRange = lMaxTime - lMinTime;
	while (lResult.size() != iCount) {
		EAQuery lQuery;
		int lFrom = rand() % fGetN();
		int lTo = rand() % fGetN();
		lQuery.cFrom = fGetNodeName(lFrom);
		lQuery.cTime = lMinTime + rand() % lTimeRange;
		lQuery.cTo = fGetNodeName(lTo);

		if (lQuery.cFrom == lQuery.cTo) {
			continue;
		}

		if (lExistCount < lExistNeeded) {
			if (fDijkstraConn(this, lFrom, lQuery.cTime, lTo).fIsEmpty()) {
				continue;
			}
			lExistCount++;
		}

		lResult.push_back(lQuery);
	}

	return lResult;
}

vector<string> TdGraph::fGetCities() {
	vector<string> lResult;
	for (int i = 0; i < fGetN(); i++) {
		lResult.push_back(fGetNodeName(i));
	}

	return lResult;
}

int TdGraph::fGetTimeRange() {
	return fGetMaxTime() - fGetMinTime();
}

int TdGraph::fGetMinTime() {
	int lRange = -1;

	for (size_t i = 0; i < fGetN(); i++) {
		unordered_map<int, Arc<TDArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<TDArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			int lMinTime = j->second.cData.cCostF.fGetEarliestIntPoint().cDep;
			if (lMinTime == -1) {
				continue;
			}
			if (lRange == -1) {
				lRange = lMinTime;
				continue;
			}
			lRange = min(lMinTime, lRange);
		}
	}

	return lRange;
}

int TdGraph::fGetMaxTime() {
	int lRange = -1;

	for (size_t i = 0; i < fGetN(); i++) {
		unordered_map<int, Arc<TDArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<TDArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			IntPoint lLatestIntPoint = j->second.cData.cCostF.fGetLatestIntPoint();
			int lMaxTime = lLatestIntPoint.cDep + lLatestIntPoint.cTravel;
			if (lMaxTime == -2) {
				continue;
			}
			if (lRange == -1) {
				lRange = lMaxTime;
				continue;
			}
			lRange = max(lMaxTime, lRange);
		}
	}

	return lRange;
}

double TdGraph::fGetAvgHeight() {
	fGetHeight(false);
}

int TdGraph::fGetHeight() {
	fGetHeight(true);
}

vector<Event> TdGraph::fGetEvents(unordered_set<string> iCities, pair<int, int> iRange) {
	vector<Event> lEvents;

	for (int i = 0; i < cGraph.fGetN(); i++) {
		if (iCities.empty() == false && iCities.find(fGetNodeName(i)) == iCities.end()) {
			continue;
		}

		unordered_map<int, Arc<TDArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<TDArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			const vector<IntPoint> *lIntPoints = j->second.cData.cCostF.fGetIntPoints();
			for (size_t k = 0; k < lIntPoints->size(); k++) {

				int lDep = (*lIntPoints)[k].cDep;
				if (iRange.first <= lDep && lDep <= iRange.second) {
					Event lEventFrom;
					lEventFrom.cCityIndex = i;
					lEventFrom.cCity = fGetNodeName(i);
					lEventFrom.cTime = lDep;
					lEvents.push_back(lEventFrom);
				}

//				if (iCities.find(fGetNodeName(j->first)) != iCities.end()) {
//					int lArr = (*lIntPoints)[k].cDep + (*lIntPoints)[k].cTravel;
//					if ((iRange.first <= lArr && lArr <= iRange.second) ||
//						(iRange.first <= lArr - cPeriod && lArr - cPeriod <= iRange.second)) {
//						Event lEventTo;
//						lEventTo.cCityIndex = j->first;
//						lEventTo.cCity = fGetNodeName(j->first);
//						lEventTo.cTime = lArr;
//						lEvents.push_back(lEventTo);
//					}
//				}
			}
		}
	}

	return lEvents;
}

int TdGraph::fGetEventCount() {
	int lResult = 0;

	for (int i = 0; i < cGraph.fGetN(); i++) {
		unordered_map<int, Arc<TDArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<TDArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			const vector<IntPoint> *lIntPoints = j->second.cData.cCostF.fGetIntPoints();
			lResult += lIntPoints->size() * 2;
		}
	}

	return lResult;
}

/*
 * The resulted graph has the same ID->index index->ID relations
 */
UgGraph* TdGraph::fToUgGraph() {
	UgGraph *lUgGraph = new UgGraph();

	//create vertices
	for (size_t i = 0; i < fGetN(); i++) {
		TDNodeData *lTdData = cGraph.fGetNodeData(i);

		int lNodeIndex = lUgGraph->fAddNode();
		UGNodeData *lNodeData = lUgGraph->fGetNodeData(lNodeIndex);
		lNodeData->cName = lTdData->cName;
		lNodeData->cCoor = false;
	}

	lUgGraph->fComputeIdIndexMap();

	//arcs
	for (size_t i = 0; i < fGetN(); i++) {
		unordered_map<int, Arc<TDArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<TDArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			const vector<IntPoint> *lIntPoints = j->second.cData.cCostF.fGetIntPoints();

			int lFromIndex = i;
			int lToIndex = j->first;

			//add arc
			lUgGraph->fAddArc(lFromIndex, lToIndex);

			//adjust arc for new (minimum) length
			UGArcData *lUgArcData = lUgGraph->fGetArcData(lFromIndex, lToIndex);
			lUgArcData->cLength = -1;
			for (vector<IntPoint>::const_iterator k = lIntPoints->begin(); k != lIntPoints->end(); k++) {
				if (lUgArcData->cLength == -1) {
					lUgArcData->cLength = k->cTravel;
				}
				lUgArcData->cLength = min(lUgArcData->cLength, (int)k->cTravel);
			}
		}
	}

	return lUgGraph;
}

Connection TdGraph::fMakeConnection(Path iPath, int iStartTime) {
	Connection lConn;

	if (iPath.fGetSize() == 0) {
		return lConn;
	}

	int lTime = iStartTime;
	for (int i = 0; i < iPath.fGetSize() - 1; i++) {
		string lCity = fGetNodeName(iPath.fGet(i));
		lConn.fAdd(lCity, lTime);

		TDArcData *lArcData = fGetArcData(iPath.fGet(i), iPath.fGet(i + 1));
		if (lArcData == NULL) {
			ERR("Path goes along non-existent arcs" << endl);
			ERR("Path: " << iPath.fGetString() << endl);
			return Connection();
		}
		int lTravTime = lArcData->cCostF.fGetLength(lTime);
		if (lTravTime == -1) {
			return Connection();
		}
		lTime += lTravTime;
	}
	lConn.fAdd(fGetNodeName(iPath.fGet(iPath.fGetSize() - 1)), lTime);

	return lConn;
}

Connection TdGraph::fMakeConnection(vector<string> iUsp, int iStartTime) {
	Connection lConn;

	if (iUsp.size() == 0) {
		return lConn;
	}

	int lTime = iStartTime;
	for (size_t i = 0; i < iUsp.size() - 1; i++) {
		string lCity = iUsp[i];
		lConn.fAdd(lCity, lTime);

		int lFrom = fGetNodeIndex(lCity);
		int lTo = fGetNodeIndex(iUsp[i + 1]);

		TDArcData *lArcData = fGetArcData(lFrom, lTo);
		int lLength = lArcData->cCostF.fGetLength(lTime);
		if (lLength == -1) {
			return Connection();
		}
		lTime += lLength;
	}
	lConn.fAdd(iUsp[iUsp.size() - 1], lTime);

	return lConn;
}

int TdGraph::fGetReachingTime(string iFrom, int iTime, string iTo) {
	TDNodeData lData;
	lData.cName = iFrom;
	int lNodeIndex = fGetNodeIndex(lData.fGetId());

	unordered_map<int, Arc<TDArcData> > lArcs = cGraph.fGetArcs(lNodeIndex);
	for (unordered_map<int, Arc<TDArcData> >::iterator i = lArcs.begin(); i != lArcs.end(); i++) {
		if (fGetNodeName(i->first) != iTo) {
			continue;
		}

		return iTime + i->second.cData.cCostF.fGetLength(iTime);
	}

	return -1;
}

void TdGraph::fRebuildCostFunctions() {
	int lMin = fGetMinTime();
	cFromDay = fMinutesToTime(lMin).cDays;

	if (cSetPeriod <= 0) {
		int lMax = fGetMaxTime();
		Time lMaxTime = fMinutesToTime(lMax);
		cToDay = lMaxTime.cDays;
		if (cFromDay != cToDay && lMaxTime.cHours < 18) {
			cToDay--;
		}
	}
	else {
		cToDay = cFromDay + cSetPeriod - 1;
	}

	cPeriod = (cToDay - cFromDay + 1) * 1440;

	for (size_t i = 0; i < fGetN(); i++) {
		int lNodeEdgeCount = 0;
		unordered_map<int, Arc<TDArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<TDArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			TDArcData *lArcData = fGetArcData(j->second.cXIndex, j->second.cYIndex);
			lArcData->cCostF.fRebuildSearch(cFromDay, cToDay);
		}
	}
}

void TdGraph::fSetGraph(const Graph<TDNodeData, TDArcData> *iGraph) {
	cGraph = *iGraph;

	cIdIndexMapValid = false;
}

/*****************************************************************/
/* Implementation
/*****************************************************************/

double TdGraph::fGetHeight(bool iMax) {
	unordered_map<string, unordered_set<int> > lHeights;

	for (size_t i = 0; i < fGetN(); i++) {
		string lFromCity = fGetNodeName(i);
		unordered_map<int, Arc<TDArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<TDArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			string lToCity = fGetNodeName(j->first);
			const vector<IntPoint> *lIntPoints = j->second.cData.cCostF.fGetIntPoints();

			for (size_t k = 0; k < lIntPoints->size(); k++) {
				unordered_map<string, unordered_set<int> >::iterator lFromCityIt = lHeights.find(lFromCity);
				if (lFromCityIt == lHeights.end()) {
					lHeights[lFromCity] = unordered_set<int>();
					lHeights[lFromCity].insert((*lIntPoints)[k].cDep);
				}
				else {
					lFromCityIt->second.insert((*lIntPoints)[k].cDep);
				}

				unordered_map<string, unordered_set<int> >::iterator lToCityIt = lHeights.find(lToCity);
				if (lToCityIt == lHeights.end()) {
					lHeights[lToCity] = unordered_set<int>();
					lHeights[lToCity].insert((*lIntPoints)[k].cDep + (*lIntPoints)[k].cTravel);
				}
				else {
					lToCityIt->second.insert((*lIntPoints)[k].cDep + (*lIntPoints)[k].cTravel);
				}
			}
		}
	}

	if (iMax) {
		int lHeight = 0;
		for (unordered_map<string, unordered_set<int> >::iterator i = lHeights.begin(); i != lHeights.end();
				i++) {
			lHeight = max(lHeight, (int)i->second.size());
		}
		return lHeight;
	}

	double lHeight = 0;
	for (unordered_map<string, unordered_set<int> >::iterator i = lHeights.begin(); i != lHeights.end();
			i++) {
		lHeight += i->second.size();
	}
	lHeight /= lHeights.size();

	return lHeight;
}

void TdGraph::fWriteObject(ostream *iStringStream, bool iForSave) {
	if (!iForSave) {
		(*iStringStream) << "N: ";
	}
	(*iStringStream) << fGetN() << endl;
	if (!iForSave) {
		(*iStringStream) << "M: ";
	}
	(*iStringStream) << fGetM() << endl;

	//nodes
	if (!iForSave) {
		(*iStringStream) << "NODES" << endl;
	}
	for (size_t i = 0; i < fGetN(); i++) {
		if (!iForSave) {
			(*iStringStream) << i << ": ";
		}
		(*iStringStream) << cGraph.fGetNodeData(i)->fGetId() << " ";
		(*iStringStream) << cGraph.fGetNodeData(i)->fGetCoorString() << endl;
	}

	//edges
	if (!iForSave) {
		(*iStringStream) << "EDGES" << endl;
	}
	int lEdgeCount = 0;
	for (size_t i = 0; i < fGetN(); i++) {
		int lNodeEdgeCount = 0;
		unordered_map<int, Arc<TDArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<TDArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			lNodeEdgeCount++;
			lEdgeCount++;
			if (!iForSave) {
				(*iStringStream) << lEdgeCount << " (" << lNodeEdgeCount << "/" << i << "): ";
			}

			Arc<TDArcData> *lArc = &(j->second);
			string lFrom = cGraph.fGetNodeData(i)->fGetId();
			string lTo = cGraph.fGetNodeData(j->first)->fGetId();
			(*iStringStream) << lFrom << " " << lTo << " ";

			//cost function
			(*iStringStream) << lArc->cData.cCostF.fGetCostString(cTimeFormat);

			(*iStringStream) << endl;
		}
	}
}
