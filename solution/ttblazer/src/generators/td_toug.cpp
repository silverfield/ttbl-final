/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "td_toug.h"

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
TdToUg::TdToUg(vector<string> iArguments) {
	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Object* TdToUg::fGenerate(Object *iObject) {
	TdGraph *lTdGraph = dynamic_cast<TdGraph*>(iObject);

	//----- compute -----
	INFO("Starting generating UG graph" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	UgGraph *lUgGraph = lTdGraph->fToUgGraph();

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating UG graph" << endl);

	return lUgGraph;
}


string TdToUg::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TD to UG generator help: " << endl;
	lSs << "	no parameters required" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TdToUg::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TdToUg::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TdToUg::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		UNKNOWN;
	}

	DEB(LVL_CONN, "TdToUg::fProcessArgs: Arguments processed" << endl);
}
