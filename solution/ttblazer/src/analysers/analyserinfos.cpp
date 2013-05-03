/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "tryout.h"
#include "ug_connectivity.h"
#include "ug_strconn.h"
#include "ug_degrees.h"
#include "ug_paths.h"
#include "ug_accnodes.h"
#include "tg_conns.h"
#include "ug_betwess.h"
#include "gr_hdim.h"
#include "te_betwess.h"
#include "te_connectivity.h"
#include "tg_usp.h"
#include "tt_overtake.h"
#include "to_various.h"
#include "ug_density.h"

#include "analyserinfos.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

vector<string> AnalyserInfos::cNames;
vector<string> AnalyserInfos::cHelps;
vector<set<ObjType> > AnalyserInfos::cMeantFors;

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

/*****************************************************************/
/* Class - interface
/*****************************************************************/

vector<string> AnalyserInfos::fGetNames() {
	return cNames;
}

vector<set<ObjType> > AnalyserInfos::fGetMeantFors() {
	return cMeantFors;
}

string AnalyserInfos::fGetHelp(string iAName, ObjType iOT) {
	int lIndex = fGetIndexByName(iAName, iOT);
	if (lIndex == -1) {
		return "";
	}

	return cHelps[lIndex];
}

set<ObjType> AnalyserInfos::fGetMeantFor(string iAName, ObjType iOT) {
	int lIndex = fGetIndexByName(iAName, iOT);
	if (lIndex == -1) {
		return set<ObjType>();
	}

	return cMeantFors[lIndex];
}

Analyser* AnalyserInfos::fCreateAnalyser(string iAName, ObjType iOT, vector<string> iArgs) {
	if (iAName == TryOut::fSGetName() && TryOut::fSMeantFor().count(iOT) != 0) {
		return new TryOut(iArgs);
	}
	if (iAName == TgUsp::fSGetName() && TgUsp::fSMeantFor().count(iOT) != 0) {
		return new TgUsp(iArgs);
	}
	if (iAName == TgConns::fSGetName() && TgConns::fSMeantFor().count(iOT) != 0) {
		return new TgConns(iArgs);
	}
	if (iAName == GrHDim::fSGetName() && GrHDim::fSMeantFor().count(iOT) != 0) {
		return new GrHDim(iArgs);
	}
	if (iAName == ToVarious::fSGetName() && ToVarious::fSMeantFor().count(iOT) != 0) {
		return new ToVarious(iArgs);
	}

	//----- UGs -----
	if (iAName == UgConnectivity::fSGetName() && UgConnectivity::fSMeantFor().count(iOT) != 0) {
		return new UgConnectivity(iArgs);
	}
	if (iAName == UgDegrees::fSGetName() && UgDegrees::fSMeantFor().count(iOT) != 0) {
		return new UgDegrees(iArgs);
	}
	if (iAName == UgStrConn::fSGetName() && UgStrConn::fSMeantFor().count(iOT) != 0) {
		return new UgStrConn(iArgs);
	}
	if (iAName == UgPaths::fSGetName() && UgPaths::fSMeantFor().count(iOT) != 0) {
		return new UgPaths(iArgs);
	}
	if (iAName == UgBetw::fSGetName() && UgBetw::fSMeantFor().count(iOT) != 0) {
		return new UgBetw(iArgs);
	}
	if (iAName == UgAccNodes::fSGetName() && UgAccNodes::fSMeantFor().count(iOT) != 0) {
		return new UgAccNodes(iArgs);
	}
	if (iAName == UgDensity::fSGetName() && UgDensity::fSMeantFor().count(iOT) != 0) {
		return new UgDensity(iArgs);
	}

	//----- TEs -----
	if (iAName == TeBetw::fSGetName() && TeBetw::fSMeantFor().count(iOT) != 0) {
		return new TeBetw(iArgs);
	}
	if (iAName == TeConnectivity::fSGetName() && TeConnectivity::fSMeantFor().count(iOT) != 0) {
		return new TeConnectivity(iArgs);
	}

	//----- TDs -----

	//----- TTs -----
	if (iAName == TtOvertake::fSGetName() && TtOvertake::fSMeantFor().count(iOT) != 0) {
		return new TtOvertake(iArgs);
	}

	return NULL;
}

void AnalyserInfos::fInitializeInfos() {
	cNames.push_back(TryOut::fSGetName());
	cHelps.push_back(TryOut::fSGetHelp());
	cMeantFors.push_back(TryOut::fSMeantFor());

	cNames.push_back(TgUsp::fSGetName());
	cHelps.push_back(TgUsp::fSGetHelp());
	cMeantFors.push_back(TgUsp::fSMeantFor());

	cNames.push_back(TgConns::fSGetName());
	cHelps.push_back(TgConns::fSGetHelp());
	cMeantFors.push_back(TgConns::fSMeantFor());

	cNames.push_back(GrHDim::fSGetName());
	cHelps.push_back(GrHDim::fSGetHelp());
	cMeantFors.push_back(GrHDim::fSMeantFor());

	cNames.push_back(ToVarious::fSGetName());
	cHelps.push_back(ToVarious::fSGetHelp());
	cMeantFors.push_back(ToVarious::fSMeantFor());

	//----- UGs -----
	//Connectivity
	cNames.push_back(UgConnectivity::fSGetName());
	cHelps.push_back(UgConnectivity::fSGetHelp());
	cMeantFors.push_back(UgConnectivity::fSMeantFor());

	//Strong connectivity
	cNames.push_back(UgStrConn::fSGetName());
	cHelps.push_back(UgStrConn::fSGetHelp());
	cMeantFors.push_back(UgStrConn::fSMeantFor());

	//Degrees
	cNames.push_back(UgDegrees::fSGetName());
	cHelps.push_back(UgDegrees::fSGetHelp());
	cMeantFors.push_back(UgDegrees::fSMeantFor());

	//Paths
	cNames.push_back(UgPaths::fSGetName());
	cHelps.push_back(UgPaths::fSGetHelp());
	cMeantFors.push_back(UgPaths::fSMeantFor());

	//Betweenness
	cNames.push_back(UgBetw::fSGetName());
	cHelps.push_back(UgBetw::fSGetHelp());
	cMeantFors.push_back(UgBetw::fSMeantFor());

	cNames.push_back(UgAccNodes::fSGetName());
	cHelps.push_back(UgAccNodes::fSGetHelp());
	cMeantFors.push_back(UgAccNodes::fSMeantFor());

	cNames.push_back(UgDensity::fSGetName());
	cHelps.push_back(UgDensity::fSGetHelp());
	cMeantFors.push_back(UgDensity::fSMeantFor());

	//----- TEs -----
	cNames.push_back(TeBetw::fSGetName());
	cHelps.push_back(TeBetw::fSGetHelp());
	cMeantFors.push_back(TeBetw::fSMeantFor());

	cNames.push_back(TeConnectivity::fSGetName());
	cHelps.push_back(TeConnectivity::fSGetHelp());
	cMeantFors.push_back(TeConnectivity::fSMeantFor());

	//----- TDs -----

	//----- TTs -----
	//TtOvertake
	cNames.push_back(TtOvertake::fSGetName());
	cHelps.push_back(TtOvertake::fSGetHelp());
	cMeantFors.push_back(TtOvertake::fSMeantFor());
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

int AnalyserInfos::fGetIndexByName(string iAName, ObjType iOT) {
	for (vector<string>::size_type i = 0; i < cNames.size(); i++) {
		if (cNames[i] == iAName && cMeantFors[i].find(iOT) != cMeantFors[i].end()) {
			return i;
		}
	}

	return -1;
}
