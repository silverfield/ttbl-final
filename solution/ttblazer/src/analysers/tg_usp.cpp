/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"
#include "../algusp.h"

#include "tg_usp.h"

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
TgUsp::TgUsp(vector<string> iArguments) {
	cArgs = iArguments;

	cOptionsDet = false;
	cOptionsUspAnType = UTNormal;
	cOptionsLim = -1;
	cOptionsFromCitySet = CSetAll;
	cOptionsToCitySet = CSetAll;
	cOptionsFromCityPm = "";
	cOptionsToCityPm = "";
	cOptionsUspsPm = "";

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

AnalyseResult* TgUsp::fAnalyse(Object *iObject) {
	TimeGraphObject *lGraph = dynamic_cast<TimeGraphObject*>(iObject);

	//----- compute -----
	INFO("Starting USP analysis" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//get UG graph
	UgGraph* lUgGraph = lGraph->fToUgGraph();

	//build II map
	INFO("Building II map" << endl);
	lGraph->fComputeIdIndexMap();

	//make from/to city sets
	INFO("Getting from/to city sets" << endl);
	vector<string> lFromCities;
	if (cOptionsFromCitySet == CSetAll) {
		lFromCities = lUgGraph->fGetNodeNames();
	}
	if (cOptionsFromCitySet == CSetPb) {
		PostMail *lPm = PostBox::fGetMail(cOptionsFromCityPm, PB_TYPE_VECT_STRING);
		if (lPm == NULL) {
			ERR("Mail not found!" << endl);
			return NULL;
		}
		lFromCities = *((vector<string>*)lPm->fGetData());
	}
	vector<string> lToCities;
	if (cOptionsToCitySet == CSetAll) {
		lToCities = lUgGraph->fGetNodeNames();
	}
	if (cOptionsToCitySet == CSetPb) {
		PostMail *lPm = PostBox::fGetMail(cOptionsToCityPm, PB_TYPE_VECT_STRING);
		if (lPm == NULL) {
			ERR("Mail not found!" << endl);
			return NULL;
		}
		lToCities = *((vector<string>*)lPm->fGetData());
	}

	//if there is a limit on number of access nodes
	if (cOptionsLim > -1 && cOptionsLim <= lFromCities.size()) {
		lFromCities.resize(cOptionsLim);
	}
	if (cOptionsLim > -1 && cOptionsLim <= lToCities.size()) {
		lToCities.resize(cOptionsLim);
	}

	INFO("From city set size: " << lFromCities.size() << endl);
	INFO("To city set size: " << lToCities.size() << endl);

	vector<int> lFromIndices;
	for (size_t i = 0; i < lFromCities.size(); i++) {
		lFromIndices.push_back(lUgGraph->fGetNodeIndex(lFromCities[i]));
	}
	vector<int> lToIndices;
	for (size_t i = 0; i < lToCities.size(); i++) {
		lToIndices.push_back(lUgGraph->fGetNodeIndex(lToCities[i]));
	}

	TgUspResult* lAnRes = new TgUspResult();

	//perform the analysis based on the type of USP data
	if (cOptionsUspAnType == UTNormal) {
		PostMail *lPm = PostBox::fGetMail(cOptionsUspsPm, PB_TYPE_USP_NORMAL);
		if (lPm == NULL) {
			ERR("Mail not found!" << endl);
			return NULL;
		}
		vector<vector<vector<Path> > > *lPaths = (vector<vector<vector<Path> > >*)lPm->fGetData();

		ANRES << fAnalyzeUsps(lUgGraph, lPaths, lFromIndices, lToIndices, cOptionsDet);
	}
	else if (cOptionsUspAnType == UTCount) {
		PostMail *lPm = PostBox::fGetMail(cOptionsUspsPm, PB_TYPE_USP_COUNT);
		if (lPm == NULL) {
			ERR("Mail not found!" << endl);
			return NULL;
		}
		vector<vector<int> > *lUspCounts = (vector<vector<int> >*)lPm->fGetData();

		ANRES << fAnalyzeUspCounts(lUgGraph, lUspCounts, lFromIndices, lToIndices, cOptionsDet);
	}
	else if (cOptionsUspAnType == UTUsage) {
		PostMail *lPm = PostBox::fGetMail(cOptionsUspsPm, PB_TYPE_USP_USAGE);
		if (lPm == NULL) {
			ERR("Mail not found!" << endl);
			return NULL;
		}
		vector<vector<vector<pair<Path, int> > > > *lUsage =
				(vector<vector<vector<pair<Path, int> > > >*)lPm->fGetData();

		ANRES << fAnalyzeUspUsage(lUgGraph, lUsage, lFromIndices, lToIndices, cOptionsDet);
	}

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending USP analysis" << endl);

	return lAnRes;
}


string TgUsp::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TimeGraph USP analyser help: " << endl <<
			"	" << TG_USP_CMD_DET << ": lists details" << endl <<
			"	" << TG_USP_CMD_USPS << " {usp-mail}: the pre-computed USPs" << endl <<
			"	" << TG_USP_CMD_TYPE << " {" << gUspAnTypeStr[UTNormal] << "|" <<
					gUspAnTypeStr[UTCount] << "|" << gUspAnTypeStr[UTUsage] << "}: set what "
					"type of USP computation should be carried out" << endl <<
			"	" << TG_USP_CMD_FROM << " {" << TG_USP_CMD_CITY_ALL << "| " << TG_USP_CMD_CITY_PB <<
				" {post-mail}}: specify the from-city set" << endl <<
			"	" << TG_USP_CMD_TO << " {" << TG_USP_CMD_CITY_ALL << "| " << TG_USP_CMD_CITY_PB <<
				" {post-mail}}: specify the to-city set" << endl <<
			"	" << TG_USP_CMD_SET << " {post-mail}: specify the access nodes set ("
					"shortcut for specifying both from and to sets...)" << endl <<
			"	" << TG_USP_CMD_LIM << " {limit}: limit the number of access nodes" << endl <<
			endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TgUsp::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TgUsp::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TgUsp::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(TG_USP_CMD_DET) {
			cOptionsDet = true;
			continue;
		}
		else COMP(TG_USP_CMD_TYPE) {
			MOVEARG;
			COMP(gUspAnTypeStr[UTNormal]) {
				cOptionsUspAnType = UTNormal;
			}
			else COMP(gUspAnTypeStr[UTCount]) {
				cOptionsUspAnType = UTCount;
			}
			else COMP(gUspAnTypeStr[UTUsage]) {
				cOptionsUspAnType = UTUsage;
			}
			else {
				UNKNOWN;
			}
		}
		else COMP(TG_USP_CMD_FROM) {
			MOVEARG;
			COMP(TG_USP_CMD_CITY_ALL) {
				cOptionsFromCitySet = CSetAll;
			}
			else COMP(TG_USP_CMD_CITY_PB) {
				cOptionsFromCitySet = CSetPb;
				MOVEARG;
				cOptionsFromCityPm = lCurArg;
			}
			else {
				UNKNOWN;
			}
			continue;
		}
		else COMP(TG_USP_CMD_TO) {
			MOVEARG;
			COMP(TG_USP_CMD_CITY_ALL) {
				cOptionsToCitySet = CSetAll;
			}
			else COMP(TG_USP_CMD_CITY_PB) {
				cOptionsToCitySet = CSetPb;
				MOVEARG;
				cOptionsToCityPm = lCurArg;
			}
			else {
				UNKNOWN;
			}
			continue;
		}
		else COMP(TG_USP_CMD_SET) {
			cOptionsFromCitySet = CSetPb;
			cOptionsToCitySet = CSetPb;
			MOVEARG;
			cOptionsFromCityPm = lCurArg;
			cOptionsToCityPm = lCurArg;
			continue;
		}
		else COMP(TG_USP_CMD_USPS) {
			MOVEARG;
			cOptionsUspsPm = lCurArg;
			continue;
		}
		else COMP(TG_USP_CMD_LIM) {
			MOVEARG;
			cOptionsLim = fStrToInt(lCurArg);
			continue;
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "TgUsp::fProcessArgs: Arguments processed" << endl);
}
