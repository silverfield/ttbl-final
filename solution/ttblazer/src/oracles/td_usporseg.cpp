/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>
#include <set>

#include "../central.h"
#include "../algdijkstra.h"
#include "../algorithms.h"
#include "../algusp.h"

#include "td_usporseg.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

TdUsporSeg::TdUsporSeg(vector<string> iArguments) {
	cPreprocessed = false;

	cArgs = iArguments;

	cUspInd = NULL;

	fProcessArgs();
}

TdUsporSeg::~TdUsporSeg() {
	delete cUsps;
	delete cUgGraph;
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Ret TdUsporSeg::fPreprocess(Object *iObject) {
	cGraph = dynamic_cast<TdGraph*>(iObject);

	//----- preprocessing -----
	INFO("Starting preprocessing of USP-OR-SEG oracle (for TD graphs)" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//build Id->Index map
	INFO("Building ID->Index map" << endl);
	cGraph->fComputeIdIndexMap();

	INFO("Obtaining underlying graph" << endl);
	cUgGraph = cGraph->fToUgGraph();
	cUgGraph->fComputeIdIndexMap();

	INFO("Getting all cities" << endl);
	vector<int> lAllCities = fIdentityVector(cUgGraph->fGetN());

	INFO("Obtaining underlying shortest paths" << endl);
	cFromDay = cGraph->fGetFromDay();
	cToDay = cGraph->fGetToDay();
	cUsps = fTdSegUSP(cGraph, lAllCities, lAllCities, DAY_MIN, &cUspInd);

	INFO("Analysing USPs" << endl);
	INFO(fAnalyzeSegUsps(cUgGraph, cUsps, cUspInd, lAllCities, lAllCities, false));

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending preprocessing of USP-OR-SEG oracle (for TD graphs)" << endl);

	cPreprocessed = true;

	return OK;
}

int TdUsporSeg::fGetSpace() {
	int lBytes = 0;

	lBytes += fSizeOf(cArgs);
	for (vector<string>::iterator i = cArgs.begin(); i != cArgs.end(); i++) {
		lBytes += i->length();
	}
	lBytes += fSizeOf((*cUsps));
	for (size_t i = 0; i < (*cUsps).size(); i++) {
		lBytes += fSizeOf((*cUsps)[i]);
		for (size_t j = 0; j < (*cUsps)[i].size(); j++) {
			lBytes += fSizeOf((*cUsps)[i][j]);
			for (size_t k = 0; k < (*cUsps)[i][j].size(); k++) {
				lBytes += (*cUsps)[i][j][k].fGetSpace();
			}
		}
	}
	lBytes += fSizeOf((*cUspInd));
	for (size_t i = 0; i < (*cUspInd).size(); i++) {
		lBytes += fSizeOf((*cUspInd)[i]);
		for (size_t j = 0; j < (*cUspInd)[i].size(); j++) {
			lBytes += fSizeOf((*cUspInd)[i][j]);
			for (size_t k = 0; k < (*cUspInd)[i][j].size(); k++) {
				lBytes += fSizeOf((*cUspInd)[i][j][k]);
			}
		}
	}
	lBytes += cUgGraph->fGetSpace();

	lBytes += sizeof(*this);

	return lBytes;
}

Connection TdUsporSeg::fQueryConn(string iFrom, int iTime, string iTo) {
	if (cPreprocessed == false) {
		ERR("TdUsporSeg::fQueryConn: Preprocessing not carried out" << endl);
		return Connection();
	}

	//----- compute -----
	INFO("Starting querying USP-OR-SEG oracle (for TD graphs) for the connections" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Connection lConn;
	int lFromCityIndex = cUgGraph->fGetNodeIndex(iFrom);
	if (lFromCityIndex == -1) {
		ERR("Invalid departure city" << endl);
		return Connection();
	}

	int lToCityIndex = cUgGraph->fGetNodeIndex(iTo);
	if (lToCityIndex == -1) {
		ERR("Invalid arrival city" << endl);
		return Connection();
	}

	//find the best connection
	int lSeg = fTimeToSegment(DAY_MIN, cFromDay, cToDay, iTime);
	lConn = fConnUsps(iTime, (*cUsps)[lFromCityIndex][lToCityIndex],
			(*cUspInd)[lFromCityIndex][lToCityIndex][lSeg], cGraph);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying USP-OR-SEG oracle (for TD graphs) for the connections" << endl);

	return lConn;
}

int TdUsporSeg::fQueryEa(string iFrom, int iTime, string iTo) {
	INFO("Starting querying USP-OR-SEG oracle (for TD graphs) for the earliest arrival" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Connection lConn = fQueryConn(iFrom, iTime, iTo);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying USP-OR-SEG oracle (for TD graphs) for the earliest arrival" << endl);

	return lConn.fGetLength();
}

string TdUsporSeg::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP USP-OR-SEG oracle (for TD graphs)" << endl <<
			"	no help so far" << endl;

	return lSs.str();
}

set<QueryType> TdUsporSeg::fSQueryTypes() {
	set<QueryType> lQTypes;
	lQTypes.insert(QTEa);
	lQTypes.insert(QTConn);

	return lQTypes;
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TdUsporSeg::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TdUsporSeg::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TdUsporSeg::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		UNKNOWN;
	}

	DEB(LVL_CONN, "TdUsporSeg::fProcessArgs: Arguments processed" << endl);
}
