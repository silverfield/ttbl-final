/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "ug_density.h"

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
UgDensity::UgDensity(vector<string> iArguments) {
	cOptDensityDet = false;
	cOptApx = -1;

	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* UgDensity::fAnalyse(Object *iObject) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	UgDensityResult* lAnRes = new UgDensityResult();

	//----- compute -----
	INFO("Starting analysis of UG density" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	if (cOptApx < 0) {
		cOptApx = 1;
	}

	double lDensity = fDensity(lGraph, cOptApx);

	ANRES << "DENSITY: " << lDensity << endl;

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending analysis of UG density" << endl);

	return lAnRes;
}


string UgDensity::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UG Density analyser help: " << endl <<
			"	" << UG_DENSITY_CMD_APX << " {precision}: specifies the precision of the measurments" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgDensity::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgDensity::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgDensity::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(UG_DENSITY_CMD_APX) {
			MOVEARG;
			cOptApx = fStrToDouble(lCurArg);
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "UgDensity::fProcessArgs: Arguments processed" << endl);
}
