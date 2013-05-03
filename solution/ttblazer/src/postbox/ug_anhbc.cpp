/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>

#include "../central.h"
#include "../algorithms.h"

#include "ug_anhbc.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Variables
/*****************************************************************/

vector<double> UgAnHBc::cBcs;

/*****************************************************************/
/* Class - constructors
/*****************************************************************/
UgAnHBc::UgAnHBc(vector<string> iArguments) {
	cArgs = iArguments;

	cOptionsMax = false;
	cMaxCoef = -1;
	cApxPrec = -1;

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

PostMail UgAnHBc::fPostMail(Object *iObject, string iMailName) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	//----- compute -----
	INFO("Starting generating of high-BC AN set" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	if (cMaxCoef < 0) {
		cMaxCoef = MAX_COEF;
	}

	INFO("Checking if graph is fully weighted" << endl);
	bool lHasWeights = true;
	if (lGraph->fIsWeighted() == false) {
		INFO("Graph is not fully weighted, using length \"1\" for each arc" << endl);
		lHasWeights = false;
	}

	INFO("Computing betweenness values" << endl);
	if (cApxPrec > 0) {
		cBcs = fApxBetweenness(lGraph, NULL, cApxPrec, lHasWeights);
	}
	else {
		cBcs = fBetweenness(lGraph, NULL, lHasWeights);
	}

	vector<int> lCities;
	for (int i = 0; i < lGraph->fGetN(); i++) {
		lCities.push_back(i);
	}

	//sort by BC
	sort(lCities.begin(), lCities.end(), fBcsComp);

	INFO("Going to find the right AN set size" << endl);
	int lReqNeighSize = sqrt(lGraph->fGetN());
	int lUpSize = lReqNeighSize;

	//find first the correct interval where the optimal AN size is
	INFO("PHASE 1 ---------------------------------" << endl);
	for (lUpSize = lReqNeighSize; lUpSize <= lGraph->fGetN(); lUpSize += lReqNeighSize) {
		//make set
		unordered_set<int> lANs (lCities.begin(), lCities.begin() + lUpSize);
		AnDiag lDiag = fDiagnoseAnSet(lGraph, lANs);
		INFO(endl << "For size " << lUpSize << " AN set has following properties: " << endl <<
				lDiag.fGetString() << endl << endl);

		if (fNeighOk(lDiag, lReqNeighSize, lGraph->fGetN()) == true) {
			break;
		}
	}
	lUpSize = min((int)lCities.size(), lUpSize);
	INFO("Optimal size <= " << lUpSize << " but > " << lUpSize - lReqNeighSize << endl);

	//find within this interval the optimal AN size
	INFO("PHASE 2 ---------------------------------" << endl);
	int lSize = lUpSize;
	for (lSize = lUpSize; lSize >= lUpSize - lReqNeighSize; lSize--) {
		//make set
		unordered_set<int> lANs (lCities.begin(), lCities.begin() + lSize);
		AnDiag lDiag = fDiagnoseAnSet(lGraph, lANs);
		INFO(endl << "For size " << lSize << " AN set has following properties: " << endl <<
				lDiag.fGetString() << endl << endl);

		if (fNeighOk(lDiag, lReqNeighSize, lGraph->fGetN()) == false) {
			break;
		}
	}
	lSize++;
	INFO("Optimal size = " << lSize << endl);

	//make vector of city names
	vector<string> *lStringCities = new vector<string>();
	for (size_t i = 0; i < lSize; i++) {
		lStringCities->push_back(lGraph->fGetNodeName(lCities[i]));
	}
	unordered_set<int> lANs (lCities.begin(), lCities.begin() + lSize);

	//create the mail itself
	PostMail lMail(lStringCities, "High-BC AN set: " + fGetVectorString(*lStringCities, "\n	"),
			PB_TYPE_VECT_STRING);

	INFO(endl << "Resulting access node set properties:" << endl);
	INFO(fStringDiagAnSet(lGraph, lANs, false));

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating of high-BC AN set" << endl);

	return lMail;
}

string UgAnHBc::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UG High-BC ANs postman help: " << endl <<
			"	" << UG_AN_HBC_CMD_MAX << " {k}: all neighborhoods must have size under sqrt(n) * k"
					<< " (default is that average square should be under n)" << endl
			<< endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

bool UgAnHBc::fNeighOk(AnDiag iDiag, int iReqSize, int iN) {
	if (cOptionsMax == false) {
		if (iDiag.cAvgNeighSize <= iN &&
			iDiag.cAvgBackNeighSize <= iN) {
			return true;
		}
	}
	else {
		if (iDiag.cMaxNeighSize <= iReqSize &&
			iDiag.cMaxBackNeighSize <= iReqSize) {
			return true;
		}
	}

	return false;
}

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgAnHBc::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgAnHBc::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgAnHBc::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(UG_AN_HBC_CMD_MAX) {
			MOVEARG;
			cMaxCoef = fStrToDouble(lCurArg);
			cOptionsMax = true;
		}
		else COMP(UG_AN_HBC_CMD_APX) {
			MOVEARG;
			cApxPrec = fStrToDouble(lCurArg);
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "UgAnHBc::fProcessArgs: Arguments processed" << endl);
}
