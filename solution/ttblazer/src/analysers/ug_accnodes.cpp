/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"
#include "../algaccn.h"
#include "../postbox/postbox.h"

#include "ug_accnodes.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class - constructors
/*****************************************************************/
UgAccNodes::UgAccNodes(vector<string> iArguments) {
	cArgs = iArguments;

	cOptionsAccNodesPm = "";
	cOptionsDet = false;
	cOptionsLim = -1;

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* UgAccNodes::fAnalyse(Object *iObject) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	UgAccNodesResult* lAnRes = new UgAccNodesResult();

	//----- compute -----
	INFO("Starting analysis of UG access node set" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//build Id->Index map
	INFO("Building ID->Index map" << endl);
	lGraph->fComputeIdIndexMap();

	if (cOptionsAccNodesPm == "") {
		ERR("No access node set specified!" << endl);
		return NULL;
	}

	//get access nodes from the post mail
	PostMail *lPm = PostBox::fGetMail(cOptionsAccNodesPm, PB_TYPE_VECT_STRING);
	if (lPm == NULL) {
		ERR("Mail not found!" << endl);
		return NULL;
	}
	vector<string> lAccNodes = *((vector<string>*)lPm->fGetData());

	//if there is a limit on number of access nodes
	if (cOptionsLim > -1 && cOptionsLim <= lAccNodes.size()) {
		lAccNodes.resize(cOptionsLim);
	}

	//make set
	unordered_set<int> lANs;
	for (size_t i = 0; i < lAccNodes.size(); i++) {
		int lIndex = lGraph->fGetNodeIndex(lAccNodes[i]);
		if (lIndex == -1) {
			ERR("City " << lAccNodes[i] << " not found in UG!" << endl);
			return NULL;
		}
		lANs.insert(lIndex);
	}

	ANRES << fStringDiagAnSet(lGraph, lANs, cOptionsDet);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending analysis of UG access node set" << endl);

	return lAnRes;
}


string UgAccNodes::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UG Access Nodes analyser help: " << endl <<
			"	" << UG_ACC_NODES_SET << " {post-mail}: specify the access nodes set" << endl <<
			"	" << UG_ACC_NODES_DET << ": list also details for each node" << endl <<
			"	" << UG_ACC_NODES_LIM << " {limit}: limit the number of access nodes" << endl <<
			endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgAccNodes::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgAccNodes::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgAccNodes::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;

		COMP(UG_ACC_NODES_SET) {
			MOVEARG;
			cOptionsAccNodesPm = lCurArg;
		}
		else COMP(UG_ACC_NODES_DET) {
			cOptionsDet = true;
		}
		else COMP(UG_ACC_NODES_LIM) {
			MOVEARG;
			cOptionsLim = fStrToInt(lCurArg);
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "UgAccNodes::fProcessArgs: Arguments processed" << endl);
}
