/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "te_toug.h"

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
TeToUg::TeToUg(vector<string> iArguments) {
	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Object* TeToUg::fGenerate(Object *iObject) {
	TeGraph *lTeGraph = dynamic_cast<TeGraph*>(iObject);

	//----- compute -----
	INFO("Starting generating UG graph" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	UgGraph *lUgGraph = lTeGraph->fToUgGraph();

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating UG graph" << endl);

	return lUgGraph;
}


string TeToUg::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TE to UG generator help: " << endl;
	lSs << "	no parameters required" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TeToUg::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TeToUg::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TeToUg::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		UNKNOWN;
	}

	DEB(LVL_CONN, "TeToUg::fProcessArgs: Arguments processed" << endl);
}
