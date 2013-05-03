/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>
#include <queue>

#include "../central.h"
#include "../algorithms.h"

#include "ug_locsep.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Class - constructors
/*****************************************************************/
UgLocsep::UgLocsep(vector<string> iArguments) {
	cArgs = iArguments;

	cOptMax = false;
	cMaxCoef = -1;
	cNeighCount = -1;
	cOptSetPm = "";

	fProcessArgs();
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

PostMail UgLocsep::fPostMail(Object *iObject, string iMailName) {
	UgGraph *lGraph = dynamic_cast<UgGraph*>(iObject);

	//----- compute -----
	INFO("Starting generating of UgLocsep AN set" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	if (cMaxCoef < 0) {
		cMaxCoef = MAX_COEF;
	}

	boost::unordered_set<int> lANs;

	if (cOptSetPm != "") {
		INFO("Getting initial AN set" << endl);

		PostMail *lPm = PostBox::fGetMail(cOptSetPm, PB_TYPE_VECT_STRING);
		if (lPm == NULL) {
			ERR("Mail not found!" << endl);
			return PostMail();
		}
		vector<string> lCities = *((vector<string>*)lPm->fGetData());
		for (size_t i = 0; i < lCities.size(); i++) {
			lANs.insert(lGraph->fGetNodeIndex(lCities[i]));
		}
	}

	//find the AN set
	lANs = fLocsep(lGraph, lANs, cOptMax);

	//make vector of city names
	vector<string> *lStringCities = new vector<string>();
	for (unordered_set<int>::iterator i = lANs.begin(); i != lANs.end(); i++) {
		lStringCities->push_back(lGraph->fGetNodeName(*i));
	}

	//create the mail itself
	PostMail lMail(lStringCities, "Resulting AN set: " + fGetVectorString(*lStringCities, "\n	"),
			PB_TYPE_VECT_STRING);

	INFO(endl << "Resulting access node set properties:" << endl);
	INFO(fStringDiagAnSet(lGraph, lANs, false));

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending generating of UgLocsep AN set" << endl);

	return lMail;
}

string UgLocsep::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP UgLocsep ANs postman help: " << endl <<
			"	" << UG_LOCSEP_CMD_MAX << ": all neighborhoods must have size under sqrt(n) * k"
					<< " (default is that average square should be under n)" << endl <<
			"	" << UG_LOCSEP_CMD_SET << " {pm}: given postmail is used as a initial AN set" << endl <<
			"	" << UG_LOCSEP_CMD_NEIGH << " {size} uses neighborhood of given size when inspecting "
					"ANs" << endl <<
			endl;

	return lSs.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

unordered_set<int> UgLocsep::fLocsep(UgGraph *iGraph, unordered_set<int> iInitSet, bool iMax) {
	INFO("Locsep algorithm started." << endl);

	//copy parameters
	cGraph = iGraph;
	cANs = iInitSet;
	cMax = iMax;

	cFNeighOk = false;
	cBNeighOk = false;
	cTuningPhase = false;

	INFO("Initial AN set: " << fGetSetString(cANs) << endl);

	int n = iGraph->fGetN();
	cReqSize = (int)sqrt(n);

	if (cNeighCount == -1) {
		cNeighCount = cReqSize;
	}

	cPotentials = vector<double>(n, 0);

	//prepare support structures
	INFO("Fetching reversed UG" << endl);
	cBackGraph = *cGraph;
	cBackGraph.fReverseOrientation();

	INFO("Fetching non-oriented UG" << endl);
	cNorientGraph = *cGraph;
	cNorientGraph.fRemoveOrientation();

	INFO("Fetching neighborhoods and LANs" << endl);
	for (int i = 0; i < cGraph->fGetN(); i++) {
		cFNeighs[i] = new unordered_set<int>();
		cFLans[i] = new unordered_set<int>();
		cBNeighs[i] = new unordered_set<int>();
		cBLans[i] = new unordered_set<int>();

		fUpdateNeighLans(i);
	}

	INFO("Getting potentials" << endl);
	for (int i = 0; i < cGraph->fGetN(); i++) {
		fUpdatePotential(i);
	}

	//algorithm
	INFO("\n");
	INFO("PHASE 1: Adding high-potential ANs till the requirement is satisfied *******" << endl);
	INFO("\n");

	bool lAllOk = fFinishCrit(iMax);
	while (lAllOk == false) {
		for (int i = 0; i < 10; i++) {
			//list diagnostic values
			fList(UG_LOCSEP_LIST_TOP);
			fParameters();

			//select next candidate for AN
			int lBest = fBestPotential();
			fSelect(lBest);

			//check finish criterion
			lAllOk = fFinishCrit(iMax);
			if (lAllOk) {
				break;
			}
		}

		if (lAllOk) {
			break;
		}

		for (int i = 0; i < 3; i++) {
			//list diagnostic values
			fList(UG_LOCSEP_LIST_TOP);
			fParameters();

			//select next candidate to remove from ANs
			int lWorst = fWorstPotential();
			fDeselect(lWorst);
		}
	}

	INFO("\n");
	INFO("PHASE 2: Removing unnecessary ANs **************************************" << endl);
	INFO("\n");

	int lWorst = 0;
	while (lAllOk) {
		//list diagnostic values
		fList(UG_LOCSEP_LIST_TOP);
		fParameters();

		//select next candidate to remove from ANs
		lWorst = fWorstPotential();
		if (lWorst == -1) {
			break;
		}
		fDeselect(lWorst);

		//check finish criterion
		lAllOk = fFinishCrit(iMax);
	}

	//list diagnostic values
	fList(UG_LOCSEP_LIST_TOP);
	fParameters();

	//need to add last deselected node back to AN set
	if (lWorst != -1) {
		fSelect(lWorst);
	}

	//list diagnostic values
	fList(UG_LOCSEP_LIST_TOP);
	fParameters();

	INFO("\n");
	INFO("PHASE 3: Tuning of the set ******************************" << endl);
	INFO("\n");

	cTuningPhase = true;
	double lLowestLans = cAnDiag.cAvgLANSize + cAnDiag.cAvgBackLANSize;
	unordered_set<int> lBestAnSet = cANs;
	for (int i = 0; i < sqrt(n); i++) {
		//select next candidate for AN
		int lBest = fBestPotential();
		fSelect(lBest);

		//list diagnostic values
		fList(UG_LOCSEP_LIST_TOP);
		fParameters();

		double lLans = cAnDiag.cAvgLANSize + cAnDiag.cAvgBackLANSize;
		if (lLans < lLowestLans) {
			lLowestLans = lLans;
			lBestAnSet = cANs;
		}
	}

	cANs = lBestAnSet;

	//free some previously allocated memory
	for (int i = 0; i < cGraph->fGetN(); i++) {
		delete cFNeighs[i];
		delete cFLans[i];
		delete cBNeighs[i];
		delete cBLans[i];
	}

	return cANs;
}

void UgLocsep::fSelect(int iX) {
	INFO("Selecting " << iX << " (" << cGraph->fGetNodeName(iX) << ")" << endl);

	if (fIsAn(iX)) {
		return;
	}

	//add to AN set
	cANs.insert(iX);

	//update reachable area
	fUpdateArea(iX);

	INFO("Node " << iX << " (" << cGraph->fGetNodeName(iX) << ") selected" << endl);
}

void UgLocsep::fDeselect(int iX) {
	INFO("Deselecting " << iX << " (" << cGraph->fGetNodeName(iX) << ")" << endl);

	if (fIsAn(iX) == false) {
		return;
	}

	//add to AN set
	cANs.erase(iX);

	//update reachable area
	fUpdateArea(iX);

	INFO("Node " << iX << " (" << cGraph->fGetNodeName(iX) << ") deselected" << endl);
}

void UgLocsep::fUpdateArea(int iX) {
	//get the set of influenced vertices
	unordered_set<int> lNeighs (cFNeighs[iX]->begin(), cFNeighs[iX]->end());
	lNeighs.insert(cBNeighs[iX]->begin(), cBNeighs[iX]->end());

	//re-compute neighborhood
	INFO("Re-computing neighborhoods and LANs" << endl);
	for (unordered_set<int>::iterator i = lNeighs.begin(); i != lNeighs.end(); i++) {
		fUpdateNeighLans(*i);
	}

	//update potentials
	INFO("Updating potentials" << endl);
	for (unordered_set<int>::iterator i = lNeighs.begin(); i != lNeighs.end(); i++) {
		fUpdatePotential(*i);
	}
}

void UgLocsep::fUpdateNeighLans(int iX) {
	fFUpdateNeighLans(iX);
	fBUpdateNeighLans(iX);
}

void UgLocsep::fFUpdateNeighLans(int iX) {
	*(cFLans[iX]) = fGetLANsForNode(cGraph, iX, cANs, cFNeighs[iX]);
}

void UgLocsep::fBUpdateNeighLans(int iX) {
	*(cBLans[iX]) = fGetLANsForNode(&cBackGraph, iX, cANs, cBNeighs[iX]);
}

void UgLocsep::fUpdatePotential(int iX) {
	cPotentials[iX] = fPotentialCount(iX, cNeighCount);
}

bool UgLocsep::fFinishCrit(bool iMax) {
	int lReqSize = sqrt(cGraph->fGetN());
	int lReqLan = 4;

	if (cANs.size() > lReqSize * 10) {
		INFO("Premature finish criterion due to large AN set" << endl);
		return true;
	}

	//front - neighs
	cFNeighOk = true;
	double lTotalF = 0;
	for (unordered_map<int, unordered_set<int>* >::iterator i = cFNeighs.begin();
			i != cFNeighs.end(); i++) {
		if (fIsAn(i->first)) {
			continue;
		}
		if (iMax && i->second->size() > lReqSize * cMaxCoef + 0.1) {
			cFNeighOk = false;
			return false;
		}
		lTotalF += i->second->size() * i->second->size();
	}

	lTotalF /= (cFNeighs.size() - cANs.size());
	if (!iMax && lTotalF > cGraph->fGetN()) {
		cFNeighOk = false;
		return false;
	}

	//front - lans
//	lTotalF = 0;
//	for (unordered_map<int, unordered_set<int>* >::iterator i = cFLans.begin();
//			i != cFLans.end(); i++) {
//		if (fIsAn(i->first)) {
//			continue;
//		}
//		if (i->second->size() > lReqLan * cMaxCoef && iMax) {
//			return false;
//		}
//		lTotalF += i->second->size() * i->second->size();
//	}
//
//	lTotalF /= (cFLans.size() - cANs.size());
//	if (!iMax && lTotalF > lReqLan * lReqLan) {
//		return false;
//	}

	//back - neigh
	double lTotalB = 0;
	cBNeighOk = true;
	for (unordered_map<int, unordered_set<int>* >::iterator i = cBNeighs.begin();
			i != cBNeighs.end(); i++) {
		if (fIsAn(i->first)) {
			continue;
		}
		if (iMax && i->second->size() > lReqSize * cMaxCoef + 0.1) {
			cBNeighOk = false;
			return false;
		}
		lTotalB += i->second->size() * i->second->size();
	}

	lTotalB /= (cBNeighs.size() - cANs.size());
	if (!iMax && lTotalB > cGraph->fGetN()) {
		cBNeighOk = false;
		return false;
	}

	//back - lans
//	lTotalB = 0;
//	for (unordered_map<int, unordered_set<int>* >::iterator i = cBLans.begin();
//			i != cBLans.end(); i++) {
//		if (fIsAn(i->first)) {
//			continue;
//		}
//		if (i->second->size() > lReqLan * cMaxCoef && iMax) {
//			return false;
//		}
//		lTotalB += i->second->size() * i->second->size();
//	}
//
//	lTotalB /= (cBLans.size() - cANs.size());
//	if (!iMax && lTotalB > lReqLan * lReqLan) {
//		return false;
//	}

	return true;
}

int UgLocsep::fBestPotential() {
	double lMax = -1;
	int lBest = -1;
	for (size_t i = 0; i < cPotentials.size(); i++) {
		if (cPotentials[i] > lMax && fIsAn(i) == false) {
			lMax = cPotentials[i];
			lBest = i;
		}
	}

	return lBest;
}

int UgLocsep::fWorstPotential() {
	double lMin = -1;
	int lBest = -1;
	for (size_t i = 0; i < cPotentials.size(); i++) {
		if ((lMin == -1 || cPotentials[i] < lMin) && fIsAn(i)) {
			lMin = cPotentials[i];
			lBest = i;
		}
	}

	return lBest;
}

int UgLocsep::fIntersecSize(unordered_set<int> *iX, unordered_set<int> *iY) {
	int lResult = 0;

	unordered_set<int> *lSmall;
	unordered_set<int> *lBig;
	if (iX->size() > iY->size()) {
		lBig = iX;
		lSmall = iY;
	}
	else {
		lBig = iY;
		lSmall = iX;
	}

	for (unordered_set<int>::iterator i = lSmall->begin(); i != lSmall->end(); i++) {
		lResult += (lBig->find(*i) != lBig->end());
	}

	return lResult;
}

int UgLocsep::fSubtractSize(unordered_set<int> *iX, unordered_set<int> *iY) {
	int lResult = 0;

	for (unordered_set<int>::iterator i = iY->begin(); i != iY->end(); i++) {
		lResult += (iX->find(*i) != iX->end());
	}

	return iX->size() - lResult;
}

double UgLocsep::fPotentialAvg(int iX, unordered_set<int> &iRestr) {
	gLogger->fInfoOn(false);

	//get front neighborhood within restricted area
	unordered_set<int> lFNeigh = fNeighRestr(cGraph, iX, &iRestr);

	//get back neighborhood within restricted area
	unordered_set<int> lBNeigh = fNeighRestr(&cBackGraph, iX, &iRestr);

	double lPotential = 0;

	//for all nodes in back neighbourhood (BN)
	for (unordered_set<int>::iterator i = lBNeigh.begin();
			i != lBNeigh.end(); i++) {
		int lSurplus = max(0, (int)cFNeighs[*i]->size() - cReqSize);
		if (lSurplus == 0) {
			continue;
		}

		//compute what is reachable...
		unordered_set<int> lReachable = fNeighRestr(cGraph, *i, &iRestr);

		//...how much of it is in FN
		int lSubtract = fSubtractSize(&lFNeigh, &lReachable);

		//increase potential by as much as we can decrease surplus
		if (cFNeighOk == false || (cTuningPhase && cAnDiag.cAvgNeighSize > cAnDiag.cAvgBackNeighSize)) {
			lPotential += min(lSubtract, lSurplus);
		}
	}

	//exactly the same thing for front neighbourhood, but in reversed UG
	for (unordered_set<int>::iterator i = lFNeigh.begin();
			i != lFNeigh.end(); i++) {
		int lSurplus = max(0, (int)cBNeighs[*i]->size() - cReqSize);
		if (lSurplus == 0) {
			continue;
		}

		unordered_set<int> lReachable = fNeighRestr(&cBackGraph, *i, &iRestr);

		int lSubtract = fSubtractSize(&lBNeigh, &lReachable);

		if (cBNeighOk == false || (cTuningPhase && cAnDiag.cAvgBackNeighSize > cAnDiag.cAvgNeighSize)) {
			lPotential += min(lSubtract, lSurplus);
		}
	}

	gLogger->fInfoOn(true);

	return lPotential;
}

double UgLocsep::fPotentialAvg2(int iX, unordered_set<int> &iRestr) {
	gLogger->fInfoOn(false);
	//INFO("X = " << iX << " " << cGraph->fGetNodeName(iX) << endl);
	////INFO("Restr is  " << fGetVectorString(fGetSorted(iRestr)) << endl);

	//get front neighborhoods
	unordered_set<int> lFNeigh = fNeighRestr(cGraph, iX, &iRestr);
	unordered_map<int, unordered_set<int> > lFNeighs;
	vector<int> lFTo = cGraph->fGetTo(iX);
	for (size_t i = 0; i < lFTo.size(); i++) {
		lFNeighs[lFTo[i]] = fArcNeighRestr(cGraph, iX, lFTo[i], &iRestr);
	}

	//get back neighborhoods
	unordered_set<int> lBNeigh = fNeighRestr(&cBackGraph, iX, &iRestr);
	unordered_map<int, unordered_set<int> > lBNeighs;
	vector<int> lBTo = cBackGraph.fGetTo(iX);
	for (size_t i = 0; i < lBTo.size(); i++) {
		lBNeighs[lBTo[i]] = fArcNeighRestr(&cBackGraph, iX, lBTo[i], &iRestr);
	}

	double lPotential = 0;

	////INFO("FNeigh is " << fGetVectorString(fGetSorted(lFNeigh)) << endl);

	//for all local back neighborhoods (LBN) of x
	for (unordered_map<int, unordered_set<int> >::iterator i = lBNeighs.begin();
			i != lBNeighs.end(); i++) {
		////INFO("   bneigh " << fGetVectorString(fGetSorted(i->second)) << endl);

		//those reachable from given LBN, not via x
		unordered_set<int> lReachable = fSetSearchRestr(cGraph, i->second, &iRestr);

		////INFO("       is " << fGetVectorString(fGetSorted(lReachable)) << endl);

		//get the number of vertices in x's front neighbourhood, that were not reached
		int lSubtract = fSubtractSize(&lFNeigh, &lReachable);

		//INFO("       Subtract: " << lSubtract << endl);

		//check front neighbourhood sizes for all vertices in given LBN
		vector<pair<int, int> > lFNeighSizes = fGetFNeighSizes(i->second);
		for (size_t j = 0; j < lFNeighSizes.size(); j++) {
			//we are interested only in those with too large front neighbourhood (> sqrt(n))
			if (lFNeighSizes[j].second < cReqSize) {
				break;
			}

			//update the potential with the useful subtraction from given front neighbourhood
			lPotential += min(lSubtract, lFNeighSizes[j].second - cReqSize);
		}
	}

	////INFO("BNeigh is " << fGetVectorString(fGetSorted(lBNeigh)) << endl);

	//exactly the same thing, but in reversed UG
	for (unordered_map<int, unordered_set<int> >::iterator i = lFNeighs.begin();
			i != lFNeighs.end(); i++) {
		////INFO("   fneigh " << fGetVectorString(fGetSorted(i->second)) << endl);

		//those reachable from given from-neighborhood, not via iX
		unordered_set<int> lReachable = fSetSearchRestr(&cBackGraph, i->second, &iRestr);

		////INFO("    reach " << fGetVectorString(fGetSorted(lReachable)) << endl);

		//B-neigh - lReachable should be as big as possible
		int lSubtract = fSubtractSize(&lBNeigh, &lReachable);

		//INFO("       Subtract: " << lSubtract << endl);

		vector<pair<int, int> > lBNeighSizes = fGetBNeighSizes(i->second);
		for (size_t j = 0; j < lBNeighSizes.size(); j++) {
			if (lBNeighSizes[j].second < cReqSize) {
				break;
			}

			lPotential += min(lSubtract, lBNeighSizes[j].second - cReqSize);
		}
	}

	//INFO("       POTENTIAL: " << lPotential << endl);

	gLogger->fInfoOn(true);

	return lPotential;
}

double UgLocsep::fPotentialMax(int iX, unordered_set<int> &iRestr) {
	return fPotentialAvg(iX, iRestr);
}

double UgLocsep::fPotentialCount(int iX, int iCount) {
	unordered_set<int> lNeigh = fNeigh(&cNorientGraph, iX, iCount);

	double lPotential;
	if (cOptMax) {
		lPotential = fPotentialMax(iX, lNeigh);
	}
	else {
		lPotential = fPotentialAvg(iX, lNeigh);
	}

	return lPotential;
}

unordered_set<int> UgLocsep::fNeigh(UgGraph *iGraph, int iX, int iCount) {
	unordered_set<int> lNeigh;
	std::queue<int> lQueue;

	lQueue.push(iX);
	lNeigh.insert(iX);

	//breadth first search up to count iCount
	while (lQueue.empty() == false && lNeigh.size() < iCount + 1) {
		int lCurrent = lQueue.front();
		lQueue.pop();

		//process neighbors
		vector<int> lTo = iGraph->fGetTo(lCurrent);
		for (size_t i = 0; i < lTo.size(); i++) {
			if (cANs.find(lTo[i]) != cANs.end() ||
				lNeigh.find(lTo[i]) != lNeigh.end()) {
				continue;
			}

			lNeigh.insert(lTo[i]);
			if (lNeigh.size() >= iCount + 1) {
				break;
			}

			lQueue.push(lTo[i]);
		}
	}

	lNeigh.erase(iX);

	return lNeigh;
}

unordered_set<int> UgLocsep::fNeighRestr(UgGraph *iGraph, int iX, unordered_set<int> *iRestr) {
	unordered_set<int> lNeigh;
	std::queue<int> lQueue;

	lQueue.push(iX);
	lNeigh.insert(iX);

	//breadth first search
	while (lQueue.empty() == false) {
		int lCurrent = lQueue.front();
		lQueue.pop();

		//process neighbors
		vector<int> lTo = iGraph->fGetTo(lCurrent);
		for (size_t i = 0; i < lTo.size(); i++) {
			if (cANs.find(lTo[i]) != cANs.end() ||
				lNeigh.find(lTo[i]) != lNeigh.end() ||
				iRestr->find(lTo[i]) == iRestr->end()) {
				continue;
			}

			lNeigh.insert(lTo[i]);
			lQueue.push(lTo[i]);
		}
	}

	lNeigh.erase(iX);

	return lNeigh;
}

unordered_set<int> UgLocsep::fArcNeighRestr(UgGraph *iGraph, int iX, int iTo, unordered_set<int> *iRestr) {
	unordered_set<int> lNeigh;
	std::queue<int> lQueue;

	if (iGraph->fArcExists(iX, iTo) == false) {
		ERR("UgLocsep::fArcNeighRestr: " << iX << " to " << iTo << " arc does not exist!" << endl);
		return lNeigh;
	}

	lQueue.push(iTo);
	lNeigh.insert(iX);
	lNeigh.insert(iTo);

	//breadth first search
	while (lQueue.empty() == false) {
		int lCurrent = lQueue.front();
		lQueue.pop();

		//process neighbors
		vector<int> lTo = iGraph->fGetTo(lCurrent);
		for (size_t i = 0; i < lTo.size(); i++) {
			if (cANs.find(lTo[i]) != cANs.end() ||
				lNeigh.find(lTo[i]) != lNeigh.end() ||
				iRestr->find(lTo[i]) == iRestr->end()) {
				continue;
			}

			lNeigh.insert(lTo[i]);
			lQueue.push(lTo[i]);
		}
	}

	lNeigh.erase(iX);

	return lNeigh;
}

unordered_set<int> UgLocsep::fSetSearchRestr(UgGraph *iGraph, unordered_set<int> &iX,
		unordered_set<int> *iRestr) {
	unordered_set<int> lNeigh = iX;
	std::queue<int> lQueue;
	for (unordered_set<int>::iterator i = iX.begin(); i != iX.end(); i++) {
		lQueue.push(*i);
	}

	//breadth first search
	while (lQueue.empty() == false) {
		int lCurrent = lQueue.front();
		lQueue.pop();

		//process neighbors
		vector<int> lTo = iGraph->fGetTo(lCurrent);
		for (size_t i = 0; i < lTo.size(); i++) {
			if (cANs.find(lTo[i]) != cANs.end() ||
				lNeigh.find(lTo[i]) != lNeigh.end() ||
				iRestr->find(lTo[i]) == iRestr->end()) {
				continue;
			}

			lNeigh.insert(lTo[i]);
			lQueue.push(lTo[i]);
		}
	}

	return lNeigh;
}

int UgLocsep::fGetMaxFNeighSize(boost::unordered_set<int> &iX) {
	int lMax = 0;

	for (unordered_set<int>::iterator i = iX.begin(); i != iX.end(); i++) {
		lMax = max(lMax, (int)cFNeighs[*i]->size());
	}

	return lMax;
}

int UgLocsep::fGetMaxBNeighSize(boost::unordered_set<int> &iX) {
	int lMax = 0;

	for (unordered_set<int>::iterator i = iX.begin(); i != iX.end(); i++) {
		lMax = max(lMax, (int)cBNeighs[*i]->size());
	}

	return lMax;
}

vector<pair<int, int> > UgLocsep::fGetFNeighSizes(unordered_set<int> &iX) {
	vector<pair<int, int> > lFNeighSizes;

	for (unordered_set<int>::iterator i = iX.begin(); i != iX.end(); i++) {
		lFNeighSizes.push_back(pair<int, int>(*i, cFNeighs[*i]->size()));
	}

	sort(lFNeighSizes.begin(), lFNeighSizes.end(), fCompNeighSize);

	return lFNeighSizes;
}

vector<pair<int, int> > UgLocsep::fGetBNeighSizes(unordered_set<int> &iX) {
	vector<pair<int, int> > lBNeighSizes;

	for (unordered_set<int>::iterator i = iX.begin(); i != iX.end(); i++) {
		lBNeighSizes.push_back(pair<int, int>(*i, cFNeighs[*i]->size()));
	}

	sort(lBNeighSizes.begin(), lBNeighSizes.end(), fCompNeighSize);

	return lBNeighSizes;
}

void UgLocsep::fList(int iTop) {
	int lWidth = 10;

	vector<double> lPotentials = cPotentials;
	vector<int> lTransl = fSortVector(&lPotentials, true);

	int lFrom = 0;
	if (iTop > 0) {
		lFrom = max(0, cGraph->fGetN() - iTop);
	}

	INFO("Listing diagnostic values" << endl);
	for (int i = lFrom; i < cGraph->fGetN(); i++) {
		int j = lTransl[i];
		string lIsAn = fIsAn(j) ? "AN" : "normal";
		INFO("	" << fPadString(fIntToStr(j), 5) << " " <<
				fPadString(cGraph->fGetNodeName(j), 50) << " " <<
				fPadString(lIsAn, lWidth) <<
				fPadString("P " + fDoubleToStr(cPotentials[j]), lWidth) <<
				fPadString("FN " + fIntToStr(cFNeighs[j]->size()), lWidth) <<
				fPadString("FL " + fIntToStr(cFLans[j]->size()), lWidth) <<
				fPadString("BN " + fIntToStr(cBNeighs[j]->size()), lWidth) <<
				fPadString("BL " + fIntToStr(cBLans[j]->size()), lWidth) <<
				endl);
	}
	INFO("\n");
}

void UgLocsep::fParameters() {
	int lReqSize = sqrt(cGraph->fGetN());

	cAnDiag = AnDiag();

	//front
	cAnDiag.cMaxNeighSize = 0;
	cAnDiag.cAvgNeighSize = 0;
	for (unordered_map<int, unordered_set<int>* >::iterator i = cFNeighs.begin();
			i != cFNeighs.end(); i++) {
		if (fIsAn(i->first)) {
			continue;
		}
		if (i->second->size() > cAnDiag.cMaxNeighSize) {
			cAnDiag.cMaxNeighSize = i->second->size();
		}

		cAnDiag.cAvgNeighSize += i->second->size() * i->second->size();
	}
	cAnDiag.cAvgNeighSize /= (cFNeighs.size() - cANs.size());

	cAnDiag.cMaxLANSize = 0;
	cAnDiag.cAvgLANSize = 0;
	for (unordered_map<int, unordered_set<int>* >::iterator i = cFLans.begin();
			i != cFLans.end(); i++) {
		if (fIsAn(i->first)) {
			continue;
		}
		if (i->second->size() > cAnDiag.cMaxLANSize) {
			cAnDiag.cMaxLANSize = i->second->size();
		}

		cAnDiag.cAvgLANSize += i->second->size() * i->second->size();
	}
	cAnDiag.cAvgLANSize /= (cFLans.size() - cANs.size());


	//back
	cAnDiag.cMaxBackNeighSize = 0;
	cAnDiag.cAvgBackNeighSize = 0;
	for (unordered_map<int, unordered_set<int>* >::iterator i = cBNeighs.begin();
			i != cBNeighs.end(); i++) {
		if (fIsAn(i->first)) {
			continue;
		}
		if (i->second->size() > cAnDiag.cMaxBackNeighSize) {
			cAnDiag.cMaxBackNeighSize = i->second->size();
		}

		cAnDiag.cAvgBackNeighSize += i->second->size() * i->second->size();
	}
	cAnDiag.cAvgBackNeighSize /= (cBNeighs.size() - cANs.size());

	cAnDiag.cMaxBackLANSize = 0;
	cAnDiag.cAvgBackLANSize = 0;
	for (unordered_map<int, unordered_set<int>* >::iterator i = cBLans.begin();
			i != cBLans.end(); i++) {
		if (fIsAn(i->first)) {
			continue;
		}
		if (i->second->size() > cAnDiag.cMaxBackLANSize) {
			cAnDiag.cMaxBackLANSize = i->second->size();
		}

		cAnDiag.cAvgBackLANSize += i->second->size() * i->second->size();
	}
	cAnDiag.cAvgBackLANSize /= (cBLans.size() - cANs.size());

	string lReqSizeString;
	if (cOptMax) {
		lReqSizeString = "MAX " + fIntToStr(lReqSize * cMaxCoef);
	}
	else {
		lReqSizeString = "AVG SQUARE " + fIntToStr(cGraph->fGetN());
	}
	INFO(endl <<
			"SQRT(n): " << lReqSize << endl <<
			"AN set size: " << cANs.size() << endl <<
			"REQUIRED NEIGH. size: " << lReqSizeString << endl <<
			cAnDiag.fGetString() << endl);
}

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("UgLocsep::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("UgLocsep::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void UgLocsep::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(UG_LOCSEP_CMD_MAX) {
			MOVEARG;
			cMaxCoef = fStrToDouble(lCurArg);
			cOptMax = true;
		}
		else COMP(UG_LOCSEP_CMD_SET) {
			MOVEARG;
			cOptSetPm = lCurArg;
		}
		else COMP(UG_LOCSEP_CMD_NEIGH) {
			MOVEARG;
			cNeighCount = fStrToInt(lCurArg);
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "UgLocsep::fProcessArgs: Arguments processed" << endl);
}
