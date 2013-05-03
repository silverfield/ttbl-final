/*****************************************************************/
/* Includes
/*****************************************************************/

#include <iostream>
#include <fstream>

#include <boost/unordered_map.hpp>

#include "timetable.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Class
/*****************************************************************/

string Timetable::fGetBasicFactsString() {
	stringstream lResult (stringstream::out);

	lResult << "El. Conns.: " << fGetN() << endl;
	lResult << "Size: " << fGetSizeString(sizeof(*this) + fGetSpace()) << endl;

	return lResult.str();
}

int Timetable::fGetSpace() {
	int lBytes = 0;

	lBytes += fSizeOf(cElCons);
	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		lBytes += i->fGetSpace();
	}

	return lBytes;
}

string Timetable::fGetObjectString(bool iForSave) {
	stringstream lResult (stringstream::out);

	DEB(LVL_TIMETABLE, "Timetable::fGetObjectString: going to get the timetable string" << endl);

	fWriteObject(&lResult, iForSave);

	return lResult.str();
}

Ret Timetable::fSaveObject(const char *iFileName) {
	ofstream lFile;
	lFile.open(iFileName);

	if (!lFile) {
		ERR("Timetable::fSaveObject: failed to open file " << iFileName << endl);
		return NOK;
	}

	fWriteObject(&lFile, true);
	lFile.close();

	return OK;
}

Ret Timetable::fLoadObject(const char *iFileName) {
	ifstream lFile;
	lFile.open(iFileName);

	if (!lFile) {
		ERR("Timetable::fLoadObject: failed to open file " << iFileName << endl);
		return NOK;
	}

	int lN = 0;

	lFile >> lN;
	string lLine;
	getline(lFile, lLine);

	//elementary connections
	for (int i = 0; i < lN; i++) {
		//read
		getline(lFile, lLine);
		vector<string> lItems = fSplitString(lLine, ' ');

		//check format
		if ((cTimeFormat == TFOn && lItems.size() != 6) || (cTimeFormat == TFOff && lItems.size() != 4)) {
			ERR("Timetable::fLoadObject: Wrong format for arc" << endl);
			return NOK;
		}

		ElCon lElCon;
		if (cTimeFormat == TFOn) {
			lElCon.cFrom = lItems[0];
			lElCon.cTo = lItems[1];
			lElCon.cDep = fStringToMinutesNoCheck(lItems[2] + " " + lItems[3]);
			lElCon.cArr = fStringToMinutesNoCheck(lItems[4] + " " + lItems[5]);
			if (lElCon.cArr - lElCon.cDep < 0) {
				cout << lElCon.fGetString(TFOn) << endl;
			}
		}
		else {
			lElCon.cFrom = lItems[0];
			lElCon.cTo = lItems[1];
			lElCon.cDep = fStrToIntNoCheck(lItems[2]);
			lElCon.cArr = fStrToIntNoCheck(lItems[3]);
		}

		fAdd(lElCon);
		DEB(LVL_TIMETABLE, "Timetable::fLoadObject: Connection added: " <<
				lElCon.fGetString(cTimeFormat) << endl);
	}

	DEB(LVL_TIMETABLE, "Timetable::fLoadObject: timetable loaded" << endl);

	lFile.close();

	return OK;
}

void Timetable::fChangeTimeFormat(TimeFormat iTF) {
	cTimeFormat = iTF;
}

vector<EAQuery> Timetable::fGetEAInstances(int iCount, double iExistRatio) {
	vector<EAQuery> lResult;

	vector<string> lCities = fGetCities();

	int lTimeRange = fGetTimeRange();
	while (lResult.size() != iCount) {
		EAQuery lQuery;
		int lFrom = rand() % lCities.size();
		int lTo = rand() % lCities.size();
		lQuery.cFrom = lCities[lFrom];
		lQuery.cTime = rand() % lTimeRange;
		lQuery.cTo = lCities[lTo];

		if (lQuery.cFrom == lQuery.cTo) {
			continue;
		}

		lResult.push_back(lQuery);
	}

	return lResult;
}

vector<string> Timetable::fGetCities() {
	unordered_set<string> lCities;
	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		lCities.insert(i->cFrom);
		lCities.insert(i->cTo);
	}

	vector<string> lResult(lCities.begin(), lCities.end());

	return lResult;
}

int Timetable::fGetTimeRange() {
	return fGetMaxTime() - fGetMinTime();
}

int Timetable::fGetMinTime() {
	int lRange = INT_MAX;

	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		lRange = min(i->cArr, lRange);
		lRange = min(i->cDep, lRange);
	}

	return lRange;
}

int Timetable::fGetMaxTime() {
	int lRange = 0;

	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		lRange = max(i->cArr, lRange);
		lRange = max(i->cDep, lRange);
	}

	return lRange;
}

double Timetable::fGetAvgHeight() {
	return fGetHeight(false);
}

int Timetable::fGetHeight() {
	return fGetHeight(true);
}

vector<Event> Timetable::fGetEvents(unordered_set<string> iCities, pair<int, int> iRange) {
	vector<Event> lEvents;

	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		if (iCities.find(i->cFrom) != iCities.end() && i->cDep >= iRange.first &&
				i->cDep <= iRange.second) {
			Event lEventFrom;
			lEventFrom.cCityIndex = -1;
			lEventFrom.cCity = i->cFrom;
			lEventFrom.cTime = i->cDep;
			lEvents.push_back(lEventFrom);
		}

		if (iCities.find(i->cTo) != iCities.end() && i->cArr >= iRange.first &&
				i->cArr <= iRange.second) {
			Event lEventTo;
			lEventTo.cCityIndex = -1;
			lEventTo.cCity = i->cTo;
			lEventTo.cTime = i->cArr;
			lEvents.push_back(lEventTo);
		}
	}

	return lEvents;
}

UgGraph* Timetable::fToUgGraph() {
	UgGraph *lUgGraph = new UgGraph();

	//create vertices
	unordered_set<string> lCreatedIds;
	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		UGNodeData lFromNodeData;
		lFromNodeData.cName = i->cFrom;
		string lFromId = lFromNodeData.fGetId();
		if (lCreatedIds.find(lFromId) == lCreatedIds.end()) {
			int lNodeIndex = lUgGraph->fAddNode();
			UGNodeData *lNodeData = lUgGraph->fGetNodeData(lNodeIndex);
			lNodeData->cName = lFromNodeData.cName;
			lNodeData->cCoor = false;
		}
		lCreatedIds.insert(lFromId);

		UGNodeData lToNodeData;
		lToNodeData.cName = i->cTo;
		string lToId = lToNodeData.fGetId();
		if (lCreatedIds.find(lToId) == lCreatedIds.end()) {
			int lNodeIndex = lUgGraph->fAddNode();
			UGNodeData *lNodeData = lUgGraph->fGetNodeData(lNodeIndex);
			lNodeData->cName = lToNodeData.cName;
			lNodeData->cCoor = false;
		}
		lCreatedIds.insert(lToId);
	}

	lUgGraph->fComputeIdIndexMap();

	//arcs
	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		TDNodeData lFromNodeData;
		lFromNodeData.cName = i->cFrom;
		string lFromId = lFromNodeData.fGetId();
		int lFromIndex = lUgGraph->fGetNodeIndex(lFromId);

		TDNodeData lToNodeData;
		lToNodeData.cName = i->cTo;
		string lToId = lToNodeData.fGetId();
		int lToIndex = lUgGraph->fGetNodeIndex(lToId);

		if (lFromIndex == lToIndex) {
			continue;
		}

		if (lUgGraph->fArcExists(lFromIndex, lToIndex) == false) {
			lUgGraph->fAddArc(lFromIndex, lToIndex);
		}

		//adjust arc for new (minimum) length
		UGArcData *lUgArcData = lUgGraph->fGetArcData(lFromIndex, lToIndex);
		if (lUgArcData->cLength == -1 || lUgArcData->cLength > i->fGetLength()) {
			lUgArcData->cLength = i->fGetLength();
		}
	}

	return lUgGraph;
}

Ret Timetable::fAdd(ElCon iElCon) {
	if (cElCons.find(iElCon) != cElCons.end()) {
		DEB(LVL_ALREADY, "Timetable::fAdd: connection already in the timetable. Skipping entry" << endl);
		return NOK;
	}

	cElCons.insert(iElCon);
	return OK;
}

Ret Timetable::fRemove(ElCon iElCon) {
	if (cElCons.find(iElCon) == cElCons.end()) {
		DEB(LVL_TIMETABLE, "Timetable::fRemove: connection not in the timetable" << endl);
		return NOK;
	}

	cElCons.erase(iElCon);
	return OK;
}

TeGraph* Timetable::fToTeGraph() {
	TeGraph *lTeGraph = new TeGraph();
	lTeGraph->fChangeTimeFormat(cTimeFormat);

	//get cities
	INFO("Getting cities" << endl);
	vector<string> lCities = fGetCities();
	unordered_map<string, int> lCityToIndex;
	for (size_t i = 0; i < lCities.size(); i++) {
		lCityToIndex[lCities[i]] = i;
	}

	//create vertices
	INFO("Creating vertices" << endl);
	vector<unordered_set<int> > lCreatedIds(lCities.size(), unordered_set<int>());
	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		int lFromCityIndex = lCityToIndex[i->cFrom];
		if (lCreatedIds[lFromCityIndex].find(i->cDep) == lCreatedIds[lFromCityIndex].end()) {
			int lNodeIndex = lTeGraph->fAddNode();
			TENodeData *lNodeData = lTeGraph->fGetNodeData(lNodeIndex);
			lNodeData->cName = i->cFrom;
			lNodeData->cTime = i->cDep;
			lCreatedIds[lFromCityIndex].insert(i->cDep);
		}

		int lToCityIndex = lCityToIndex[i->cTo];
		if (lCreatedIds[lToCityIndex].find(i->cArr) == lCreatedIds[lToCityIndex].end()) {
			int lNodeIndex = lTeGraph->fAddNode();
			TENodeData *lNodeData = lTeGraph->fGetNodeData(lNodeIndex);
			lNodeData->cName = i->cTo;
			lNodeData->cTime = i->cArr;
			lCreatedIds[lToCityIndex].insert(i->cArr);
		}
	}

	INFO("Computing ID->Index map" << endl);
	lTeGraph->fComputeIdIndexMap();

	//arcs - connections
	INFO("Creating connection edges" << endl);
	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		TENodeData lFromNodeData;
		lFromNodeData.cName = i->cFrom;
		lFromNodeData.cTime = i->cDep;
		string lFromId = lFromNodeData.fGetId(cTimeFormat);
		int lFromIndex = lTeGraph->fGetNodeIndex(lFromId);

		TENodeData lToNodeData;
		lToNodeData.cName = i->cTo;
		lToNodeData.cTime = i->cArr;
		string lToId = lToNodeData.fGetId(cTimeFormat);
		int lToIndex = lTeGraph->fGetNodeIndex(lToId);

		lTeGraph->fAddArc(lFromIndex, lToIndex);
		TEArcData *lArcData = lTeGraph->fGetArcData(lFromIndex, lToIndex);
		lArcData->cLength = i->fGetLength();
	}

	INFO("Sorting indices" << endl);
	vector<vector<int> > lSortedIds;
	for (size_t i = 0; i < lCreatedIds.size(); i++) {
		lSortedIds.push_back(vector<int>(lCreatedIds[i].begin(), lCreatedIds[i].end()));
	}
	for (size_t i = 0; i < lSortedIds.size(); i++) {
		sort(lSortedIds[i].begin(), lSortedIds[i].end());
	}

	//arcs - waiting edges
	INFO("Creating waiting edges" << endl);
	for (size_t i = 0; i < lSortedIds.size(); i++) {
		for (size_t j = 0; j < lSortedIds[i].size() - 1; j++) {
			TENodeData lFromNodeData;
			lFromNodeData.cName = lCities[i];
			lFromNodeData.cTime = lSortedIds[i][j];
			string lFromId = lFromNodeData.fGetId(cTimeFormat);
			int lFromIndex = lTeGraph->fGetNodeIndex(lFromId);

			TENodeData lToNodeData;
			lToNodeData.cName = lCities[i];
			lToNodeData.cTime = lSortedIds[i][j + 1];
			string lToId = lToNodeData.fGetId(cTimeFormat);
			int lToIndex = lTeGraph->fGetNodeIndex(lToId);

			lTeGraph->fAddArc(lFromIndex, lToIndex);
			TEArcData *lArcData = lTeGraph->fGetArcData(lFromIndex, lToIndex);
			lArcData->cLength = lTeGraph->fGetNodeData(lToIndex)->cTime -
					lTeGraph->fGetNodeData(lFromIndex)->cTime;
		}
	}

	return lTeGraph;
}

TdGraph* Timetable::fToTdGraph() {
	TdGraph *lTdGraph = new TdGraph();
	lTdGraph->fChangeTimeFormat(cTimeFormat);

	//create vertices
	set<string> lCreatedIds;
	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		TDNodeData lFromNodeData;
		lFromNodeData.cName = i->cFrom;
		string lFromId = lFromNodeData.fGetId();
		if (lCreatedIds.find(lFromId) == lCreatedIds.end()) {
			int lNodeIndex = lTdGraph->fAddNode();
			TDNodeData *lNodeData = lTdGraph->fGetNodeData(lNodeIndex);
			lNodeData->cName = lFromNodeData.cName;
			lNodeData->cCoor = false;
		}
		lCreatedIds.insert(lFromId);

		TDNodeData lToNodeData;
		lToNodeData.cName = i->cTo;
		string lToId = lToNodeData.fGetId();
		if (lCreatedIds.find(lToId) == lCreatedIds.end()) {
			int lNodeIndex = lTdGraph->fAddNode();
			TDNodeData *lNodeData = lTdGraph->fGetNodeData(lNodeIndex);
			lNodeData->cName = lToNodeData.cName;
			lNodeData->cCoor = false;
		}
		lCreatedIds.insert(lToId);
	}

	lTdGraph->fComputeIdIndexMap();

	//arcs
	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		TDNodeData lFromNodeData;
		lFromNodeData.cName = i->cFrom;
		string lFromId = lFromNodeData.fGetId();
		int lFromIndex = lTdGraph->fGetNodeIndex(lFromId);

		TDNodeData lToNodeData;
		lToNodeData.cName = i->cTo;
		string lToId = lToNodeData.fGetId();
		int lToIndex = lTdGraph->fGetNodeIndex(lToId);

		if (lTdGraph->fArcExists(lFromIndex, lToIndex) == false) {
			lTdGraph->fAddArc(lFromIndex, lToIndex);
		}
		TDArcData *lArcData = lTdGraph->fGetArcData(lFromIndex, lToIndex);
		if (i->cArr - i->cDep < 0) {
			cout << "PROBLEM" << endl;
		}
		lArcData->cCostF.fAddIntPoint(i->cDep, i->cArr - i->cDep);
	}

	lTdGraph->fRebuildCostFunctions();

	return lTdGraph;
}

Timetable* Timetable::fSubTimetable(UgGraph* iUgGraph, int iMaxHeight, int iMinRange, int iMaxRange) {
	Timetable *lTimetable = new Timetable();

	//create the list of cities
	set<string> lUgCities;
	for (int i = 0; i < iUgGraph->fGetN(); i++) {
		lUgCities.insert(iUgGraph->fGetNodeId(i));
	}

	//copy to vector, so that we can randomize the whole thing
	vector<unordered_set<ElCon, ElConHash, ElConEqual>::iterator> lConnections;
	lConnections.reserve(cElCons.size());
	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		lConnections.push_back(i);
	}

	//take only elementary connections that correspond to the UG and for each city only up to max height
	unordered_map<string, unordered_set<int> > lEvents;
	vector<int> lPermutation = fRandomPermutation(lConnections.size());
	for (vector<ElCon>::size_type i = 0; i < lConnections.size(); i++) {
		//disregard connections out of UG
		if (lUgCities.find(lConnections[lPermutation[i]]->cFrom) == lUgCities.end() ||
			lUgCities.find(lConnections[lPermutation[i]]->cTo) == lUgCities.end()) {
			continue;
		}

		string lFromCity = lConnections[lPermutation[i]]->cFrom;
		string lToCity = lConnections[lPermutation[i]]->cTo;
		int lFromTime = lConnections[lPermutation[i]]->cDep;
		int lToTime = lConnections[lPermutation[i]]->cArr;

		if (lEvents.find(lFromCity) == lEvents.end()) {
			lEvents[lFromCity] = unordered_set<int>();
		}
		if (lEvents.find(lToCity) == lEvents.end()) {
			lEvents[lToCity] = unordered_set<int>();
		}

		if (iMaxHeight != -1 &&
			((lEvents[lFromCity].size() == iMaxHeight && lEvents[lFromCity].count(lFromTime) == 0) ||
			(lEvents[lToCity].size() == iMaxHeight && lEvents[lToCity].count(lToTime) == 0))) {
			continue;
		}

		if (iMinRange != -1 && lFromTime < iMinRange) {
			continue;
		}

		if (iMaxRange != -1 && lToTime > iMaxRange) {
			continue;
		}

		lTimetable->fAdd(*(lConnections[lPermutation[i]]));
		if (iMaxHeight != -1) {
			lEvents[lFromCity].insert(lFromTime);
			lEvents[lToCity].insert(lToTime);
		}
	}

	return lTimetable;
}

struct ElConDepComp {
    inline bool operator()(const ElCon& iLeft, const ElCon& iRight) {
    	if (iLeft.cDep == iRight.cDep) {
    		return iLeft.cArr > iRight.cArr;
    	}
        return iLeft.cDep < iRight.cDep;
    }
};

vector<vector<vector<ElCon> > > Timetable::fGetSortedElCons(vector<string> *iCities) {
	vector<string> lCities;
	vector<string> *lCitiesPtr;
	if (iCities == NULL) {
		lCities = fGetCities();
		lCitiesPtr = &lCities;
	}
	else {
		lCitiesPtr = iCities;
	}

	int c = lCitiesPtr->size();

	unordered_map<string, int> lCityToIndex;
	for (size_t i = 0; i < lCitiesPtr->size(); i++) {
		lCityToIndex[(*lCitiesPtr)[i]] = i;
	}

	vector<vector<vector<ElCon> > > lResult(c, vector<vector<ElCon> >(c, vector<ElCon>()));

	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		lResult[lCityToIndex[i->cFrom]][lCityToIndex[i->cTo]].push_back(*i);
	}

	ElConDepComp lElConComp;

	for (size_t i = 0; i < c; i++) {
		for (size_t j = 0; j < c; j++) {
			sort(lResult[i][j].begin(), lResult[i][j].end(), lElConComp);
		}
	}

	return lResult;
}

/*****************************************************************/
/* Implementation
/*****************************************************************/

double Timetable::fGetHeight(bool iMax) {
	unordered_map<string, unordered_set<int> > lHeights;

	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		unordered_map<string, unordered_set<int> >::iterator lFromCityIt = lHeights.find(i->cFrom);
		if (lFromCityIt == lHeights.end()) {
			lHeights[i->cFrom] = unordered_set<int>();
			lHeights[i->cFrom].insert(i->cDep);
		}
		else {
			lFromCityIt->second.insert(i->cDep);
		}

		unordered_map<string, unordered_set<int> >::iterator lToCityIt = lHeights.find(i->cTo);
		if (lToCityIt == lHeights.end()) {
			lHeights[i->cTo] = unordered_set<int>();
			lHeights[i->cTo].insert(i->cArr);
		}
		else {
			lToCityIt->second.insert(i->cArr);
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

void Timetable::fWriteObject(ostream *iStringStream, bool iForSave) {
	if (!iForSave) {
		(*iStringStream) << "N: ";
	}
	(*iStringStream) << fGetN() << endl;

	//elementary connections
	if (!iForSave) {
		(*iStringStream) << "Elem. conns: " << endl;
	}
	int lElConIndex = -1;
	for (unordered_set<ElCon, ElConHash, ElConEqual>::iterator i = cElCons.begin(); i != cElCons.end(); i++) {
		lElConIndex++;
		if (!iForSave) {
			(*iStringStream) << lElConIndex << ": ";
		}

		(*iStringStream) << i->fGetString(cTimeFormat) << endl;
	}
}
