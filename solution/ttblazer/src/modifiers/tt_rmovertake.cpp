/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "tt_rmovertake.h"

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
TtRmOvertake::TtRmOvertake(vector<string> iArguments) {
	cArgs = iArguments;

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Ret TtRmOvertake::fModify(Object *iObject) {
	Timetable *lTimetable = dynamic_cast<Timetable*>(iObject);

	//----- compute -----
	INFO("Starting removing overtaken edges" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//get and remove overtaken edges
	vector<pair<ElCon, ElCon> > lOvertaken = fGetOvertakenEdges(lTimetable);
	for (size_t i = 0; i < lOvertaken.size(); i++) {
		lTimetable->fRemove(lOvertaken[i].first);
	}

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending removing overtaken edges" << endl);

	return OK;
}


string TtRmOvertake::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP Remove overtaken edges modifier help: " << endl <<
			"	no arguments" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TtRmOvertake::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TtRmOvertake::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TtRmOvertake::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		UNKNOWN;
	}

	DEB(LVL_CONN, "TtRmOvertake::fProcessArgs: Arguments processed" << endl);
}
