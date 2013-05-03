/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "ug_highbc.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

vector<double> UgHighBc::cBetw;

/*****************************************************************/
/* Class - constructors
/*****************************************************************/
UgHighBc::UgHighBc(vector<string> iArguments) {
	cArgs = iArguments;

	cThreshold = 0;
	cLimit = -1;
	cApxPrec = -1;

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

PostMail UgHighBc::fPostMail(Object *iObject, string iMailName) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	//----- compute -----
	INFO("Starting generating of high-BC cities mail" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	INFO("Checking if graph is fully weighted" << endl);
	bool lHasWeights = true;
	if (lGraph->fIsWeighted() == false) {
		INFO("Graph is not fully weighted, using length \"1\" for each arc" << endl);
		lHasWeights = false;
	}

	INFO("Computing betweenness values" << endl);
	if (cApxPrec > 0) {
		cBetw = fApxBetweenness(lGraph, NULL, cApxPrec, lHasWeights);
	}
	else {
		cBetw = fBetweenness(lGraph, NULL, lHasWeights);
	}

	INFO("Building mail" << endl);
	vector<int> lCities;
	for (int i = 0; i < lGraph->fGetN(); i++) {
		if (cBetw[i] > cThreshold) {
			lCities.push_back(i);
		}
	}

	//sort by degree
	sort(lCities.begin(), lCities.end(), fBetwsComp);

	//take only first "limit" cities
	if (cLimit > -1) {
		int lLimit = min((int)lCities.size(), cLimit);
		lCities.resize(lLimit);
	}

	//make vector of city names
	vector<string> *lStringCities = new vector<string>;
	for (size_t i = 0; i < lCities.size(); i++) {
		lStringCities->push_back(lGraph->fGetNodeName(lCities[i]));
	}

	//create the mail itself
	PostMail lMail(lStringCities, "High-BC cities: " + fGetVectorString(*lStringCities, "\n	"),
			PB_TYPE_VECT_STRING);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating of high-BC cities mail" << endl);

	return lMail;
}


string UgHighBc::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UG High-BC cities postman help: " << endl <<
			"	-t {threshold}: nodes with betweenness centrality under this value will not go into "
				"result (default 0)" << endl <<
			"	-apx {precision}: uses approximate betw. values only"
			"	-l {limit}: maximum of limit nodes will go into the result" << endl
			<< endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgHighBc::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgHighBc::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgHighBc::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP("-t") {
			MOVEARG;
			cThreshold = fStrToDouble(lCurArg);
		}
		else COMP("-l") {
			MOVEARG;
			cLimit = fStrToInt(lCurArg);
		}
		else COMP("-apx") {
			MOVEARG;
			cApxPrec = fStrToDouble(lCurArg);
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "UgHighBc::fProcessArgs: Arguments processed" << endl);
}
