/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "tg_usp_pb.h"

using namespace std;

/*****************************************************************/
/* Class - constructors
/*****************************************************************/
TgUspPb::TgUspPb(vector<string> iArguments) {
	cArgs = iArguments;

	cOptionsUspAnType = UTNormal;
	cOptionsLim = -1;
	cOptionsFromCityAll= true;
	cOptionsToCityAll = true;
	cOptionsFromCityPm = "";
	cOptionsToCityPm = "";

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

PostMail TgUspPb::fPostMail(Object *iObject, string iMailName) {
	TimeGraphObject *lGraph = dynamic_cast<TimeGraphObject*>(iObject);

	//----- compute -----
	INFO("Starting generating of USP mail" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//get UG graph
	UgGraph* lUgGraph = lGraph->fToUgGraph();

	//build II map
	INFO("Building II map" << endl);
	lGraph->fComputeIdIndexMap();

	//make from/to city sets
	INFO("Getting from/to city sets" << endl);
	vector<string> lFromCities;
	if (cOptionsFromCityAll == true) {
		lFromCities = lUgGraph->fGetNodeNames();
	}
	if (cOptionsFromCityAll == false) {
		PostMail *lPm = PostBox::fGetMail(cOptionsFromCityPm, PB_TYPE_VECT_STRING);
		if (lPm == NULL) {
			ERR("Mail not found!" << endl);
			return PostMail();
		}
		lFromCities = *((vector<string>*)lPm->fGetData());
	}
	vector<string> lToCities;
	if (cOptionsToCityAll == true) {
		lToCities = lUgGraph->fGetNodeNames();
	}
	if (cOptionsToCityAll == false) {
		PostMail *lPm = PostBox::fGetMail(cOptionsToCityPm, PB_TYPE_VECT_STRING);
		if (lPm == NULL) {
			ERR("Mail not found!" << endl);
			return PostMail();
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

	PostMail lMail;
	if (cOptionsUspAnType == UTNormal) {
		vector<vector<vector<Path> > > *lPaths = fUSP(lUgGraph, lGraph, lFromIndices, lToIndices);
		lMail = PostMail(lPaths, "USPs (normal)", PB_TYPE_USP_NORMAL);
	}
	else if (cOptionsUspAnType == UTCount) {
		vector<vector<int> > *lCounts = fUSPCountOnly(lUgGraph, lGraph, lFromIndices, lToIndices);
		lMail = PostMail(lCounts, "USPs (count)", PB_TYPE_USP_COUNT);
	}
	else if (cOptionsUspAnType == UTUsage) {
		vector<vector<vector<pair<Path, int> > > > *lUsage = fUSPUsageAlso(
				lUgGraph, lGraph, lFromIndices, lToIndices);
		lMail = PostMail(lUsage, "USPs (usage)", PB_TYPE_USP_USAGE);
	}

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating of USP mail" << endl);

	return lMail;
}


string TgUspPb::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TG USP postman help: " << endl <<
			"	" << TG_USP_PB_CMD_TYPE << " {" << gUspAnTypeStr[UTNormal] << "|" <<
					gUspAnTypeStr[UTCount] << "|" << gUspAnTypeStr[UTUsage] << "}: set what "
					"type of USP computation should be carried out" << endl <<
			"	" << TG_USP_PB_CMD_FROM << " {" << TG_USP_PB_CMD_CITY_ALL << "| " << TG_USP_PB_CMD_CITY_PB <<
				" {post-mail}}: specify the from-city set" << endl <<
			"	" << TG_USP_PB_CMD_TO << " {" << TG_USP_PB_CMD_CITY_ALL << "| " << TG_USP_PB_CMD_CITY_PB <<
				" {post-mail}}: specify the to-city set" << endl <<
			"	" << TG_USP_PB_CMD_SET << " {post-mail}: specify the access nodes set ("
					"shortcut for specifying both from and to sets...)" << endl <<
			"	" << TG_USP_PB_CMD_LIM << " {limit}: limit the number of access nodes" << endl
			<< endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TgUspPb::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TgUspPb::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TgUspPb::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(TG_USP_PB_CMD_TYPE) {
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
		else COMP(TG_USP_PB_CMD_FROM) {
			MOVEARG;
			COMP(TG_USP_PB_CMD_CITY_ALL) {
				cOptionsFromCityAll = true;
			}
			else COMP(TG_USP_PB_CMD_CITY_PB) {
				cOptionsFromCityAll = false;
				MOVEARG;
				cOptionsFromCityPm = lCurArg;
			}
			else {
				UNKNOWN;
			}
			continue;
		}
		else COMP(TG_USP_PB_CMD_TO) {
			MOVEARG;
			COMP(TG_USP_PB_CMD_CITY_ALL) {
				cOptionsFromCityAll = true;
			}
			else COMP(TG_USP_PB_CMD_CITY_PB) {
				cOptionsFromCityAll = false;
				MOVEARG;
				cOptionsFromCityPm = lCurArg;
			}
			else {
				UNKNOWN;
			}
			continue;
		}
		else COMP(TG_USP_PB_CMD_SET) {
			cOptionsFromCityAll = false;
			cOptionsToCityAll = false;
			MOVEARG;
			cOptionsFromCityPm = lCurArg;
			cOptionsToCityPm = lCurArg;
			continue;
		}
		else COMP(TG_USP_PB_CMD_LIM) {
			MOVEARG;
			cOptionsLim = fStrToInt(lCurArg);
			continue;
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "TgUspPb::fProcessArgs: Arguments processed" << endl);
}
