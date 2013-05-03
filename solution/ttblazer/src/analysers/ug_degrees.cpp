/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "ug_degrees.h"

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
UgDegrees::UgDegrees(vector<string> iArguments) {
	cOptDegreesDet = false;

	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* UgDegrees::fAnalyse(Object *iObject) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	UgDegreesResult* lDegreesResult = new UgDegreesResult();

	//----- compute -----
	INFO("Starting analysis of UG degrees" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	vector<int> lDegreeFreqs = fDegreeFreqs(lGraph->fGetGraph());
	double lAvgDeg = fAvgDegree(lDegreeFreqs);
	int lMinDeg = fFirstNonZeroValue(lDegreeFreqs);
	int lMaxDeg = fLastNonZeroValue(lDegreeFreqs);

	//----- set results -----
	lDegreesResult->fSetDegrees(lAvgDeg,lMinDeg, lMaxDeg);
	if (cOptDegreesInf == true) {
		lDegreesResult->fSetDegreesInf(lDegreeFreqs);
	}
	if (cOptDegreesDet) {
		map<int, vector<int> > lGroups = fDegreeGroups(lGraph->fGetGraph());
		lDegreesResult->fSetDegreesDet(lGroups);
	}

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending analysis of UG degrees" << endl);

	return lDegreesResult;
}


string UgDegrees::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UG Degrees analyser help: " << endl <<
			"	" << UG_DEG_CMD_INF << ": output ubiquity of each degree" << endl <<
			"	" << UG_DEG_CMD_DET << ": output each node's degree" << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgDegrees::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgDegrees::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgDegrees::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(UG_DEG_CMD_DET) {
			cOptDegreesDet = true;
			cOptDegreesInf = true;
			continue;
		}
		COMP(UG_DEG_CMD_INF) {
			cOptDegreesInf = true;
			continue;
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "UgDegrees::fProcessArgs: Arguments processed" << endl);
}
