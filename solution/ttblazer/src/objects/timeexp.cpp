/*****************************************************************/
/* Includes
/*****************************************************************/

#include <fstream>
#include <iostream>
#include <algorithm>

#include "../central.h"

#include "timeexp.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* TeGraph - constructors
/*****************************************************************/

TeGraph::TeGraph() {
	cGraph = Graph<TENodeData, TEArcData>();

	cIdIndexMapValid = false;
}
/*****************************************************************/
/* TeGraph - Interface
/*****************************************************************/

string TeGraph::fGetObjectString(bool iForSave) {
	ostringstream lResult;

	DEB(LVL_TIMEEXP, "TeGraph::fGetObjectString: going to get the graph string" << endl);

	fWriteObject(&lResult, iForSave);

	return lResult.str();
}

Ret TeGraph::fSaveObject(const char *iFileName) {
	ofstream lFile;
	lFile.open(iFileName);

	if (!lFile) {
		ERR("TeGraph::fSaveObject: failed to open file " << iFileName << endl);
		return NOK;
	}

	fWriteObject(&lFile, true);
	lFile.close();

	return OK;
}

Ret TeGraph::fLoadObject(const char *iFileName) {
	ifstream lFile;
	lFile.open(iFileName);

	if (!lFile) {
		ERR("TeGraph::fLoadObject: failed to open file " << iFileName << endl);
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
		if ((cTimeFormat == TFOn && lItems.size() != 3) || (cTimeFormat == TFOff && lItems.size() != 2)) {
			ERR("TeGraph::fLoadObject: Wrong format for arc" << endl);
			return NOK;
		}

		//add node
		int lNodeIndex = fAddNode();

		//add node data
		TENodeData *lData = fGetNodeData(lNodeIndex);
		lData->cName = lItems[0];
		if (cTimeFormat == TFOn) {
			lData->cTime = fStringToMinutesNoCheck(lItems[1] + " " + lItems[2]);

			lNameToIndex.insert(pair<string, int>(lItems[0] + " " + lItems[1] + " " + lItems[2],
					lNodeIndex));
		}
		else {
			lData->cTime = fStrToInt(lItems[1]);

			lNameToIndex.insert(pair<string, int>(lItems[0] + " " + lItems[1], lNodeIndex));
		}
	}

	DEB(LVL_TIMEEXP, "TeGraph::fLoadObject: nodes loaded" << endl);
	DEB(LVL_TIMEEXP, "TeGraph::fLoadObject: name-to-index " << fGetMapString(lNameToIndex) << endl);

	//edges
	for (int i = 0; i < lM; i++) {
		//read
		getline(lFile, lLine);
		vector<string> lItems = fSplitString(lLine, " ");

		//check format
		if ((cTimeFormat == TFOn && lItems.size() != 6) || (cTimeFormat == TFOff && lItems.size() != 4)) {
			ERR("TeGraph::fLoadObject: Wrong format for arc" << endl);
			return NOK;
		}

		//add arc
		unordered_map<string, int>::iterator lFromIt = lNameToIndex.end();
		unordered_map<string, int>::iterator lToIt = lNameToIndex.end();
		string lFromKey = "";
		string lToKey = "";
		if (cTimeFormat == TFOn) {
			lFromKey = lItems[0] + " " + lItems[1] + " " + lItems[2];
			lToKey = lItems[3] + " " + lItems[4] + " " + lItems[5];
			lFromIt = lNameToIndex.find(lFromKey);
			lToIt = lNameToIndex.find(lToKey);
		}
		else {
			lFromKey = lItems[0] + " " + lItems[1];
			lToKey = lItems[2] + " " + lItems[3];
			lFromIt = lNameToIndex.find(lFromKey);
			lToIt = lNameToIndex.find(lToKey);
		}

		if (lFromIt == lNameToIndex.end() || lToIt == lNameToIndex.end()) {
			ERR("TeGraph::fLoadObject: Arc between non-existent vertices - " << lFromKey << ", " <<
					lToKey << endl);
			return NOK;
		}

		int lFromI = lFromIt->second;
		int lToI = lToIt->second;
		DEBF(LVL_TIMEEXP, "TeGraph::fLoadObject: adding arc %d->%d\n", lFromI, lToI);
		fAddArc(lFromI, lToI);

		//add arc data
		TEArcData *lArcData = fGetArcData(lFromI, lToI);
		lArcData->cLength = fGetNodeData(lToI)->cTime - fGetNodeData(lFromI)->cTime;
	}

	DEB(LVL_TIMEEXP, "TeGraph::fLoadObject: edges loaded" << endl);

	cIdIndexMapValid = false;

	lFile.close();

	return OK;
}

std::string TeGraph::fGetBasicFactsString() {
	stringstream lResult (stringstream::out);

	lResult << "Nodes: " << fGetN() << endl;
	lResult << "Edges: " << fGetM() << endl;
	lResult << "II map valid: " << gBoolStr[cIdIndexMapValid] << endl;
	lResult << "Size: " << fGetSizeString(sizeof(*this) + fGetSpace()) << endl;

	return lResult.str();
}

int TeGraph::fGetSpace() {
	int lBytes = 0;

	lBytes += GraphObject::fGetSpace();
	lBytes += cGraph.fGetSpace();

	return lBytes;
}

string TeGraph::fGetNodeString(int iIndex) {
	if (cGraph.fNodeIndexExists(iIndex) == false) {
		ERR("TeGraph::fGetNodeString: Node with specified index does not exist" << endl);
		return "";
	}

	stringstream lSs (stringstream::out);
	lSs << iIndex << ": " << cGraph.fGetNodeData(iIndex)->fGetId(cTimeFormat) << endl;
	lSs << "	Deg: " << cGraph.fGetDeg(iIndex) << endl;
	lSs << "	Neighs: " << fGetArcsString(iIndex) << endl;

	return lSs.str();
}

string TeGraph::fGetArcString(int iFrom, int iTo) {
	if (cGraph.fArcExists(iFrom, iTo) == false) {
		ERR("TeGraph::fGetArcString: Arc does not exist" << endl);
		return "";
	}

	TEArcData *lArcData = cGraph.fGetArcData(iFrom, iTo);

	string lArcString = "";
	lArcString += cGraph.fGetNodeData(iFrom)->fGetId(cTimeFormat) + " ";
	lArcString += cGraph.fGetNodeData(iTo)->fGetId(cTimeFormat) + " ";
	lArcString += lArcData->fGetLengthString();

	return lArcString;
}

string TeGraph::fGetNodeName(int iX) {
	if (cGraph.fNodeIndexExists(iX) == false) {
		ERR("TeGraph::fGetNodeName: Node with specified index does not exist" << endl);
		return "";
	}

	return fGetNodeData(iX)->cName;
}

string TeGraph::fGetArcsString(int iX) {
	if (cGraph.fNodeIndexExists(iX) == false) {
		ERR("TeGraph::fGetToString: Node with specified index does not exist" << endl);
		return "";
	}

	unordered_map<int, Arc<TEArcData> > lArcs = fGetArcs(iX);
	string lResult = "";
	for (unordered_map<int, Arc<TEArcData> >::iterator i = lArcs.begin(); i != lArcs.end(); i++) {
		if (i != lArcs.begin()) {
			lResult += ", ";
		}
		lResult = lResult + fIntToStr(i->first) + " [" + fGetNodeId(i->first) + "]";
	}

	return lResult;
}

int TeGraph::fGetPathLength(Path iPath) {
	if (iPath.fGetSize() == 0) {
		return -1;
	}
	int lLength = 0;
	for (int i = 0; i < iPath.fGetSize() - 1; i++) {
		TEArcData *lArcData = fGetArcData(iPath.fGet(i), iPath.fGet(i + 1));
		if (lArcData->cLength == -1) {
			return iPath.fGetSize() - 1;
		}
		lLength += lArcData->cLength;
	}

	return lLength;
}

string TeGraph::fGetPathString(Path iPath, std::string iSep = ", ") {
	string lPathString = "";
	for (int i = 0; i < iPath.fGetSize(); i++) {
		string lNodeString = fGetNodeId(iPath.fGet(i));

		lPathString += "[" + lNodeString + "]";
		if (i != iPath.fGetSize() - 1) {
			lPathString += iSep;
		}
	}

	return lPathString;
}

vector<SPQuery> TeGraph::fGetSPInstances(int iCount) {
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

int TeGraph::fGetLength(int iFrom, int iTo, int iTime = -1) {
	if (fArcExists(iFrom, iTo) == false) {
		ERR("TeGraph::fGetLength: Arc does not exist" << endl);
		return -1;
	}

	return fGetArcData(iFrom, iTo)->cLength;
}

GraphObject* TeGraph::fSubGraph(vector<int> iIndices) {
	TeGraph *lSubTeGraph = new TeGraph();

	Graph<TENodeData, TEArcData> lSubGraph = cGraph.fSubGraph(iIndices);
	lSubTeGraph->fSetGraph(&lSubGraph);

	return lSubTeGraph;
}

void TeGraph::fChangeTimeFormat(TimeFormat iTF) {
	cTimeFormat = iTF;

	fInvalidateIdIndexMap();
}

vector<EAQuery> TeGraph::fGetEAInstances(int iCount, double iExistRatio) {
	vector<EAQuery> lResult;

	vector<string> lCities = fGetCities();

	while (lResult.size() != iCount) {
		EAQuery lQuery;
		int lFrom = rand() % fGetN();
		int lTo = rand() % lCities.size();
		lQuery.cFrom = fGetNodeName(lFrom);
		lQuery.cTime = fGetNodeTime(lFrom);
		lQuery.cTo = lCities[lTo];

		if (lQuery.cFrom == lQuery.cTo) {
			continue;
		}

		lResult.push_back(lQuery);
	}

	return lResult;
}

vector<string> TeGraph::fGetCities() {
	unordered_set<string> lCities;
	for (int i = 0; i < fGetN(); i++) {
		lCities.insert(fGetNodeName(i));
	}

	vector<string> lResult(lCities.begin(), lCities.end());

	return lResult;
}

int TeGraph::fGetTimeRange() {
	return fGetMaxTime() - fGetMinTime();
}

int TeGraph::fGetMinTime() {
	int lRange = INT_MAX;

	for (size_t i = 0; i < fGetN(); i++) {
		lRange = min(fGetNodeTime(i), lRange);
	}

	return lRange;
}

int TeGraph::fGetMaxTime() {
	int lRange = 0;

	for (size_t i = 0; i < fGetN(); i++) {
		lRange = max(fGetNodeTime(i), lRange);
	}

	return lRange;
}

int TeGraph::fGetHeight() {
	unordered_map<string, int> lHeights;

	for (size_t i = 0; i < fGetN(); i++) {
		string lCity = fGetNodeName(i);
		unordered_map<string, int>::iterator lCityHeight = lHeights.find(lCity);
		if (lCityHeight == lHeights.end()) {
			lHeights[lCity] = 1;
		}
		else {
			lCityHeight->second++;
		}
	}

	int lHeight = 0;
	for (unordered_map<string, int>::iterator i = lHeights.begin(); i != lHeights.end();
			i++) {
		lHeight = max(lHeight, i->second);
	}

	return lHeight;
}

vector<Event> TeGraph::fGetEvents(unordered_set<string> iCities, pair<int, int> iRange) {
	vector<Event> lEvents;

	for (int i = 0; i < cGraph.fGetN(); i++) {
		if ((iCities.size() != 0 && iCities.find(fGetNodeName(i)) == iCities.end()) ||
			(fGetNodeTime(i) < iRange.first ||
			iRange.second < fGetNodeTime(i))) {
			continue;
		}

		Event lEvent;
		lEvent.cCityIndex = i;
		lEvent.cCity = fGetNodeName(i);
		lEvent.cTime = fGetNodeTime(i);
		lEvents.push_back(lEvent);
	}

	return lEvents;
}

UgGraph* TeGraph::fToUgGraph() {
	UgGraph *lUgGraph = new UgGraph();

	//create vertices
	unordered_set<string> lCreatedIds;
	for (size_t i = 0; i < fGetN(); i++) {
		TENodeData *lTeData = cGraph.fGetNodeData(i);

		UGNodeData lNodeData;
		lNodeData.cName = lTeData->cName;
		string lId = lNodeData.fGetId();
		if (lCreatedIds.find(lId) == lCreatedIds.end()) {
			int lNodeIndex = lUgGraph->fAddNode();
			UGNodeData *lNodeData = lUgGraph->fGetNodeData(lNodeIndex);
			lNodeData->cName = lTeData->cName;
			lNodeData->cCoor = false;
		}
		lCreatedIds.insert(lId);
	}

	lUgGraph->fComputeIdIndexMap();

	//arcs
	for (size_t i = 0; i < fGetN(); i++) {
		unordered_map<int, Arc<TEArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<TEArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			//get indices
			TENodeData *lTeFromData = cGraph.fGetNodeData(i);
			string lFromCity = lTeFromData->cName;
			TENodeData *lTeToData = cGraph.fGetNodeData(j->first);
			string lToCity = lTeToData->cName;
			if (lFromCity == lToCity) {
				continue;
			}

			TEArcData *lTeArcData = cGraph.fGetArcData(i, j->first);
			int lFromIndex = lUgGraph->fGetNodeIndex(lFromCity);
			int lToIndex = lUgGraph->fGetNodeIndex(lToCity);

			//add arc
			if (lUgGraph->fArcExists(lFromIndex, lToIndex) == false) {
				lUgGraph->fAddArc(lFromIndex, lToIndex);
			}

			//adjust arc for new (minimum) length
			UGArcData *lUgArcData = lUgGraph->fGetArcData(lFromIndex, lToIndex);
			if (lUgArcData->cLength == -1 || lUgArcData->cLength > lTeArcData->cLength) {
				lUgArcData->cLength = lTeArcData->cLength;
			}
		}
	}

	return lUgGraph;
}

Connection TeGraph::fMakeConnection(Path iPath, int iStartTime) {
	Connection lConn;
	for (int i = 0; i < iPath.fGetSize(); i++) {
		lConn.fAdd(fGetNodeName(iPath.fGet(i)), fGetNodeTime(iPath.fGet(i)));
	}

	return lConn;
}

Connection TeGraph::fMakeConnection(vector<string> iUsp, int iStartTime) {
	Connection lConn;

	if (iUsp.size() == 0) {
		return lConn;
	}

	TENodeData lData;
	lData.cName = iUsp[0];
	lData.cTime = iStartTime;
	int lNodeIndex = fGetNodeIndex(lData.fGetId(cTimeFormat));
	lConn.fAdd(iUsp[0], iStartTime);

	int lTime = iStartTime;
	for (size_t i = 0; i < iUsp.size() - 1; i++) {
		vector<int> lSequence = fGetEarliestSequence(iUsp[i + 1], lNodeIndex);
		if (lSequence.size() == 0) {
			return Connection();
		}
		for (size_t i = 0; i < lSequence.size(); i++) {
			lConn.fAdd(fGetNodeName(lSequence[i]), fGetNodeTime(lSequence[i]));
			lNodeIndex = lSequence[i];
		}
	}

	return lConn;
}

int TeGraph::fGetReachingTime(string iFrom, int iTime, string iTo) {
	TENodeData lData;
	lData.cName = iFrom;
	lData.cTime = iTime;
	int lNodeIndex = fGetNodeIndex(lData.fGetId(cTimeFormat));
	if (lNodeIndex == -1) {
		return -1;
	}

	int lToIndex = fGetEarliestIndex(iTo, lNodeIndex);
	if (lToIndex == -1) {
		return -1;
	}

	return fGetNodeTime(lToIndex);
}

int TeGraph::fGetNodeTime(int iX) {
	if (cGraph.fNodeIndexExists(iX) == false) {
		ERR("TeGraph::fGetNodeTime: Node with specified index does not exist" << endl);
		return -1;
	}

	return fGetNodeData(iX)->cTime;
}

int TeGraph::fGetEarliestRealIndex(string iCity, int iTime) {
	//not implemented yet
	return -1;
}

int TeGraph::fGetWaitTime(string iToCity, int iFromIndex) {
	string lCurrentCity = fGetNodeName(iFromIndex);
	int lCurIndex = iFromIndex;
	int lTime = 0;
	while (lCurIndex != -1) {
		unordered_map<int, Arc<TEArcData> > lArcs = fGetArcs(lCurIndex);
		lCurIndex = -1;
		for (unordered_map<int, Arc<TEArcData> >::iterator i = lArcs.begin(); i != lArcs.end(); i++) {
			//waiting edge
			if (lCurIndex == -1 && fGetNodeName(i->first) == lCurrentCity) {
				lCurIndex = i->first;
				lTime += i->second.cData.cLength;
			}
			//edge to destination city
			if (fGetNodeName(i->first) == iToCity) {
				return lTime;
			}
		}
	}

	return -1;
}

int TeGraph::fGetEarliestIndex(string iToCity, int iFromIndex) {
	string lCurrentCity = fGetNodeName(iFromIndex);
	int lCurIndex = iFromIndex;
	while (lCurIndex != -1) {
		unordered_map<int, Arc<TEArcData> > lArcs = fGetArcs(lCurIndex);
		lCurIndex = -1;
		for (unordered_map<int, Arc<TEArcData> >::iterator i = lArcs.begin(); i != lArcs.end(); i++) {
			//waiting edge
			if (lCurIndex == -1 && fGetNodeName(i->first) == lCurrentCity) {
				lCurIndex = i->first;
			}
			//edge to destination city
			if (fGetNodeName(i->first) == iToCity) {
				return i->first;
			}
		}
	}

	return -1;
}

vector<int> TeGraph::fGetEarliestSequence(string iToCity, int iFromIndex) {
	vector<int> lSequence;
	int lCurIndex = iFromIndex;
	while (lCurIndex != -1) {
		string lCurrentCity = fGetNodeName(lCurIndex);
		unordered_map<int, Arc<TEArcData> > lArcs = fGetArcs(lCurIndex);
		lCurIndex = -1;
		for (unordered_map<int, Arc<TEArcData> >::iterator i = lArcs.begin(); i != lArcs.end(); i++) {
			//waiting edge
			if (lCurIndex == -1 && fGetNodeName(i->first) == lCurrentCity) {
				lCurIndex = i->first;
			}
			//edge to destination city
			else if (fGetNodeName(i->first) == iToCity) {
				lSequence.push_back(i->first);
				return lSequence;
			}
		}

		if (lCurIndex != -1) {
			lSequence.push_back(lCurIndex);
			continue;
		}

		return vector<int>();
	}

	return vector<int>();
}

void TeGraph::fSetGraph(const Graph<TENodeData, TEArcData> *iGraph) {
	cGraph = *iGraph;

	cIdIndexMapValid = false;
}

/*****************************************************************/
/* Implementation
/*****************************************************************/

void TeGraph::fWriteObject(ostream *iStringStream, bool iForSave) {
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
		(*iStringStream) << cGraph.fGetNodeData(i)->fGetId(cTimeFormat) << endl;
	}

	//edges
	if (!iForSave) {
		(*iStringStream) << "EDGES" << endl;
	}
	int lEdgeCount = 0;
	for (size_t i = 0; i < fGetN(); i++) {
		int lNodeEdgeCount = 0;
		unordered_map<int, Arc<TEArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<TEArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			lNodeEdgeCount++;
			lEdgeCount++;
			if (!iForSave) {
				(*iStringStream) << lEdgeCount << " (" << lNodeEdgeCount << "/" << i << "): ";
			}

			Arc<TEArcData> *lArc = &(j->second);
			string lFrom = cGraph.fGetNodeData(i)->fGetId(cTimeFormat);
			string lTo = cGraph.fGetNodeData(j->first)->fGetId(cTimeFormat);
			(*iStringStream) << lFrom << " " << lTo;
			(*iStringStream) << endl;
		}
	}
}
