/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "to_various.h"

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
ToVarious::ToVarious(vector<string> iArguments) {
	cArgs = iArguments;

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* ToVarious::fAnalyse(Object *iObject) {
	TimetableObject *lTtObj = dynamic_cast<TimetableObject*>(iObject);

	//----- compute -----
	INFO("Starting VARIOUS analysis" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	double lAvgHeight = lTtObj->fGetAvgHeight();
	int lHeight = lTtObj->fGetHeight();
	int lTimeRange = lTtObj->fGetTimeRange();
	int lMinTime = lTtObj->fGetMinTime();
	int lMaxTime = lTtObj->fGetMaxTime();
	UgGraph* lUgGraph = lTtObj->fToUgGraph();
	int lUgN = lUgGraph->fGetN();
	int lUgM = lUgGraph->fGetM();
	delete lUgGraph;

	//set results
	ToVariousResult* lAnRes = new ToVariousResult();

	ANRES << "HEIGHT: " << lHeight << endl;
	ANRES << "AVG HEIGHT: " << lAvgHeight << endl;
	ANRES << "TIME RANGE: " << fMinutesToString(lTimeRange) << endl;
	ANRES << "FROM TIME RANGE: " << fMinutesToString(lMinTime) << endl;
	ANRES << "TO TIME RANGE: " << fMinutesToString(lMaxTime) << endl;
	ANRES << "CITIES COUNT: " << lUgN << endl;
	ANRES << "UG ARCS COUNT: " << lUgM << endl;

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending VARIOUS analysis" << endl);

	return lAnRes;
}


string ToVarious::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TimeGraph VARIOUS (height, time range) analyser help: " << endl <<
			"	no parameters required" <<
			endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("ToVarious::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("ToVarious::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void ToVarious::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		UNKNOWN;
	}

	DEB(LVL_CONN, "ToVarious::fProcessArgs: Arguments processed" << endl);
}
