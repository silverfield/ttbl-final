/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "ug_strconn.h"

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
UgStrConn::UgStrConn(vector<string> iArguments) {
	cOptPartition = false;
	cOptPartitionDet = false;

	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* UgStrConn::fAnalyse(Object *iObject) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	UgStrConnResult* lConnResult = new UgStrConnResult();

	//----- compute -----
	INFO("Starting analysis of UG strong connectivity" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Partition lPartition = fStrongConn(lGraph->fGetGraph());

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending analysis of UG strong connectivity" << endl);

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


string UgStrConn::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UG Strong connectivity analyser help: " << endl <<
			"	" << UG_STRCONN_CMD_PARTINF << ": get also the information about the "
					"connected components" << endl <<
			"	" << UG_STRCONN_CMD_PARTDET << ": output also the components themselves" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgStrConn::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgStrConn::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgStrConn::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(UG_STRCONN_CMD_PARTINF) {
			cOptPartition = true;
			continue;
		}
		COMP(UG_STRCONN_CMD_PARTDET) {
			cOptPartitionDet = true;
			cOptPartition = true;
			continue;
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "UgStrConn::fProcessArgs: Arguments processed" << endl);
}
