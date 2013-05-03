/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "tt_tote.h"

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
TtToTe::TtToTe(vector<string> iArguments) {
	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Object* TtToTe::fGenerate(Object *iObject) {
	Timetable *lTimetable = dynamic_cast<Timetable*>(iObject);

	//----- compute -----
	INFO("Starting generating TE graph" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	TeGraph *lTeGraph = lTimetable->fToTeGraph();

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating TE graph" << endl);

	return lTeGraph;
}


string TtToTe::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TT to TE generator help: " << endl;
	lSs << "	no parameters required" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TtToTe::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TtToTe::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TtToTe::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		UNKNOWN;
	}

	DEB(LVL_CONN, "TtToTe::fProcessArgs: Arguments processed" << endl);
}
