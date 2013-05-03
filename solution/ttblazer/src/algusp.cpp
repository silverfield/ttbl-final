/*****************************************************************/
/* Includes
/*****************************************************************/

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/heap/fibonacci_heap.hpp>

#include "central.h"

#include "algdijkstra.h"
#include "algusp.h"
#include "fibheap.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Vars
/*****************************************************************/

string gUspAnTypeStr [] = {"normal", "count", "usage"};

/*****************************************************************/
/* Structs
/*****************************************************************/

/*****************************************************************/
/* Functions
/*****************************************************************/

string fAnalyzeSegUsps(UgGraph *iUgGraph, vector<vector<vector<Path> > > *iPaths,
		vector<vector<vector<vector<size_t> > > > *iIndices,
		vector<int> &iFromIndices, vector<int> &iToIndices, bool iDet) {
	ostringstream lSs;

	double lOveralUsp = 0;

	lSs << "*** WHOLE ***" << endl;

	lSs << fAnalyzeUsps(iUgGraph, iPaths, iFromIndices, iToIndices, iDet);

	size_t lSegments = (*iIndices)[0][0].size();

	for (size_t k = 0; k < lSegments; k++) {
		lSs << "*** SEGMENT " << k << "***" << endl;

		vector<vector<int> > lUspCounts;
		lUspCounts.reserve(iFromIndices.size());

		double lAvgUspSize = 0;
		int lTotalUsps = 0;
		for (size_t i = 0; i < (*iIndices).size(); i++) {
			lUspCounts.push_back(vector<int>(iToIndices.size(), 0));
			for (size_t j = 0; j < (*iIndices)[i].size(); j++) {
				int lCount = (*iIndices)[i][j][k].size();
				for (size_t k = 0; k < (*iPaths)[i][j].size(); k++) {
					lTotalUsps++;
					lAvgUspSize += (*iPaths)[i][j][k].fGetSize();
				}
				lUspCounts[i][j] = lCount;
			}
		}
		lAvgUspSize /= (double)lTotalUsps;

		lSs << "AVG. USP SIZE: " << lAvgUspSize << endl;
		lSs << fAnalyzeUspCounts(iUgGraph, &lUspCounts, iFromIndices, iToIndices, iDet);
		lOveralUsp += fGetUspCoef(iUgGraph, &lUspCounts, iFromIndices, iToIndices);
	}
	lOveralUsp /= lSegments;

	lSs << "OVERAL AVG. USP COEF. : " << lOveralUsp << endl;

	return lSs.str();
}

string fAnalyzeUsps(UgGraph *iUgGraph, vector<vector<vector<Path> > > *iPaths,
		vector<int> &iFromIndices, vector<int> &iToIndices, bool iDet) {
	vector<vector<int> > lUspCounts;
	lUspCounts.reserve(iFromIndices.size());

	double lAvgUspSize = 0;
	int lTotalUsps = 0;
	for (size_t i = 0; i < iPaths->size(); i++) {
		lUspCounts.push_back(vector<int>(iToIndices.size(), 0));
		for (size_t j = 0; j < (*iPaths)[i].size(); j++) {
			int lCount = (*iPaths)[i][j].size();
			for (size_t k = 0; k < (*iPaths)[i][j].size(); k++) {
				lTotalUsps++;
				lAvgUspSize += (*iPaths)[i][j][k].fGetSize();
			}
			lUspCounts[i][j] = lCount;
		}
	}
	lAvgUspSize /= (double)lTotalUsps;

	ostringstream lSs;

	lSs << "AVG. USP SIZE: " << lAvgUspSize << endl;
	lSs << fAnalyzeUspCounts(iUgGraph, &lUspCounts, iFromIndices, iToIndices, iDet);

	return lSs.str();
}

double fGetUspCoef(UgGraph *iUgGraph, vector<vector<int> > *iCounts,
		vector<int> &iFromIndices, vector<int> &iToIndices) {
	//compute frequencies
	double lAvgUsp = 0;
	int lPairCount = 0;
	map<int, int> lFreqCount; //[x different paths] -> [number of pairs it which it happened]
	for (size_t i = 0; i < (*iCounts).size(); i++) {
		for (size_t j = 0; j < (*iCounts)[i].size(); j++) {
			lPairCount++;
			int lCount = (*iCounts)[i][j];
			lAvgUsp += lCount;

			if (lFreqCount.find(lCount) == lFreqCount.end()) {
				lFreqCount[lCount] = 0;
			}
			lFreqCount[lCount]++;
		}
	}
	lAvgUsp /= lPairCount;

	return lAvgUsp;
}

string fAnalyzeUspCounts(UgGraph *iUgGraph, vector<vector<int> > *iCounts,
		vector<int> &iFromIndices, vector<int> &iToIndices, bool iDet) {
	ostringstream lSs;

	//compute frequencies
	double lAvgUsp = 0;
	int lTotalDiffUspCount = 0;
	int lPairCount = 0;
	map<int, int> lFreqCount; //[x different paths] -> [number of pairs it which it happened]
	for (size_t i = 0; i < (*iCounts).size(); i++) {
		for (size_t j = 0; j < (*iCounts)[i].size(); j++) {
			lPairCount++;
			int lCount = (*iCounts)[i][j];
			lTotalDiffUspCount += lCount;
			lAvgUsp += lCount;

			if (lFreqCount.find(lCount) == lFreqCount.end()) {
				lFreqCount[lCount] = 0;
			}
			lFreqCount[lCount]++;
		}
	}
	lAvgUsp /= lPairCount;

	lSs << "AVG. USP COEF. : " << lAvgUsp << endl;
	lSs << "TOTAL DIFFERENT USPs: " << lTotalDiffUspCount << endl;
	lSs << "Number of USPs with PAIR FREQUENCIES" << endl;
	for (map<int, int>::iterator i = lFreqCount.begin(); i != lFreqCount.end(); i++) {
		double lPercents = (double)i->second / (double)lPairCount;
		lSs << i->first << " different USPs with " << i->second << " pairs (" <<
				lPercents * 100 << "%)" << endl;
	}
	if (iDet) {
		lSs << "Number of USPs for EACH PAIR" << endl;
		for (size_t i = 0; i < (*iCounts).size(); i++) {
			for (size_t j = 0; j < (*iCounts)[i].size(); j++) {
				int lCount = (*iCounts)[i][j];
				lSs << "From " << iUgGraph->fGetNodeName(iFromIndices[i]) <<
						" to " << iUgGraph->fGetNodeName(iToIndices[j])
						<< " : " << lCount << endl;
			}
		}
	}

	return lSs.str();
}

string fAnalyzeUspUsage(UgGraph *iUgGraph,
		vector<vector<vector<pair<Path, int> > > > *iUsage,
		vector<int> &iFromIndices, vector<int> &iToIndices, bool iDet) {
	vector<vector<int> > lUspCounts;
	lUspCounts.reserve(iFromIndices.size());

	int lTotalUspSize = 0;
	int lTotalUspCount = 0;
	int lMaxUspSize = 0;


	for (size_t i = 0; i < iUsage->size(); i++) {
		lUspCounts.push_back(vector<int>(iToIndices.size(), 0));
		for (size_t j = 0; j < (*iUsage)[i].size(); j++) {
			for (size_t k = 0; k < (*iUsage)[i][j].size(); k++) {
				int lPathSize = (*iUsage)[i][j][k].first.fGetSize();
				int lUsed = (*iUsage)[i][j][k].second;

				lTotalUspSize += lPathSize * lUsed;
				lTotalUspCount += lUsed;

				if (lMaxUspSize < lPathSize) {
					lMaxUspSize = lPathSize;
				}
			}
			int lCount = (*iUsage)[i][j].size();
			lUspCounts[i][j] = lCount;
		}
	}

	ostringstream lSs;

	string lAnUspCounts = fAnalyzeUspCounts(iUgGraph, &lUspCounts, iFromIndices, iToIndices, false);

	lSs << lAnUspCounts;

	double lAvgUspSize = (double)lTotalUspSize / (double)lTotalUspCount;
	lSs << "AVG. USP SIZE. : " << lAvgUspSize << endl;
	lSs << "MAX. USP SIZE. : " << lMaxUspSize << endl;

	if (iDet) {
		lSs << "Number of USPs for EACH PAIR" << endl;
		for (size_t i = 0; i < lUspCounts.size(); i++) {
			for (size_t j = 0; j < lUspCounts[i].size(); j++) {
				int lCount = lUspCounts[i][j];
				lSs << "From " << iUgGraph->fGetNodeName(iFromIndices[i]) <<
						" to " << iUgGraph->fGetNodeName(iToIndices[j])
						<< " : " << lCount << endl;
				for (size_t k = 0; k < (*iUsage)[i][j].size(); k++) {
					int lPathSize = (*iUsage)[i][j][k].first.fGetSize();
					int lUsed = (*iUsage)[i][j][k].second;
					lSs << "	USP of size " << lPathSize << " used " << lUsed << " times" << endl;
				}
			}
		}
	}

	return lSs.str();
}

Connection fConnUsps(int iTime, path_set &iUsps, TdGraph *iGraph) {
	Connection lConn;
	for (unordered_set<Path>::iterator i = iUsps.begin(); i != iUsps.end(); i++) {
		Connection lCurConn = iGraph->fMakeConnection(*i, iTime);
		if (lCurConn.fIsEmpty()) {
			continue;
		}

		if (lConn.fIsEmpty() ||
			(lConn.fGetLength() > lCurConn.fGetLength() && !lCurConn.fIsEmpty())) {
			lConn = lCurConn;
		}
	}
	return lConn;
}

Connection fConnUsps(int iTime, vector<Path> &iUsps, TdGraph *iGraph) {
	Connection lConn;
	for (size_t i = 0; i < iUsps.size(); i++) {
		Connection lCurConn = iGraph->fMakeConnection(iUsps[i], iTime);
		if (lCurConn.fIsEmpty()) {
			continue;
		}

		if (lConn.fIsEmpty() ||
			(lConn.fGetLength() > lCurConn.fGetLength() && !lCurConn.fIsEmpty())) {
			lConn = lCurConn;
		}
	}
	return lConn;
}

Connection fConnUsps(int iTime, vector<Path> &iUsps, vector<size_t> &iUspInd, TdGraph *iGraph) {
	Connection lConn;
	for (size_t i = 0; i < iUspInd.size(); i++) {
		size_t j = iUspInd[i];
		Connection lCurConn = iGraph->fMakeConnection(iUsps[j], iTime);
		if (lCurConn.fIsEmpty()) {
			continue;
		}

		if (lConn.fIsEmpty() ||
			(lConn.fGetLength() > lCurConn.fGetLength() && !lCurConn.fIsEmpty())) {
			lConn = lCurConn;
		}
	}
	return lConn;
}

Connection fConnUspTree(int iFrom, int iTime, int iTo, UspTree &iUspTree, TdGraph *iGraph) {
	int n = iGraph->fGetN();

	int r = iUspTree.fGetRestr()->size();
	unordered_set<int> *lRestr = iUspTree.fGetRestr();

	if (lRestr->find(iTo) == lRestr->end()) {
		return Connection();
	}

	vector<int> lDecompr(r, -1);
	int lCounter = 0;
	for (unordered_set<int>::iterator i = lRestr->begin(); i != lRestr->end(); i++) {
		lDecompr[lCounter] = *i;
		lCounter++;
	}
	unordered_map<int, int> lCompr;
	for (size_t i = 0; i < lDecompr.size(); i++) {
		lCompr[lDecompr[i]] = i;
	}

	//initialize reached at vector
	vector<int> lReachedAt(r, -1);
	lReachedAt[lCompr[iFrom]] = iTime;
	FibHeap lReachedAtHeap(lReachedAt);
	lReachedAtHeap.fUpdate(lCompr[iFrom]);

	//initialize came-from vector
	vector<int> lCameFrom(r, -1);
	lCameFrom[lCompr[iFrom]] = iFrom;

	unordered_set<int> lSettled; //those with final distance
	unordered_set<int> lExplored; //those that are visible, but not settled
	lExplored.insert(iFrom);

	bool lFound = false;
	while (1) {
		int lComprBest = -1;
		int lBestTime = -1;

		//no more explored vertices
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lComprBest = lReachedAtHeap.fTopIndex();
		lBestTime = lReachedAtHeap.fTopVal();
		lReachedAtHeap.fPop();

		//finish criterion - no more vertices to settle
		if (lComprBest == -1) {
			break;
		}

		int lBest = lDecompr[lComprBest];

		if (lBest == iTo) {
			lFound = true;
			break;
		}

		DEB(LVL_ALGS, "Algorithms::fConnUspTree: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//update
		unordered_set<int> *lBestTo = iUspTree.fGet(lBest);
		if (lBestTo == NULL) {
			continue;
		}
		for (unordered_set<int>::iterator i = lBestTo->begin(); i != lBestTo->end(); i++) {
			int lToIndex = *i;
			if (lRestr->find(lToIndex) == lRestr->end()) {
				continue;
			}
			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}
			int lLength = iGraph->fGetLength(lBest, lToIndex, lBestTime);
			if (lLength == -1) {
				continue;
			}

			int lComprToIndex = lCompr[lToIndex];

			if (lReachedAt[lComprToIndex] == -1 || lReachedAt[lComprToIndex] >
					lReachedAt[lComprBest] + lLength) {
				DEB(LVL_ALGDET, "Algorithms::fConnUspTree: Best reached at" << lReachedAt[lComprBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fConnUspTree: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fConnUspTree: The one being updated reached at" <<
						lReachedAt[lComprToIndex] << endl);

				lReachedAt[lComprToIndex] = lReachedAt[lComprBest] + lLength;
				lReachedAtHeap.fUpdate(lComprToIndex);
				lCameFrom[lComprToIndex] = lBest;

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
	int lComprPathIndex = lCompr[lPathIndex];
	while (lCameFrom[lComprPathIndex] != lPathIndex) {
		lPathVertices.push_back(lPathIndex);
		lPathIndex = lCameFrom[lComprPathIndex];
		lComprPathIndex = lCompr[lPathIndex];
	}
	lPathVertices.push_back(lPathIndex);
	reverse(lPathVertices.begin(), lPathVertices.end());

	Path lPath(lPathVertices);

	//convert to connection
	Connection lConn = iGraph->fMakeConnection(lPath, iTime);

	return lConn;
}

unordered_map<int, Connection> fConnUspTreeToCities(int iFrom, int iTime, unordered_set<int> &iToCities,
		UspTree &iUspTree, TdGraph *iGraph) {

	int n = iGraph->fGetN();

	int t = iToCities.size();

	int r = iUspTree.fGetRestr()->size();
	unordered_set<int> *lRestr = iUspTree.fGetRestr();

	vector<int> lDecompr(r, -1);
	int lCounter = 0;
	for (unordered_set<int>::iterator i = lRestr->begin(); i != lRestr->end(); i++) {
		lDecompr[lCounter] = *i;
		lCounter++;
	}

	unordered_map<int, int> lCompr;
	for (size_t i = 0; i < lDecompr.size(); i++) {
		lCompr[lDecompr[i]] = i;
	}

	//initialize reached at vector
	vector<int> lReachedAt(r, -1);
	lReachedAt[lCompr[iFrom]] = iTime;
	FibHeap lReachedAtHeap(lReachedAt);
	lReachedAtHeap.fUpdate(lCompr[iFrom]);

	//initialize came-from vector
	vector<int> lCameFrom(r, -1);
	lCameFrom[lCompr[iFrom]] = iFrom;

	//initialize sets
	unordered_set<int> lSettledCities; //those cities with final distance
	lSettledCities.insert(iFrom);
	int lSettledCount = 0; //number of settled to-cities
	if (iToCities.count(iFrom) != 0) {
		lSettledCount++;
	}
	unordered_set<int> lSettled; //those with final distance
	unordered_set<int> lExplored; //those that are visible, but not settled
	lExplored.insert(iFrom);

	while (1) {
		int lComprBest = -1;
		int lBestTime = -1;

		//no more explored vertices
		if (lExplored.size() == 0) {
			break;
		}

		//find vertex to settle
		lComprBest = lReachedAtHeap.fTopIndex();
		lBestTime = lReachedAtHeap.fTopVal();
		lReachedAtHeap.fPop();

		//finish criterion - no more vertices to settle
		if (lComprBest == -1) {
			break;
		}

		int lBest = lDecompr[lComprBest];

		DEB(LVL_ALGS, "Algorithms::fConnUspTreeToCities: Best vertex " << lBest << endl);

		lSettled.insert(lBest);
		lExplored.erase(lBest);

		//if it is the first time we visit a city, mark the city as settled
		if (lSettledCities.find(lBest) == lSettledCities.end()) {
			lSettledCities.insert(lBest);
			if (iToCities.find(lBest) != iToCities.end()) {
				lSettledCount++;
			}
		}

		//on settling all to-cities, we can stop
		if (lSettledCount == t) {
			break;
		}

		//update
		unordered_set<int> *lBestTo = iUspTree.fGet(lBest);
		if (lBestTo == NULL) {
			continue;
		}
		for (unordered_set<int>::iterator i = lBestTo->begin(); i != lBestTo->end(); i++) {
			int lToIndex = *i;
			if (lRestr->find(lToIndex) == lRestr->end()) {
				continue;
			}
			if (lSettled.find(lToIndex) != lSettled.end()) {
				continue;
			}
			int lLength = iGraph->fGetLength(lBest, lToIndex, lBestTime);
			if (lLength == -1) {
				continue;
			}

			int lComprToIndex = lCompr[lToIndex];

			if (lReachedAt[lComprToIndex] == -1 || lReachedAt[lComprToIndex] >
					lReachedAt[lComprBest] + lLength) {
				DEB(LVL_ALGDET, "Algorithms::fConnUspTreeToCities: Best reached at " <<
						lReachedAt[lComprBest] << endl);
				DEB(LVL_ALGDET, "Algorithms::fConnUspTreeToCities: Length " << lLength << endl);
				DEB(LVL_ALGDET, "Algorithms::fConnUspTreeToCities: The one being updated (" << lToIndex <<
						") at " << lReachedAt[lComprToIndex] << endl);

				lReachedAt[lComprToIndex] = lReachedAt[lComprBest] + lLength;
				lReachedAtHeap.fUpdate(lComprToIndex);
				lCameFrom[lComprToIndex] = lBest;

				lExplored.insert(lToIndex);
			}
		}
	}

	//get the paths
	unordered_map<int, Path> lPaths;
	for (unordered_set<int>::iterator i = iToCities.begin(); i != iToCities.end(); i++) {
		if (lRestr->find(*i) == lRestr->end() || lCameFrom[lCompr[*i]] == -1) {
			lPaths[*i] = Path();
			continue;
		}

		int lPathIndex = *i;
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
		lPaths[*i] = lPath;
	}

	//convert to connections
	unordered_map<int, Connection> lConns;
	for (unordered_map<int, Path>::iterator i = lPaths.begin(); i != lPaths.end(); i++) {
		lConns[i->first] = iGraph->fMakeConnection(i->second, iTime);
	}

	return lConns;
}

vector<vector<vector<Path> > >* fUSP(
		UgGraph *iUgGraph, TimeGraphObject *iGraph,
		vector<int> &iUgCitiesFrom, vector<int> &iUgCitiesTo) {

	//prepare cities in sets
	unordered_set<string> lCitiesFrom;
	for (size_t i = 0; i < iUgCitiesFrom.size(); i++) {
		lCitiesFrom.insert(iUgGraph->fGetNodeName(iUgCitiesFrom[i]));
	}
	unordered_set<string> lCitiesTo;
	for (size_t i = 0; i < iUgCitiesTo.size(); i++) {
		lCitiesTo.insert(iUgGraph->fGetNodeName(iUgCitiesTo[i]));
	}

	//prepare cities in strings
	unordered_map<string, int> lCitiesFromToIndex;
	for (size_t i = 0; i < iUgCitiesFrom.size(); i++) {
		string lCityName = iUgGraph->fGetNodeName(iUgCitiesFrom[i]);
		lCitiesFromToIndex[lCityName] = i;
	}
	unordered_map<string, int> lCitiesToToIndex;
	for (size_t i = 0; i < iUgCitiesTo.size(); i++) {
		string lCityName = iUgGraph->fGetNodeName(iUgCitiesTo[i]);
		lCitiesToToIndex[lCityName] = i;
	}

	vector<string> lAllCities = iUgGraph->fGetNodeNames();

	//get all the events corresponding to departure cities
	vector<Event> lEvents = iGraph->fGetEvents(lCitiesFrom, ANY_RANGE);

	//for each pair of departing-destination city we have a set of shortest UG paths
	vector<vector<path_set> > lPaths(iUgCitiesFrom.size(),
			vector<path_set>(iUgCitiesTo.size(), path_set()));

	INFO("Going to run search from " << lEvents.size() << " events" << endl);

	int lOldPercentage = -1;
	//from all events launch a search...
	for (size_t i = 0; i < lEvents.size(); i++) {
		int lPercentage = (i * 100) / lEvents.size();
		if (lPercentage != lOldPercentage) {
			INFO("Progress " << lPercentage << " % " << endl);
			lOldPercentage = lPercentage;
		}

		DEB(LVL_ALGS, "Algorithms::fUSP: search from "
				<< lEvents[i].cCity << " at time " << fTimeFormatToString(lEvents[i].cTime) << endl);

		//...to all destination cities
		unordered_map<string, CityPath> lUgSp =
				fDijkWrapUgSpToCities(iGraph, lEvents[i].cCity, lEvents[i].cTime, lCitiesTo);

		//add paths to the corresponding hash set
		for (unordered_map<string, CityPath>::iterator j = lUgSp.begin(); j != lUgSp.end(); j++) {
			int lFrom = lCitiesFromToIndex[lEvents[i].cCity];
			int lTo = lCitiesToToIndex[j->first];

			if (lFrom == lTo || j->second.fGetSize() != 0) {
				lPaths[lFrom][lTo].insert(fCityPathToPath(j->second, iUgGraph));
			}

			DEB(LVL_ALGS, "Algorithms::fUSP: From " << lEvents[i].cCity << " at "
					<< fTimeFormatToString(lEvents[i].cTime) << " to " << j->first <<
					": " << j->second.fGetString() << endl);
		}
	}

	//convert hash-sets to vectors
	vector<vector<vector<Path> > >* lReturnPaths = new vector<vector<vector<Path> > >(iUgCitiesFrom.size(),
			vector<vector<Path> >(iUgCitiesTo.size(), vector<Path>()));
	for (size_t i = 0; i < lPaths.size(); i++) {
		for (size_t j = 0; j < lPaths[i].size(); j++) {
			for (unordered_set<Path, PathHash, PathEqual>::iterator k = lPaths[i][j].begin();
					k != lPaths[i][j].end(); k++) {
				(*lReturnPaths)[i][j].push_back(*k);
			}
			lPaths[i][j].clear();
		}
		lPaths[i].clear();
	}

	return lReturnPaths;
}

vector<Path> fUspsToCitiesRestr(TdGraph *iGraph, int iFrom, int iTime,
		vector<int> &iToCities, unordered_set<int> *iRestr) {

	int n = iGraph->fGetN();

	//number of to-cities (those we are interested in)
	int t = iToCities.size();

	unordered_set<int> lToCities(iToCities.begin(), iToCities.end());

	//initialize distance vector - best distance to node and to city
	vector<int> lDistances(n, -1);
	lDistances[iFrom] = 0;
	FibHeap lDistancesHeap(lDistances);
	lDistancesHeap.fUpdate(iFrom);

	//initialize came-from vector
	//come-from[city] = index of a node we came from to the city
	vector<int> lCameFrom = vector<int>(n, -1);
	lCameFrom[iFrom] = iFrom;

	//initialize sets, we explore the graph as in default Dijkstra, only the finish criterion differs
	unordered_set<int> lSettledCities; //those cities with final distance
	lSettledCities.insert(iFrom);
	int lSettledCount = 0; //number of settled to-cities
	if (lToCities.count(iFrom) != 0) {
		lSettledCount++;
	}
	while (1) {
		int lBest = -1;
		int lBestDistance = -1;

		//find vertex to settle
		lBest = lDistancesHeap.fTopIndex();
		lBestDistance = lDistancesHeap.fTopVal();
		lDistancesHeap.fPop();

		//DEB(LVL_ALGS, "Algorithms::fUspsToCitiesRestr: Best vertex " << lBest << endl);

		//finish criterion - no more vertices to settle
		if (lBest == -1) {
			break;
		}

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
			//DEB(LVL_ALGDET, "Algorithms::fUspsToCitiesRestr: To index " << lToIndex << " (city " <<
			//		iGraph->fGetNodeName(lToIndex) << ")" << endl);

			if (iRestr != NULL && iRestr->find(lToIndex) == iRestr->end()) {
				continue;
			}
			if (lSettledCities.find(lToIndex) != lSettledCities.end()) {
				continue;
			}

			int lLength = iGraph->fGetLength(lBest, lToIndex, iTime + lBestDistance);
			if (lLength == -1) {
				continue;
			}

			//we want UG paths
			if (lDistances[lToIndex] == -1 || lDistances[lToIndex] >= lDistances[lBest] + lLength) {
//				DEB(LVL_ALGDET, "Algorithms::fUspsToCitiesRestr: Distance to best " <<
//						lDistances[lBest] << endl);
//				DEB(LVL_ALGDET, "Algorithms::fUspsToCitiesRestr: Length " << lLength << endl);
//				DEB(LVL_ALGDET, "Algorithms::fUspsToCitiesRestr: Distance to being updated " <<
//						lDistances[lToIndex] << endl);

				lDistances[lToIndex] = lDistances[lBest] + lLength;
				lCameFrom[lToIndex] = lBest;
				lDistancesHeap.fUpdate(lToIndex);
			}
		}
	}


	//get the paths
	vector<Path> lPaths;
	for (size_t i = 0; i < iToCities.size(); i++) {
		if (lCameFrom[iToCities[i]] == -1) {
			lPaths.push_back(Path());
			continue;
		}

		int lPathIndex = iToCities[i];
		vector<int> lIndices;
		while (lCameFrom[lPathIndex] != lPathIndex) {
			lIndices.push_back(lPathIndex);
			lPathIndex = lCameFrom[lPathIndex];
		}
		lIndices.push_back(lPathIndex);
		reverse(lIndices.begin(), lIndices.end());

		Path lPath(lIndices);
		lPaths.push_back(lPath);
	}

	return lPaths;
}

vector<vector<vector<Path> > >* fTdUSP(
		TdGraph *iGraph, vector<int> &iUgCitiesFrom, vector<int> &iUgCitiesTo) {

	vector<vector<vector<Path> > >* lReturnPaths = new vector<vector<vector<Path> > >(iUgCitiesFrom.size(),
			vector<vector<Path> >(iUgCitiesTo.size(), vector<Path>()));
	int lOldPercentage = -1;
	for (size_t i = 0; i < iUgCitiesFrom.size(); i++) {
		int lPercentage = (i * 100) / iUgCitiesFrom.size();
		if (lPercentage != lOldPercentage) {
			INFO("Progress " << lPercentage << " % " << endl);
			lOldPercentage = lPercentage;
		}

		gLogger->fInfoOn(false);
		vector<vector<Path> > *lFromPaths = fRestrUSP(iGraph, iUgCitiesFrom[i], iUgCitiesTo, NULL, ANY_RANGE);
		gLogger->fInfoOn(true);
		(*lReturnPaths)[i] = *lFromPaths;
		delete lFromPaths;
	}

	return lReturnPaths;
}

vector<vector<Path> >* fRestrUSP(TdGraph *iGraph,
		int iFrom, vector<int> &iUgCitiesTo,
		unordered_set<int> *iNeigh, pair<int, int> iRange) {

	unordered_map<int, int> lCitiesToToIndex;
	for (size_t i = 0; i < iUgCitiesTo.size(); i++) {
		lCitiesToToIndex[iUgCitiesTo[i]] = i;
	}

	//get all the events corresponding to the departure city
	unordered_set<string> lFromCity;
	lFromCity.insert(iGraph->fGetNodeName(iFrom));
	vector<Event> lEvents = iGraph->fGetEvents(lFromCity, iRange);

	//add also the last possible event if we use ranges
	if (iRange.second != ANY_RANGE.second) {
		lEvents.push_back(Event(iGraph->fGetNodeName(iFrom), iRange.second));
	}

	//for each pair of departing-destination city we have a set of shortest UG paths
	vector<path_set> lPaths(iUgCitiesTo.size(), path_set());

	INFO("Going to run search from " << lEvents.size() << " events" << endl);

	int lOldPercentage = -1;
	//from all events launch a search...
	for (size_t i = 0; i < lEvents.size(); i++) {
		int lPercentage = (i * 100) / lEvents.size();
		if (lPercentage != lOldPercentage) {
			INFO("Progress " << lPercentage << " % " << endl);
			lOldPercentage = lPercentage;
		}

		DEB(LVL_ALGS, "Algorithms::fRestrUSP: search from "
				<< lEvents[i].cCity << " at time " << fTimeFormatToString(lEvents[i].cTime) << endl);

		//...to all destination cities
		vector<Path> lUgSp = fUspsToCitiesRestr(iGraph, iFrom, lEvents[i].cTime, iUgCitiesTo, iNeigh);

		//add paths to the corresponding hash set
		for (size_t j = 0; j < lUgSp.size(); j++) {
			int lFrom = iFrom;
			int lTo = iUgCitiesTo[j];
			int lToIndex = lCitiesToToIndex[lTo];

			if (lUgSp[j].fGetSize() == 0) {
				if (lFrom == lTo) {
					lPaths[lToIndex].insert(lUgSp[j]);
					continue;
				}
				else {
					continue;
				}
			}

			if (lPaths[lToIndex].find(lUgSp[j]) != lPaths[lToIndex].end()) {
				continue;
			}

			//check if this path is going to be useful]
			int lArrival = iGraph->fMakeConnection(lUgSp[j], lEvents[i].cTime).fGetLastTime();
			int lOtherArr = fConnUsps(lEvents[i].cTime, lPaths[lToIndex], iGraph).fGetLastTime();
			if (lArrival == lOtherArr) {
				continue;
			}

			lPaths[lToIndex].insert(lUgSp[j]);

			DEB(LVL_ALGS, "Algorithms::fRestrUSP: From " << lEvents[i].cCity << " at "
					<< fTimeFormatToString(lEvents[i].cTime) << " to " << iGraph->fGetNodeName(lTo) <<
					": " << lUgSp[j].fGetString() << endl);
		}
	}

	//convert hash-sets to vectors
	vector<vector<Path> > * lReturnPaths = new vector<vector<Path> >(iUgCitiesTo.size(), vector<Path>());
	for (size_t i = 0; i < lPaths.size(); i++) {
		for (path_set::iterator j = lPaths[i].begin(); j != lPaths[i].end(); j++) {
			(*lReturnPaths)[i].push_back(*j);
		}
		lPaths[i].clear();
	}

	return lReturnPaths;
}

int fTimeToSegment(int iSegSize, int iFromDay, int iToDay, int iTime) {
	int lSeg = (iTime - iFromDay * DAY_MIN) / iSegSize;

	lSeg = lSeg % (iToDay - iFromDay + 1);

	return lSeg;
}

int fSegments(int iSegSize, int iFromDay, int iToDay) {
	int lPeriod = (iToDay - iFromDay + 1) * DAY_MIN;
	return (lPeriod - 1) / iSegSize + 1;
}

pair<int, int> fSegmentRange(int iSegSize, int iSegment, int iFromDay) {
	return pair<int, int>(iFromDay * DAY_MIN + iSegment * iSegSize,
			iFromDay * DAY_MIN + (iSegment + 1) * iSegSize - 1);
}

vector<vector<vector<Path> > > * fTdSegUSP(TdGraph *iGraph,
	vector<int> &iUgCitiesFrom, vector<int> &iUgCitiesTo, int iSegSize,
	vector<vector<vector<vector<size_t> > > > **oPathIndices) {

	int lFromDay = iGraph->fGetFromDay();
	int lToDay = iGraph->fGetToDay();

	int lSegments = fSegments(iSegSize, lFromDay, lToDay);

	vector<vector<vector<Path> > >* lReturnPaths =
			new vector<vector<vector<Path> > >(
					iUgCitiesFrom.size(), vector<vector<Path> >(
							iUgCitiesTo.size(), vector<Path>()));

	if (*oPathIndices != NULL) {
		delete *oPathIndices;
	}
	*oPathIndices = new vector<vector<vector<vector<size_t> > > >(
			iUgCitiesFrom.size(), vector<vector<vector<size_t> > >(
					iUgCitiesTo.size(), vector<vector<size_t> >(
							lSegments, vector<size_t>())));

	int lOldPercentage = -1;
	for (size_t i = 0; i < iUgCitiesFrom.size(); i++) {
		for (int k = 0; k < lSegments; k++) {
			int lPercentage = ((i * lSegments + k) * 100) / (iUgCitiesFrom.size() * lSegments);
			if (lPercentage != lOldPercentage) {
				INFO("Progress " << lPercentage << " % " << endl);
				lOldPercentage = lPercentage;
			}

			pair<int, int> lRange = fSegmentRange(iSegSize, k, lFromDay);
			gLogger->fInfoOn(false);
			vector<vector<Path> > *lFromPaths = fRestrUSP(iGraph, iUgCitiesFrom[i], iUgCitiesTo, NULL,
					lRange);
			gLogger->fInfoOn(true);

			//add USPs and note indices
			for (int j = 0; j < iUgCitiesTo.size(); j++) {
				int lFromIndex = (*lReturnPaths)[i][j].size();

				(*lReturnPaths)[i][j].insert((*lReturnPaths)[i][j].end(),
						(*lFromPaths)[j].begin(), (*lFromPaths)[j].end());

				int lToIndex = (*lReturnPaths)[i][j].size() - 1;

				for (int l = lFromIndex; l <= lToIndex; l++) {
					(**oPathIndices)[i][j][k].push_back(l);
				}
			}

			delete lFromPaths;
		}

		//compact USPs and indices
		for (int j = 0; j < iUgCitiesTo.size(); j++) {
			unordered_map<Path, int, PathHash, PathEqual> lPath2Index;

			vector<Path> &lPaths = (*lReturnPaths)[i][j];
			vector<Path> lNewPaths;

			for (int k = 0; k < lPaths.size(); k++) {
				if (lPath2Index.find(lPaths[k]) == lPath2Index.end()) {
					lPath2Index[lPaths[k]] = lNewPaths.size();
					lNewPaths.push_back(lPaths[k]);
				}
			}

			vector<vector<size_t> > &lPathIndices = (**oPathIndices)[i][j];

			for (int k = 0; k < lPathIndices.size(); k++) {
				vector<size_t> &lPathSegIndices = lPathIndices[k];
				for (int l = 0; l < lPathSegIndices.size(); l++) {
					Path &lPath = lPaths[lPathSegIndices[l]];
					lPathSegIndices[l] = lPath2Index[lPath];
				}
			}

			lPaths = lNewPaths;
		}
	}

	return lReturnPaths;
}

vector<vector<UspTree> >* fUspTrees(
		UgGraph *iUgGraph, TimeGraphObject *iGraph,
		vector<int> &iUgCitiesFrom, vector<int> &iUgCitiesTo) {

	//get usps
	INFO("Getting USPs" << endl);
	vector<vector<vector<Path> > >* lUsps = fUSP(iUgGraph, iGraph, iUgCitiesFrom, iUgCitiesTo);

	//make usp trees
	INFO("Making USP trees" << endl);
	vector<vector<UspTree> >* lUspTrees = new vector<vector<UspTree> >(
			iUgCitiesFrom.size(), vector<UspTree>(iUgCitiesTo.size(), UspTree()));
	for (size_t i = 0; i < lUsps->size(); i++) {
		for (size_t j = 0; j < (*lUsps)[i].size(); j++) {
			for (size_t k = 0; k < (*lUsps)[i][j].size(); k++) {
				for (int l = 0; l < (*lUsps)[i][j][k].fGetSize() - 1; l++) {
					int lFrom =(*lUsps)[i][j][k].fGet(l);
					int lTo = (*lUsps)[i][j][k].fGet(l + 1);
					(*lUspTrees)[i][j].fAdd(lFrom, lTo);
				}
			}
			(*lUsps)[i][j].clear();
		}
		(*lUsps)[i].clear();
	}

	delete(lUsps);

	return lUspTrees;
}

vector<vector<vector<pair<Path, int> > > >* fUSPUsageAlso(
		UgGraph *iUgGraph, TimeGraphObject *iGraph,
		vector<int> &iUgCitiesFrom, vector<int> &iUgCitiesTo) {

	//prepare cities in sets
	unordered_set<string> lCitiesFrom;
	for (size_t i = 0; i < iUgCitiesFrom.size(); i++) {
		lCitiesFrom.insert(iUgGraph->fGetNodeName(iUgCitiesFrom[i]));
	}
	unordered_set<string> lCitiesTo;
	for (size_t i = 0; i < iUgCitiesTo.size(); i++) {
		lCitiesTo.insert(iUgGraph->fGetNodeName(iUgCitiesTo[i]));
	}

	//prepare cities in strings
	unordered_map<string, int> lCitiesFromToIndex;
	for (size_t i = 0; i < iUgCitiesFrom.size(); i++) {
		string lCityName = iUgGraph->fGetNodeName(iUgCitiesFrom[i]);
		lCitiesFromToIndex[lCityName] = i;
	}
	unordered_map<string, int> lCitiesToToIndex;
	for (size_t i = 0; i < iUgCitiesTo.size(); i++) {
		string lCityName = iUgGraph->fGetNodeName(iUgCitiesTo[i]);
		lCitiesToToIndex[lCityName] = i;
	}

	//get all the events corresponding to departure cities
	vector<Event> lEvents = iGraph->fGetEvents(lCitiesFrom, ANY_RANGE);

	//for each pair of departing-destination city we have a set of shortest UG paths
	vector<vector<unordered_map<Path, int, PathHash, PathEqual> > > lPaths(iUgCitiesFrom.size(),
			vector<unordered_map<Path, int, PathHash, PathEqual> >(iUgCitiesTo.size(),
					unordered_map<Path, int, PathHash, PathEqual>()));

	INFO("Going to run search from " << lEvents.size() << " events" << endl);

	int lOldPercentage = -1;
	//from all events launch a search...
	for (size_t i = 0; i < lEvents.size(); i++) {
		int lPercentage = (i * 100) / lEvents.size();
		if (lPercentage != lOldPercentage) {
			INFO("Progress " << lPercentage << " % " << endl);
			lOldPercentage = lPercentage;
		}

		DEB(LVL_ALGS, "Algorithms::fUSP: search from "
				<< lEvents[i].cCity << " at time " <<
				fTimeFormatToString(lEvents[i].cTime) << endl);

		//...to all destination cities
		unordered_map<string, CityPath> lUgSp =
				fDijkWrapUgSpToCities(iGraph, lEvents[i].cCity, lEvents[i].cTime, lCitiesTo);

		//add paths to the corresponding hash set
		for (unordered_map<string, CityPath>::iterator j = lUgSp.begin(); j != lUgSp.end(); j++) {
			int lFrom = lCitiesFromToIndex[lEvents[i].cCity];
			int lTo = lCitiesToToIndex[j->first];

			Path lPath = fCityPathToPath(j->second, iUgGraph);

			if (lFrom == lTo || lPath.fGetSize() != 0) {
				unordered_map<Path, int, PathHash, PathEqual>::iterator lIt =
						lPaths[lFrom][lTo].find(lPath);
				if (lIt == lPaths[lFrom][lTo].end()) {
					lPaths[lFrom][lTo][lPath] = 1;
				}
				else {
					lIt->second++;
				}
			}

			DEB(LVL_ALGS, "Algorithms::fUSP: From " << lEvents[i].cCity << " at "
					<< fTimeFormatToString(lEvents[i].cTime) << " to " << j->first <<
					": " << j->second.fGetString() << endl);
		}
	}

	//convert hash-sets to vectors
	vector<vector<vector<pair<Path, int> > > >* lReturnPaths =
			new vector<vector<vector<pair<Path, int> > > >(iUgCitiesFrom.size(),
			vector<vector<pair<Path, int> > >(iUgCitiesTo.size(), vector<pair<Path, int> >()));
	for (size_t i = 0; i < lPaths.size(); i++) {
		for (size_t j = 0; j < lPaths[i].size(); j++) {
			for (unordered_map<Path, int, PathHash, PathEqual>::iterator k = lPaths[i][j].begin();
					k != lPaths[i][j].end(); k++) {
				(*lReturnPaths)[i][j].push_back(pair<Path, int>(k->first, k->second));
			}
			lPaths[i][j].clear();
		}
		lPaths[i].clear();
	}

	return lReturnPaths;
}

vector<vector<int> >* fUSPCountOnly(
		UgGraph *iUgGraph, TimeGraphObject *iGraph,
		vector<int> &iUgCitiesFrom, vector<int> &iUgCitiesTo) {

	//prepare cities in sets
	unordered_set<string> lCitiesFrom;
	for (size_t i = 0; i < iUgCitiesFrom.size(); i++) {
		lCitiesFrom.insert(iUgGraph->fGetNodeName(iUgCitiesFrom[i]));
	}
	unordered_set<string> lCitiesTo;
	for (size_t i = 0; i < iUgCitiesTo.size(); i++) {
		lCitiesTo.insert(iUgGraph->fGetNodeName(iUgCitiesTo[i]));
	}

	//prepare cities in strings
	unordered_map<string, int> lCitiesFromToIndex;
	for (size_t i = 0; i < iUgCitiesFrom.size(); i++) {
		string lCityName = iUgGraph->fGetNodeName(iUgCitiesFrom[i]);
		lCitiesFromToIndex[lCityName] = i;
	}
	unordered_map<string, int> lCitiesToToIndex;
	for (size_t i = 0; i < iUgCitiesTo.size(); i++) {
		string lCityName = iUgGraph->fGetNodeName(iUgCitiesTo[i]);
		lCitiesToToIndex[lCityName] = i;
	}

	//get all the events corresponding to departure cities
	vector<Event> lEvents = iGraph->fGetEvents(lCitiesFrom, ANY_RANGE);

	//for each pair of departing-destination city we have a set of shortest UG paths
	vector<vector<unordered_set<size_t> > > lPathHashes(iUgCitiesFrom.size(),
			vector<unordered_set<size_t> >(iUgCitiesTo.size(),
					unordered_set<size_t>()));

	INFO("Going to run search from " << lEvents.size() << " events" << endl);

	int lOldPercentage = -1;
	//from all events launch a search...
	for (size_t i = 0; i < lEvents.size(); i++) {
		int lPercentage = (i * 100) / lEvents.size();
		if (lPercentage != lOldPercentage) {
			INFO("Progress " << lPercentage << " % " << endl);
			lOldPercentage = lPercentage;
		}

		DEB(LVL_ALGS, "Algorithms::fUSP: search from "
				<< lEvents[i].cCity << " at time " <<
				fTimeFormatToString(lEvents[i].cTime) << endl);

		//...to all destination cities
		unordered_map<string, CityPath> lUgSp =
				fDijkWrapUgSpToCities(iGraph, lEvents[i].cCity, lEvents[i].cTime, lCitiesTo);

		//add paths to the corresponding hash set
		for (unordered_map<string, CityPath>::iterator j = lUgSp.begin(); j != lUgSp.end(); j++) {
			int lFrom = lCitiesFromToIndex[lEvents[i].cCity];
			int lTo = lCitiesToToIndex[j->first];

			if (lFrom == lTo || j->second.fGetSize() != 0) {
				lPathHashes[lFrom][lTo].insert(gCityPathHash(j->second));
			}

			DEB(LVL_ALGS, "Algorithms::fUSP: From " << lEvents[i].cCity << " at "
					<< fTimeFormatToString(lEvents[i].cTime) << " to " << j->first <<
					": " << j->second.fGetString() << endl);
		}
	}

	//get USP counts for each pair
	vector<vector<int> >* lReturnCounts = new vector<vector<int> >(
			iUgCitiesFrom.size(), vector<int>(iUgCitiesTo.size(), 0));
	for (size_t i = 0; i < lPathHashes.size(); i++) {
		for (size_t j = 0; j < lPathHashes[i].size(); j++) {
			(*lReturnCounts)[i][j] = lPathHashes[i][j].size();
		}
		lPathHashes[i].clear();
	}

	return lReturnCounts;
}

//TODO - nech vybere najlepsi prienik!!
vector<vector<vector<Path> > >* fMultiUSP(
		UgGraph *iUgGraph, TimeGraphObject *iGraph,
		vector<int> &iUgCitiesFrom, vector<int> &iUgCitiesTo) {

	//prepare cities in strings
	vector<string> lCitiesFrom(iUgCitiesFrom.size(), "");
	for (size_t i = 0; i < iUgCitiesFrom.size(); i++) {
		lCitiesFrom[i] = iUgGraph->fGetNodeName(iUgCitiesFrom[i]);
	}
	unordered_map<string, int> lCitiesFromToIndex;
	for (size_t i = 0; i < lCitiesFrom.size(); i++) {
		lCitiesFromToIndex[lCitiesFrom[i]] = i;
	}
	vector<string> lCitiesTo(iUgCitiesTo.size(), "");
	for (size_t i = 0; i < iUgCitiesTo.size(); i++) {
		lCitiesTo[i] = iUgGraph->fGetNodeName(iUgCitiesTo[i]);
	}
	unordered_map<string, int> lCitiesToToIndex;
	for (size_t i = 0; i < lCitiesTo.size(); i++) {
		lCitiesToToIndex[lCitiesTo[i]] = i;
	}

	//get all cities
	vector<string> lAllCities = iUgGraph->fGetNodeNames();

	//get all the events corresponding to departure cities
	vector<Event> lEvents = iGraph->fGetEvents(unordered_set<string>(lCitiesFrom.begin(), lCitiesFrom.end()),
			ANY_RANGE);

	//for each pair of departing-destination city we have a set of shortest UG paths
	vector<vector<unordered_set<Path, PathHash, PathEqual> > > lPaths(iUgCitiesFrom.size(),
			vector<unordered_set<Path, PathHash, PathEqual> >(iUgCitiesTo.size(),
					unordered_set<Path, PathHash, PathEqual>()));

	INFO("Going to run search from " << lEvents.size() << " events" << endl);

	int lOldPercentage = -1;
	//from all events launch a search...
	for (size_t i = 0; i < lEvents.size(); i++) {
		int lPercentage = (i * 100) / lEvents.size();
		if (lPercentage != lOldPercentage) {
			INFO("Progress " << lPercentage << " % " << endl);
			lOldPercentage = lPercentage;
		}
		DEB(LVL_ALGS, "Algorithms::fMultiUSP: search from "
				<< lEvents[i].cCity << " (" << lCitiesToToIndex[lEvents[i].cCity] << ") at time " <<
				fTimeFormatToString(lEvents[i].cTime) << endl);

		//...to all destination cities
		unordered_map<string, vector<Path> > lMultiUgSp =
				fDijkWrapMultiUgSpToCities(iGraph, lEvents[i].cCity, lEvents[i].cTime, iUgCitiesTo,
						&lAllCities);

		//add paths to the corresponding hash set
		for (unordered_map<string, vector<Path> >::iterator j = lMultiUgSp.begin(); j != lMultiUgSp.end(); j++) {
			int lFrom = lCitiesFromToIndex[lEvents[i].cCity];
			int lTo = lCitiesToToIndex[j->first];

			DEB(LVL_ALGS, "Algorithms::fMultiUSP: From " << lEvents[i].cCity << " at "
					<< fTimeFormatToString(lEvents[i].cTime) << " to " << j->first << ": "
					<< j->second.size() <<
					" paths" << endl);
			for (size_t k = 0; k < j->second.size(); k++) {
				DEB(LVL_ALGDET, "Algorithms::fMultiUSP:	" <<
						j->second[k].fGetString() << endl);
				if (lFrom == lTo || j->second[k].fGetSize() != 0) {
					lPaths[lFrom][lTo].insert(j->second[k]);
				}
			}
		}

//		//THROUGH PATHS MADE FROM CONNECTIONS version
//		//to all destination cities
//		map<string, vector<Connection> > lMultiConn =
//				fDijkWrapMultiConnToCities(iGraph, lEvents[i].cCity, lEvents[i].cTime, iUgCitiesTo,
//						&lAllCities);
//
//		for (map<string, vector<Connection> >::iterator j = lMultiConn.begin(); j != lMultiConn.end(); j++) {
//			//add paths to the hash set
//			int lFrom = lCitiesFromToIndex[lEvents[i].cCity];
//			int lTo = lCitiesToToIndex[j->first];
//
//			unordered_set<Path, PathHash, PathEqual> lLocalSet;
//			for (size_t k = 0; k < j->second.size(); k++) {
//				lLocalSet.insert(iUgGraph->fMakePath(j->second[k]));
//				lPaths[lFrom][lTo].insert(iUgGraph->fMakePath(j->second[k]));
//			}
//			INFO("from " << lEvents[i].cCity << " at " << lEvents[i].cTime << " to " << j->first <<
//					", " << lLocalSet.size() << " connections: " << endl);
//			for (unordered_set<Path, PathHash, PathEqual>::iterator k = lLocalSet.begin();
//					k != lLocalSet.end(); k++) {
//				Path lPath = *k;
//				INFO("path: " << lPath.fGetString() << endl);
//			}
//		}
	}

	//convert hash-sets to vectors
	vector<vector<vector<Path> > >* lReturnPaths = new vector<vector<vector<Path> > >(iUgCitiesFrom.size(),
			vector<vector<Path> >(iUgCitiesTo.size(), vector<Path>()));
	for (size_t i = 0; i < lPaths.size(); i++) {
		for (size_t j = 0; j < lPaths[i].size(); j++) {
			for (unordered_set<Path, PathHash, PathEqual>::iterator k = lPaths[i][j].begin();
					k != lPaths[i][j].end(); k++) {
				(*lReturnPaths)[i][j].push_back(*k);
			}
			lPaths[i][j].clear();
		}
		lPaths[i].clear();
	}

	return lReturnPaths;
}
