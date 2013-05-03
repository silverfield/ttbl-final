/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "gr_hdim.h"

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
GrHDim::GrHDim(vector<string> iArguments) {
	cPrec = -1;
	cRep = -1;
	cMulti = false;

	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* GrHDim::fAnalyse(Object *iObject) {
	GraphObject *lGraph = dynamic_cast<GraphObject*>(iObject);

	GrHDimResult* lHDimResult = new GrHDimResult();

	if (cRep == -1) {
		INFO("Repetitions unspecified, using 1" << endl);
		cRep = 1;
	}
	if (cMulti == false && (cPrec < 0 || cPrec > 1)) {
		INFO("Precision unspecified, using 1" << endl);
		cPrec = 1;
	}

	//----- compute -----
	INFO("Starting analysis of GR highway dimension" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	INFO("Checking if graph is fully weighted" << endl);
	bool lHasWeights = true;
	if (lGraph->fIsWeighted() == false) {
		INFO("Graph is not fully weighted, using length \"1\" for each arc" << endl);
		lHasWeights = false;
	}

	int lHD = fApxHighwayDim(lGraph, lHasWeights, cMulti, cPrec, cRep);
	double lNormHD = (double)lHD / (double)lGraph->fGetN();

	//----- set results -----
	lHDimResult->fSetHDim(lHD, lNormHD);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending analysis of highway dimension" << endl);

	return lHDimResult;
}


string GrHDim::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP HDim analyser help: " << endl <<
			"	" << GR_HDIM_CMD_REP << " {number} : repetitions making HD computation more accurate"
				<< endl <<
			"	" << GR_HDIM_CMD_PREC << " {number} : precision - specifying how many shortest paths are"
					" considered in computation of HD. Range from 0 to 1, or -1 if all SPs should be"
					" considered." << endl <<
			"	" << GR_HDIM_CMD_MULTI << ": use this parameter to specify that multi-SPs should be "
					"considered" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("GrHDim::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("GrHDim::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void GrHDim::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(GR_HDIM_CMD_PREC) {
			MOVEARG;
			cPrec = fStrToDouble(lCurArg);
		}
		else COMP(GR_HDIM_CMD_REP) {
			MOVEARG;
			cRep = fStrToInt(lCurArg);
		}
		else COMP(GR_HDIM_CMD_MULTI) {
			cMulti = true;
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "GrHDim::fProcessArgs: Arguments processed" << endl);
}
