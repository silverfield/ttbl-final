/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"
#include "../algusp.h"

#include "tryout.h"

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
TryOut::TryOut(vector<string> iArguments) {
	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* TryOut::fAnalyse(Object *iObject) {
	TeGraph *lGraph = dynamic_cast<TeGraph*>(iObject);

	TryOutResult* lTryOutResult = new TryOutResult();

	//----- compute -----
	INFO("Starting TryOut analysis" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending TryOut analysis" << endl);

	return lTryOutResult;
}


string TryOut::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TryOut analyser help: " << endl <<
			"	This is just to try out new functionality before implementing real analyser" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TryOut::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TryOut::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TryOut::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
//		MOVEARG;
//		COMP(UG_BETW_CMD_DET) {
//			cOptBetwDet = true;
//			continue;
//		}
//		COMP(UG_BETW_CMD_APX) {
//			MOVEARG
//			cApxPrec = fStrToInt(lCurArg);
//			cOptApx = true;
//			continue;
//		}
//		else {
//			UNKNOWN;
//		}
	}

	DEB(LVL_CONN, "TryOut::fProcessArgs: Arguments processed" << endl);
}
