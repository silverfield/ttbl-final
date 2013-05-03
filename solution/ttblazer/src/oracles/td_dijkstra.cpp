/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>
#include <set>

#include "../central.h"
#include "../algdijkstra.h"
#include "../algorithms.h"

#include "td_dijkstra.h"

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

TdDijkstra::TdDijkstra(vector<string> iArguments) {
	cGraph = NULL;
	cPreprocessed = false;

	cArgs = iArguments;
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Ret TdDijkstra::fPreprocess(Object *iObject) {
	cGraph = dynamic_cast<TdGraph*>(iObject);

	//----- prerequisites -----

	//----- preprocessing -----
	INFO("Starting preprocessing of Dijkstra's oracle (for TD graphs)" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//build Id->Index map
	INFO("Building ID->Index map" << endl);
	cGraph->fComputeIdIndexMap();

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending preprocessing of Dijkstra's oracle (for TD graphs)" << endl);

	cPreprocessed = true;

	return OK;
}

int TdDijkstra::fGetSpace() {
	int lBytes = 0;

	lBytes += fSizeOf(cArgs);
	for (vector<string>::iterator i = cArgs.begin(); i != cArgs.end(); i++) {
		lBytes += i->length();
	}
	lBytes += fSizeOf(cCities);
	for (map<string, int>::iterator i = cCities.begin(); i != cCities.end(); i++) {
		lBytes += i->first.length();
	}

	lBytes += sizeof(*this);

	return lBytes;
}

Connection TdDijkstra::fQueryConn(string iFrom, int iTime, string iTo) {
	if (cPreprocessed == false) {
		ERR("TdDijkstra::fQueryConn: Preprocessing not carried out" << endl);
		return Connection();
	}

	//----- compute -----
	INFO("Starting querying Dijkstra's oracle (for TD graphs) for the connections" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//get indices
	int lFromCityIndex = cGraph->fGetNodeIndex(iFrom);
	if (lFromCityIndex == -1) {
		ERR("Invalid departure city" << endl);
		if (lRet == OK) {gAlgTimer->fStopTimer();}
		return Connection();
	}

	int lToCityIndex = cGraph->fGetNodeIndex(iTo);
	if (lToCityIndex == -1) {
		ERR("Invalid arrival city" << endl);
		if (lRet == OK) {gAlgTimer->fStopTimer();}
		return Connection();
	}

	Connection lConn = fDijkstraConn(cGraph, lFromCityIndex, iTime, lToCityIndex);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying Dijkstra's oracle (for TD graphs) for the connections" << endl);

	return lConn;
}

int TdDijkstra::fQueryEa(string iFrom, int iTime, string iTo) {
	INFO("Starting querying Dijkstra's oracle (for TD graphs) for the earliest arrival" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	int lEa = fDijkWrapEa(cGraph, iFrom, iTime, iTo);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying Dijkstra's oracle (for TD graphs) for the earliest arrival" << endl);

	return lEa;
}

string TdDijkstra::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP Dijkstra's oracle (for TD graphs)" << endl <<
			"	No parameters required" << endl;

	return lSs.str();
}

set<QueryType> TdDijkstra::fSQueryTypes() {
	set<QueryType> lQTypes;
	lQTypes.insert(QTEa);
	lQTypes.insert(QTConn);

	return lQTypes;
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/
