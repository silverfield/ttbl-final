/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "tt_subtt.h"
#include "../cmdproc.h"

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
TtSubTt::TtSubTt(vector<string> iArguments) {
	cArgs = iArguments;

	cN = -1;
	cMaxHeight = -1;
	cMinRange = -1;
	cMaxRange = -1;

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Object* TtSubTt::fGenerate(Object *iObject) {
	Timetable *lTimetable = dynamic_cast<Timetable*>(iObject);

	//----- compute -----
	INFO("Starting generating sub-TT" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//get the sub-UG
	INFO("Getting the sub-UG of required size" << endl);
	UgGraph* lUgGraph = lTimetable->fToUgGraph();
	if (cN != -1) {
		Graph<UGNodeData, UGArcData> lComponent = fSubCon(lUgGraph->fGetGraph(), cN);
		lUgGraph->fSetGraph(&lComponent);
	}

	//get the sub-TT
	INFO("Getting the sub-TT" << endl);
	Timetable *lSubTt = lTimetable->fSubTimetable(lUgGraph, cMaxHeight, cMinRange, cMaxRange);

	delete(lUgGraph);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating sub-TT" << endl);

	return lSubTt;
}


string TtSubTt::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP Sub-TT generator help: " << endl <<
			"	" << TT_SUBTT_N << " {size}: goal size of the underlying connected graph of the sub-TT" << endl <<
			"	" << TT_SUBTT_MINR << " {time}: min time range of the sub-TT" << endl <<
			"	" << TT_SUBTT_MAXR << " {time}: max time range of the sub-TT" << endl <<
			"	" << TT_SUBTT_H << " {height}: max height of the sub-TT" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TtSubTt::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TtSubTt::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TtSubTt::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(TT_SUBTT_N) {
			MOVEARG;
			cN = fStrToInt(lCurArg);
		}
		else COMP(TT_SUBTT_MINR) {
			GET_TIME;
			cMinRange = lTime;
		}
		else COMP(TT_SUBTT_MAXR) {
			GET_TIME;
			cMaxRange = lTime;
		}
		else COMP(TT_SUBTT_H) {
			MOVEARG;
			cMaxHeight = fStrToInt(lCurArg);
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "TtSubTt::fProcessArgs: Arguments processed" << endl);
}
