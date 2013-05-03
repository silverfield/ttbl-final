/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>
#include <set>

#include "../central.h"
#include "../algdijkstra.h"

#include "tg_dijkstra.h"

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

TgDijkstra::TgDijkstra(vector<string> iArguments) {
	cGraph = NULL;
	cPreprocessed = false;

	cArgs = iArguments;
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Ret TgDijkstra::fPreprocess(Object *iObject) {
	cGraph = dynamic_cast<TimeGraphObject*>(iObject);

	//----- prerequisites -----

	//----- preprocessing -----
	INFO("Starting preprocessing of Dijkstra's oracle (for Time graphs)" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//build Id->Index map
	INFO("Building ID->Index map" << endl);
	cGraph->fComputeIdIndexMap();

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending preprocessing of Dijkstra's oracle (for Time graphs)" << endl);

	cPreprocessed = true;

	return OK;
}

int TgDijkstra::fGetSpace() {
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

Connection TgDijkstra::fQueryConn(string iFrom, int iTime, string iTo) {
	if (cPreprocessed == false) {
		ERR("TgDijkstra::fQueryConn: Preprocessing not carried out" << endl);
		return Connection();
	}

	//----- compute -----
	INFO("Starting querying Dijkstra's oracle (for Time graphs) for the connection" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Connection lConn = fDijkWrapConn(cGraph, iFrom, iTime, iTo);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying Dijkstra's oracle (for Time graphs) for the connection" << endl);

	return lConn;
}

int TgDijkstra::fQueryEa(string iFrom, int iTime, string iTo) {
	INFO("Starting querying Dijkstra's oracle (for Time graphs) for the earliest arrival" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	int lEa = fDijkWrapEa(cGraph, iFrom, iTime, iTo);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying Dijkstra's oracle (for Time graphs) for the earliest arrival" << endl);

	return lEa;
}

string TgDijkstra::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP Dijkstra's oracle (for Time graphs)" << endl <<
			"	No parameters required" << endl;

	return lSs.str();
}

set<QueryType> TgDijkstra::fSQueryTypes() {
	set<QueryType> lQTypes;
	lQTypes.insert(QTEa);
	lQTypes.insert(QTConn);

	return lQTypes;
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/
