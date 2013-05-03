/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "ug_connectivity.h"

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
UgConnectivity::UgConnectivity(vector<string> iArguments) {
	cOptPartition = false;
	cOptPartitionDet = false;

	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* UgConnectivity::fAnalyse(Object *iObject) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	UgConnResult* lConnResult = new UgConnResult();

	//----- compute -----
	INFO("Starting analysis of UG connectivity" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Partition lPartition = fConn(lGraph->fGetGraph());

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending analysis of UG connectivity" << endl);

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


string UgConnectivity::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UG Connectivity analyser help: " << endl <<
			"	" << UG_CONN_CMD_PARTINF << ": get also the information about the "
					"connected components (their sizes...)" << endl <<
			"	" << UG_CONN_CMD_PARTDET << ": see the components themselves" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgConnectivity::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgConnectivity::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgConnectivity::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(UG_CONN_CMD_PARTINF) {
			cOptPartition = true;
			continue;
		}
		COMP(UG_CONN_CMD_PARTDET) {
			cOptPartitionDet = true;
			cOptPartition = true;
			continue;
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "UgConnectivity::fProcessArgs: Arguments processed" << endl);
}
