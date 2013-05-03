/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "ug_highdeg.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

vector<int> UgHighDeg::cDegs;

/*****************************************************************/
/* Class - constructors
/*****************************************************************/
UgHighDeg::UgHighDeg(vector<string> iArguments) {
	cArgs = iArguments;

	cThreshold = 0;
	cLimit = -1;

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

PostMail UgHighDeg::fPostMail(Object *iObject, string iMailName) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	//----- compute -----
	INFO("Starting generating of high-degree cities mail" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	INFO("Getting reversed graph" << endl);
	UgGraph lReversed = *lGraph;
	lReversed.fReverseOrientation();

	INFO("Computing degrees" << endl);
	vector<int> lNormalDegs = lGraph->fGetDegrees();
	vector<int> lRevDegs = lReversed.fGetDegrees();
	cDegs = vector<int>(lGraph->fGetN(), 0);

	for (size_t i = 0; i < lNormalDegs.size(); i++) {
		cDegs[i] = lNormalDegs[i] + lRevDegs[i];
	}

	vector<int> lCities;
	for (int i = 0; i < lGraph->fGetN(); i++) {
		lCities.push_back(i);
	}

	//sort by degree
	sort(lCities.begin(), lCities.end(), fDegsComp);

	//take only first "limit" cities
	if (cLimit > -1) {
		int lLimit = min((int)lCities.size(), cLimit);
		lCities.resize(lLimit);
	}

	//make vector of city names
	vector<string> *lStringCities = new vector<string>();
	for (size_t i = 0; i < lCities.size(); i++) {
		lStringCities->push_back(lGraph->fGetNodeName(lCities[i]));
	}

	//create the mail itself
	PostMail lMail(lStringCities, "High-DEG cities: " + fGetVectorString(*lStringCities, "\n	"),
			PB_TYPE_VECT_STRING);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating of high-degree cities mail" << endl);

	return lMail;
}


string UgHighDeg::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UG High-DEG cities postman help: " << endl <<
			"	-t {threshold}: nodes with degree under this value will not go into "
				"result (default 0)" << endl <<
			"	-l {limit}: maximum of limit nodes will go into the result" << endl
			<< endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgHighDeg::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgHighDeg::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgHighDeg::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP("-t") {
			MOVEARG;
			cThreshold = fStrToInt(lCurArg);
		}
		else COMP("-l") {
			MOVEARG;
			cLimit = fStrToInt(lCurArg);
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "UgHighDeg::fProcessArgs: Arguments processed" << endl);
}
