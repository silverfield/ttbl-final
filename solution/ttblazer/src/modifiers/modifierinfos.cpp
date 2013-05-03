/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"

#include "modifierinfos.h"
#include "tt_rmovertake.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

vector<std::string> ModifierInfos::cNames;
vector<std::string> ModifierInfos::cHelps;
vector<set<ObjType> > ModifierInfos::cMeantFors;

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

/*****************************************************************/
/* Class - interface
/*****************************************************************/

vector<string> ModifierInfos::fGetNames() {
	return cNames;
}

vector<set<ObjType> > ModifierInfos::fGetMeantFors() {
	return cMeantFors;
}

string ModifierInfos::fGetHelp(string iMName, ObjType iOT) {
	int lIndex = fGetIndexByName(iMName, iOT);
	if (lIndex == -1) {
		return "";
	}

	return cHelps[lIndex];
}

set<ObjType> ModifierInfos::fGetMeantFor(string iMName, ObjType iOT) {
	int lIndex = fGetIndexByName(iMName, iOT);
	if (lIndex == -1) {
		return set<ObjType>();
	}

	return cMeantFors[lIndex];
}

Modifier* ModifierInfos::fCreateModifier(string iMName, ObjType iOT, vector<string> iArgs) {
	//UG

	//TE

	//TD

	//TT
	if (iMName == TtRmOvertake::fSGetName() && TtRmOvertake::fSMeantFor().count(iOT) != 0) {
		return new TtRmOvertake(iArgs);
	}

	return NULL;
}

void ModifierInfos::fInitializeInfos() {
	//UG

	//TE

	//TD

	//TT
	cNames.push_back(TtRmOvertake::fSGetName());
	cHelps.push_back(TtRmOvertake::fSGetHelp());
	cMeantFors.push_back(TtRmOvertake::fSMeantFor());
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

int ModifierInfos::fGetIndexByName(string iMName, ObjType iOT) {
	for (vector<string>::size_type i = 0; i < cNames.size(); i++) {
		if (cNames[i] == iMName && cMeantFors[i].find(iOT) != cMeantFors[i].end()) {
			return i;
		}
	}

	return -1;
}
