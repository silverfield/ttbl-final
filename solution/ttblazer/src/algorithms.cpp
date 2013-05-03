/*****************************************************************/
/* Includes
/*****************************************************************/
#include <vector>
#include <queue>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include <boost/unordered_set.hpp>
#include <functional>
#include <iostream>
#include <string>

#include "central.h"
#include "objects/graph.h"

#include "algorithms.h"
#include "algdijkstra.h"
#include "fibheap.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Variables - definitions from alg-templates
/*****************************************************************/

//fStrongConn
Partition gStrongConnPartition;
int gStrongConnLabel;
vector<int> gStrongConnLabels;
vector<int> gStrongConnLows;
vector<bool> gStrongConnInStack;
std::stack<int> gStrongConnStack;

/*****************************************************************/
/* Simple functions
/*****************************************************************/

/* iDegreeFreq[i] is the frequency of degree "i". Returned is the average degree*/
double fAvgDegree(vector<int> iDegreeFreq) {
	int lSum = 0;

	int lCount = 0;
	for (vector<int>::size_type i = 0; i < iDegreeFreq.size(); i++) {
		lSum += i * iDegreeFreq[i];
		lCount += iDegreeFreq[i];
	}

	return (double)lSum / (double)lCount;
}

int fFirstNonZeroValue(vector<int> iVector) {
	for (vector<int>::size_type i = 0; i < iVector.size(); i++) {
		if (iVector[i] != 0) {
			return i;
		}
	}

	return -1;
}

int fLastNonZeroValue(vector<int> iVector) {
	for (vector<int>::size_type i = iVector.size() - 1; i >= 0; i--) {
		if (iVector[i] != 0) {
			return i;
		}
	}

	return -1;
}

/* normalizes to given range */
vector<double> fNormalize(vector<double> iVector, double iRangeMin, double iRangeMax) {
	if (iVector.size() == 0) {
		return vector<double>();
	}
	double lMin = iVector[0];
	double lMax = iVector[0];
	for (size_t i = 1; i < iVector.size(); i++) {
		lMin = min(iVector[i], lMin);
		lMax = max(iVector[i], lMax);
	}

	vector<double> lNormalized(iVector.size(), 0);
	for (size_t i = 0; i < iVector.size(); i++) {
		double lRelativePos = (iVector[i] - lMin) / (lMax - lMin);
		lNormalized[i] = iRangeMin + (iRangeMax - iRangeMin) * lRelativePos;
	}

	return lNormalized;
}

/*****************************************************************/
/* Dijkstra implementations
/*****************************************************************/

/* returns only the distance.
 * for time-dependent graph, if iTime == -1, all edge costs are 1 (as if iHasWeights == false)
 *
 * either destination index is specified, or destination city (in which case this solves earliest
 * arrival problem).
 *
 * unspecified value for iTo = -1
 * unspecified value for iToCity = "" */
int fDijkstraDist(GraphObject *iGraph, int iFrom, int iTime, int iTo, string iToCity,
		bool iHasWeights) {
	int n = iGraph->fGetN();

	//initialize distance vector
	vector<int> lDistances(n, -1);
	FibHeap lDistancesHeap(lDistances);
	lDistances[iFrom] = 0;
	lDistancesHeap.fUpdate(iFrom);

	//initialize sets
	unordered_set<int> lSettled; //those with final distance
	unordered_set<int> lExplored; //those that are visible, but not settled
	lExplored.insert(iFrom);

	bool lFound = false;
	while (1) {
		int lBest = -1;
		int lBestDistance = -1;
		//no more explored vertices - probably not connected
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion
		if (lBest == -1) {
			break;
		}
		if (lBest == iTo || (iToCity != "" && iGraph->fGetNodeName(lBest) == iToCity)) {
			iTo = lBest;
			lFound = true;
			break;
		}

		DEB(LVL_ALGS, "Algorithms::fDijkstra: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (iHasWeights == false) {
				lLength = 1;
			}
			if (lLength == -1) {
				continue;
			}
			if (iGraph->fGetObjectType() == OTTd && iTime == -1) {
				lLength = 1;
			}
			if (lDistances[lToIndex] == -1 || lDistances[lToIndex] > lDistances[lBest] + lLength ||
				(lDistances[lToIndex] == lDistances[lBest] + lLength)) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstra: Distance to best " << lDistances[lBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstra: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstra: Distance to being updated " <<
						lDistances[lToIndex] << endl);

				lDistances[lToIndex] = lDistances[lBest] + lLength;
				lDistancesHeap.fUpdate(lToIndex);

				lExplored.insert(lToIndex);
			}
		}
	}

	if (lFound == false) {
		return -1;
	}

	return lDistances[iTo];
}

/* returns the shortest path. Ties are won by paths with smaller number of hops
 * for time-dependent graph, if iTime == -1, all edge costs are 1 (as if iHasWeights == false)
 *
 * either destination index is specified, or destination city (in which case this solves earliest
 * arrival problem).
 *
 * unspecified value for iTo = -1
 * unspecified value for iToCity = ""*/
Path fDijkstraSp(GraphObject *iGraph, int iFrom, int iTime, int iTo, string iToCity, bool iHasWeights,
		unordered_set<string> &iAllowedCities) {

	int n = iGraph->fGetN();

	//initialize distance vector
	vector<int> lDistances(n, -1);
	lDistances[iFrom] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(iFrom);

	//initialize came-from vector
	vector<int> lCameFrom(n, -1);
	lCameFrom[iFrom] = iFrom;

	//initialize hop vector
	vector<int> lHops(n, -1);
	lHops[iFrom] = 0;

	//initialize sets
	unordered_set<int> lSettled; //those with final distance
	unordered_set<int> lExplored; //those that are visible, but not settled
	lExplored.insert(iFrom);

	bool lFound = false;
	while (1) {
		int lBest = -1;
		int lBestDistance = -1;
		//no more explored vertices - probably not connected
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion
		if (lBest == -1) {
			break;
		}
		if (lBest == iTo || (iToCity != "" && iGraph->fGetNodeName(lBest) == iToCity)) {
			iTo = lBest;
			lFound = true;
			break;
		}

		DEB(LVL_ALGS, "Algorithms::fDijkstraSp: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			if (iAllowedCities.size() != 0 && iAllowedCities.count(iGraph->fGetNodeName(lToIndex)) == 0) {
				continue;
			}
			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (iHasWeights == false) {
				lLength = 1;
			}
			if (lLength == -1) {
				continue;
			}
			if (iGraph->fGetObjectType() == OTTd && iTime == -1) {
				lLength = 1;
			}
			if (lDistances[lToIndex] == -1 || lDistances[lToIndex] > lDistances[lBest] + lLength ||
				(lDistances[lToIndex] == lDistances[lBest] + lLength && lHops[lToIndex] >
					lHops[lBest] + 1)) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSp: Distance to best " << lDistances[lBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSp: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSp: Distance to being updated (" << lToIndex << ") " <<
						lDistances[lToIndex] << endl);

				lDistances[lToIndex] = lDistances[lBest] + lLength;
				lDistancesHeap.fUpdate(lToIndex);
				lCameFrom[lToIndex] = lBest;
				lHops[lToIndex] = lHops[lBest] + 1;

				lExplored.insert(lToIndex);
			}
		}
	}

	if (lFound == false) {
		return Path();
	}

	//get the path
	vector<int> lPathVertices;
	int lPathIndex = iTo;
	while (lCameFrom[lPathIndex] != lPathIndex) {
		lPathVertices.push_back(lPathIndex);
		lPathIndex = lCameFrom[lPathIndex];
	}
	lPathVertices.push_back(lPathIndex);
	reverse(lPathVertices.begin(), lPathVertices.end());

	Path lPath(lPathVertices);
	return lPath;
}

Connection fDijkstraConn(TdGraph *iGraph, int iFrom, int iTime, int iTo) {
	int n = iGraph->fGetN();

	//initialize distance vector
	vector<int> lDistances(n, -1);
	lDistances[iFrom] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(iFrom);

	//initialize came-from vector
	vector<int> lCameFrom(n, -1);
	lCameFrom[iFrom] = iFrom;

	//initialize sets
	unordered_set<int> lSettled; //those with final distance
	unordered_set<int> lExplored; //those that are visible, but not settled
	lExplored.insert(iFrom);

	bool lFound = false;
	while (1) {
		int lBest = -1;
		int lBestDistance = -1;
		//no more explored vertices - probably not connected
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion
		if (lBest == -1) {
			break;
		}
		if (lBest == iTo) {
			lFound = true;
			break;
		}

		DEB(LVL_ALGS, "Algorithms::fDijkstraConn: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (lLength == -1) {
				continue;
			}
			if (lDistances[lToIndex] == -1 || lDistances[lToIndex] > lDistances[lBest] + lLength) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstraConn: Distance to best " << lDistances[lBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraConn: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraConn: Distance to being updated (" << lToIndex << ") " <<
						lDistances[lToIndex] << endl);

				lDistances[lToIndex] = lDistances[lBest] + lLength;
				lDistancesHeap.fUpdate(lToIndex);
				lCameFrom[lToIndex] = lBest;

				lExplored.insert(lToIndex);
			}
		}
	}

	if (lFound == false) {
		return Connection();
	}

	//get the path
	vector<int> lPathVertices;
	int lPathIndex = iTo;
	while (lCameFrom[lPathIndex] != lPathIndex) {
		lPathVertices.push_back(lPathIndex);
		lPathIndex = lCameFrom[lPathIndex];
	}
	lPathVertices.push_back(lPathIndex);
	reverse(lPathVertices.begin(), lPathVertices.end());

	Path lPath(lPathVertices);

	return iGraph->fMakeConnection(lPath, iTime);
}

/* returns the shortest paths to all vertices. Ties are won by paths with smaller number of hops
 * for time-dependent graph, if iTime == -1, all edge costs are 1 (as if iHasWeights == false) */
vector<Path> fDijkstraSpToAll(GraphObject *iGraph, int iFrom, int iTime, bool iHasWeights) {
	int n = iGraph->fGetN();

	//initialize distance vector
	vector<int> lDistances(n, -1);
	lDistances[iFrom] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(iFrom);

	//initialize came-from vector
	vector<int> lCameFrom(n, -1);
	lCameFrom[iFrom] = iFrom;

	//initialize hop vector
	vector<int> lHops(n, -1);
	lHops[iFrom] = 0;

	//initialize sets
	unordered_set<int> lSettled; //those with final distance
	unordered_set<int> lExplored; //those that are visible, but not settled
	lExplored.insert(iFrom);

	bool lFound = false;
	while (1) {
		int lBest = -1;
		int lBestDistance = -1;
		//no more explored vertices - probably not connected
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion
		if (lBest == -1) {
			break;
		}

		DEB(LVL_ALGS, "Algorithms::fDijkstraSpToAll: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (iHasWeights == false) {
				lLength = 1;
			}
			if (lLength == -1) {
				continue;
			}
			if (iGraph->fGetObjectType() == OTTd && iTime == -1) {
				lLength = 1;
			}
			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}
			if (lDistances[lToIndex] == -1 || lDistances[lToIndex] > lDistances[lBest] + lLength ||
				(lDistances[lToIndex] == lDistances[lBest] + lLength && lHops[lToIndex] >
					lHops[lBest] + 1)) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSpToAll: Distance to best " <<
						lDistances[lBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSpToAll: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSpToAll: Distance to being updated " <<
						lDistances[lToIndex] << endl);

				lDistances[lToIndex] = lDistances[lBest] + lLength;
				lDistancesHeap.fUpdate(lToIndex);
				lCameFrom[lToIndex] = lBest;
				lHops[lToIndex] = lHops[lBest] + 1;

				lExplored.insert(lToIndex);
			}
		}
	}

	//get the paths
	vector<Path> lPaths;
	for (int i = 0; i < n; i++) {
		if (lDistances[i] == -1) {
			lPaths.push_back(Path());
			continue;
		}
		vector<int> lPathVertices;
		int lPathIndex = i;
		while (lCameFrom[lPathIndex] != lPathIndex) {
			lPathVertices.push_back(lPathIndex);
			lPathIndex = lCameFrom[lPathIndex];
		}
		lPathVertices.push_back(lPathIndex);
		reverse(lPathVertices.begin(), lPathVertices.end());

		Path lPath(lPathVertices);
		lPaths.push_back(lPath);
	}

	return lPaths;
}

/* returns random shortest paths to all vertices
 * for time-dependent graph, if iTime == -1, all edge costs are 1 (as if iHasWeights == false) */
vector<Path> fDijkstraRandomSpToAll(GraphObject *iGraph, int iFrom, int iTime, bool iHasWeights) {
	int n = iGraph->fGetN();

	//initialize distance vector
	vector<int> lDistances(n, -1);
	lDistances[iFrom] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(iFrom);

	//initialize came-from vector
	vector<vector<int> > lCameFrom(n, vector<int>());

	//initialize sets
	unordered_set<int> lSettled; //those with final distance
	unordered_set<int> lExplored; //those that are visible, but not settled
	lExplored.insert(iFrom);

	while (1) {
		int lBest = -1;
		int lBestDistance = -1;
		//no more explored vertices - probably not connected
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion
		if (lBest == -1) {
			break;
		}

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		DEB(LVL_ALGS, "Algorithms::fDijkstraRandomSpToAll: Best vertex " << lBest << endl);

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (iHasWeights == false) {
				lLength = 1;
			}
			if (lLength == -1) {
				continue;
			}
			if (iGraph->fGetObjectType() == OTTd && iTime == -1) {
				lLength = 1;
			}
			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}
			if (lDistances[lToIndex] == -1 || lDistances[lToIndex] > lDistances[lBest] + lLength) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstraRandomSpToAll: Distance to best " <<
						lDistances[lBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraRandomSpToAll: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraRandomSpToAll: Distance to being updated " <<
						lDistances[lToIndex] << endl);

				lDistances[lToIndex] = lDistances[lBest] + lLength;
				lDistancesHeap.fUpdate(lToIndex);
				lCameFrom[lToIndex].clear();
				lCameFrom[lToIndex].push_back(lBest);

				lExplored.insert(lToIndex);
			}
			else if (lDistances[lToIndex] == lDistances[lBest] + lLength) {
				lCameFrom[lToIndex].push_back(lBest);

				lExplored.insert(lToIndex);
			}
		}
	}

	//get the paths
	vector<Path> lPaths;
	for (int i = 0; i < n; i++) {
		if (lDistances[i] == -1) {
			lPaths.push_back(Path());
			continue;
		}
		vector<int> lPathVertices;
		int lPathIndex = i;
		while (lCameFrom[lPathIndex].size() != 0) {
			lPathVertices.push_back(lPathIndex);
			int lRandomNext = rand() % lCameFrom[lPathIndex].size();
			lPathIndex = lCameFrom[lPathIndex][lRandomNext];
		}
		lPathVertices.push_back(lPathIndex);
		reverse(lPathVertices.begin(), lPathVertices.end());

		Path lPath(lPathVertices);
		lPaths.push_back(lPath);
	}

	return lPaths;
}

/* extracts all the shortest paths to iTo, based on iCameFrom argument.
 *
 * Complexity is O(n * #paths), where #paths can get exponential, though in extreme cases only */
vector<Path> fDijkstraPaths(GraphObject *iGraph, vector<vector<int> > &iCameFrom,
		int iFrom, int iTo, bool iIncludingTo) {
	if (iFrom != iTo && iCameFrom[iTo].size() == 0) {
		return vector<Path>();
	}
	if (iFrom == iTo) {
		Path lPath;
		lPath.fAdd(iTo);
		return vector<Path>(1, lPath);
	}

	list<vector<int>* > lPathsInd;
	if (iIncludingTo) {
		vector<int> *lFirstPathInd = new vector<int>();
		lFirstPathInd->push_back(iTo);
		lPathsInd.push_back(lFirstPathInd);
	}
	else {
		for (size_t i = 0; i < iCameFrom[iTo].size(); i++) {
			vector<int> *lFirstPathInd = new vector<int>();
			lFirstPathInd->push_back(iCameFrom[iTo][i]);
			lPathsInd.push_back(lFirstPathInd);
		}
	}
	list<vector<int>* >::iterator lPathIt = lPathsInd.begin();
	while (lPathIt != lPathsInd.end()) {
		int lLastVertex = (**lPathIt)[(*lPathIt)->size() - 1];

		//this is a complete path, move on
		if (iCameFrom[lLastVertex].size() == 0) {
			lPathIt++;
			continue;
		}

		//add new paths to the list, processing them later
		for (size_t i = 1; i < iCameFrom[lLastVertex].size(); i++) {
			vector<int> *lNewPathInd = new vector<int>();
			lNewPathInd->insert(lNewPathInd->end(), (*lPathIt)->begin(), (*lPathIt)->end());
			lNewPathInd->push_back(iCameFrom[lLastVertex][i]);
			lPathsInd.push_back(lNewPathInd);
		}

		//add to this path the next vertex
		(*lPathIt)->push_back(iCameFrom[lLastVertex][0]);

		DEB(LVL_ALGDET, "Algorithms::fDijkstraPaths: processing " << lPathsInd.size() << " paths" << endl);
		DEB(LVL_ALGDETT, "Algorithms::fDijkstraPaths: ------------------" << endl);
		for (list<vector<int>* >::iterator i = lPathsInd.begin(); i != lPathsInd.end(); i++) {
			DEB(LVL_ALGDETT, "Algorithms::fDijkstraPaths: " << fGetVectorString(**i) << endl);
		}
	}

	//make actual Path objects
	vector<Path> lPaths;
	lPathIt = lPathsInd.begin();
	int lTotal = 0;
	while (lPathIt != lPathsInd.end()) {
		lTotal++;

		//reverse indices
		reverse((*lPathIt)->begin(), (*lPathIt)->end());

		//make and add Path object
		Path lPath(**lPathIt);
		DEB(LVL_ALGDET, "Algorithms::fDijkstraPaths: " << lPath.fGetString() << endl);
		DEB(LVL_ALGDET, "Algorithms::fDijkstraPaths: " << iGraph->fGetPathString(lPath, ", ") << endl);
		lPaths.push_back(lPath);

		//destroy vector of indices
		delete *lPathIt;

		//move on
		lPathIt++;
	}

	DEB(LVL_ALGDET, "Algorithms::fDijkstraPaths: " << lTotal << " paths to " << iTo << endl);

	return lPaths;
}

/* returns all shortest path from given vertex
 * for time-dependent graph, if iTime == -1, all edge costs are 1 (as if iHasWeights == false) */
vector<vector<Path> > fDijkstraMultiSpToAll(GraphObject *iGraph, int iFrom, int iTime, bool iHasWeights) {
	int n = iGraph->fGetN();

	//initialize distance vector
	vector<int> lDistances(n, -1);
	lDistances[iFrom] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(iFrom);

	//initialize came-from vector
	vector<vector<int> > lCameFrom(n, vector<int>());

	//initialize sets
	unordered_set<int> lSettled; //those with final distance
	unordered_set<int> lExplored; //those that are visible, but not settled
	lExplored.insert(iFrom);

	while (1) {
		int lBest = -1;
		int lBestDistance = -1;
		//no more explored vertices - probably not connected
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion
		if (lBest == -1) {
			break;
		}

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		DEB(LVL_ALGS, "Algorithms::fDijkstraMultiSpToAll: Best vertex " << lBest << endl);

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (iHasWeights == false) {
				lLength = 1;
			}
			if (lLength == -1) {
				continue;
			}
			if (iGraph->fGetObjectType() == OTTd && iTime == -1) {
				lLength = 1;
			}
			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}
			if (lDistances[lToIndex] == -1 || lDistances[lToIndex] > lDistances[lBest] + lLength) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToAll: Distance to best " <<
						lDistances[lBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToAll: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToAll: Distance to being updated " <<
						lDistances[lToIndex] << endl);

				lDistances[lToIndex] = lDistances[lBest] + lLength;
				lDistancesHeap.fUpdate(lToIndex);
				lCameFrom[lToIndex].clear();
				lCameFrom[lToIndex].push_back(lBest);

				lExplored.insert(lToIndex);
			}
			else if (lDistances[lToIndex] == lDistances[lBest] + lLength) {
				lCameFrom[lToIndex].push_back(lBest);

				lExplored.insert(lToIndex);
			}
		}
	}

	DEB(LVL_ALGS, "Algorithms::fDijkstraMultiSpToAll: Going to create the paths" << endl);

	//get the paths
	vector<vector<Path> > lPaths;
	for (int i = 0; i < n; i++) {
		DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToAll: Getting paths from " << iFrom <<
				" to " << i << endl);
		vector<Path> lPathsToI = fDijkstraPaths(iGraph, lCameFrom, iFrom, i, true);
		lPaths.push_back(lPathsToI);
	}

	return lPaths;
}

vector<vector<vector<Path> > > fDijkstraAllMultiSp(GraphObject *iGraph, bool iHasWeights) {
	int n = iGraph->fGetN();

	vector<vector<vector<Path> > > lPaths(n, vector<vector<Path> >());

	int lOldPercents = -1;
	for (int i = 0; i < iGraph->fGetN(); i++) {
		int lPercents = (i * 100) / iGraph->fGetN();
		if (lPercents != lOldPercents) {
			INFO("Progress " << lPercents << "%" << endl);
			lOldPercents = lPercents;
		}
		DEB(LVL_ALGS, "Algorithms::fDijkstraAllMultiSp: Progress " << i << " / " << n << endl);
		lPaths[i] = fDijkstraMultiSpToAll(iGraph, i, -1, iHasWeights);
	}

	return lPaths;
}

/* computes shortest paths to all cities starting from a node with index iFrom and at time iTime.
 *
 * providing cities as an argument could save one GetCities computation */
unordered_map<string, Path> fDijkstraSpToCities(TimeGraphObject *iGraph, int iFrom, int iTime,
		unordered_set<string> &iToCities, bool iHasWeights) {
	int n = iGraph->fGetN();

	//number of to-cities (those we are interested in)
	int t = iToCities.size();

	//initialize distance vector - best distance to node and to city
	vector<int> lDistances(n, -1);
	unordered_map<string, int> lCityDist;
	lDistances[iFrom] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(iFrom);
	lCityDist[iGraph->fGetNodeName(iFrom)] = 0;

	//initialize came-from vector
	//come-from[city] = index of a node we came from to the city
	vector<int> lCameFrom(n, -1);
	unordered_map<string, int> lCityCameFrom;

	//initialize sets, we explore the graph as in default Dijkstra, only the finish criterion differs
	unordered_set<string> lSettledCities; //those cities with final distance
	lSettledCities.insert(iGraph->fGetNodeName(iFrom));
	int lSettledCount = 0; //number of settled to-cities
	if (iToCities.count(iGraph->fGetNodeName(iFrom)) != 0) {
		lSettledCount++;
	}
	unordered_set<int> lSettled;
	unordered_set<int> lExplored; //those vertices that are visible, but not settled
	lExplored.insert(iFrom);

	while (1) {
		int lBest = -1;
		int lBestDistance = -1;

		//no more explored vertices
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion - no more vertices to settle
		if (lBest == -1) {
			break;
		}

		DEB(LVL_ALGS, "Algorithms::fDijkstraSpToCities: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//if it is the first time we visit a city, mark the city as settled
		if (lSettledCities.count(iGraph->fGetNodeName(lBest)) == 0) {
			lSettledCities.insert(iGraph->fGetNodeName(lBest));
			if (iToCities.count(iGraph->fGetNodeName(lBest)) != 0) {
				lSettledCount++;
			}
			lCityCameFrom[iGraph->fGetNodeName(lBest)] = lBest;
		}

		//on settling all to-cities, we can stop
		if (lSettledCount == t) {
			break;
		}

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			DEB(LVL_ALGDET, "Algorithms::fDijkstraSpToCities: To index " << lToIndex << " (city " <<
					iGraph->fGetNodeName(lToIndex) << ")" << endl);

			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}

			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (iHasWeights == false) {
				lLength = 1;
			}
			if (lLength == -1) {
				continue;
			}
			if (iGraph->fGetObjectType() == OTTd && iTime == -1) {
				lLength = 1;
			}
			//we want real paths
			if (lDistances[lToIndex] == -1 || lDistances[lToIndex] > lDistances[lBest] + lLength) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSpToCities: Distance to best " <<
						lDistances[lBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSpToCities: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSpToCities: Distance to being updated " <<
						lDistances[lToIndex] << endl);

				lDistances[lToIndex] = lDistances[lBest] + lLength;
				lDistancesHeap.fUpdate(lToIndex);
				lCameFrom[lToIndex] = lBest;

				lExplored.insert(lToIndex);
			}
		}
	}

	//get the paths
	unordered_map<string, Path> lPaths;
	for (unordered_set<string>::iterator i = iToCities.begin(); i != iToCities.end(); i++) {
		if (lCityCameFrom.count(*i) == 0) {
			lPaths[*i] = Path();
			continue;
		}

		int lPathIndex = lCityCameFrom[*i];
		vector<int> lPathVertices;
		while (lCameFrom[lPathIndex] != -1) {
			lPathVertices.push_back(lPathIndex);
			lPathIndex = lCameFrom[lPathIndex];
		}
		lPathVertices.push_back(lPathIndex);
		reverse(lPathVertices.begin(), lPathVertices.end());

		Path lPath(lPathVertices);
		lPaths[*i] = lPath;
	}

	return lPaths;
}

unordered_map<string, CityPath> fDijkstraUgSpToCities(TimeGraphObject *iGraph, int iFrom, int iTime,
		unordered_set<string> &iToCities, bool iHasWeights) {
	int n = iGraph->fGetN();

	//number of to-cities (those we are interested in)
	int t = iToCities.size();

	//initialize distance vector - best distance to node and to city
	vector<int> lDistances(n, -1);
	unordered_map<string, int> lCityDist;
	lDistances[iFrom] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(iFrom);
	lCityDist[iGraph->fGetNodeName(iFrom)] = 0;

	//initialize came-from vector
	//come-from[city] = index of a node we came from to the city
	unordered_map<string, int> lCityCameFrom;

	//initialize sets, we explore the graph as in default Dijkstra, only the finish criterion differs
	unordered_set<string> lSettledCities; //those cities with final distance
	lSettledCities.insert(iGraph->fGetNodeName(iFrom));
	int lSettledCount = 0; //number of settled to-cities
	if (iToCities.count(iGraph->fGetNodeName(iFrom)) != 0) {
		lSettledCount++;
	}
	unordered_set<int> lSettled;
	unordered_set<int> lExplored; //those vertices that are visible, but not settled
	lExplored.insert(iFrom);

	while (1) {
		int lBest = -1;
		int lBestDistance = -1;

		//no more explored vertices
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion - no more vertices to settle
		if (lBest == -1) {
			break;
		}

		DEB(LVL_ALGS, "Algorithms::fDijkstraUgSpToCities: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//if it is the first time we visit a city, mark the city as settled
		if (lSettledCities.find(iGraph->fGetNodeName(lBest)) == lSettledCities.end()) {
			lSettledCities.insert(iGraph->fGetNodeName(lBest));
			if (iToCities.find(iGraph->fGetNodeName(lBest)) != iToCities.end()) {
				lSettledCount++;
			}
		}

		//on settling all to-cities, we can stop
		if (lSettledCount == t) {
			break;
		}

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			DEB(LVL_ALGDET, "Algorithms::fDijkstraUgSpToCities: To index " << lToIndex << " (city " <<
					iGraph->fGetNodeName(lToIndex) << ")" << endl);

			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}

			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (iHasWeights == false) {
				lLength = 1;
			}
			if (lLength == -1) {
				continue;
			}
			if (iGraph->fGetObjectType() == OTTd && iTime == -1) {
				lLength = 1;
			}

			//we want UG paths
			if (lDistances[lToIndex] == -1 || lDistances[lToIndex] >= lDistances[lBest] + lLength) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstraUgSpToCities: Distance to best " <<
						lDistances[lBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraUgSpToCities: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraUgSpToCities: Distance to being updated " <<
						lDistances[lToIndex] << endl);

				lDistances[lToIndex] = lDistances[lBest] + lLength;
				lDistancesHeap.fUpdate(lToIndex);

				lExplored.insert(lToIndex);
			}
			if (lCityDist.find(iGraph->fGetNodeName(lToIndex)) == lCityDist.end() ||
				lCityDist[iGraph->fGetNodeName(lToIndex)] > lDistances[lBest] + lLength) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstraUgSpToCities: Distance to best " <<
						lDistances[lBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraUgSpToCities: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraUgSpToCities: Distance to city being updated "
						<< lCityDist[iGraph->fGetNodeName(lToIndex)]<< endl);

				lCityDist[iGraph->fGetNodeName(lToIndex)] = lDistances[lBest] + lLength;
				lCityCameFrom[iGraph->fGetNodeName(lToIndex)] = lBest;
			}
		}
	}


	//get the paths
	unordered_map<string, CityPath> lPaths;
	for (unordered_set<string>::iterator i = iToCities.begin(); i != iToCities.end(); i++) {
		if (lCityCameFrom.find(*i) == lCityCameFrom.end()) {
			lPaths[*i] = CityPath();
			continue;
		}

		int lPathIndex = lCityCameFrom[*i];
		vector<string> lCities;
		lCities.push_back(*i);
		while (lCityCameFrom.find(iGraph->fGetNodeName(lPathIndex)) != lCityCameFrom.end()) {
			lCities.push_back(iGraph->fGetNodeName(lPathIndex));
			lPathIndex = lCityCameFrom[iGraph->fGetNodeName(lPathIndex)];
		}
		lCities.push_back(iGraph->fGetNodeName(lPathIndex));
		reverse(lCities.begin(), lCities.end());

		CityPath lPath(lCities);
		lPaths[*i] = lPath;
	}

	return lPaths;
}

unordered_map<string, Path> fDijkstraRandomSpToCities(TimeGraphObject *iGraph, int iFrom, int iTime,
		unordered_set<string> &iToCities, bool iHasWeights) {
	int n = iGraph->fGetN();

	//number of to-cities (those we are interested in)
	int t = iToCities.size();

	//initialize distance vector - best distance to node and to city
	vector<int> lDistances(n, -1);
	unordered_map<string, int> lCityDist;
	lDistances[iFrom] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(iFrom);
	lCityDist[iGraph->fGetNodeName(iFrom)] = 0;

	//initialize came-from vector
	//come-from[city] = indices of a nodes we came from to the city
	vector<vector<int> > lCameFrom(n, vector<int>());
	unordered_map<string, int> lCityCameFrom;

	//initialize sets, we explore the graph as in default Dijkstra, only the finish criterion differs
	unordered_set<string> lSettledCities; //those cities with final distance
	lSettledCities.insert(iGraph->fGetNodeName(iFrom));
	int lSettledCount = 0; //number of settled to-cities
	if (iToCities.count(iGraph->fGetNodeName(iFrom)) != 0) {
		lSettledCount++;
	}
	unordered_set<int> lSettled;
	unordered_set<int> lExplored; //those vertices that are visible, but not settled
	lExplored.insert(iFrom);

	while (1) {
		int lBest = -1;
		int lBestDistance = -1;

		//no more explored vertices
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion - no more vertices to settle
		if (lBest == -1) {
			break;
		}

		DEB(LVL_ALGS, "Algorithms::fDijkstraSpToCities: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//if it is the first time we visit a city, mark the city as settled
		if (lSettledCities.count(iGraph->fGetNodeName(lBest)) == 0) {
			lSettledCities.insert(iGraph->fGetNodeName(lBest));
			if (iToCities.count(iGraph->fGetNodeName(lBest)) != 0) {
				lSettledCount++;
			}
			lCityCameFrom[iGraph->fGetNodeName(lBest)] = lBest;
		}

		//on settling all to-cities, we can stop
		if (lSettledCount == t) {
			break;
		}

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			DEB(LVL_ALGDET, "Algorithms::fDijkstraSpToCities: To index " << lToIndex << " (city " <<
					iGraph->fGetNodeName(lToIndex) << ")" << endl);

			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}

			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (iHasWeights == false) {
				lLength = 1;
			}
			if (lLength == -1) {
				continue;
			}
			if (iGraph->fGetObjectType() == OTTd && iTime == -1) {
				lLength = 1;
			}
			//we want real paths
			if (lDistances[lToIndex] == -1 || lDistances[lToIndex] > lDistances[lBest] + lLength) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSpToCities: Distance to best " <<
						lDistances[lBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSpToCities: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraSpToCities: Distance to being updated " <<
						lDistances[lToIndex] << endl);

				lDistances[lToIndex] = lDistances[lBest] + lLength;
				lDistancesHeap.fUpdate(lToIndex);
				lCameFrom[lToIndex].clear();
				lCameFrom[lToIndex].push_back(lBest);

				lExplored.insert(lToIndex);
			}
			else if (lDistances[lToIndex] == lDistances[lBest] + lLength) {
				lCameFrom[lToIndex].push_back(lBest);

				lExplored.insert(lToIndex);
			}
		}
	}

	//get the paths
	unordered_map<string, Path> lPaths;
	for (unordered_set<string>::iterator i = iToCities.begin(); i != iToCities.end(); i++) {
		if (lCityCameFrom.count(*i) == 0) {
			lPaths[*i] = Path();
			continue;
		}

		int lPathIndex = lCityCameFrom[*i];
		vector<int> lPathVertices;
		while (lCameFrom[lPathIndex].size() != 0) {
			lPathVertices.push_back(lPathIndex);
			int lRandomNext = rand() % lCameFrom[lPathIndex].size();
			lPathIndex = lCameFrom[lPathIndex][lRandomNext];
		}
		lPathVertices.push_back(lPathIndex);
		reverse(lPathVertices.begin(), lPathVertices.end());

		Path lPath(lPathVertices);
		lPaths[*i] = lPath;
	}

	return lPaths;
}

struct ArrowComp {
    inline bool operator()(const pair<int, int>& iLeft, const pair<int, int>& iRight) {
    	if (iLeft.first == iRight.first) {
    		return iLeft.second < iRight.second;
    	}
        return iLeft.first < iRight.first;
    }
};

class ArrowsToCity {
private:
	unordered_map<int, vector<pair<int, int> > > cArrows;

	//returns AFTER which element of iArrows would target be placed
	//on equality returns the index of the LAST equal item
	int fBinarySearch(int iTarget, int iFrom, int iTo, vector<pair<int, int> > &iArrows) {
		if (iFrom == iTo) {
			if (iTarget >= iArrows[iFrom].first) {
				return iFrom;
			}
			else {
				return iFrom - 1;
			}
		}

		int lMiddle = (iFrom + iTo + 1) / 2;

		if (iArrows[lMiddle].first < iTarget) {
			return fBinarySearch(iTarget, lMiddle, iTo, iArrows);
		}
		else if (iArrows[lMiddle].first == iTarget) {
			return fBinarySearch(iTarget, lMiddle, iTo, iArrows);
		}
		else {
			return fBinarySearch(iTarget, iFrom, lMiddle - 1, iArrows);
		}
	}
public:
	void fAdd(int iFrom, int iArr, int iDep) {
		if (cArrows.find(iFrom) == cArrows.end()) {
			cArrows[iFrom] = vector<pair<int, int> >();
		}
		cArrows[iFrom].push_back(pair<int, int>(iArr, iDep));
	}

	void fSort() {
		ArrowComp lArrowComp;
		for (unordered_map<int, vector<pair<int, int> > >::iterator i = cArrows.begin(); i != cArrows.end(); i++) {
			sort(i->second.begin(), i->second.end(), lArrowComp);
		}
	}

	pair<int, int> fGet(int iRangeTo, int iFromCity) {
		vector<pair<int, int> > *lArrows = &(cArrows[iFromCity]);
		int lTo = fBinarySearch(iRangeTo, 0, lArrows->size() - 1, *lArrows);

		if (lTo > lArrows->size() - 1) {
			return pair<int, int>(-1, -1);
		}

		return (*lArrows)[lTo];
	}

	int fGetSize() {
		return cArrows.size();
	}

	vector<int> fGetCities() {
		vector<int> lResult;
		lResult.reserve(cArrows.size());
		for (unordered_map<int, vector<pair<int, int> > >::iterator i = cArrows.begin(); i != cArrows.end(); i++) {
			lResult.push_back(i->first);
		}

		return lResult;
	}

	string fGetString() {
		stringstream lSs;

		for (unordered_map<int, vector<pair<int, int> > >::iterator i = cArrows.begin(); i != cArrows.end(); i++) {
			lSs << "From " << i->first << endl;
			for (size_t j = 0; j < i->second.size(); j++) {
				lSs << i->second[j].first << " <- " << i->second[j].second << ", ";
			}
			lSs << endl;
		}

		return lSs.str();
	}
};

vector<Path> fDijkstraUgPaths(GraphObject *iGraph, vector<ArrowsToCity> &iArrows,
		int iFrom, int iTo, int iArrivalTime) {
	if (iFrom == iTo) {
		Path lPath;
		lPath.fAdd(iTo);
		return vector<Path>(1, lPath);
	}

	if (iArrows[iTo].fGetSize() == 0) {
		return vector<Path>();
	}

	list<vector<int>* > lPathsInd;
	vector<int> *lFirstPathInd = new vector<int>();
	lFirstPathInd->push_back(iTo);
	lPathsInd.push_back(lFirstPathInd);

	list<int> lTimes;
	lTimes.push_back(iArrivalTime);

	list<vector<int>* >::iterator lPathIt = lPathsInd.begin();
	list<int>::iterator lTimeIt = lTimes.begin();
	while (lPathIt != lPathsInd.end()) {
		DEB(LVL_ALGDET, "Algorithms::fDijkstraUgPaths: processing " << lPathsInd.size() << " paths" << endl);
		DEB(LVL_ALGDETT, "Algorithms::fDijkstraUgPaths: ------------------" << endl);
		list<int>::iterator lHelpTimeIt = lTimes.begin();
		for (list<vector<int>* >::iterator i = lPathsInd.begin(); i != lPathsInd.end(); i++) {
			DEB(LVL_ALGDETT, "Algorithms::fDijkstraUgPaths: " << fGetVectorString(**i) << endl);
			DEB(LVL_ALGDETT, "Algorithms::fDijkstraUgPaths: " << *lHelpTimeIt << endl);
			lHelpTimeIt++;
		}

		int lLastVertex = (**lPathIt)[(*lPathIt)->size() - 1];
		int lTime = *lTimeIt;

		//complete path
		if (lLastVertex == iFrom) {
			lPathIt++;
			lTimeIt++;
			continue;
		}

		DEB(LVL_ALGDETT, "Algorithms::fDijkstraUgPaths: Current path: " <<
				fGetVectorString(**lPathIt) << endl);
		bool lFirst = true;
		vector<int> lCities = iArrows[lLastVertex].fGetCities();
		for (size_t i = 0; i < lCities.size(); i++) {
			bool lRepetition = false;
			for (size_t j = 0; j < (*lPathIt)->size(); j++) {
				if ((**lPathIt)[j] == lCities[i]) {
					lRepetition = true;
					break;
				}
			}
			if (lRepetition) {
				continue;
			}
			pair<int, int> lArrow = iArrows[lLastVertex].fGet(lTime, lCities[i]);
			if (lArrow.first == -1 || lArrow.first < lArrow.second) {
				continue;
			}
			if (lFirst) {
				//add to this path the next vertex
				(*lPathIt)->push_back(lCities[i]);
				*lTimeIt = lArrow.second;

				lFirst = false;
			}
			else {
				//make a new path
				vector<int> *lNewPathInd = new vector<int>();
				lNewPathInd->insert(lNewPathInd->end(), (*lPathIt)->begin(), (*lPathIt)->end());
				lNewPathInd->pop_back();
				lNewPathInd->push_back(lCities[i]);
				lPathsInd.push_back(lNewPathInd);

				lTimes.push_back(lArrow.second);
			}
		}

		//could not continue given path - remove it
		if (lFirst) {
			list<vector<int>* >::iterator lPathToErase = lPathIt;
			list<int>::iterator lTimeToErase = lTimeIt;
			lPathIt++;
			lTimeIt++;
			lPathsInd.erase(lPathToErase);
			lTimes.erase(lTimeToErase);
			continue;
		}
	}

	//make actual Path objects
	vector<Path> lPaths;
	lPathIt = lPathsInd.begin();
	int lTotal = 0;
	while (lPathIt != lPathsInd.end()) {
		lTotal++;

		//reverse indices
		reverse((*lPathIt)->begin(), (*lPathIt)->end());

		//make and add Path object
		Path lPath(**lPathIt);
		DEB(LVL_ALGDET, "Algorithms::fDijkstraUgPaths: " << lPath.fGetString() << endl);
		lPaths.push_back(lPath);

		//destroy vector of indices
		delete *lPathIt;

		//move on
		lPathIt++;
	}

	DEB(LVL_ALGDET, "Algorithms::fDijkstraUgPaths: " << lTotal << " paths to " << iTo << endl);

	return lPaths;
}

/* Depending on iWannaUgPaths finds:
 * - iWannaUgPaths == false: all shortest paths leading to all cities specified by argument
 * - iWannaUgPaths == true: all underlying SP leading to all cities specified by argument
 *
 * The offset node is specified by iFrom and iTime - in TE graphs, iTime is -1 (and simply not used)
 *
 * iToCityInd contains indices of cities we are interested in. The indices map to iAllCities - a
 * vector of all cities in the graph, which, when not provided, will be extracted from the graph */
vector<vector<Path> > fDijkstraMultiSpToCities(TimeGraphObject *iGraph, int iFrom, int iTime,
		vector<int> iToCityInd, vector<string> *iAllCities, bool iWannaUgPaths, bool iHasWeights) {
	int n = iGraph->fGetN();

	//get all cities if not provided in args
	vector<string> lAllCities;
	vector<string> *lAllCitiesPtr;
	if (iAllCities == NULL) {
		lAllCities = iGraph->fGetCities();
		lAllCitiesPtr = &lAllCities;
	}
	else {
		lAllCitiesPtr = iAllCities;
	}

	//number of all cities
	int a = (*lAllCitiesPtr).size();

	//number of to-cities (those we are interested in)
	int t = iToCityInd.size();

	//back translations for to-city: to-city index -> index in iToCityInd
	vector<int> lToCityIndex(a, -1);
	for (size_t i = 0; i < iToCityInd.size(); i++) {
		lToCityIndex[iToCityInd[i]] = i;
	}

	//we need also node-index -> city index
	vector<int> lNodeToCity;
	unordered_map<string, int> lCityToIndex;
	for (size_t i = 0; i < (*lAllCitiesPtr).size(); i++) {
		lCityToIndex[(*lAllCitiesPtr)[i]] = i;
	}
	for (int i = 0; i < n; i++) {
		lNodeToCity.push_back(lCityToIndex[iGraph->fGetNodeName(i)]);
	}

	//initialize distance vector - distance
	vector<int> lDistances(n + a, -1);
	lDistances[iFrom] = 0;
	lDistances[n + lNodeToCity[iFrom]] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(iFrom);
	lDistancesHeap.fUpdate(n + lNodeToCity[iFrom]);

	//initialize came-from vector
	//we have 'a' extra dummy nodes at the end - one for each city
	vector<vector<int> > lCameFrom(n + a , vector<int>());

	//used only with UG paths.
	//for each city, stores for each of its neighbors the arrows arriving from that neighbor
	vector<ArrowsToCity> lArrows(a, ArrowsToCity());

	//initialize sets
	vector<bool> lSettledCities(a, false); //those cities with final distance
	lSettledCities[lNodeToCity[iFrom]] = true;
	int lSettledCount = 0; //number of settled to-cities
	if (lToCityIndex[lNodeToCity[iFrom]] != -1) {
		lSettledCount++;
	}
	unordered_set<int> lSettled; //those vertices that are visible, but not settled
	unordered_set<int> lExplored; //those vertices that are visible, but not settled
	lExplored.insert(iFrom);

	while (1) {
		int lBest = -1;
		int lBestDistance = -1;

		//no more explored vertices
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion
		if (lBest == -1) {
			break;
		}

		DEB(LVL_ALGS, "Algorithms::fDijkstraMultiSpToCities: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//if it is the first time we visit a city, mark the city as settled
		if (lSettledCities[lNodeToCity[lBest]] == false) {
			lSettledCities[lNodeToCity[lBest]] = true;
			if (lToCityIndex[lNodeToCity[lBest]] != -1) {
				lSettledCount++;
			}
			if (iWannaUgPaths == false) {
				lCameFrom[n + lNodeToCity[lBest]].push_back(lBest);
			}
		}
		if (lSettledCount == t) {
			break;
		}

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;

			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}

			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (iHasWeights == false) {
				lLength = 1;
			}
			if (lLength == -1) {
				continue;
			}
			if (iGraph->fGetObjectType() == OTTd && iTime == -1) {
				lLength = 1;
			}
			//we want real paths
			if (iWannaUgPaths == false) {
				if (lDistances[lToIndex] == -1 || lDistances[lToIndex] > lDistances[lBest] + lLength) {
					DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToCities: Distance to best " <<
							lDistances[lBest] << endl);
					DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToCities: Length " << lLength << endl);
					DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToCities: Distance to being updated " <<
							lDistances[lToIndex] << endl);

					lDistances[lToIndex] = lDistances[lBest] + lLength;
					lDistancesHeap.fUpdate(lToIndex);
					lCameFrom[lToIndex].clear();
					lCameFrom[lToIndex].push_back(lBest);

					lExplored.insert(lToIndex);
				}
				else if (lDistances[lToIndex] == lDistances[lBest] + lLength) {
					lCameFrom[lToIndex].push_back(lBest);

					lExplored.insert(lToIndex);
				}
			}
			//we want UG paths
			else {
				if (lDistances[lToIndex] == -1 || lDistances[lToIndex] >= lDistances[lBest] + lLength) {
					DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToCities: Distance to best " <<
							lDistances[lBest] << endl);
					DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToCities: Length " << lLength << endl);
					DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToCities: Distance to being updated " <<
							" (" << lToIndex << ") " << lDistances[lToIndex] << endl);

					lDistances[lToIndex] = lDistances[lBest] + lLength;
					lDistancesHeap.fUpdate(lToIndex);

					lExplored.insert(lToIndex);
				}

				if (lDistances[n + lNodeToCity[lToIndex]] == -1 ||
					lDistances[n + lNodeToCity[lToIndex]] > lDistances[lBest] + lLength) {
					DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToCities: Distance to best " <<
						lDistances[lBest] << endl);
					DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToCities: Length " << lLength << endl);
					DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToCities: Distance to city being updated "
							" (" << (*lAllCitiesPtr)[lNodeToCity[lToIndex]] <<
							") " << lDistances[n + lNodeToCity[lToIndex]] << endl);

					lDistances[n + lNodeToCity[lToIndex]] = lDistances[lBest] + lLength;
				}

				if (lNodeToCity[lToIndex] != lNodeToCity[lBest]) {
					lArrows[lNodeToCity[lToIndex]].fAdd(lNodeToCity[lBest], lDistances[lBest] + lLength,
							lDistances[lBest]);
				}
			}
		}
	}

	DEB(LVL_ALGS, "Algorithms::fDijkstraMultiSpToAll: Going to create the paths" << endl);

	for (size_t i = 0; i < lArrows.size(); i++) {
		lArrows[i].fSort();
		DEB(LVL_ALGDETT, "Algorithms::fDijkstraMultiSpToAll: To " << i << "-----------------" << endl);
		DEB(LVL_ALGDETT, "Algorithms::fDijkstraMultiSpToAll: " << lArrows[i].fGetString() << endl);
	}

	DEB(LVL_ALGDETT, "Algorithms::fDijkstraMultiSpToAll: Cities: " << endl);
	for (size_t i = 0; i < iAllCities->size(); i++) {
		DEB(LVL_ALGDETT, "Algorithms::fDijkstraMultiSpToAll:	" << i << ": " << (*iAllCities)[i] << endl);
	}

	//get the paths
	vector<vector<Path> > lPaths;
	for (int i = 0; i < t; i++) {
		DEB(LVL_ALGDET, "Algorithms::fDijkstraMultiSpToAll: Getting paths from " << iFrom <<
				" at time " << iTime << "( " << iGraph->fGetNodeId(iFrom) <<
				" ) to city " << iToCityInd[i] << " (" <<
				(*lAllCitiesPtr)[iToCityInd[i]] << ")" << endl);
		vector<Path> lPathsToI;
		if (iWannaUgPaths == false) {
			lPathsToI = fDijkstraPaths(iGraph, lCameFrom, iFrom, n + iToCityInd[i], false);
			if (lNodeToCity[iFrom] == iToCityInd[i]) {
				lPathsToI = vector<Path>(1, Path(vector<int>(1, iFrom)));
			}
		}
		else {
			lPathsToI = fDijkstraUgPaths(iGraph, lArrows, lNodeToCity[iFrom],
					iToCityInd[i], lDistances[n + iToCityInd[i]]);
		}
		lPaths.push_back(lPathsToI);
	}

	return lPaths;
}

Path fDijkstraRestr(TdGraph *iGraph, int iFrom, int iTime,
		int iTo, unordered_set<int> &iRestr) {

	int n = iGraph->fGetN();

	vector<int> lDecompr(iRestr.size(), -1);
	int lCounter = 0;
	for (unordered_set<int>::iterator i = iRestr.begin(); i != iRestr.end(); i++) {
		lDecompr[lCounter] = *i;
		lCounter++;
	}
	unordered_map<int, int> lCompr;
	for (size_t i = 0; i < lDecompr.size(); i++) {
		lCompr[lDecompr[i]] = i;
	}

	//initialize distance vector - best distance to node and to city
	vector<int> lDistances(iRestr.size(), -1);
	lDistances[lCompr[iFrom]] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(lCompr[iFrom]);

	//initialize came-from vector
	vector<int> lCameFrom = vector<int>(iRestr.size(), -1);
	lCameFrom[lCompr[iFrom]] = iFrom;

	unordered_set<int> lSettled;
	unordered_set<int> lExplored; //those vertices that are visible, but not settled
	lExplored.insert(iFrom);

	bool lFound = false;
	while (1) {
		int lComprBest = -1;
		int lBestDistance = -1;

		//no more explored vertices
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lComprBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion - no more vertices to settle
		if (lComprBest == -1) {
			break;
		}

		int lBest = lDecompr[lComprBest];

		DEB(LVL_ALGS, "Algorithms::fDijkstraRestr: Best vertex " << lBest << endl);

		if (lBest == iTo) {
			lFound = true;
			break;
		}

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			DEB(LVL_ALGDET, "Algorithms::fDijkstraRestr: To index " << lToIndex << " (city " <<
					iGraph->fGetNodeName(lToIndex) << ")" << endl);

			if (iRestr.find(lToIndex) == iRestr.end()) {
				continue;
			}
			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}

			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (lLength == -1) {
				continue;
			}

			int lComprToIndex = lCompr[lToIndex];

			//we want UG paths
			if (lDistances[lComprToIndex] == -1 || lDistances[lComprToIndex] >=
					lDistances[lComprBest] + lLength) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstraRestr: Distance to best " <<
						lDistances[lComprBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraRestr: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraRestr: Distance to being updated " <<
						lDistances[lComprToIndex] << endl);

				lDistances[lComprToIndex] = lDistances[lComprBest] + lLength;
				lCameFrom[lComprToIndex] = lBest;
				lDistancesHeap.fUpdate(lComprToIndex);

				lExplored.insert(lToIndex);
			}
		}
	}

	if (lFound == false) {
		return Path();
	}

	//get the path
	vector<int> lPathVertices;
	int lPathIndex = iTo;
	int lComprPathIndex = lCompr[lPathIndex];
	while (lCameFrom[lComprPathIndex] != lPathIndex) {
		lPathVertices.push_back(lPathIndex);
		lPathIndex = lCameFrom[lComprPathIndex];
		lComprPathIndex = lCompr[lPathIndex];
	}
	lPathVertices.push_back(lPathIndex);
	reverse(lPathVertices.begin(), lPathVertices.end());

	Path lPath(lPathVertices);
	return lPath;
}

vector<Path> fDijkstraToCitiesRestr(TdGraph *iGraph, int iFrom, int iTime,
		vector<int> &iToCities, unordered_set<int> &iRestr) {
	int n = iGraph->fGetN();

	//number of to-cities (those we are interested in)
	int t = iToCities.size();

	unordered_set<int> lToCities(iToCities.begin(), iToCities.end());

	vector<int> lDecompr(iRestr.size(), -1);
	int lCounter = 0;
	for (unordered_set<int>::iterator i = iRestr.begin(); i != iRestr.end(); i++) {
		lDecompr[lCounter] = *i;
		lCounter++;
	}
	unordered_map<int, int> lCompr;
	for (size_t i = 0; i < lDecompr.size(); i++) {
		lCompr[lDecompr[i]] = i;
	}

	//initialize distance vector - best distance to node and to city
	vector<int> lDistances(iRestr.size(), -1);
	lDistances[lCompr[iFrom]] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(lCompr[iFrom]);

	//initialize came-from vector
	//come-from[city] = index of a node we came from to the city
	vector<int> lCameFrom = vector<int>(iRestr.size(), -1);
	lCameFrom[lCompr[iFrom]] = iFrom;

	//initialize sets, we explore the graph as in default Dijkstra, only the finish criterion differs
	unordered_set<int> lSettledCities; //those cities with final distance
	lSettledCities.insert(iFrom);
	int lSettledCount = 0; //number of settled to-cities
	if (lToCities.count(iFrom) != 0) {
		lSettledCount++;
	}
	unordered_set<int> lSettled;
	unordered_set<int> lExplored; //those vertices that are visible, but not settled
	lExplored.insert(iFrom);

	while (1) {
		int lComprBest = -1;
		int lBestDistance = -1;

		//no more explored vertices
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lComprBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//finish criterion - no more vertices to settle
		if (lComprBest == -1) {
			break;
		}

		int lBest = lDecompr[lComprBest];

		DEB(LVL_ALGS, "Algorithms::fDijkstraToCitiesRestr: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//if it is the first time we visit a city, mark the city as settled
		if (lSettledCities.find(lBest) == lSettledCities.end()) {
			lSettledCities.insert(lBest);
			if (lToCities.find(lBest) != lToCities.end()) {
				lSettledCount++;
			}
		}

		//on settling all to-cities, we can stop
		if (lSettledCount == t) {
			break;
		}

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			DEB(LVL_ALGDET, "Algorithms::fDijkstraToCitiesRestr: To index " << lToIndex << " (city " <<
					iGraph->fGetNodeName(lToIndex) << ")" << endl);

			if (iRestr.find(lToIndex) == iRestr.end()) {
				continue;
			}
			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}

			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (lLength == -1) {
				continue;
			}

			int lComprToIndex = lCompr[lToIndex];

			//we want UG paths
			if (lDistances[lComprToIndex] == -1 || lDistances[lComprToIndex] >=
					lDistances[lComprBest] + lLength) {
				DEB(LVL_ALGDET, "Algorithms::fDijkstraToCitiesRestr: Distance to best " <<
						lDistances[lComprBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraToCitiesRestr: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fDijkstraToCitiesRestr: Distance to being updated " <<
						lDistances[lComprToIndex] << endl);

				lDistances[lComprToIndex] = lDistances[lComprBest] + lLength;
				lCameFrom[lComprToIndex] = lBest;
				lDistancesHeap.fUpdate(lComprToIndex);

				lExplored.insert(lToIndex);
			}
		}
	}


	//get the paths
	vector<Path> lPaths;
	for (size_t i = 0; i < iToCities.size(); i++) {
		if (lCameFrom[lCompr[iToCities[i]]] == -1) {
			lPaths.push_back(Path());
			continue;
		}

		int lPathIndex = iToCities[i];
		int lComprPathIndex = lCompr[lPathIndex];
		vector<int> lIndices;
		while (lCameFrom[lComprPathIndex] != lPathIndex) {
			lIndices.push_back(lPathIndex);
			lPathIndex = lCameFrom[lComprPathIndex];
			lComprPathIndex = lCompr[lPathIndex];
		}
		lIndices.push_back(lPathIndex);
		reverse(lIndices.begin(), lIndices.end());

		Path lPath(lIndices);
		lPaths.push_back(lPath);
	}

	return lPaths;
}

/*****************************************************************/
/* Floyd Warshall
/*****************************************************************/

/* reconstructs a path from the iNext and iDist arrays */
vector<int> fFWPath(int iFrom, int iTo, vector<vector<int> > &iNext, vector<vector<int> > &iDist) {
	DEB(LVL_ALGDET, "Algorithms::fFWPath: From = " << iFrom << ", To = " << iTo << endl);
	//same vertex
	if (iFrom == iTo) {
		return vector<int>();
	}

	//no path
	if (iDist[iFrom][iTo] == -1) {
		return vector<int>();
	}

	list<int> lPath;
	lPath.insert(lPath.end(), iFrom);
	lPath.insert(lPath.end(), iTo);
	list<int>::iterator lBeforeEnd = lPath.end();
	lBeforeEnd--;

	for(list<int>::iterator i = lPath.begin(); i != lBeforeEnd; i++) {
		list<int>::iterator lNextIt = i;
		lNextIt++;
		int lNext = iNext[*i][*lNextIt];
		DEB(LVL_ALGDET, "Algorithms::fFWPath: Next = " << lNext << endl);
		if (lNext == -1) {
			continue;
		}
		lPath.insert(lNextIt, lNext);
		i--;
		DEB(LVL_ALGDET, "Algorithms::fFWPath: So far path = " << fGetListString(lPath) << endl);
	}

	vector<int> lResult = vector<int>(lPath.begin(), lPath.end());
	return lResult;
}

/* returns shortest (and minimum hop) path for each pair of vertices in the graph.  */
vector<vector<Path> > fFloydWarshall(GraphObject *iGraph, bool iHasWeights) {
	int n = iGraph->fGetN();

	INFO("Initialization of Floyd Warshall algorithm" << endl);

	//initialize
	vector<vector<int> > lNext(n, vector<int>());
	vector<vector<int> > lDist(n, vector<int>());
	vector<vector<int> > lHops(n, vector<int>());
	vector<vector<Path> > lPaths(n, vector<Path>());
	for (vector<vector<int> >::size_type i = 0; i < n; i++) {
		lNext[i] = vector<int>(n, -1);
		lDist[i] = vector<int>(n, -1);
		lHops[i] = vector<int>(n, 0);
		lPaths[i] = vector<Path>(n, Path());

		lDist[i][i] = 0;
	}
	for (int i = 0; i < n; i++) {
		vector<int> lTo = iGraph->fGetTo(i);
		for (vector<int>::iterator j = lTo.begin(); j != lTo.end(); j++) {
			lDist[i][*j] = 1;
			if (iHasWeights) {
				lDist[i][*j] = iGraph->fGetLength(i, *j, -1);
			}
		}
	}

	INFO("Iteration part of Floyd Warshall algorithm" << endl);
	//iterate
	int lOldPercents = -1;
	for (int k = 0; k < n; k++) {
		int lPercents = (k * 100) / n;
		if (lPercents != lOldPercents) {
			INFO("Progress " << lPercents << "%" << endl);
			lOldPercents = lPercents;
		}

		DEB(LVL_ALGS, "Algorithms::fFloydWarshall: k = " << k << endl);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (lDist[i][k] == -1 || lDist[k][j] == -1) {
					continue;
				}
				if (lDist[i][j] == -1 || lDist[i][k] + lDist[k][j] < lDist[i][j]) {
					lDist[i][j] = lDist[i][k] + lDist[k][j];
					lNext[i][j] = k;

					lHops[i][j] = lHops[i][k] + lHops[k][j] + 1;
				}
				else if (lDist[i][k] + lDist[k][j] == lDist[i][j] &&
						lHops[i][k] + lHops[k][j] + 1 < lHops[i][j]) {
					lNext[i][j] = k;

					lHops[i][j] = lHops[i][k] + lHops[k][j] + 1;
				}
			}
		}
	}

	INFO("Creating paths" << endl);

	//create paths
	lOldPercents = -1;
	for (int i = 0; i < n; i++) {
		int lPercents = (i * 100) / n;
		if (lPercents != lOldPercents) {
			INFO("Progress " << lPercents << "%" << endl);
			lOldPercents = lPercents;
		}

		DEB(LVL_ALGS, "Algorithm::fFloydWarshall: Creating paths progress " << i << " / " << n << endl);
		for (int j = 0; j < n; j++) {
			DEB(LVL_ALGDET, "Algorithms::fFloydWarshall: From = " << i << ", To = " << j << endl);
			vector<int> lVertices = fFWPath(i, j, lNext, lDist);
			lPaths[i][j] = Path(lVertices);
		}
	}

	return lPaths;
}

/* returns all shortest paths between two vertices */
vector<Path> fFWPaths(int iFrom, int iTo, vector<vector<vector<int> > > *iNext,
		vector<vector<int> > *iDist) {
	//DEB(LVL_ALGS, "Algorithms::fFWPath: From = " << iFrom << ", To = " << iTo << endl);
	//same vertex
	if (iFrom == iTo) {
		return vector<Path>();
	}

	//no path
	if ((*iDist)[iFrom][iTo] == -1) {
		return vector<Path>();
	}

	vector<list<int>* > lPaths;
	vector<list<int>::iterator > lIterators;
	list<int> *lPath = new list<int>();
	lPath->push_back(iFrom);
	lPath->push_back(iTo);
	lPaths.push_back(lPath);
	lIterators.push_back(lPaths[0]->begin());

	//go through all the list of paths
	for (vector<list<int> >::size_type j = 0; j < lPaths.size(); j++) {
		list<int>::iterator lBeforeEnd = lPaths[j]->end();
		lBeforeEnd--;

		//determine the iterator offset
		int lIteratorOffset = 0;
		list<int>::iterator lCountingIt = lIterators[j];
		while (lCountingIt != lPaths[j]->begin()) {
			lIteratorOffset++;
			lCountingIt--;
		}

		//process one path
		for (list<int>::iterator i = lIterators[j]; i != lBeforeEnd; i++) {
			list<int>::iterator lNextIt = i;
			lNextIt++;
			vector<int> *lNext = &((*iNext)[*i][*lNextIt]);
			DEB(LVL_ALGDET, "Algorithms::fFWPaths: Next[" << *i << "][" << *lNextIt << "] size is " <<
					lNext->size() << endl);

			//consider all next values
			for (int k = lNext->size() - 1; k >= 0; k--) {
				int lNextValue = (*lNext)[k];
				DEB(LVL_ALGDET, "Algorithms::fFWPaths: k = " << k << ",  Next = " << lNextValue << endl);

				//work on current path
				if (k == 0) {
					lPaths[j]->insert(lNextIt, lNextValue);
					i--;
					lIteratorOffset--;
				}
				//create new path
				else {
					list<int> *lNewList = new list<int>(*lPaths[j]);
					list<int>::iterator lNewIterator = lNewList->begin();
					for (int m = 0; m <= lIteratorOffset; m++) {
						lNewIterator++;
					}
					lNewList->insert(lNewIterator, lNextValue);
					lNewIterator--;
					lNewIterator--;
					lPaths.push_back(lNewList);
					lIterators.push_back(lNewIterator);
				}
			}

			lIteratorOffset++;
		}
	}

	vector<Path> lResult;
	for (vector<list<int>* >::size_type i = 0; i < lPaths.size(); i++) {
		DEB(LVL_ALGDET, "Algorithms::fFWPaths: Path " << i << ": " << fGetListString(*(lPaths[i])) << endl);
		lResult.push_back(Path(vector<int>(lPaths[i]->begin(), lPaths[i]->end())));
		delete lPaths[i];
	}

	return lResult;
}

/* returns all shortest paths in the graph */
vector<vector<vector<Path> > > fMultiFloydWarshall(GraphObject *iGraph, bool iHasWeights) {
	int n = iGraph->fGetN();

	bool lHasZeroCycle = fHasCycle(iGraph, 0, 0);
	if (iHasWeights == false) {
		lHasZeroCycle = false;
	}

	INFO("Initialization of Multi Floyd Warshall algorithm. Has cycle = " << gBoolStr[lHasZeroCycle] <<
			endl);

	//initialize
	vector<vector<vector<int> > > lNext(n, vector<vector<int> >());
	vector<vector<int> > lDist(n, vector<int>());
	vector<vector<bool> > lArc(n, vector<bool>());
	vector<vector<vector<Path> > > lPaths(n, vector<vector<Path> >());
	for (vector<vector<int> >::size_type i = 0; i < n; i++) {
		lNext[i] = vector<vector<int> >(n, vector<int>());
		lDist[i] = vector<int>(n, -1);
		lArc[i] = vector<bool>(n, false);
		lPaths[i] = vector<vector<Path> >(n, vector<Path>());

		lDist[i][i] = 0;
	}
	for (int i = 0; i < n; i++) {
		vector<int> lTo = iGraph->fGetTo(i);
		for (vector<int>::iterator j = lTo.begin(); j != lTo.end(); j++) {
			lArc[i][*j] = true;
			lDist[i][*j] = 1;
			if (iHasWeights) {
				lDist[i][*j] = iGraph->fGetLength(i, *j, -1);
			}
		}
	}

	INFO("Iteration part of Multi Floyd Warshall algorithm" << endl);
	//iterate
	int lOldPercents = -1;
	for (int k = 0; k < n; k++) {
		int lPercents = (k * 100) / n;
		if (lPercents != lOldPercents) {
			INFO("Progress " << lPercents << "%" << endl);
			lOldPercents = lPercents;
		}

		DEB(LVL_ALGS, "Algorithms::fMultiFloydWarshall: k = " << k << endl);
		for (int i = 0; i < n; i++) {
			//checking if i -> j could be improved through k
			for (int j = 0; j < n; j++) {
				//skip cases when i == j, or k is one of them
				if (i == j || i == k || k == j) {
					continue;
				}
				//disregard case when one or both distances are infinite
				if (lDist[i][k] < 0 || lDist[k][j] < 0) {
					continue;
				}
				//we found a better i -> j path, do an update
				if (lDist[i][j] == -1 || lDist[i][k] + lDist[k][j] < lDist[i][j]) {
					lDist[i][j] = lDist[i][k] + lDist[k][j];
					lNext[i][j].clear();
					lNext[i][j].push_back(k);
					continue;
				}
				//disregard case when one or both distances are zero in case of an existing zero cycle
				//in the graph (could cause infinite loops)
				if (lHasZeroCycle && (lDist[i][k] == 0 || lDist[k][j] == 0)) {
					continue;
				}
				/* we found an alternative for i -> j: through k.
				 * - However, we support only "local forks" - next[i][j] array may contain more values only
				 * if they are all neighbors of i (at the end of arcs starting at i).
				 * This is done in order to remove duplicity when reconstructing paths */
//				if (lDist[i][k] + lDist[k][j] == lDist[i][j] &&
//						 lArc[i][k] == true && lNext[i][j].size() != 0 && lArc[i][lNext[i][j][0]]) {
//					lNext[i][j].push_back(k);
//				}
			}
		}
	}

	INFO("Creating paths" << endl);

	//create paths
	lOldPercents = -1;
	for (int i = 0; i < n; i++) {
		int lPercents = (i * 100) / n;
		if (lPercents != lOldPercents) {
			INFO("Progress " << lPercents << "%" << endl);
			lOldPercents = lPercents;
		}

		DEB(LVL_ALGS, "Creating paths progress: " << i << "/" << n << endl);
		for (int j = 0; j < n; j++) {
			DEB(LVL_ALGS, "Algorithms::fMultiFloydWarshall: From = " << i << ", To = " << j << endl);
			vector<Path> lIJPaths = fFWPaths(i, j, &lNext, &lDist);
			DEB(LVL_ALGS, "Algorithms::fMultiFloydWarshall: " << lIJPaths.size() << " paths found" << endl);
			lPaths[i][j] = lIJPaths;
		}
	}

	return lPaths;
}

/*****************************************************************/
/* Betweenness
/*****************************************************************/

/* returns betweenness centrality for each node. output is normalized (1 = max betw. 0 = min betw.) */
vector<double> fBetweenness(GraphObject *iGraph, vector<double> *oBetwPercents, bool iHasWeights) {
	//get ALL shortest paths
	vector<vector<vector<Path> > > lPaths = fDijkstraAllMultiSp(iGraph, iHasWeights);

	if (oBetwPercents != NULL) {
		oBetwPercents->clear();
		oBetwPercents->insert(oBetwPercents->end(), iGraph->fGetN(), 0);
	}

	//get values
	int lPathsCount = 0;
	vector<double> lResult (iGraph->fGetN(), 0);
	for (size_t i = 0; i < lPaths.size(); i++) {
		for (size_t j = 0; j < lPaths[i].size(); j++) {
			double lAddition = (double)1 / (double)lPaths[i][j].size();
			for (size_t k = 0; k < lPaths[i][j].size(); k++) {
				lPathsCount++;
				Path *lPath = &(lPaths[i][j][k]);
				for (int l = 1; l < lPath->fGetSize() - 1; l++) {
					lResult[lPath->fGet(l)] += lAddition;
					if (oBetwPercents != NULL) {
						(*oBetwPercents)[lPath->fGet(l)] += 1;
					}
				}
			}
		}
	}

	if (oBetwPercents != NULL) {
		for (size_t i = 0; i < oBetwPercents->size(); i++) {
			(*oBetwPercents)[i] = (*oBetwPercents)[i] / lPathsCount;
		}
	}

	INFO(lPathsCount << " of shortest paths used for betweenness calculation" << endl);

	//re-scale
	lResult = fNormalize(lResult, 0, 1);

	return lResult;
}

/* returns approximate betweenness centrality for each node. Precision is a double from 0 to 1 */
vector<double> fApxBetweenness(GraphObject *iGraph, vector<double> *oBetwPercents,
		double iPrecision, bool iHasWeights) {
	if (iPrecision <= 0 || iPrecision > 1) {
		ERR("Algorithms::fApxBetweenness: Precision is out of range" << endl);
		return vector<double>();
	}

	srand(time(NULL));

	if (oBetwPercents != NULL) {
		oBetwPercents->clear();
		oBetwPercents->insert(oBetwPercents->end(), iGraph->fGetN(), 0);
	}

	//get random vertices from which we are going to look for shortest paths
	int lN = iGraph->fGetN();
	vector<int> lPerm = fRandomPermutation(lN);
	int lConsider = lN * iPrecision;

	//get some shortest paths
	vector<Path> lPaths;
	vector<vector<double> > lAdditions (lN, vector<double>(lN, 0));
	int lOldPercents = -1;
	for (int i = 0; i < lConsider; i++) {
		int lPercents = (i * 100) / lConsider;
		if (lPercents != lOldPercents) {
			INFO("Progress " << lPercents << "%" << endl);
			lOldPercents = lPercents;
		}
		vector<Path> lCurrentPaths = fDijkstraRandomSpToAll(iGraph, lPerm[i], -1, iHasWeights);
		for (size_t j = 0; j < lCurrentPaths.size(); j++) {
			if (lCurrentPaths[j].fIsEmpty()) {
				continue;
			}
			lAdditions[lCurrentPaths[j].fGetFirst()][lCurrentPaths[j].fGetLast()] += 1;
			lPaths.push_back(lCurrentPaths[j]);
		}
	}
	for (int i = 0; i < lN; i++) {
		for (int j = 0; j < lN; j++) {
			if (lAdditions[i][j] < 0.5) {
				continue;
			}
			lAdditions[i][j] = 1 / lAdditions[i][j];
		}
	}

	INFO(lPaths.size() << " of shortest paths used for betweenness approximation" << endl);

	//get values
	vector<double> lResult (iGraph->fGetN(), 0);
	for (size_t i = 0; i < lPaths.size(); i++) {
		double lAddition = lAdditions[lPaths[i].fGetFirst()][lPaths[i].fGetLast()];
		for (int j = 1; j < lPaths[i].fGetSize() - 1; j++) {
			lResult[lPaths[i].fGet(j)] += lAddition;
			if (oBetwPercents != NULL) {
				(*oBetwPercents)[lPaths[i].fGet(j)] += 1;
			}
		}
	}
	if (oBetwPercents != NULL) {
		for (size_t i = 0; i < oBetwPercents->size(); i++) {
			(*oBetwPercents)[i] /= lPaths.size();
		}
	}

	//re-scale
	lResult = fNormalize(lResult, 0, 1);

	return lResult;
}

/*****************************************************************/
/* Radius and HD
/*****************************************************************/

/* returns the vector of node indices, that are within the radius of a node iNode at time iTime.
 * uses Dijkstra's algorithm to find the vertices in the radius */
unordered_set<int> fNodesInRadius(GraphObject *iGraph, int iNode, int iTime, int iRadius, bool iHasWeights) {
	int n = iGraph->fGetN();

	DEB(LVL_ALGS, "Algorithms::fNodesInRadius: Checking radius for node " << iNode << " at time " <<
			iTime << endl);

	//initialize distance vector
	vector<int> lDistances;
	for (vector<int>::size_type i = 0; i < n; i++) {
		lDistances.push_back(-1);
	}
	lDistances[iNode] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(iNode);

	//initialize sets
	unordered_set<int> lSettled; //those with final distance
	unordered_set<int> lExplored; //those that are visible, but not settled
	lExplored.insert(iNode);

	bool lFound = false;
	while (1) {
		int lBest = -1;
		int lBestDistance = -1;
		//no more explored vertices - probably not connected
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		if (lBestDistance > iRadius) {
			break;
		}

		//finish criterion
		if (lBest == -1) {
			break;
		}

		DEB(LVL_ALGS, "Algorithms::fNodesInRadius: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//update
		vector<int> lBestTo = iGraph->fGetTo(lBest);
		for (vector<int>::iterator i = lBestTo.begin(); i != lBestTo.end(); i++) {
			int lToIndex = *i;
			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (iHasWeights == false) {
				lLength = 1;
			}
			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}
			if (lDistances[lToIndex] == -1 || lDistances[lToIndex] > lDistances[lBest] + lLength ||
				lDistances[lToIndex] == lDistances[lBest] + lLength) {

				lDistances[lToIndex] = lDistances[lBest] + lLength;
				lDistancesHeap.fUpdate(lToIndex);

				lExplored.insert(lToIndex);
			}
		}
	}

	DEB(LVL_ALGS, "Algorithms::fNodesInRadius: Found vertices in radius " << iRadius << ": " << endl);
	DEB(LVL_ALGS, "	" << fGetSetString(lSettled) << endl);

	return lSettled;
}

/* returns shortest paths in the radius of a node iNode at the time iTime, satisfying iMinPathLength */
vector<Path> fSpsInRadius(GraphObject *iGraph, int iNode, int iTime,
		int iRadius, int iMinPathLength, bool iMulti, double iPrecision, bool iHasWeights) {
	//get subset that correspond to the radius
	unordered_set<int> lSubset = fNodesInRadius(iGraph, iNode, iTime, iRadius, iHasWeights);
	vector<int> lVecSubset(lSubset.begin(), lSubset.end());

	//get the shortest paths in the subset
	vector<Path> lPaths = fSpsForSubset(iGraph, lVecSubset, iMinPathLength, iMulti, iPrecision, iHasWeights);

	//translate the subset-indices of the paths back to graph-indices
	for (size_t i = 0; i < lPaths.size(); i++) {
		Path *lPath = &(lPaths[i]);
		for (int l = 0; l < lPath->fGetSize(); l++) {
			lPath->fSet(l, lVecSubset[lPath->fGet(l)]);
		}
	}

	return lPaths;
}

/* returns shortest paths for each pair of nodes in the subset. only paths completely within
 * the subset and satisfying iMinPathLength are output.
 *
 * paths returned are NOT translated -> all indices are within range o..iSubset.size() */
vector<Path> fSpsForSubset(GraphObject *iGraph, vector<int> &iSubset, int iMinPathLength,
		bool iMulti, double iPrecision, bool iHasWeights) {
	//create a subgraph with subset indices
	GraphObject *lSubGraph = iGraph->fSubGraph(iSubset);

	//run algorithm to find all shortest paths on this subgraph
	vector<Path> lFilteredPaths;
	int lTotal = 0;
	if (iMulti) {
		gDefaultLogger->fInfoOn(false);
		vector<vector<vector<Path> > > lPaths = fDijkstraAllMultiSp(lSubGraph, iHasWeights);
		gDefaultLogger->fInfoOn(true);
		for (size_t i = 0; i < lPaths.size(); i++) {
			for (size_t j = 0; j < lPaths[i].size(); j++) {
				for (size_t k = 0; k < lPaths[i][j].size(); k++) {
					lTotal++;
					if (lSubGraph->fGetPathLength(lPaths[i][j][k]) < iMinPathLength) {
						continue;
					}
					lFilteredPaths.push_back(lPaths[i][j][k]);
				}
			}
		}
	}
	else {
		gDefaultLogger->fInfoOn(false);
		vector<Path> lPaths = fGetSps(lSubGraph, iPrecision, iHasWeights);
		gDefaultLogger->fInfoOn(true);
		for (size_t i = 0; i < lPaths.size(); i++) {
			lTotal++;
			if (lSubGraph->fGetPathLength(lPaths[i]) < iMinPathLength) {
				continue;
			}
			lFilteredPaths.push_back(lPaths[i]);
		}
	}

	return lFilteredPaths;
}

vector<int> fApxMinSPCover(GraphObject *iGraph, GraphObject *iReversed, int iNode, int iTime, int iRadius,
		int iMinPathLength, bool iMulti, double iPrecision, bool iHasWeights) {
	//get subset that correspond to the radius
	unordered_set<int> lSubset = fNodesInRadius(iGraph, iNode, iTime, iRadius, iHasWeights);
	unordered_set<int> lRevSubset = fNodesInRadius(iReversed, iNode, iTime, iRadius, iHasWeights);
	lSubset.insert(lRevSubset.begin(), lRevSubset.end());

	vector<int> lVecSubset(lSubset.begin(), lSubset.end());

	DEB(LVL_ALGS, "Algorithms::fApxMinSPCover: Subset in the radius " << iRadius << " of a node " <<
			iNode << ": " << fGetVectorString(lVecSubset) << endl);

	//get the shortest paths in the subset
	DEB(LVL_ALGS, "Algorithms::fApxMinSPCover: Going to find all pairs of SP in the radius" << endl);
	vector<Path> lPaths = fSpsForSubset(iGraph, lVecSubset, iMinPathLength, iMulti,
			iPrecision, iHasWeights);

	//get cover
	DEB(LVL_ALGS, "Algorithms::fApxMinSPCover: Going to find the cover for the shortest paths" << endl);
	vector<int> lCover = fApxMinSPCover(lPaths, lSubset.size());

	//translate cover indices to graph indices
	for (size_t i = 0; i < lCover.size(); i++) {
		lCover[i] = lVecSubset[lCover[i]];
	}

	return lCover;
}

//struct CompCovers {
//	vector<set<Path*> > *cCoveredPaths;
//
//    CompCovers(vector<set<Path*> > *iCoveredPaths) {
//    	cCoveredPaths = iCoveredPaths;
//    }
//
//	bool operator()(int iLeft, int iRight)
//	{
//		return (*cCoveredPaths)[iLeft].size() < (*cCoveredPaths)[iRight].size();
//	}
//};
//CompCovers lCompCovers(&lCoveredPaths);
//priority_queue<int, vector<int>, CompCovers> lQueue (lCompCovers);

/* greedy approach with apx. ratio H(n) at worst (n = number of paths). The problem is
 * minimum set cover problem where the universe is the shortest paths
 *
 * input MUST be paths with indices in a continuous range (0...n)
 * returned set is NOT translated */
vector<int> fApxMinSPCover(vector<Path> &iPaths, int iN) {
	//for each node -> paths it covers
	vector<unordered_set<Path*> > lCoveredPaths(iN, unordered_set<Path*>());

	//initialize lCoveredPaths structure
	int lTotalPaths = 0;
	for (size_t i = 0; i < iPaths.size(); i++) {
		Path *lPath = &(iPaths[i]);
		if (lPath->fGetSize() <= 2) {
			continue;
		}

		lTotalPaths++;

		for (int l = 0; l < lPath->fGetSize(); l++) {
			lCoveredPaths[lPath->fGet(l)].insert(lPath);
		}
	}

	DEB(LVL_ALGS, "Algorithms::fApxMinSPCover: Total of " << lTotalPaths << " paths" << endl);

	//continue greedy
	vector<int> lResult;
	int lCoveredCount = 0;
	while (lCoveredCount < lTotalPaths) {
		//get the node covering most paths
		int lBest = 0;
		for (size_t i = 1; i < lCoveredPaths.size(); i++) {
			if (lCoveredPaths[i].size() > lCoveredPaths[lBest].size()) {
				lBest = i;
			}
		}
		DEB(LVL_ALGS, "Algorithms::fApxMinSPCover: Node at the top of the queue: " << lBest << endl);
		DEB(LVL_ALGS, "Algorithms::fApxMinSPCover: Covers: " << lCoveredPaths[lBest].size() << endl);

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

	return lResult;
}

int fRadius(GraphObject *iGraph, bool iHasWeights) {
	INFO("Going to find out radius. Weights = " << gBoolStr[iHasWeights] << endl);

	vector<vector<Path> > lPaths = fFloydWarshall(iGraph, iHasWeights);

	int lMaxLength = 0;
	for (size_t i = 0; i < lPaths.size(); i++) {
		for (size_t j = 0; j < lPaths[i].size(); j++) {
			int lPathLength = iGraph->fGetPathLength(lPaths[i][j]);
			lMaxLength = max(lMaxLength, lPathLength);
		}
	}

	return lMaxLength / 2;
}

int fApxRadius(GraphObject *iGraph, bool iHasWeights, double iPrecision) {
	INFO("Going to find out approximate radius. Weights = " << gBoolStr[iHasWeights] << endl);

	vector<Path> lPaths = fGetSps(iGraph, iPrecision, iHasWeights);

	int lMaxLength = 0;
	for (size_t i = 0; i < lPaths.size(); i++) {
		int lPathLength = iGraph->fGetPathLength(lPaths[i]);
		lMaxLength = max(lMaxLength, lPathLength);
	}

	return lMaxLength / 2;
}

/* Approximation of graph's highway dimension. There are following inaccuracies:
 * - not checking every radius from every node -> lower bound on HD
 * - using APX algorithm to look for min-covers -> upper bound on HD
 * - if precision in (0..1), taking only corresponding fraction of shortest paths in given radius -> lower
 * 		bound on HD
 * - if multi = false, disregarding multiple shortest paths between a pair of vertices -> lower bound on HD
 *
 * Algorithm checks radius of size 2, 4, 8, ... log(radius of graph) - the bigger the radius, the less
 * nodes are checked. iRepetitions parameter influences how many nodes we check for each radius
 *  */
int fApxHighwayDim(GraphObject *iGraph, bool iHasWeights, bool iMulti, double iPrecision, int iRepetitions) {
	INFO("Going to find out approximate highway dimension");
	INFO("Precision = " << iPrecision << endl);
	INFO("Multi = " << gBoolStr[iMulti] << endl);
	INFO("Repetitions = " << iRepetitions << endl);
	INFO("Weights = " << gBoolStr[iHasWeights] << endl);

	if (iPrecision > 1) {
		ERR("Algorithms::fApxHighwayDim: Precision is out of range" << endl);
		return -1;
	}

	//get radius of the graph
	int lRadius = -1;
	if (iPrecision < 0) {
		lRadius = fRadius(iGraph, iHasWeights);
	}
	else {
		lRadius = fApxRadius(iGraph, iHasWeights, iPrecision);
	}
	INFO("Radius is determined as " << lRadius << endl);
	int lLog = (int)(log(lRadius) / log(2));
	DEB(LVL_ALGS, "Algorithms::fApxHighwayDim: Logarithm of the radius is determined as " << lLog << endl);

	GraphObject *lReversedGraph = dynamic_cast<GraphObject*>(iGraph->fGetCopy());
	lReversedGraph->fReverseOrientation();

	int lN = iGraph->fGetN();

	int lMaxHd = 0;
	//try different radius
	for (int i = 1; i <= lLog; i++) {
		//try smaller radius more times, depending also on repetitions
		int l2R = (int)(pow(2, i));
		int lR = l2R / 2;
		for (int j = 0; j < (lLog - i + 1) * iRepetitions; j++) {
			int lNode = rand() % lN;
			INFO("Trying out radius " << l2R << " from node " << lNode << endl);
			DEB(LVL_ALGS, "Algorithms::fApxHighwayDim: Trying out radius " << l2R << " from node " <<
					lNode << endl);
			vector<int> lCover = fApxMinSPCover(iGraph, lReversedGraph, lNode, -1, l2R, lR, iMulti,
					iPrecision, iHasWeights);
			DEB(LVL_ALGS, "Algorithms::fApxHighwayDim: Found a cover of size " << lCover.size() << endl);
			if (lCover.size() > lMaxHd) {
				lMaxHd = lCover.size();
			}
		}
	}

	delete lReversedGraph;

	return lMaxHd;
}

/*****************************************************************/
/* Density
/*****************************************************************/

double fDensity(UgGraph *iGraph, int iRepetitions) {
	INFO("Going to find out density of the UG");
	INFO("Repetitions = " << iRepetitions << endl);

	int lN = iGraph->fGetN();
	int lSqrt4N = sqrt(sqrt(iGraph->fGetN()));

	double lMaxDensity = 0;
	//try different radius
	for (int i = lSqrt4N; i <= lN; i += lSqrt4N) {
		//try smaller sub graphs more times, depending also on repetitions
		for (int j = 0; j < (lN / i) * iRepetitions; j++) {
			Graph<UGNodeData, UGArcData> lSubGr = fSubCon(iGraph->fGetGraph(), i);
			double lDensity = (double)lSubGr.fGetM() / (double)lSubGr.fGetN();
			INFO("Size = " << lSubGr.fGetN() << "/" << lSubGr.fGetM() << ". Density = " << lDensity << endl);
			if (lMaxDensity < lDensity) {
				lMaxDensity = lDensity;
			}
		}
	}

	return lMaxDensity;
}

/*****************************************************************/
/* Others
/*****************************************************************/

vector<Path> fGetSps(GraphObject* iGraph, double iFracOfAll, bool iHasWeights) {
	if (iFracOfAll <= 0 || iFracOfAll > 1) {
		ERR("Algorithms::fGetSps: Fraction is out of range" << endl);
		return vector<Path>();
	}

	//get random vertices from which we are going to look for shortest paths
	int lN = iGraph->fGetN();
	vector<int> lPerm = fRandomPermutation(lN);
	int lConsider = lN * iFracOfAll;

	//get some shortest paths
	vector<Path> lPaths;
	int lOldPercents = -1;
	for (int i = 0; i < lConsider; i++) {
		int lPercents = (i * 100) / lConsider;
		if (lPercents != lOldPercents) {
			INFO("Progress " << lPercents << "%" << endl);
			lOldPercents = lPercents;
		}
		vector<Path> lCurrentPaths = fDijkstraRandomSpToAll(iGraph, lPerm[i], -1, iHasWeights);
		for (size_t j = 0; j < lCurrentPaths.size(); j++) {
			if (lCurrentPaths[j].fIsEmpty()) {
				continue;
			}
			lPaths.push_back(lCurrentPaths[j]);
		}
	}

	return lPaths;
}

vector<Connection> fGetConns(TimeGraphObject* iGraph, double iFracOfAll) {
	if (iFracOfAll <= 0 || iFracOfAll > 1) {
		ERR("Algorithms::fGetConns: Fraction is out of range" << endl);
		return vector<Connection>();
	}

	srand(time(NULL));

	//get random events from which we are going to look for connections
	vector<Event> lEvents = iGraph->fGetEvents(unordered_set<string>(), ANY_RANGE);
	int e = lEvents.size();
	vector<int> lPerm = fRandomPermutation(e);
	int lConsider = e * iFracOfAll;

	vector<string> lVecCities = iGraph->fGetCities();
	unordered_set<string> lCities(lVecCities.begin(), lVecCities.end());

	//get some connections
	vector<Connection> lConns;
	int lOldPercents = -1;
	for (int i = 0; i < lConsider; i++) {
		int lPercents = (i * 100) / lConsider;
		if (lPercents != lOldPercents) {
			INFO("Progress " << lPercents << "%" << endl);
			lOldPercents = lPercents;
		}
		unordered_map<string, Connection> lCurrentConns = fDijkWrapRandomConnToCities(iGraph,
				lEvents[lPerm[i]].cCity, lEvents[lPerm[i]].cTime, lCities);
		for (unordered_map<string, Connection>::iterator j = lCurrentConns.begin();
				j != lCurrentConns.end(); j++) {
			if (j->second.fIsEmpty()) {
				continue;
			}
			lConns.push_back(j->second);
		}
	}

	return lConns;
}

/* Finds out, if graph has a cycle formed with arcs with lengths in a given range (-1 = infinity) */
bool fHasCycle(GraphObject *iGraph, int iArcMinLength, int iArcMaxLength) {
	int n = iGraph->fGetN();

	vector<bool> lErased(n, false);
	vector<bool> lVisited(n, false);
	int lErasedCount = 0;
	int lToVisitStart = 0;

	while (true) {
		list<int> lToErase;
		list<int> lToVisit;

		while (lToVisitStart < n && lErased[lToVisitStart] == true) {
			lToVisitStart++;
		}
		lToVisit.push_back(lToVisitStart);

		while (lToVisit.empty() == false) {
			int lNext = lToVisit.front();
			lToVisit.pop_front();
			if (lVisited[lNext] == true) {
				//we got cycle
				return true;
			}

			lVisited[lNext] = true;
			lToErase.push_back(lNext);
			lErasedCount++;

			//add neighbors to queue
			vector<int> lTo = iGraph->fGetTo(lNext);
			for (size_t i = 0; i < lTo.size(); i++) {
				int lArcLength = iGraph->fGetLength(lNext, lTo[i], -1);
				if (lErased[lTo[i]] == false &&
					(iArcMinLength == -1 || lArcLength >= iArcMinLength) &&
					(iArcMaxLength == -1 || lArcLength <= iArcMaxLength)) {
					lToVisit.push_back(lTo[i]);
				}
			}
		}

		for (list<int>::iterator i = lToErase.begin(); i != lToErase.end(); i++) {
			lErased[*i] = true;
		}

		if (lErasedCount == n) {
			break;
		}
	}

	return false;
}

/*  returns pairs (overtaken edge, overtaking edge) */
vector<pair<ElCon, ElCon> > fGetOvertakenEdges(Timetable *iTimetable) {
	vector<string> lCities = iTimetable->fGetCities();
	int c = lCities.size();

	vector<vector<vector<ElCon> > > lElCons = iTimetable->fGetSortedElCons(&lCities);

	vector<pair<ElCon, ElCon> > lResult;

	for (size_t i = 0; i < c; i++) {
		for (size_t j = 0; j < c; j++) {
			vector<bool> lOvertaken(lElCons[i][j].size(), false);
			for (size_t k = 1; k < lElCons[i][j].size(); k++) {
				int l = k - 1;
				while (lElCons[i][j][k].cArr < lElCons[i][j][l].cArr) {
					if (lOvertaken[l] == false) {
						lResult.push_back(pair<ElCon, ElCon>(lElCons[i][j][l], lElCons[i][j][k]));
						lOvertaken[l] = true;
					}
					l--;
				}
			}
		}
	}

	return lResult;
}
