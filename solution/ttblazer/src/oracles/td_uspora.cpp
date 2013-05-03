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
#include "../algaccn.h"
#include "../postbox/postbox.h"

#include "td_uspora.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

TdUspora::TdUspora(vector<string> iArguments) {
	cPreprocessed = false;

	cArgs = iArguments;

	cOptAnPm = "";

	cLastQueryLocal = false;

	fProcessArgs();
}

TdUspora::~TdUspora() {
	delete cUgGraph;
	delete cFAnUsps;
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Ret TdUspora::fPreprocess(Object *iObject) {
	cGraph = dynamic_cast<TdGraph*>(iObject);

	//----- preprocessing -----
	INFO("Starting preprocessing of USP-OR-A oracle (for TD graphs)" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	INFO("Building ID->Index map" << endl);
	cGraph->fComputeIdIndexMap();

	INFO("Obtaining underlying graph" << endl);
	cUgGraph = cGraph->fToUgGraph();
	cUgGraph->fComputeIdIndexMap();

	INFO("Fetching access nodes" << endl);
	vector<string> lAns;
	if (cOptAnPm == "") {
		INFO("No access node set specified, using all cities as ANs" << endl);
		lAns = cUgGraph->fGetNodeNames();
	}
	else {
		//get access nodes from the post mail
		PostMail *lPm = PostBox::fGetMail(cOptAnPm, PB_TYPE_VECT_STRING);
		if (lPm == NULL) {
			ERR("Mail not found!" << endl);
			return NOK;
		}
		lAns = *((vector<string>*)lPm->fGetData());
	}

	INFO("Processing access nodes" << endl);

	//make set of int-s
	for (size_t i = 0; i < lAns.size(); i++) {
		int lIndex = cUgGraph->fGetNodeIndex(lAns[i]);
		if (lIndex == -1) {
			ERR("City " << lAns[i] << " not found in UG!" << endl);
			return NOK;
		}
		cAns.insert(lIndex);
	}

	//make translations city-name -> AN index
	for (size_t i = 0; i < lAns.size(); i++) {
		cCity2AnIndex[cUgGraph->fGetNodeIndex(lAns[i])] = i;
	}

	INFO("Obtaining local access nodes" << endl);
	cFLans = fGetLANs(cUgGraph, cAns, NULL, &cFNeighs);

	INFO("Getting local USP trees" << endl);
	cFLocUspTrees.reserve(cUgGraph->fGetN());
	int lOldPercentage = -1;
	for (int i = 0; i < cUgGraph->fGetN(); i++) {
		int lPercentage = (i * 100) / cUgGraph->fGetN();
		if (lPercentage != lOldPercentage) {
			INFO("Progress " << lPercentage << " % " << endl);
			lOldPercentage = lPercentage;
		}

		vector<int> lTo(cFLans[i].begin(), cFLans[i].end());

		gLogger->fInfoOn(false);
		vector<vector<Path> > *lUsps = fRestrUSP(cGraph, i, lTo, &(cFNeighs[i]), ANY_RANGE);
		gLogger->fInfoOn(true);

		cFLocUspTrees.push_back(UspTree());
		UspTree *lTree = &(cFLocUspTrees[i]);
		for (size_t j = 0; j < (*lUsps).size(); j++) {
			for (size_t k = 0; k < (*lUsps)[j].size(); k++) {
				lTree->fAdd((*lUsps)[j][k]);
			}
		}

		delete lUsps;
	}

	INFO("Obtaining back local access nodes and neighborhoods" << endl);
	cBLans = fGetBackLANs(cUgGraph, cAns, NULL, &cBNeighs);

	INFO("Adjusting FLans and BLans for access nodes" << endl);
	for (unordered_set<int>::iterator i = cAns.begin(); i != cAns.end(); i++) {
		cFLans[*i] = unordered_set<int>();
		cFLans[*i].insert(*i);
		cBLans[*i] = unordered_set<int>();
		cBLans[*i].insert(*i);
	}

	INFO("Obtaining USPs among ANs" << endl);
	vector<int> lAnVec;
	for (size_t i = 0; i < lAns.size(); i++) {
		lAnVec.push_back(cUgGraph->fGetNodeIndex(lAns[i]));
	}
	cFAnUsps = fTdUSP(cGraph, lAnVec, lAnVec);

	INFO("Analysing USPs among ANs" << endl);
	INFO(fAnalyzeUsps(cUgGraph, cFAnUsps, lAnVec, lAnVec, false));

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending preprocessing of USP-OR-A oracle (for TD graphs)" << endl);

	cPreprocessed = true;

	return OK;
}

int TdUspora::fGetSpace() {
	int lBytes = 0;

	lBytes += fSizeOf(cArgs);
	for (vector<string>::iterator i = cArgs.begin(); i != cArgs.end(); i++) {
		lBytes += i->length();
	}
	lBytes += cUgGraph->fGetSpace();

	lBytes += fSizeOf(cAns);
	lBytes += fSizeOf(cCity2AnIndex);

	lBytes += fSizeOf(cFLans);
	for (size_t i = 0; i < cFLans.size(); i++) {
		lBytes += fSizeOf(cFLans[i]);
	}
	lBytes += fSizeOf(cBLans);
	for (size_t i = 0; i < cBLans.size(); i++) {
		lBytes += fSizeOf(cBLans[i]);
	}
	lBytes += fSizeOf(cFLocUspTrees);
	for (size_t i = 0; i < cFLocUspTrees.size(); i++) {
		lBytes += cFLocUspTrees[i].fGetSpace();
	}
	lBytes += fSizeOf(cFNeighs);
	for (size_t i = 0; i < cFNeighs.size(); i++) {
		lBytes += fSizeOf(cFNeighs[i]);
	}
	lBytes += fSizeOf(cBNeighs);
	for (size_t i = 0; i < cBNeighs.size(); i++) {
		lBytes += fSizeOf(cBNeighs[i]);
	}
	lBytes += fSizeOf((*cFAnUsps));
	for (size_t i = 0; i < (*cFAnUsps).size(); i++) {
		lBytes += fSizeOf((*cFAnUsps)[i]);
		for (size_t j = 0; j < (*cFAnUsps)[i].size(); j++) {
			lBytes += fSizeOf((*cFAnUsps)[i][j]);
			for (size_t k = 0; k < (*cFAnUsps)[i][j].size(); k++) {
				lBytes += (*cFAnUsps)[i][j][k].fGetSpace();
			}
		}
	}

	lBytes += sizeof(*this);

	return lBytes;
}

Connection TdUspora::fAnUspConn(int iFrom, int iTime, int iTo) {
	if (iFrom == iTo) {
		Connection lConn;
		lConn.fAdd(cGraph->fGetNodeName(iFrom), iTime);
		return lConn;
	}

	int lAnFrom = cCity2AnIndex[iFrom];
	int lAnTo = cCity2AnIndex[iTo];

	return fConnUsps(iTime, (*cFAnUsps)[lAnFrom][lAnTo], cGraph);
}

#define TERMINATE if (lRet == OK) {gAlgTimer->fStopTimer();} \
		if (lLocalConn.fIsEmpty()) { \
			INFO("Connection not found" << endl); \
			return Connection(); \
		} \
		else { \
			INFO("Ending querying USP-OR-A oracle (for TD graphs) for the connections" << endl); \
			return lLocalConn; \
		}

Connection TdUspora::fQueryConn(string iFrom, int iTime, string iTo) {
	if (cPreprocessed == false) {
		ERR("TdUspora::fQueryConn: Preprocessing not carried out" << endl);
		return Connection();
	}

	//----- compute -----
	INFO("Starting querying USP-OR-A oracle (for TD graphs) for a connections" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//get indices
	int lFromCityIndex = cUgGraph->fGetNodeIndex(iFrom);
	if (lFromCityIndex == -1) {
		ERR("Invalid departure city" << endl);
		if (lRet == OK) {gAlgTimer->fStopTimer();}
		return Connection();
	}

	int lToCityIndex = cUgGraph->fGetNodeIndex(iTo);
	if (lToCityIndex == -1) {
		ERR("Invalid arrival city" << endl);
		if (lRet == OK) {gAlgTimer->fStopTimer();}
		return Connection();
	}

//	INFO("Front LANS:" << endl);
//	for (unordered_set<int>::iterator i = cFLans[lFromCityIndex].begin(); i != cFLans[lFromCityIndex].end();
//			i++) {
//		INFO("	" << cGraph->fGetNodeName(*i) << endl);
//	}
//	INFO("Back LANS:" << endl);
//	for (unordered_set<int>::iterator i = cBLans[lToCityIndex].begin(); i != cBLans[lToCityIndex].end();
//			i++) {
//		INFO("	" << cGraph->fGetNodeName(*i) << endl);
//	}

	//check if the destination is within the departure's neighborhood
	Connection lLocalConn;
	if (cFNeighs[lFromCityIndex].count(lToCityIndex) != 0) {
		cLastQueryLocal = true;
		lLocalConn = fDijkWrapConnRestr(cGraph, lFromCityIndex, iTime, lToCityIndex,
				cFNeighs[lFromCityIndex]);
		INFO(lLocalConn.fGetString(", ") << endl);
	}
	else {
		cLastQueryLocal = false;
	}

	bool lFromIsAN = (cAns.count(lFromCityIndex) != 0);
	bool lToIsAN = (cAns.count(lToCityIndex) != 0);

	//----- Local front search -----

	if (cFLans[lFromCityIndex].size() == 0 || cFLocUspTrees[lFromCityIndex].fIsEmpty()) {
		TERMINATE;
	}

	unordered_map<int, Connection> lFLanConns = fConnUspTreeToCities(lFromCityIndex, iTime,
			cFLans[lFromCityIndex], cFLocUspTrees[lFromCityIndex], cGraph);

	//check if we can improve local
	if (lLocalConn.fIsEmpty() == false) {
		bool lUseLocal = true;
		for (unordered_set<int>::iterator i = cFLans[lFromCityIndex].begin();
				i != cFLans[lFromCityIndex].end(); i++) {
			int lArrival = lFLanConns[*i].fGetLastTime();
			if (lArrival != -1 && lArrival < lLocalConn.fGetLastTime()) {
				lUseLocal = false;
				break;
			}
		}
		if (lUseLocal) {
			TERMINATE;
		}
	}

	//----- Inter-AN search -----

	bool lFoundConn = false;
	unordered_map<int, Connection> lBLanConns;
	for (unordered_set<int>::iterator i = cBLans[lToCityIndex].begin();
			i != cBLans[lToCityIndex].end(); i++) {
		lBLanConns[*i] = Connection();
		Connection *lBLanConn = &(lBLanConns[*i]);

		for (unordered_set<int>::iterator j = cFLans[lFromCityIndex].begin();
				j != cFLans[lFromCityIndex].end(); j++) {
			if (lFLanConns[*j].fGetSize() == 0) {
				continue;
			}

			//get connection from given AN to given AN
			int lFLanTime = lFLanConns[*j].fGetLastTime();
			Connection lUspConn = fAnUspConn(*j, lFLanTime, *i);

			//update minimum
			if (lUspConn.fGetSize() == 0) {
				continue;
			}
			if ((lBLanConn->fIsEmpty() && !lUspConn.fIsEmpty()) ||
				lBLanConn->fGetLastTime() > lUspConn.fGetLastTime()) {
				lFoundConn = true;
				*lBLanConn = lUspConn;
			}
		}
	}

	if (lFoundConn == false) {
		TERMINATE;
	}

	//check if we can improve local
	if (lLocalConn.fIsEmpty() == false) {
		bool lUseLocal = true;
		for (unordered_set<int>::iterator i = cBLans[lToCityIndex].begin();
				i != cBLans[lToCityIndex].end(); i++) {
			int lArrival = lBLanConns[*i].fGetLastTime();
			if (lArrival != -1 && lArrival < lLocalConn.fGetLastTime()) {
				lUseLocal = false;
				break;
			}
		}
		if (lUseLocal) {
			TERMINATE;
		}
	}

	//----- Local back search -----

	//do restricted Dijkstra from each back access node
	Connection lLastPiece;
	if (lToIsAN) {
		int lTime = lBLanConns[lToCityIndex].fGetLastTime();
		lLastPiece.fAdd(iTo, lTime);
	}
	else {
		unordered_set<int> *cBackNeigh = &(cBNeighs[lToCityIndex]);
		for (unordered_map<int, Connection>::iterator i = lBLanConns.begin();
				i != lBLanConns.end(); i++) {
			int lTime = i->second.fGetLastTime();
			if (lTime == -1) {
				continue;
			}
			Connection lConn = fDijkWrapConnRestr(cGraph, i->first, lTime, lToCityIndex, *cBackNeigh);
			if (lConn.fIsEmpty()) {
				continue;
			}
			if (lLastPiece.fIsEmpty() || lLastPiece.fGetLastTime() > lConn.fGetLastTime()) {
				lLastPiece = lConn;
			}
		}
	}

	if (lLastPiece.fIsEmpty()) {
		TERMINATE;
	}

	//check if we can improve local
	if (lLocalConn.fIsEmpty() == false && lLocalConn.fGetLastTime() < lLastPiece.fGetLastTime()) {
		TERMINATE;
	}

	//----- Connect the pieces together -----

	Connection lFinalConn;

	string lBackLAN = lLastPiece.fGetFirstCity();
	int lBackLanIndex = cGraph->fGetNodeIndex(lBackLAN);

	string lLAN = lBLanConns[lBackLanIndex].fGetFirstCity();
	int lLanIndex = cGraph->fGetNodeIndex(lLAN);

	lFinalConn.fConcat(lFLanConns[lLanIndex]);
	lFinalConn.fConcat(lBLanConns[lBackLanIndex]);
	lFinalConn.fConcat(lLastPiece);
	lFinalConn.fRemoveDuplicates();

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying USP-OR-A oracle (for TD graphs) for the connections" << endl);

	return lFinalConn;
}

int TdUspora::fQueryEa(string iFrom, int iTime, string iTo) {
	INFO("Starting querying USP-OR-A oracle (for TD graphs) for the earliest arrival" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Connection lConn = fQueryConn(iFrom, iTime, iTo);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying USP-OR-A oracle (for TD graphs) for the earliest arrival" << endl);

	return lConn.fGetLength();
}

string TdUspora::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP USP-OR-A oracle (for time graphs)" << endl <<
			"	" << TD_USPORA_SET << " {post-mail}: specify the access nodes set" << endl <<
			endl;

	return lSs.str();
}

set<QueryType> TdUspora::fSQueryTypes() {
	set<QueryType> lQTypes;
	lQTypes.insert(QTEa);
	lQTypes.insert(QTConn);

	return lQTypes;
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TdUspora::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TdUspora::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TdUspora::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG

		COMP(TD_USPORA_SET) {
			MOVEARG;
			cOptAnPm = lCurArg;
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "TdUspora::fProcessArgs: Arguments processed" << endl);
}
