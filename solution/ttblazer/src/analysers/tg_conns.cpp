/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"
#include "../algdijkstra.h"

#include "tg_conns.h"

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
TgConns::TgConns(vector<string> iArguments) {
	cOptConnsDet = false;
	cOptApx = -1;

	cArgs = iArguments;
	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* TgConns::fAnalyse(Object *iObject) {
	TimeGraphObject *lGraph = dynamic_cast<TimeGraphObject*>(iObject);

	TgConnsResult* lAnRes = new TgConnsResult();

	//----- compute -----
	INFO("Starting analysis of TG connections" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//build II map
	INFO("Building II map" << endl);
	lGraph->fComputeIdIndexMap();

	vector<Connection> lConns;
	if (cOptApx < 0) {
		INFO("Starting exact algorithm" << endl);
		vector<Event> lEvents = lGraph->fGetEvents(unordered_set<string>(), ANY_RANGE);

		vector<string> lVecCities = lGraph->fGetCities();
		unordered_set<string> lCities(lVecCities.begin(), lVecCities.end());

		for (size_t i = 0; i < lEvents.size(); i++) {
			unordered_map<string, Connection> lConnsTo = fDijkWrapRandomConnToCities(lGraph,
					lEvents[i].cCity, lEvents[i].cTime, lCities);
			for (unordered_map<string, Connection>::iterator j = lConnsTo.begin(); j != lConnsTo.end();
					j++) {
				lConns.push_back(j->second);
			}
		}
	}
	else {
		if (cOptApx > 1) {
			ERR("APX parameter out of range" << endl);
			return NULL;
		}

		INFO("Getting connections" << endl);
		lConns = fGetConns(lGraph, cOptApx);
	}

	int lTotalConns = 0;
	double lAvgConnLength = 0;
	int lTotalConnLength = 0;
	double lAvgConnSize = 0;
	int lTotalConnSize = 0;
	int lMaxConnLength = 0;
	int lMinConnLength = -1;
	int lMaxConnSize = 0;
	int lMinConnSize = lGraph->fGetN();
	Connection lMaxLengthConn;
	Connection lMaxSizeConn;

	INFO("Extracting required data from results" << endl);
	int lOldPercents = -1;
	for (size_t i = 0; i < lConns.size(); i++) {
		int lPercents = (i * 100) / lConns.size();
		if (lPercents != lOldPercents) {
			INFO("Progress " << lPercents << "%" << endl);
			lOldPercents = lPercents;
		}

		DEB(LVL_CONNS, "TgConns::fAnalyse: Conn[" << i << "]" <<
				lConns[i].fGetString(", ") << endl);
		if (lConns[i].fGetSize() == 0) {
			continue;
		}
		lTotalConns++;

		int lSize = lConns[i].fGetSize() - 1;
		int lLength = lConns[i].fGetLength();

		lTotalConnLength += lLength;
		lTotalConnSize += lSize;
		if (lMaxConnLength < lLength) {
			lMaxConnLength = lLength;
			lMaxLengthConn = lConns[i];
		}
		if (lMinConnLength == -1 || lMinConnLength > lLength) {
			lMinConnLength = lLength;
		}
		if (lMaxConnSize < lSize) {
			lMaxConnSize = lSize;
			lMaxSizeConn = lConns[i];
		}
		if (lMinConnSize > lSize) {
			lMinConnSize = lSize;
		}
	}
	lAvgConnLength = (double)lTotalConnLength / (double)lTotalConns;
	lAvgConnSize = (double)lTotalConnSize / (double)lTotalConns;

	//----- set results -----
	ANRES << "AVG CONN LENGTH: " << lAvgConnLength << endl;
	ANRES << "AVG CONN SIZE: " << lAvgConnSize << endl;
	ANRES << "MAX CONN LENGTH: " << lMaxConnLength << endl;
	ANRES << "MAX CONN SIZE: " << lMaxConnSize << endl;
	ANRES << "MIN CONN LENGTH: " << lMinConnLength << endl;
	ANRES << "MIN CONN SIZE: " << lMinConnSize << endl;
	int64_t lTotalPossibleConns = lGraph->fGetEventCount() * lGraph->fGetCityCount();
	int lPercents = (lTotalConns * 100) / lTotalPossibleConns;
	if (cOptApx < 0) {
		ANRES << "TOTAL # OF CONNS: " << lTotalConns << "/" << lTotalPossibleConns <<
			" possible (" << lPercents << "%)" << endl;
	}
	else {
		int64_t lForcastedCount = (int)(lTotalConns * ((double)1 / cOptApx));
		int lForcastedPerc = (lForcastedCount * 100) / lTotalPossibleConns;
		ANRES << "APX TOTAL # OF CONNS: " << lForcastedCount <<
				"/" << lTotalPossibleConns <<
			" possible (" << lForcastedPerc << "%)" << endl;
		ANRES << "# OF CONNS USED FOR APPROX.: " << lTotalConns << "/" << lTotalPossibleConns <<
			" possible (" << lPercents << "%)" << endl;
	}
	if (cOptConnsDet == true) {
		ANRES << "CONN OF MAX LENGTH (Size = " << lMaxLengthConn.fGetSize() << ", Length = " <<
				lMaxLengthConn.fGetLength() << "): " <<
				lMaxLengthConn.fGetString("\n	") << endl;
		ANRES << "CONN OF MAX SIZE (Size = " << lMaxSizeConn.fGetSize() << ", Length = " <<
				lMaxSizeConn.fGetLength() << "): " <<
				lMaxSizeConn.fGetString("\n	") << endl;
	}

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending analysis of Time graph connections" << endl);

	return lAnRes;
}


string TgConns::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TG Conns analyser help: " << endl <<
			"	" << TG_CONNS_CMD_DET << ": output also the maximum optimal connections" << endl <<
			"	" << TG_CONNS_CMD_APX << " {precision}: randomly selects a portion of all optimal"
					" connections - precision (double from 0 to 1) specifies the fraction (of all OCs)." << endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TgConns::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TgConns::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TgConns::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(TG_CONNS_CMD_DET) {
			cOptConnsDet = true;
			continue;
		}
		else COMP(TG_CONNS_CMD_APX) {
			MOVEARG;
			cOptApx = fStrToDouble(lCurArg);
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "TgConns::fProcessArgs: Arguments processed" << endl);
}
