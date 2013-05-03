/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "ug_highbc.h"
#include "ug_highdeg.h"
#include "ug_anhdeg.h"
#include "ug_anhbc.h"
#include "tg_junctions.h"
#include "tg_usp_pb.h"
#include "ug_locsep.h"

#include "postboxinfos.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

vector<std::string> PostmanInfos::cNames;
vector<std::string> PostmanInfos::cHelps;
vector<set<ObjType> > PostmanInfos::cMeantFors;
vector<string> PostmanInfos::cGenerates;

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

/*****************************************************************/
/* Class - interface
/*****************************************************************/

vector<string> PostmanInfos::fGetNames() {
	return cNames;
}

vector<set<ObjType> > PostmanInfos::fGetMeantFors() {
	return cMeantFors;
}

vector<string> PostmanInfos::fGetGenerates() {
	return cGenerates;
}

string PostmanInfos::fGetHelp(string iName, ObjType iOT) {
	int lIndex = fGetIndexByName(iName, iOT);
	if (lIndex == -1) {
		return "";
	}

	return cHelps[lIndex];
}

set<ObjType> PostmanInfos::fGetMeantFor(string iName, ObjType iOT) {
	int lIndex = fGetIndexByName(iName, iOT);
	if (lIndex == -1) {
		return set<ObjType>();
	}

	return cMeantFors[lIndex];
}

string PostmanInfos::fGetGenerates(string iName, ObjType iOT) {
	int lIndex = fGetIndexByName(iName, iOT);
	if (lIndex == -1) {
		return "";
	}

	return cGenerates[lIndex];
}

Postman* PostmanInfos::fCreatePostman(string iName, ObjType iOT, vector<string> iArgs) {
	if (iName == UgHighBc::fSGetName() && UgHighBc::fSMeantFor().count(iOT) != 0) {
		return new UgHighBc(iArgs);
	}
	if (iName == UgHighDeg::fSGetName() && UgHighDeg::fSMeantFor().count(iOT) != 0) {
		return new UgHighDeg(iArgs);
	}
	if (iName == UgAnHDeg::fSGetName() && UgAnHDeg::fSMeantFor().count(iOT) != 0) {
		return new UgAnHDeg(iArgs);
	}
	if (iName == UgAnHBc::fSGetName() && UgAnHBc::fSMeantFor().count(iOT) != 0) {
		return new UgAnHBc(iArgs);
	}
	if (iName == TgJunctions::fSGetName() && TgJunctions::fSMeantFor().count(iOT) != 0) {
		return new TgJunctions(iArgs);
	}
	if (iName == TgUspPb::fSGetName() && TgUspPb::fSMeantFor().count(iOT) != 0) {
		return new TgUspPb(iArgs);
	}
	if (iName == UgLocsep::fSGetName() && UgLocsep::fSMeantFor().count(iOT) != 0) {
		return new UgLocsep(iArgs);
	}

	return NULL;
}

void PostmanInfos::fInitializeInfos() {
	cNames.push_back(UgHighBc::fSGetName());
	cHelps.push_back(UgHighBc::fSGetHelp());
	cMeantFors.push_back(UgHighBc::fSMeantFor());
	cGenerates.push_back(UgHighBc::fSGetGenerates());

	cNames.push_back(UgHighDeg::fSGetName());
	cHelps.push_back(UgHighDeg::fSGetHelp());
	cMeantFors.push_back(UgHighDeg::fSMeantFor());
	cGenerates.push_back(UgHighDeg::fSGetGenerates());

	cNames.push_back(UgAnHDeg::fSGetName());
	cHelps.push_back(UgAnHDeg::fSGetHelp());
	cMeantFors.push_back(UgAnHDeg::fSMeantFor());
	cGenerates.push_back(UgAnHDeg::fSGetGenerates());

	cNames.push_back(UgAnHBc::fSGetName());
	cHelps.push_back(UgAnHBc::fSGetHelp());
	cMeantFors.push_back(UgAnHBc::fSMeantFor());
	cGenerates.push_back(UgAnHBc::fSGetGenerates());

	cNames.push_back(TgJunctions::fSGetName());
	cHelps.push_back(TgJunctions::fSGetHelp());
	cMeantFors.push_back(TgJunctions::fSMeantFor());
	cGenerates.push_back(TgJunctions::fSGetGenerates());

	cNames.push_back(TgUspPb::fSGetName());
	cHelps.push_back(TgUspPb::fSGetHelp());
	cMeantFors.push_back(TgUspPb::fSMeantFor());
	cGenerates.push_back(TgUspPb::fSGetGenerates());

	cNames.push_back(UgLocsep::fSGetName());
	cHelps.push_back(UgLocsep::fSGetHelp());
	cMeantFors.push_back(UgLocsep::fSMeantFor());
	cGenerates.push_back(UgLocsep::fSGetGenerates());
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

int PostmanInfos::fGetIndexByName(string iName, ObjType iOT) {
	for (vector<string>::size_type i = 0; i < cNames.size(); i++) {
		if (cNames[i] == iName && cMeantFors[i].find(iOT) != cMeantFors[i].end()) {
			return i;
		}
	}

	return -1;
}
