/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "te_connectivity.h"

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
TeConnectivity::TeConnectivity(vector<string> iArguments) {
	cOptPartition = false;
	cOptPartitionDet = false;

	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* TeConnectivity::fAnalyse(Object *iObject) {
	TeGraph *lGraph = dynamic_cast<TeGraph*>(iObject);

	TeConnResult* lConnResult = new TeConnResult();

	//----- compute -----
	INFO("Starting analysis of TE connectivity" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Partition lPartition = fConn(lGraph->fGetGraph());

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending analysis of TE connectivity" << endl);

	//----- set results -----
	if (cOptPartition == true) {
		lConnResult->fSetPartition(lPartition);
	}
	if (cOptPartitionDet == true) {
		lConnResult->fSetPartitionDet(true);
	}
	lConnResult->fSetConn(lPartition.fGetSetCount() == 1);

	return lConnResult;
}


string TeConnectivity::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TE Connectivity analyser help: " << endl <<
			"	" << TE_CONN_CMD_PARTINF << ": get also the information about the "
					"connected components (their sizes...)" << endl <<
			"	" << TE_CONN_CMD_PARTDET << ": see the components themselves" << endl;
	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TeConnectivity::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TeConnectivity::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TeConnectivity::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(TE_CONN_CMD_PARTINF) {
			cOptPartition = true;
			continue;
		}
		COMP(TE_CONN_CMD_PARTDET) {
			cOptPartitionDet = true;
			cOptPartition = true;
			continue;
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "TeConnectivity::fProcessArgs: Arguments processed" << endl);
}
