/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"

#include "algaccn.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Functions
/*****************************************************************/

/*
 * Searches neighborhood of the graph from the node iNode. Neighborhood are all nodes reachable NOT
 * via access nodes (local access nodes still belong to the neighborhood) . This is a recursive function
 * that returns via output parameters:
 * - set of neighborhood nodes
 * - local access nodes for the neighborhood
 *
 * The time complexity is simply the number of arcs encountered in the neighborhood
 */
void fSearchNeighborhood(UgGraph *iGraph, int iNode, unordered_set<int> &iANs,
		unordered_set<int> *oLANs, unordered_set<int> *oNeighborhood) {
	//mark the node as being in the neighborhood
	oNeighborhood->insert(iNode);

	//if this is the access node, add it to the set of local access nodes and do not continue search
	if (iANs.count(iNode) != 0) {
		oLANs->insert(iNode);
		return;
	}

	//continue the search from the neighbors
	vector<int> lTo = iGraph->fGetTo(iNode);
	for (size_t i = 0; i < lTo.size(); i++) {
		//search only in not-yet visited nodes
		if (oNeighborhood->count(lTo[i]) == 0) {
			fSearchNeighborhood(iGraph, lTo[i], iANs, oLANs, oNeighborhood);
		}
	}
}

/*
 * The function returns the vector of local access nodes for the specific node (iFrom) given
 * a set of access nodes. A neighborhood of the node is also returned via output parameter.
 * For an access node, a set of local access nodes and its neighborhood is also returned.
 *
 * Local access nodes for a node x is the smallest set of access nodes such that in
 * order to reach the remaining of the graph, we must go through at least one of these nodes
 *
 * A neighborhood are all nodes reachable NOT via access nodes. Thus the starting node as well
 * as local access nodes are part of this neighborhood.
 */
unordered_set<int> fGetLANsForNode(UgGraph *iGraph, int iFrom, unordered_set<int> &iANs,
		unordered_set<int> *oNeighborhood) {
	unordered_set<int> lLANs;

	//we need to have a valid neighborhood set for fSearchNeighborhood
	unordered_set<int> *lNeighborhood;
	if (oNeighborhood == NULL) {
		lNeighborhood = new unordered_set<int>();
	}
	else {
		oNeighborhood->clear();
		lNeighborhood = oNeighborhood;
	}

	//we want to find access nodes also for access nodes
	//thus erase iFrom from the set of ANs as the search would stop immediately
	bool lErased = false;
	if (iANs.count(iFrom) != 0) {
		iANs.erase(iFrom);
		lErased = true;
	}

	//recursive searching in the neighborhood - up to access nodes
	fSearchNeighborhood(iGraph, iFrom, iANs, &lLANs, lNeighborhood);

	//put iFrom back to the set of access nodes
	if (lErased) {
		iANs.insert(iFrom);
	}

	if (oNeighborhood == NULL) {
		delete lNeighborhood;
	}

	return lLANs;
}

/*
 * returns for each node of the graph the set of its access nodes. In output parameters (if not null) one
 * can also obtain the neighborhood sizes for each node and neighborhoods themselves
 *
 */
vector<unordered_set<int> > fGetLANs(UgGraph *iGraph, unordered_set<int> &iANs,
		vector<int> *oNeighborhoodSizes, vector<unordered_set<int> > *oNeighborhoods) {
	//local access nodes for each vertex
	vector<unordered_set<int> > lLANs (iGraph->fGetN(), unordered_set<int>());
	vector<bool> lProcessed(iGraph->fGetN(), false);

	//size of each node's neighborhood
	if (oNeighborhoodSizes != NULL) {
		oNeighborhoodSizes->clear();
		oNeighborhoodSizes->insert(oNeighborhoodSizes->end(), iGraph->fGetN(), -1);
	}

	//each node's neighborhood
	if (oNeighborhoods != NULL) {
		oNeighborhoods->clear();
		oNeighborhoods->insert(oNeighborhoods->end(), iGraph->fGetN(), unordered_set<int>());
	}

	for (size_t i = 0; i < lLANs.size(); i++) {
		if (lProcessed[i]) {
			continue;
		}

		//get local access nodes and neighborhood size for the vertex i
		unordered_set<int> *lNeighborhood;
		if (oNeighborhoods != NULL) {
			lNeighborhood = &((*oNeighborhoods)[i]);
		}
		else {
			lNeighborhood = new unordered_set<int>();
		}
		lLANs[i] = fGetLANsForNode(iGraph, i, iANs, lNeighborhood);
		if (oNeighborhoodSizes != NULL) {
			(*oNeighborhoodSizes)[i] = lNeighborhood->size();
		}
		lProcessed[i] = true;

		//this serves as an optimization
		//
		//nodes in the neighborhood have the same access nodes but may have smaller neighborhoods
		//(because of oriented edges). If we are not interested in the neighborhoods, we can set
		//the access node values for these nodes as we did for i
		//
		//note that we may do this only if the node i itself is not an access node
		if (oNeighborhoodSizes == NULL && oNeighborhoods == NULL && iANs.count(i) == 0) {
			for (unordered_set<int>::iterator j = lNeighborhood->begin(); j != lNeighborhood->end(); j++) {
				//skip access nodes
				if (iANs.count(*j) != 0) {
					continue;
				}
				lLANs[*j] = lLANs[i];
				lProcessed[*j] = true;
			}
		}

		if (oNeighborhoods == NULL) {
			delete lNeighborhood;
		}
	}

	return lLANs;
}

vector<unordered_set<int> > fGetBackLANs(UgGraph *iGraph, unordered_set<int> &iANs,
		vector<int> *oNeighborhoodSizes, vector<unordered_set<int> > *oNeighborhoods) {
	UgGraph lBackGraph = *iGraph;
	lBackGraph.fReverseOrientation();

	vector<unordered_set<int> > lLANs = fGetLANs(&lBackGraph, iANs, oNeighborhoodSizes, oNeighborhoods);

	return lLANs;
}

AnDiag fDiagnoseAnSet(UgGraph* iGraph, unordered_set<int> &iANs) {
	//get results
	INFO("Getting local access nodes" << endl);
	vector<int> lNeighSizes;
	vector<unordered_set<int> > lLANs = fGetLANs(iGraph, iANs, &lNeighSizes, NULL);
	unordered_map<int, int> lLanUsage;
	for (unordered_set<int>::iterator i = iANs.begin(); i != iANs.end(); i++) {
		lLanUsage[*i] = 0;
	}

	INFO("Getting back local access nodes" << endl);
	vector<int> lBackNeighSizes;
	vector<unordered_set<int> > lBackLANs = fGetBackLANs(iGraph, iANs, &lBackNeighSizes, NULL);
	unordered_map<int, int> lBackLanUsage;
	for (unordered_set<int>::iterator i = iANs.begin(); i != iANs.end(); i++) {
		lBackLanUsage[*i] = 0;
	}

	//process results
	INFO("Processing results" << endl);
	double lAvgLANSize = 0;
	double lAvgNeighSize = 0;
	int lMaxLANSize = 0;
	int lMaxNeighSize = 0;
	double lAvgBackLANSize = 0;
	double lAvgBackNeighSize = 0;
	int lMaxBackLANSize = 0;
	int lMaxBackNeighSize = 0;
	for (int i = 0; i < iGraph->fGetN(); i++) {
		//skip access nodes
		if (iANs.count(i) != 0) {
			continue;
		}

		//front
		for (unordered_set<int>::iterator j = lLANs[i].begin(); j != lLANs[i].end(); j++) {
			lLanUsage[*j]++;
		}

		lAvgLANSize += lLANs[i].size() * lLANs[i].size();
		lAvgNeighSize += lNeighSizes[i] * lNeighSizes[i];

		lMaxLANSize = max(lMaxLANSize, (int)lLANs[i].size());
		lMaxNeighSize = max(lMaxNeighSize, lNeighSizes[i]);

		//back
		for (unordered_set<int>::iterator j = lBackLANs[i].begin(); j != lBackLANs[i].end(); j++) {
			lBackLanUsage[*j]++;
		}

		lAvgBackLANSize += lBackLANs[i].size() * lBackLANs[i].size();
		lAvgBackNeighSize += lBackNeighSizes[i] * lBackNeighSizes[i];

		lMaxBackLANSize = max(lMaxBackLANSize, (int)lBackLANs[i].size());
		lMaxBackNeighSize = max(lMaxBackNeighSize, lBackNeighSizes[i]);
	}
	lAvgLANSize /= (iGraph->fGetN() - iANs.size());
	lAvgNeighSize /= (iGraph->fGetN() - iANs.size());
	lAvgBackLANSize /= (iGraph->fGetN() - iANs.size());
	lAvgBackNeighSize /= (iGraph->fGetN() - iANs.size());

	AnDiag lDiag;
	lDiag.cAvgLANSize = lAvgLANSize;
	lDiag.cAvgNeighSize = lAvgNeighSize;
	lDiag.cMaxLANSize = lMaxLANSize;
	lDiag.cMaxNeighSize = lMaxNeighSize;
	lDiag.cAvgBackLANSize = lAvgBackLANSize;
	lDiag.cAvgBackNeighSize = lAvgBackNeighSize;
	lDiag.cMaxBackLANSize = lMaxBackLANSize;
	lDiag.cMaxBackNeighSize = lMaxBackNeighSize;

	return lDiag;
}

bool fLanUsageComp (std::pair<std::string, int> i, std::pair<std::string, int> j) {
	return (i.second > j.second);
}

string fStringDiagAnSet(UgGraph *iGraph, unordered_set<int> &iANs, bool iDet) {
	stringstream lSs;

	//get results
	INFO("Getting local access nodes" << endl);
	vector<int> lNeighSizes;
	vector<unordered_set<int> > lLANs = fGetLANs(iGraph, iANs, &lNeighSizes, NULL);
	unordered_map<int, int> lLanUsage;
	for (unordered_set<int>::iterator i = iANs.begin(); i != iANs.end(); i++) {
		lLanUsage[*i] = 0;
	}

	INFO("Getting back local access nodes" << endl);
	vector<int> lBackNeighSizes;
	vector<unordered_set<int> > lBackLANs = fGetBackLANs(iGraph, iANs, &lBackNeighSizes, NULL);
	unordered_map<int, int> lBackLanUsage;
	for (unordered_set<int>::iterator i = iANs.begin(); i != iANs.end(); i++) {
		lBackLanUsage[*i] = 0;
	}

	//process results
	INFO("Processing results" << endl);
	double lAvgLANSize = 0;
	double lAvgNeighSize = 0;
	int lMaxLANSize = 0;
	int lMaxNeighSize = 0;
	double lAvgBackLANSize = 0;
	double lAvgBackNeighSize = 0;
	int lMaxBackLANSize = 0;
	int lMaxBackNeighSize = 0;
	for (int i = 0; i < iGraph->fGetN(); i++) {
		//skip access nodes
		if (iANs.count(i) != 0) {
			continue;
		}

		//front
		for (unordered_set<int>::iterator j = lLANs[i].begin(); j != lLANs[i].end(); j++) {
			lLanUsage[*j]++;
		}

		lAvgLANSize += lLANs[i].size() * lLANs[i].size();
		lAvgNeighSize += lNeighSizes[i] * lNeighSizes[i];

		lMaxLANSize = max(lMaxLANSize, (int)lLANs[i].size());
		lMaxNeighSize = max(lMaxNeighSize, lNeighSizes[i]);

		//back
		for (unordered_set<int>::iterator j = lBackLANs[i].begin(); j != lBackLANs[i].end(); j++) {
			lBackLanUsage[*j]++;
		}

		lAvgBackLANSize += lBackLANs[i].size() * lBackLANs[i].size();
		lAvgBackNeighSize += lBackNeighSizes[i] * lBackNeighSizes[i];

		lMaxBackLANSize = max(lMaxBackLANSize, (int)lBackLANs[i].size());
		lMaxBackNeighSize = max(lMaxBackNeighSize, lBackNeighSizes[i]);
	}
	lAvgLANSize /= (iGraph->fGetN() - iANs.size());
	lAvgNeighSize /= (iGraph->fGetN() - iANs.size());
	lAvgBackLANSize /= (iGraph->fGetN() - iANs.size());
	lAvgBackNeighSize /= (iGraph->fGetN() - iANs.size());

//	double lBelowAvgNeigh = 0;
//	double lBelowAvgBackNeigh = 0;
//	double lVolumeAvgNeigh = 0;
//	double lVolumeAvgBackNeigh = 0;
//	for (int i = 0; i < iGraph->fGetN(); i++) {
//		//skip access nodes
//		if (iANs.count(i) != 0) {
//			continue;
//		}
//
//		lBelowAvgNeigh += (lNeighSizes[i] <= lAvgNeighSize) ? 1 : 0;
//		lBelowAvgBackNeigh += (lBackNeighSizes[i] <= lAvgBackNeighSize) ? 1 : 0;
//		lVolumeAvgNeigh += (lNeighSizes[i] <= lAvgNeighSize) ? lNeighSizes[i] : 0;
//		lVolumeAvgBackNeigh += (lBackNeighSizes[i] <= lAvgBackNeighSize) ? lBackNeighSizes[i] : 0;
//	}
//	lBelowAvgNeigh /= (iGraph->fGetN() - iANs.size());
//	lBelowAvgBackNeigh /= (iGraph->fGetN() - iANs.size());
//	lVolumeAvgNeigh /= ((iGraph->fGetN() - iANs.size()) * lAvgNeighSize);
//	lVolumeAvgBackNeigh /= ((iGraph->fGetN() - iANs.size()) * lAvgBackNeighSize);

	//----- set main results -----
	lSs << "SIZE of UG: " << iGraph->fGetN() << endl;
	lSs << "SQRT of the size of UG: " << sqrt(iGraph->fGetN()) << endl;
	lSs << "SIZE of the set: " << iANs.size() << endl;
	lSs << "R1 of the set: " << (double)iANs.size() / sqrt(iGraph->fGetN()) << endl;
	lSs << "R2 of the set: " << max(lAvgNeighSize, lAvgBackNeighSize) / (double)iGraph->fGetN() << endl;
	lSs << "R3 of the set: " << max(lAvgLANSize, lAvgBackLANSize) << endl;
	lSs << "AVG SQR LAN size: " << lAvgLANSize << endl;
	lSs << "AVG SQR NEIGH size: " << lAvgNeighSize << endl;
	lSs << "AVG SQR BACK LAN size: " << lAvgBackLANSize << endl;
	lSs << "AVG SQR BACK NEIGH size: " << lAvgBackNeighSize << endl;
	lSs << "MAX LAN size: " << lMaxLANSize << endl;
	lSs << "MAX NEIGH size: " << lMaxNeighSize << endl;
	lSs << "MAX BACK LAN size: " << lMaxBackLANSize << endl;
	lSs << "MAX BACK NEIGH size: " << lMaxBackNeighSize << endl;

	//----- details for each node's ANs -----
	if (iDet) {
		lSs << "AN USAGE:" << endl;
		vector<pair<string, int> > lLanUsageVec;
		for (unordered_map<int, int>::iterator i = lLanUsage.begin(); i != lLanUsage.end(); i++) {
			lLanUsageVec.push_back(pair<string, int>(iGraph->fGetNodeName(i->first), i->second));
		}
		sort(lLanUsageVec.begin(), lLanUsageVec.end(), fLanUsageComp);
		for (size_t i = 0; i < lLanUsageVec.size(); i++) {
			lSs << "	" << lLanUsageVec[i].first << ": " << lLanUsageVec[i].second << endl;
		}
		lSs << "BACK AN USAGE:" << endl;
		vector<pair<string, int> > lBackLanUsageVec;
		for (unordered_map<int, int>::iterator i = lBackLanUsage.begin(); i != lBackLanUsage.end(); i++) {
			lBackLanUsageVec.push_back(pair<string, int>(iGraph->fGetNodeName(i->first), i->second));
		}
		sort(lBackLanUsageVec.begin(), lBackLanUsageVec.end(), fLanUsageComp);
		for (size_t i = 0; i < lBackLanUsageVec.size(); i++) {
			lSs << "	" << lBackLanUsageVec[i].first << ": " << lBackLanUsageVec[i].second << endl;
		}

		lSs << "ACCESS NODES for ANs" << endl;
		for (int i = 0; i < iGraph->fGetN(); i++) {
			if (iANs.count(i) != 0) {
				lSs << "	LAN SIZE FOR " << iGraph->fGetNodeName(i) << ": " << lLANs[i].size() << endl;
				lSs << "	NEIGHBORHOOD SIZE FOR " << iGraph->fGetNodeName(i) << ": " << lNeighSizes[i] << endl;
				lSs << "	LANs FOR " << iGraph->fGetNodeName(i) << ": " << endl;
				for (unordered_set<int>::iterator j = lLANs[i].begin(); j != lLANs[i].end(); j++) {
					lSs << "		" << iGraph->fGetNodeName(*j) << endl;
				}
				lSs << "	BACK LAN SIZE FOR " << iGraph->fGetNodeName(i) << ": " << lBackLANs[i].size() << endl;
				lSs << "	BACK NEIGHBORHOOD SIZE FOR " << iGraph->fGetNodeName(i) << ": " << lBackNeighSizes[i] << endl;
				lSs << "	BACK LANs FOR " << iGraph->fGetNodeName(i) << ": " << endl;
				for (unordered_set<int>::iterator j = lBackLANs[i].begin(); j != lBackLANs[i].end(); j++) {
					lSs << "		" << iGraph->fGetNodeName(*j) << endl;
				}
			}
		}

		lSs << "ACCESS NODES for non-ANs" << endl;
		for (int i = 0; i < iGraph->fGetN(); i++) {
			if (iANs.count(i) == 0) {
				lSs << "	LAN SIZE FOR " << iGraph->fGetNodeName(i) << ": " << lLANs[i].size() << endl;
				lSs << "	NEIGHBORHOOD SIZE FOR " << iGraph->fGetNodeName(i) << ": " << lNeighSizes[i] << endl;
				lSs << "	LANs FOR " << iGraph->fGetNodeName(i) << ": " << endl;
				for (unordered_set<int>::iterator j = lLANs[i].begin(); j != lLANs[i].end(); j++) {
					lSs << "		" << iGraph->fGetNodeName(*j) << endl;
				}
				lSs << "	BACK LAN SIZE FOR " << iGraph->fGetNodeName(i) << ": " << lBackLANs[i].size() << endl;
				lSs << "	BACK NEIGHBORHOOD SIZE FOR " << iGraph->fGetNodeName(i) << ": " << lBackNeighSizes[i] << endl;
				lSs << "	BACK LANs FOR " << iGraph->fGetNodeName(i) << ": " << endl;
				for (unordered_set<int>::iterator j = lBackLANs[i].begin(); j != lBackLANs[i].end(); j++) {
					lSs << "		" << iGraph->fGetNodeName(*j) << endl;
				}
			}
		}
	}

	return lSs.str();
}
