/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "ug_dijkstra.h"
#include "tg_dijkstra.h"
#include "td_dijkstra.h"
#include "tg_neural.h"
#include "td_uspor.h"
#include "td_uspora.h"
#include "td_usporaseg.h"
#include "td_usporseg.h"
#include "td_uspora3.h"
#include "tg_neuralea.h"
#include "tg_neuralrout.h"

#include "oracleinfos.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

vector<std::string> OracleInfos::cNames;
vector<std::string> OracleInfos::cHelps;
vector<set<ObjType> > OracleInfos::cMeantFors;
vector<set<QueryType> > OracleInfos::cQueryTypes;

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

/*****************************************************************/
/* Class - interface
/*****************************************************************/

vector<string> OracleInfos::fGetNames() {
	return cNames;
}

vector<set<ObjType> > OracleInfos::fGetMeantFors() {
	return cMeantFors;
}

string OracleInfos::fGetHelp(string iDoName, ObjType iOT) {
	int lIndex = fGetIndexByName(iDoName, iOT);
	if (lIndex == -1) {
		return "";
	}

	return cHelps[lIndex];
}

set<ObjType> OracleInfos::fGetMeantFor(string iDoName, ObjType iOT) {
	int lIndex = fGetIndexByName(iDoName, iOT);
	if (lIndex == -1) {
		return set<ObjType>();
	}

	return cMeantFors[lIndex];
}

bool OracleInfos::fAnwers(string iDoName, ObjType iOT, QueryType iQT) {
	int lIndex = fGetIndexByName(iDoName, iOT);
	if (lIndex == -1) {
		return "";
	}

	return (cQueryTypes[lIndex].find(iQT) != cQueryTypes[lIndex].end());
}

set<QueryType> OracleInfos::fGetQueryTypes(string iDoName, ObjType iOT) {
	int lIndex = fGetIndexByName(iDoName, iOT);
	if (lIndex == -1) {
		return set<QueryType>();
	}

	return cQueryTypes[lIndex];
}

Oracle* OracleInfos::fCreateOracle(string iDoName, ObjType iOT, vector<string> iArgs) {
	if (iDoName == TgNeural::fSGetName() && TgNeural::fSMeantFor().count(iOT) != 0) {
		return new TgNeural(iArgs);
	}
	if (iDoName == TgNeuralEa::fSGetName() && TgNeuralEa::fSMeantFor().count(iOT) != 0) {
		return new TgNeuralEa(iArgs);
	}
	if (iDoName == TgNeuralRout::fSGetName() && TgNeuralRout::fSMeantFor().count(iOT) != 0) {
		return new TgNeuralRout(iArgs);
	}
	if (iDoName == TdUspor::fSGetName() && TdUspor::fSMeantFor().count(iOT) != 0) {
		return new TdUspor(iArgs);
	}
	if (iDoName == TdUsporSeg::fSGetName() && TdUsporSeg::fSMeantFor().count(iOT) != 0) {
		return new TdUsporSeg(iArgs);
	}
	if (iDoName == TdUspora::fSGetName() && TdUspora::fSMeantFor().count(iOT) != 0) {
		return new TdUspora(iArgs);
	}
	if (iDoName == TdUsporaSeg::fSGetName() && TdUsporaSeg::fSMeantFor().count(iOT) != 0) {
		return new TdUsporaSeg(iArgs);
	}
	if (iDoName == TdUspora3::fSGetName() && TdUspora3::fSMeantFor().count(iOT) != 0) {
		return new TdUspora3(iArgs);
	}
	if (iDoName == TgDijkstra::fSGetName() && TgDijkstra::fSMeantFor().count(iOT) != 0) {
		return new TgDijkstra(iArgs);
	}
	if (iDoName == TdDijkstra::fSGetName() && TdDijkstra::fSMeantFor().count(iOT) != 0) {
		return new TdDijkstra(iArgs);
	}

	//----- UGs -----
	if (iDoName == UgDijkstra::fSGetName() && UgDijkstra::fSMeantFor().count(iOT) != 0) {
		return new UgDijkstra(iArgs);
	}

	//----- TEs -----

	//----- TDs -----

	//----- TTs -----

	return NULL;
}

void OracleInfos::fInitializeInfos() {
	//neural network
	cNames.push_back(TgNeural::fSGetName());
	cHelps.push_back(TgNeural::fSGetHelp());
	cMeantFors.push_back(TgNeural::fSMeantFor());
	cQueryTypes.push_back(TgNeural::fSQueryTypes());

	//neural network (EA only)
	cNames.push_back(TgNeuralEa::fSGetName());
	cHelps.push_back(TgNeuralEa::fSGetHelp());
	cMeantFors.push_back(TgNeuralEa::fSMeantFor());
	cQueryTypes.push_back(TgNeuralEa::fSQueryTypes());

	cNames.push_back(TgNeuralRout::fSGetName());
	cHelps.push_back(TgNeuralRout::fSGetHelp());
	cMeantFors.push_back(TgNeuralRout::fSMeantFor());
	cQueryTypes.push_back(TgNeuralRout::fSQueryTypes());

	cNames.push_back(TdUspor::fSGetName());
	cHelps.push_back(TdUspor::fSGetHelp());
	cMeantFors.push_back(TdUspor::fSMeantFor());
	cQueryTypes.push_back(TdUspor::fSQueryTypes());

	cNames.push_back(TdUsporSeg::fSGetName());
	cHelps.push_back(TdUsporSeg::fSGetHelp());
	cMeantFors.push_back(TdUsporSeg::fSMeantFor());
	cQueryTypes.push_back(TdUsporSeg::fSQueryTypes());

	cNames.push_back(TdUspora::fSGetName());
	cHelps.push_back(TdUspora::fSGetHelp());
	cMeantFors.push_back(TdUspora::fSMeantFor());
	cQueryTypes.push_back(TdUspora::fSQueryTypes());

	cNames.push_back(TdUsporaSeg::fSGetName());
	cHelps.push_back(TdUsporaSeg::fSGetHelp());
	cMeantFors.push_back(TdUsporaSeg::fSMeantFor());
	cQueryTypes.push_back(TdUsporaSeg::fSQueryTypes());

	cNames.push_back(TdUspora3::fSGetName());
	cHelps.push_back(TdUspora3::fSGetHelp());
	cMeantFors.push_back(TdUspora3::fSMeantFor());
	cQueryTypes.push_back(TdUspora3::fSQueryTypes());

	cNames.push_back(TgDijkstra::fSGetName());
	cHelps.push_back(TgDijkstra::fSGetHelp());
	cMeantFors.push_back(TgDijkstra::fSMeantFor());
	cQueryTypes.push_back(TgDijkstra::fSQueryTypes());

	cNames.push_back(TdDijkstra::fSGetName());
	cHelps.push_back(TdDijkstra::fSGetHelp());
	cMeantFors.push_back(TdDijkstra::fSMeantFor());
	cQueryTypes.push_back(TdDijkstra::fSQueryTypes());

	//----- UGs -----
	//dijkstra
	cNames.push_back(UgDijkstra::fSGetName());
	cHelps.push_back(UgDijkstra::fSGetHelp());
	cMeantFors.push_back(UgDijkstra::fSMeantFor());
	cQueryTypes.push_back(UgDijkstra::fSQueryTypes());

	//----- TEs -----

	//----- TD -----

	//----- TTs -----
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

int OracleInfos::fGetIndexByName(string iDoName, ObjType iOT) {
	for (vector<string>::size_type i = 0; i < cNames.size(); i++) {
		if (cNames[i] == iDoName && cMeantFors[i].find(iOT) != cMeantFors[i].end()) {
			return i;
		}
	}

	return -1;
}
