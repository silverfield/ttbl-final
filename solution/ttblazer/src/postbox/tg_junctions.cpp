/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"
#include "../algusp.h"

#include "tg_junctions.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class - constructors
/*****************************************************************/
TgJunctions::TgJunctions(vector<string> iArguments) {
	cArgs = iArguments;

	cOptLoc = -1;

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

PostMail TgJunctions::fPostMail(Object *iObject, string iMailName) {
	TimeGraphObject *lGraph = dynamic_cast<TimeGraphObject*>(iObject);

	//----- compute -----
	INFO("Starting generating of junction cities mail" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	INFO("Creating underlying graph" << endl);
	UgGraph *lUgGraph = lGraph->fToUgGraph();
	lUgGraph->fComputeIdIndexMap();
	lGraph->fComputeIdIndexMap();

	INFO("Computing USPs" << endl);
	vector<int> lAllCities = fIdentityVector(lUgGraph->fGetN());
	vector<vector<vector<Path> > > *lUsps;
	lUsps = fUSP(lUgGraph, lGraph, lAllCities, lAllCities);

	//if we have a defined locality
	INFO("Computing junctions" << endl);
	vector<int> lJunctions;
	if (cOptLoc != -1) {
		lJunctions = fGetJunctions(lUsps, lAllCities, cOptLoc);
	}
	else {

	}

	//make vector of city names
	vector<string> *lStringCities = new vector<string>();
	for (size_t i = 0; i < lJunctions.size(); i++) {
		lStringCities->push_back(lUgGraph->fGetNodeName(lJunctions[i]));
	}

	INFO("Size of the junction set is " << lJunctions.size() << endl);

	//create the mail itself
	PostMail lMail(lStringCities, "Junction cities: " + fGetVectorString(*lStringCities, "\n	"),
			PB_TYPE_VECT_STRING);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating of junction cities mail" << endl);

	delete lUgGraph;

	return lMail;
}

string TgJunctions::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP TG Junction cities postman help: " << endl <<
			"	" << TG_JUNC_CMD_LOC << " {locality}: defines the sense of locality" << endl
			<< endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

vector<int> TgJunctions::fGetJunctions(vector<vector<vector<Path> > > *iUsps,
		vector<int> iAllCities, int iLocality) {
	int n = iAllCities.size();

	gLogger->fDebOn(true);

	//for each node -> paths it covers
	vector<unordered_set<Path*> > lCoveredPaths(n, unordered_set<Path*>());
	vector<Path> lLocalPieces;

	//compute local pieces
	INFO("Computing local pieces" << endl);
	int lTotalPieces = 0;
	for (size_t i = 0; i < (*iUsps).size(); i++) {
		for (size_t j = 0; j < (*iUsps)[i].size(); j++) {
			for (size_t k = 0; k < (*iUsps)[i][j].size(); k++) {
				if ((*iUsps)[i][j][k].fGetSize() > iLocality) {
					lTotalPieces += 2;
					vector<int> lIndices = (*iUsps)[i][j][k].fGetIndices();
					Path lLeftPiece(vector<int>(lIndices.begin(), lIndices.begin() + iLocality - 1));
					Path lRightPiece(vector<int>(lIndices.begin() + lIndices.size() - iLocality,
							lIndices.end()));

					lLocalPieces.push_back(lLeftPiece);
					lLocalPieces.push_back(lRightPiece);
				}
			}
			(*iUsps)[i][j].clear();
		}
		(*iUsps)[i].clear();
	}
	(*iUsps).clear();

	//initialize lCoveredPaths structure
	INFO("Initializing \"covered paths\" structure" << endl);
	for (size_t i = 0; i < lTotalPieces; i++) {
		Path *lPath = &(lLocalPieces[i]);
		for (int l = 0; l < lPath->fGetSize(); l++) {
			lCoveredPaths[lPath->fGet(l)].insert(lPath);
		}
	}

	//continue greedy
	INFO("Getting greedy" << endl);
	vector<int> lResult;
	int lCoveredCount = 0;
	while (lCoveredCount < lTotalPieces) {
		//get the node covering most paths
		int lBest = 0;
		for (size_t i = 1; i < lCoveredPaths.size(); i++) {
			if (lCoveredPaths[i].size() > lCoveredPaths[lBest].size()) {
				lBest = i;
			}
		}
		DEB(LVL_ALGS, "Algorithms::fGetJunctions: Node at the top of the queue: " << lBest << endl);
		DEB(LVL_ALGS, "Algorithms::fGetJunctions: Covers: " << lCoveredPaths[lBest].size() << endl);

		//update resulting cover
		lResult.push_back(lBest);

		//update the number of covered paths
		lCoveredCount += lCoveredPaths[lBest].size();

		//remove covered paths from the structure
		for (unordered_set<Path*>::iterator i = lCoveredPaths[lBest].begin(); i != lCoveredPaths[lBest].end(); i++) {
			Path *lPath = *i;
			for (int j = 0; j < lPath->fGetSize(); j++) {
				if (lPath->fGet(j) == lBest) {
					continue;
				}
				lCoveredPaths[lPath->fGet(j)].erase(lPath);
			}
		}

		//remove processed node from the structure
		lCoveredPaths[lBest] = unordered_set<Path*>();
	}
	gLogger->fDebOn(false);

	return lResult;
}

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TgJunctions::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TgJunctions::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TgJunctions::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(TG_JUNC_CMD_LOC) {
			MOVEARG;
			cOptLoc = fStrToInt(lCurArg);
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "TgJunctions::fProcessArgs: Arguments processed" << endl);
}
