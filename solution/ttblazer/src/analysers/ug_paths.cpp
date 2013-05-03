/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "ug_paths.h"

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
UgPaths::UgPaths(vector<string> iArguments) {
	cOptPathsDet = false;
	cOptApx = -1;

	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* UgPaths::fAnalyse(Object *iObject) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	UgPathsResult* lAnRes = new UgPathsResult();

	//----- compute -----
	INFO("Starting analysis of UG paths" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	INFO("Checking if graph is fully weighted" << endl);
	bool lHasWeights = true;
	if (lGraph->fIsWeighted() == false) {
		INFO("Graph is not fully weighted, using length \"1\" for each arc" << endl);
		lHasWeights = false;
	}

	vector<Path> lPaths;
	if (cOptApx < 0) {
		INFO("Starting exact Floyd Warshall algorithm" << endl);
		vector<vector<Path> > lFWPaths = fFloydWarshall(lGraph, lHasWeights);
		INFO("Floyd Warshall algorithm done" << endl);
		for (size_t i = 0; i < lFWPaths.size(); i++) {
			for (size_t j = 0; j < lFWPaths[i].size(); j++) {
				lPaths.push_back(lFWPaths[i][j]);
			}
		}
	}
	else {
		if (cOptApx > 1) {
			ERR("APX parameter out of range" << endl);
			return NULL;
		}

		INFO("Getting shortest paths" << endl);
		lPaths = fGetSps(lGraph, cOptApx, lHasWeights);
	}

	int lTotalPaths = 0;
	double lAvgPathLength = 0;
	int lTotalPathLength = 0;
	double lAvgPathSize = 0;
	int lTotalPathSize = 0;
	int lMaxPathLength = 0;
	int lMinPathLength = -1;
	int lMaxPathSize = 0;
	int lMinPathSize = lGraph->fGetN();
	Path lMaxLengthPath;
	Path lMaxSizePath;

	INFO("Extracting required data from results" << endl);
	int lOldPercents = -1;
	for (size_t i = 0; i < lPaths.size(); i++) {
		int lPercents = (i * 100) / lPaths.size();
		if (lPercents != lOldPercents) {
			INFO("Progress " << lPercents << "%" << endl);
			lOldPercents = lPercents;
		}

		DEB(LVL_PATHS, "UgPaths::fAnalyse: Path[" << i << "]" <<
				lPaths[i].fGetString() << endl);
		if (lPaths[i].fGetSize() == 0) {
			continue;
		}
		lTotalPaths++;

		int lSize = lPaths[i].fGetSize() - 1;
		int lLength = lSize;
		if (lHasWeights) {
			lLength = lGraph->fGetPathLength(lPaths[i]);
		}

		lTotalPathLength += lLength;
		lTotalPathSize += lSize;
		if (lMaxPathLength < lLength) {
			lMaxPathLength = lLength;
			lMaxLengthPath = lPaths[i];
		}
		if (lMinPathLength == -1 || lMinPathLength > lLength) {
			lMinPathLength = lLength;
		}
		if (lMaxPathSize < lSize) {
			lMaxPathSize = lSize;
			lMaxSizePath = lPaths[i];
		}
		if (lMinPathSize > lSize) {
			lMinPathSize = lSize;
		}
	}
	lAvgPathLength = (double)lTotalPathLength / (double)lTotalPaths;
	lAvgPathSize = (double)lTotalPathSize / (double)lTotalPaths;

	//----- set results -----
	ANRES << "AVG PATH LENGTH: " << lAvgPathLength << endl;
	ANRES << "AVG PATH SIZE: " << lAvgPathSize << endl;
	ANRES << "MAX PATH LENGTH: " << lMaxPathLength << endl;
	ANRES << "MAX PATH SIZE: " << lMaxPathSize << endl;
	ANRES << "MIN PATH LENGTH: " << lMinPathLength << endl;
	ANRES << "MIN PATH SIZE: " << lMinPathSize << endl;
	int64_t lTotalPossiblePaths = (lGraph->fGetN() * lGraph->fGetN()) - lGraph->fGetN();
	int lPercents = (lTotalPaths * 100) / lTotalPossiblePaths;
	if (cOptApx < 0) {
		ANRES << "TOTAL # OF PATHS: " << lTotalPaths << "/" << lTotalPossiblePaths <<
			" possible (" << lPercents << "%)" << endl;
	}
	else {
		int64_t lForcastedCount = (int)(lTotalPaths * ((double)1 / cOptApx));
		int lForcastedPerc = (lForcastedCount * 100) / lTotalPossiblePaths;
		ANRES << "APX TOTAL # OF PATHS: " << lForcastedCount <<
				"/" << lForcastedPerc <<
			" possible (" << lPercents << "%)" << endl;
		ANRES << "# OF PATHS USED FOR APPROX.: " << lTotalPaths << "/" << lTotalPossiblePaths <<
			" possible (" << lPercents << "%)" << endl;
	}
	if (cOptPathsDet == true) {
		ANRES << "PATH OF MAX LENGTH (Size = " << lMaxLengthPath.fGetSize() << ", Length = " <<
				lGraph->fGetPathLength(lMaxLengthPath) << "): " <<
				lGraph->fGetPathString(lMaxLengthPath, "\n	") << endl;
		ANRES << "PATH OF MAX SIZE (Size = " << lMaxSizePath.fGetSize() << ", Length = " <<
				lGraph->fGetPathLength(lMaxSizePath) << "): " <<
				lGraph->fGetPathString(lMaxSizePath, "\n	") << endl;
	}

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending analysis of UG paths" << endl);

	return lAnRes;
}


string UgPaths::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UG Paths analyser help: " << endl <<
			"	" << UG_PATHS_CMD_DET << ": output also the maximum shortest paths" << endl <<
			"	" << UG_PATHS_CMD_APX << " {precision}: randomly selects a portion of all shortest"
					" paths - precision (double from 0 to 1) specifies the fraction (of all SPs)." << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgPaths::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgPaths::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgPaths::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(UG_PATHS_CMD_DET) {
			cOptPathsDet = true;
			continue;
		}
		else COMP(UG_PATHS_CMD_APX) {
			MOVEARG;
			cOptApx = fStrToDouble(lCurArg);
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "UgPaths::fProcessArgs: Arguments processed" << endl);
}
