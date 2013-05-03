/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "tt_overtake.h"

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
TtOvertake::TtOvertake(vector<string> iArguments) {
	cOptionsDet = false;

	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* TtOvertake::fAnalyse(Object *iObject) {
	Timetable *lTt = dynamic_cast<Timetable*>(iObject);

	OvertakeResult* lAnRes = new OvertakeResult();

	//----- compute -----
	INFO("Starting analysis of TT overtaking" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	vector<pair<ElCon, ElCon> > lOvertakeEdges = fGetOvertakenEdges(lTt);

	ANRES << "OVERTAKING: " << gBoolStr[lOvertakeEdges.size() != 0] << endl;
	int lPercents = (lOvertakeEdges.size() * 100) / lTt->fGetN();
	ANRES << "TOTAL of " << lOvertakeEdges.size() << " overtaken el. conns. (" << lPercents << "%)" << endl;

	if (cOptionsDet) {
		ANRES << "OVERTAKING EDGES: " << endl;
		for (vector<pair<ElCon, ElCon> >::iterator i = lOvertakeEdges.begin();
				i != lOvertakeEdges.end(); i++) {
			ANRES << "	[" << i->first.fGetString(gOptions->cTimeFormat) << "] by [" <<
					i->second.fGetString(gOptions->cTimeFormat) << "]" << endl;
		}
	}

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending analysis of TT overtaking" << endl);

	return lAnRes;
}


string TtOvertake::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TT Overtaking analyser help: " << endl <<
			"	" << TT_OVERTAKE_CMD_DET << " lists also overtaking edges" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TtOvertake::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TtOvertake::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TtOvertake::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(TT_OVERTAKE_CMD_DET) {
			cOptionsDet = true;
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "TtOvertake::fProcessArgs: Arguments processed" << endl);
}
