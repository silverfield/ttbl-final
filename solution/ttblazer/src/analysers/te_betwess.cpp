/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "te_betwess.h"

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
TeBetw::TeBetw(vector<string> iArguments) {
	cOptBetwDet = false;
	cOptApx = false;
	cApxPrec = -1;

	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* TeBetw::fAnalyse(Object *iObject) {
	TeGraph *lGraph = dynamic_cast<TeGraph*>(iObject);

	TeBetwResult* lBetwResult = new TeBetwResult(dynamic_cast<GraphObject*>(iObject));

	if (cOptApx && (cApxPrec <= 0 || cApxPrec > 1)) {
		ERR("Precision of betweenness approximation is out of range!" << endl);
		return NULL;
	}

	//----- compute -----
	INFO("Starting analysis of TE betweenness" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	vector<double> lBetws;
	vector<double> lBetwPercents;
	if (cOptApx == false) {
		INFO("Starting exact algorithm to fetch betweenness values" << endl);
		lBetws = fBetweenness(lGraph, &lBetwPercents, true);
		INFO("Ending exact algorithm to fetch betweenness values" << endl);
	}
	else {
		INFO("Starting approximate algorithm to fetch betweenness values" << endl);
		lBetws = fApxBetweenness(lGraph, &lBetwPercents, cApxPrec, true);
		INFO("Ending approximate algorithm to fetch betweenness values" << endl);
	}

	double lAvgBetw = -1;
	double lMaxBetw = -1;
	double lMinBetw = 2;
	int lMinBetwIndex = -1;
	int lMaxBetwIndex = -1;
	double lTotalBetw = 0;

	INFO("Extracting required data from results" << endl);
	for (vector<double>::size_type i = 0; i < lBetws.size(); i++) {
		lTotalBetw += lBetws[i];

		if (lMaxBetw < lBetws[i]) {
			lMaxBetw = lBetws[i];
			lMaxBetwIndex = i;
		}
		if (lMinBetw > lBetws[i]) {
			lMinBetw = lBetws[i];
			lMinBetwIndex = i;
		}
	}
	lAvgBetw = lTotalBetw / (double)lBetws.size();

	//----- set results -----
	lBetwResult->fSetBetw(lAvgBetw, lMaxBetwIndex, lMinBetwIndex, lMaxBetw, lMinBetw);
	if (cOptBetwDet == true) {
		lBetwResult->fSetBetwDet(lBetws, lBetwPercents);
	}

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending analysis of TE betweenness" << endl);

	return lBetwResult;
}


string TeBetw::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TE Betweenness analyser help: " << endl <<
			"	" << TE_BETW_CMD_DET << ": lists also betweenness value for each node" << endl <<
			"	" << TE_BETW_CMD_APX << " {precision}: takes only approximate betweenness values. Precision"
					" must be a double in range from 0 to 1" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TeBetw::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TeBetw::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TeBetw::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(TE_BETW_CMD_DET) {
			cOptBetwDet = true;
			continue;
		}
		COMP(TE_BETW_CMD_APX) {
			MOVEARG
			cApxPrec = fStrToInt(lCurArg);
			cOptApx = true;
			continue;
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "TeBetw::fProcessArgs: Arguments processed" << endl);
}
