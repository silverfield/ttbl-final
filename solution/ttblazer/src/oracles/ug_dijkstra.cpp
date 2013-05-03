/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>
#include <set>

#include "../central.h"
#include "../algdijkstra.h"

#include "ug_dijkstra.h"

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

UgDijkstra::UgDijkstra(vector<string> iArguments) {
	cGraph = NULL;
	cPreprocessed = false;

	cHasWeights = true;

	cArgs = iArguments;
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Ret UgDijkstra::fPreprocess(Object *iObject) {
	cGraph = dynamic_cast<UgGraph*>(iObject);

	//----- prerequisites -----
	INFO("Starting prerequisits check for Dijkstra's oracle" << endl);
	//weighted
	if (cGraph->fIsWeighted() == false) {
		INFO("Graph is not fully weighted, using length \"1\" for each arc" << endl);
		cHasWeights = false;
	}
	INFO("Ending prerequisits check (OK) for Dijkstra's oracle" << endl);

	//----- preprocessing -----
	INFO("Starting preprocessing of Dijkstra's oracle" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//nothing to do

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending preprocessing of Dijkstra's oracle" << endl);

	cPreprocessed = true;

	return OK;
}

int UgDijkstra::fGetSpace() {
	int lBytes = 0;

	lBytes += fSizeOf(cArgs);
	for (vector<string>::iterator i = cArgs.begin(); i != cArgs.end(); i++) {
		lBytes += i->length();
	}

	lBytes += sizeof(*this);

	return lBytes;
}

Path UgDijkstra::fQueryPath(int iFrom, int iTo) {
	if (cPreprocessed == false) {
		ERR("UgDijkstra::fQueryPath: Preprocessing not carried out" << endl);
		return Path();
	}

	//----- compute -----
	INFO("Starting querying Dijkstra's oracle for the path" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Path lPath = fDijkWrapSp(cGraph, iFrom, iTo, cHasWeights);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying Dijkstra's oracle for the path" << endl);

	return lPath;
}

int UgDijkstra::fQueryDistance(int iFrom, int iTo) {
	INFO("Starting querying Dijkstra's oracle for the distance" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Path lSp = fQueryPath(iFrom, iTo);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying Dijkstra's oracle for the distance" << endl);

	return cGraph->fGetPathLength(lSp);
}

string UgDijkstra::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP Dijkstra's oracle" << endl <<
			"	No parameters required" << endl;

	return lSs.str();
}

set<QueryType> UgDijkstra::fSQueryTypes() {
	set<QueryType> lQTypes;
	lQTypes.insert(QTDist);
	lQTypes.insert(QTSp);

	return lQTypes;
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/
