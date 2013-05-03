/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>
#include <set>

#include "../central.h"
#include "../algorithms.h"

#include "ug_strcomp.h"

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

UgStrComp::UgStrComp(vector<string> iArguments) {
	cArgs = iArguments;

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Object* UgStrComp::fGenerate(Object *iObject) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	INFO("Starting generating of maximum strongly connected subgraph" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Graph<UGNodeData, UGArcData> lComponent = fStrongComp(lGraph->fGetGraph());
	UgGraph *lResult = new UgGraph();
	lResult->fSetGraph(&lComponent);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating of maximum strongly connected subgraph" << endl);

	return lResult;
}

string UgStrComp::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UG Strongly Connected Subgraphs generator" << endl <<
			"	no parameters required" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgStrComp::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgStrComp::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgStrComp::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		UNKNOWN;
	}

	DEB(LVL_CONN, "UgStrComp::fProcessArgs: Arguments processed" << endl);
}

