/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "tt_totd.h"

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
TtToTd::TtToTd(vector<string> iArguments) {
	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Object* TtToTd::fGenerate(Object *iObject) {
	Timetable *lTimetable = dynamic_cast<Timetable*>(iObject);

	//----- compute -----
	INFO("Starting generating TD graph" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	TdGraph *lTdGraph = lTimetable->fToTdGraph();

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating TD graph" << endl);

	return lTdGraph;
}


string TtToTd::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TT to TD generator help: " << endl;
	lSs << "	no parameters required" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TtToTd::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TtToTd::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TtToTd::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		UNKNOWN;
	}

	DEB(LVL_CONN, "TtToTd::fProcessArgs: Arguments processed" << endl);
}
