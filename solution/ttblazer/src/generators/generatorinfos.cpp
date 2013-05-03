/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "ug_strcomp.h"
#include "ug_subcon.h"
#include "te_toug.h"
#include "td_toug.h"
#include "tt_toug.h"
#include "tt_tote.h"
#include "tt_totd.h"
#include "tt_subtt.h"

#include "generatorinfos.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

vector<std::string> GeneratorInfos::cNames;
vector<std::string> GeneratorInfos::cHelps;
vector<set<ObjType> > GeneratorInfos::cMeantFors;
vector<ObjType> GeneratorInfos::cGenerates;

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

/*****************************************************************/
/* Class - interface
/*****************************************************************/

vector<string> GeneratorInfos::fGetNames() {
	return cNames;
}

vector<set<ObjType> > GeneratorInfos::fGetMeantFors() {
	return cMeantFors;
}

vector<ObjType> GeneratorInfos::fGetGenerates() {
	return cGenerates;
}

string GeneratorInfos::fGetHelp(string iGName, ObjType iOT) {
	int lIndex = fGetIndexByName(iGName, iOT);
	if (lIndex == -1) {
		return "";
	}

	return cHelps[lIndex];
}

set<ObjType> GeneratorInfos::fGetMeantFor(string iGName, ObjType iOT) {
	int lIndex = fGetIndexByName(iGName, iOT);
	if (lIndex == -1) {
		return set<ObjType>();
	}

	return cMeantFors[lIndex];
}

ObjType GeneratorInfos::fGetGenerates(string iGName, ObjType iOT) {
	int lIndex = fGetIndexByName(iGName, iOT);
	if (lIndex == -1) {
		return OTErr;
	}

	return cGenerates[lIndex];
}

Generator* GeneratorInfos::fCreateGenerator(string iGName, ObjType iOT, vector<string> iArgs) {
	//UGs
	if (iGName == UgSubCon::fSGetName() && UgSubCon::fSMeantFor().count(iOT) != 0) {
		return new UgSubCon(iArgs);
	}
	if (iGName == UgStrComp::fSGetName() && UgStrComp::fSMeantFor().count(iOT) != 0) {
		return new UgStrComp(iArgs);
	}

	//TEs
	if (iGName == TeToUg::fSGetName() && TeToUg::fSMeantFor().count(iOT) != 0) {
		return new TeToUg(iArgs);
	}

	//TDs
	if (iGName == TdToUg::fSGetName() && TdToUg::fSMeantFor().count(iOT) != 0) {
		return new TdToUg(iArgs);
	}

	//TTs
	if (iGName == TtToUg::fSGetName() && TtToUg::fSMeantFor().count(iOT) != 0) {
		return new TtToUg(iArgs);
	}
	if (iGName == TtToTe::fSGetName() && TtToTe::fSMeantFor().count(iOT) != 0) {
		return new TtToTe(iArgs);
	}
	if (iGName == TtToTd::fSGetName() && TtToTd::fSMeantFor().count(iOT) != 0) {
		return new TtToTd(iArgs);
	}
	if (iGName == TtSubTt::fSGetName() && TtSubTt::fSMeantFor().count(iOT) != 0) {
		return new TtSubTt(iArgs);
	}

	return NULL;
}

void GeneratorInfos::fInitializeInfos() {
	//----- UGs -----
	//subcon
	cNames.push_back(UgSubCon::fSGetName());
	cHelps.push_back(UgSubCon::fSGetHelp());
	cMeantFors.push_back(UgSubCon::fSMeantFor());
	cGenerates.push_back(UgSubCon::fSGetGenerates());

	//strcomp
	cNames.push_back(UgStrComp::fSGetName());
	cHelps.push_back(UgStrComp::fSGetHelp());
	cMeantFors.push_back(UgStrComp::fSMeantFor());
	cGenerates.push_back(UgStrComp::fSGetGenerates());

	//----- TEs -----
	//te2ug
	cNames.push_back(TeToUg::fSGetName());
	cHelps.push_back(TeToUg::fSGetHelp());
	cMeantFors.push_back(TeToUg::fSMeantFor());
	cGenerates.push_back(TeToUg::fSGetGenerates());

	//----- TDs -----
	cNames.push_back(TdToUg::fSGetName());
	cHelps.push_back(TdToUg::fSGetHelp());
	cMeantFors.push_back(TdToUg::fSMeantFor());
	cGenerates.push_back(TdToUg::fSGetGenerates());

	//----- TTs -----
	//tt2ug
	cNames.push_back(TtToUg::fSGetName());
	cHelps.push_back(TtToUg::fSGetHelp());
	cMeantFors.push_back(TtToUg::fSMeantFor());
	cGenerates.push_back(TtToUg::fSGetGenerates());

	//tt2te
	cNames.push_back(TtToTe::fSGetName());
	cHelps.push_back(TtToTe::fSGetHelp());
	cMeantFors.push_back(TtToTe::fSMeantFor());
	cGenerates.push_back(TtToTe::fSGetGenerates());

	//tt2td
	cNames.push_back(TtToTd::fSGetName());
	cHelps.push_back(TtToTd::fSGetHelp());
	cMeantFors.push_back(TtToTd::fSMeantFor());
	cGenerates.push_back(TtToTd::fSGetGenerates());

	//ttsubtt
	cNames.push_back(TtSubTt::fSGetName());
	cHelps.push_back(TtSubTt::fSGetHelp());
	cMeantFors.push_back(TtSubTt::fSMeantFor());
	cGenerates.push_back(TtSubTt::fSGetGenerates());
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

int GeneratorInfos::fGetIndexByName(string iGName, ObjType iOT) {
	for (vector<string>::size_type i = 0; i < cNames.size(); i++) {
		if (cNames[i] == iGName && cMeantFors[i].find(iOT) != cMeantFors[i].end()) {
			return i;
		}
	}

	return -1;
}
