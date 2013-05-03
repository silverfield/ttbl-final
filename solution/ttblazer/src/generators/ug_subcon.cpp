/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>
#include <set>

#include "../central.h"
#include "../algorithms.h"

#include "ug_subcon.h"

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

UgSubCon::UgSubCon(vector<string> iArguments) {
	cArgs = iArguments;

	cN = -1;

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Object* UgSubCon::fGenerate(Object *iObject) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	if (cN < 0) {
		ERR("Parameter \"n\" invalid" << endl);
		return NULL;
	}

	INFO("Starting generating of connected component" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Graph<UGNodeData, UGArcData> lComponent = fSubCon(lGraph->fGetGraph(), cN);
	UgGraph *lResult = new UgGraph();
	lResult->fSetGraph(&lComponent);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating of connected component" << endl);

	return lResult;
}

string UgSubCon::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UG Connected Subgraphs generator" << endl <<
			"	n: goal size of the connected subgraph" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgSubCon::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgSubCon::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgSubCon::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		cN = fStrToInt(lCurArg);
	}

	DEB(LVL_CONN, "UgSubCon::fProcessArgs: Arguments processed" << endl);
}

