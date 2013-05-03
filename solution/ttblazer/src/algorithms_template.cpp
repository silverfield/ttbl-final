/*****************************************************************/
/* Includes
/*****************************************************************/

#include <queue>
#include <stack>

#include "central.h"

#include "algorithms.h"
#include "objects/graph.h"

//using namespace std; //this is a part of a header file!!!

/*****************************************************************/
/* Variables
/*****************************************************************/

//fStrongConn
extern Partition gStrongConnPartition;
extern int gStrongConnLabel;
extern std::vector<int> gStrongConnLabels;
extern std::vector<int> gStrongConnLows;
extern std::vector<bool> gStrongConnInStack;
extern std::stack<int> gStrongConnStack;

/*****************************************************************/
/* Functions
/*****************************************************************/

template<class TNodeData, class TArcData>
Partition fConn(const Graph<TNodeData, TArcData> *iGraph) {
	Partition lPartition;
	if (iGraph->fIsRestructed() == false) {
		ERR("Algorithms::fConn: Graph must be restructed!" << std::endl);
		return lPartition;
	}

	Graph<TNodeData, TArcData> lGraph = *iGraph;
	lGraph.fRemoveOrientation(); //this is O(m log n)

	//empty graph -> return empty partition
	if (iGraph->fGetN() == 0) {
		return lPartition;
	}

	lPartition.fAddSet();

	//we are going to do breadth-first search
	std::queue<int> lQueue;
	std::vector<bool> lChecked;
	int lN = lGraph.fGetN();
	for (int i = 0; i < lN; i++) {
		lChecked.push_back(false);
	}
	lQueue.push(0);

	int lCheckedCount = 0;
	while (true) {
		while (lQueue.empty() == false) {
			int lNextVertex = lQueue.front();
			lQueue.pop();
			if (lChecked[lNextVertex] == true) {
				continue;
			}
			lChecked[lNextVertex] = true;
			lCheckedCount++;
			DEB(LVL_ALGS, "Algorithms::fConn: Checked " << lCheckedCount << " out of " <<
					lN << std::endl);
			lPartition.fAddToSet(lPartition.fGetSetCount() - 1, lNextVertex);
			boost::unordered_map<int, Arc<TArcData> > lArcs = lGraph.fGetArcs(lNextVertex);

			for (typename boost::unordered_map<int, Arc<TArcData> >::iterator i = lArcs.begin();
					i != lArcs.end(); i++) {
				if (lChecked[i->first] == false) {
					lQueue.push(i->first);
				}
			}
		}

		//all vertices reached -> finish
		if (lCheckedCount >= lN) {
			break;
		}

		//otherwise add new set
		lPartition.fAddSet();
		for (std::vector<bool>::size_type i = 0; i < lChecked.size(); i++) {
			if (lChecked[i] == false) {
				lQueue.push(i);
				break;
			}
		}
	}

	return lPartition;
}

template<class TNodeData, class TArcData>
Graph<TNodeData, TArcData> fSubCon(const Graph<TNodeData, TArcData> *iGraph, int iSize) {
	Partition lPartition;
	if (iGraph->fIsRestructed() == false) {
		ERR("Algorithms::fConn: Graph must be restructed!" << std::endl);
		return Graph<TNodeData, TArcData>();
	}

	Graph<TNodeData, TArcData> lGraph = *iGraph;
	std::vector<int> lTranslations = lGraph.fShuffle();
	lGraph.fRemoveOrientation(); //this is O(m log n)

	INFO("Graph shuffled and removed orientation" << std::endl);

	//empty graph -> return empty partition
	if (iGraph->fGetN() == 0) {
		return Graph<TNodeData, TArcData>();
	}

	lPartition.fAddSet();

	//we are going to do breadth-first search
	std::queue<int> lQueue;
	std::vector<bool> lChecked;
	int lN = lGraph.fGetN();
	for (int i = 0; i < lN; i++) {
		lChecked.push_back(false);
	}
	int lFirst = rand() % lGraph.fGetN();
	lQueue.push(lFirst);

	int lCheckedCount = 0;
	while (true) {
		while (lQueue.empty() == false) {
			int lNextVertex = lQueue.front();
			lQueue.pop();
			if (lChecked[lNextVertex] == true) {
				continue;
			}
			lChecked[lNextVertex] = true;
			lCheckedCount++;
			DEB(LVL_ALGS, "Algorithms::fConn: Checked " << lCheckedCount << " out of " <<
					lN << std::endl);
			if (lPartition.fGetSet(lPartition.fGetSetCount() - 1).size() < iSize) {
				lPartition.fAddToSet(lPartition.fGetSetCount() - 1, lNextVertex);
			}
			boost::unordered_map<int, Arc<TArcData> > lArcs = lGraph.fGetArcs(lNextVertex);

			for (typename boost::unordered_map<int, Arc<TArcData> >::iterator i = lArcs.begin();
					i != lArcs.end(); i++) {
				if (lChecked[i->first] == false) {
					lQueue.push(i->first);
				}
			}
		}

		//all vertices reached -> finish
		if (lCheckedCount >= lN) {
			break;
		}

		//otherwise add new set
		lPartition.fAddSet();
		for (std::vector<bool>::size_type i = 0; i < lChecked.size(); i++) {
			if (lChecked[i] == false) {
				lQueue.push(i);
				break;
			}
		}
	}

	std::vector<int> lCompIndices;

	//find partition with iSize size
	for (int i = 0; i < lPartition.fGetSetCount(); i++) {
		if (lPartition.fGetSet(i).size() == iSize) {
			lCompIndices = lPartition.fGetSet(i);
			DEB(LVL_ALGS, "Algorithms::fSubCon: Set of required size found" << std::endl);
		}
	}

	//in case of no partition of size iSize, find the largest
	if (lCompIndices.size() == 0) {
		int lLargestIndex = -1;
		int lLargest = 0;
		for (int i = 0; i < lPartition.fGetSetCount(); i++) {
			if (lPartition.fGetSet(i).size() > lLargest) {
				lLargestIndex = i;
				lLargest = lPartition.fGetSet(i).size();
			}
		}
		lCompIndices = lPartition.fGetSet(lLargestIndex);

		DEB(LVL_ALGS, "Algorithms::fSubCon: Set of required size not found. Taking maximum set" << std::endl);
	}

	Graph<TNodeData, TArcData> lSubGraph = lGraph.fSubGraph(lCompIndices);

	std::vector<int> lTranslatedComp;
	for (std::vector<int>::size_type i = 0; i < lCompIndices.size(); i++) {
		lTranslatedComp.push_back(lTranslations[lCompIndices[i]]);
	}
	Graph<TNodeData, TArcData> lCompGraph = iGraph->fSubGraph(lTranslatedComp);

	return lCompGraph;
}

template<class TNodeData, class TArcData>
void fStrongConnVisit(Graph<TNodeData, TArcData> *iGraph, int iNodeIndex) {
	//add to stack
	gStrongConnStack.push(iNodeIndex);
	gStrongConnInStack[iNodeIndex] = true;

	//assign label
	gStrongConnLabels[iNodeIndex] = gStrongConnLabel;
	gStrongConnLows[iNodeIndex] = gStrongConnLabel;
	gStrongConnLabel++;

	//go through all edges
	DEB(LVL_ALGS, "Algorithms::fStrongConnVisit: cTo: " << iGraph->fGetArcsString(iNodeIndex));
	boost::unordered_map<int, Arc<TArcData> > lArcs = iGraph->fGetArcs(iNodeIndex);
	for (typename boost::unordered_map<int, Arc<TArcData> >::iterator i = lArcs.begin();
			i != lArcs.end(); i++) {
		if (gStrongConnLabels[i->first] == -1) {
		if (gStrongConnInStack[i->first] == false)
			fStrongConnVisit(iGraph, i->first);
			gStrongConnLows[iNodeIndex] = std::min(gStrongConnLows[iNodeIndex], gStrongConnLows[i->first]);
		}
		else if (gStrongConnInStack[i->first] == true) {
			gStrongConnLows[iNodeIndex] = std::min(gStrongConnLows[iNodeIndex], gStrongConnLabels[i->first]);
		}
	}
	DEB(LVL_ALGS, "Algorithms::fStrongConnVisit: gStrongConnLows[" << iNodeIndex << "] = " <<
			gStrongConnLows[iNodeIndex] << std::endl);

	//mark new partition
	if (gStrongConnLows[iNodeIndex] == gStrongConnLabels[iNodeIndex]) {
		int lPartIndex = gStrongConnPartition.fAddSet();
		int lRemovedIndex = -1;
		do {
			//get index
			lRemovedIndex = gStrongConnStack.top();
			gStrongConnStack.pop();
			gStrongConnInStack[lRemovedIndex] = false;

			//put to partition
			gStrongConnPartition.fAddToSet(lPartIndex, lRemovedIndex);

			//remove all edges for the index
		} while (lRemovedIndex != iNodeIndex);
	}
}

template<class TNodeData, class TArcData>
Partition fStrongConn(const Graph<TNodeData, TArcData> *iGraph) {
	if (iGraph->fIsRestructed() == false) {
		ERR("Algorithms::fConn: Graph must be restructed!" << std::endl);
		return gStrongConnPartition;
	}

	Graph<TNodeData, TArcData> lGraph = *iGraph;
	int lN = lGraph.fGetN();

	//initializations
	gStrongConnLabel = 0;
	gStrongConnPartition = Partition();
	gStrongConnStack = std::stack<int>();
	gStrongConnLabels = std::vector<int>(lN + 1, -1);
	gStrongConnLows = std::vector<int>(lN + 1, -1);
	gStrongConnInStack = std::vector<bool>(lN + 1, false);

	for (int i = 0; i < lN; i++) {
		if (gStrongConnLabels[i] == -1) {
			fStrongConnVisit(&lGraph, i);
		}
	}

	return gStrongConnPartition;
}

template<class TNodeData, class TArcData>
Graph<TNodeData, TArcData> fStrongComp(const Graph<TNodeData, TArcData> *iGraph) {
	Partition lPartition = fStrongConn(iGraph);

	//get largest set from the partition
	int lLargestIndex = -1;
	int lLargest = 0;
	for (int i = 0; i < lPartition.fGetSetCount(); i++) {
		if (lPartition.fGetSet(i).size() > lLargest) {
			lLargestIndex = i;
			lLargest = lPartition.fGetSet(i).size();
		}
	}

	std::vector<int> lComp = lPartition.fGetSet(lLargestIndex);
	Graph<TNodeData, TArcData> lCompGraph = iGraph->fSubGraph(lComp);

	return lCompGraph;
}

/*
 * Outputs a map (degree) -> (indices of nodes that have such degree)
 */
template<class TNodeData, class TArcData>
std::map<int, std::vector<int> > fDegreeGroups(const Graph<TNodeData, TArcData> *iGraph) {
	std::map<int, std::vector<int> > lDegrees;
	std::vector<int> lIndices = iGraph->fGetNodeIndices();

	for (std::vector<int>::size_type i = 0; i < lIndices.size(); i++) {
		int lDegree = iGraph->fGetDeg(lIndices[i]);
		if (lDegrees.find(lDegree) == lDegrees.end()) {
			lDegrees.insert(std::pair<int, std::vector<int> >(lDegree, std::vector<int>()));
		}
		lDegrees[lDegree].push_back(lIndices[i]);
	}

	return lDegrees;
}

/*
 * Outputs a frequency of each degree - that is
 * 0 -> how many nodes have degree 0
 * 1 -> how many nodes have degree 1
 * ...
 */
template<class TNodeData, class TArcData>
std::vector<int> fDegreeFreqs(const Graph<TNodeData, TArcData> *iGraph) {
	std::vector<int> lDegrees;
	std::vector<int> lIndices = iGraph->fGetNodeIndices();

	for (std::vector<int>::size_type i = 0; i < lIndices.size(); i++) {
		int lDegree = iGraph->fGetDeg(lIndices[i]);
		if (lDegree >= lDegrees.size()) {
			for (int i = lDegrees.size(); i <= lDegree; i++) {
				lDegrees.push_back(0);
			}
		}
		lDegrees[lDegree]++;
	}

	return lDegrees;
}

