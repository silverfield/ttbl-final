/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "tt_toug.h"

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
TtToUg::TtToUg(vector<string> iArguments) {
	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Object* TtToUg::fGenerate(Object *iObject) {
	Timetable *lTimetable = dynamic_cast<Timetable*>(iObject);

	//----- compute -----
	INFO("Starting generating UG graph" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	UgGraph *lUgGraph = lTimetable->fToUgGraph();

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating UG graph" << endl);

	return lUgGraph;
}


string TtToUg::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TT to UG generator help: " << endl;
	lSs << "	no parameters required" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TtToUg::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TtToUg::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TtToUg::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		UNKNOWN;
	}

	DEB(LVL_CONN, "TtToUg::fProcessArgs: Arguments processed" << endl);
}
