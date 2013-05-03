/*****************************************************************/
/* Includes
/*****************************************************************/

#include <fstream>
#include <iostream>
#include <sstream>

#include "../../../common/src/structsizer.h"

#include "../central.h"

//using namespace std; //this is a part of corresponding header file!!!

/*****************************************************************/
/* Class - constructors
/*****************************************************************/
template <class TNodeData, class TArcData>
Graph<TNodeData, TArcData>::Graph() {
	cM = 0;
	cN = 0;
}

/*****************************************************************/
/* Interface
/*****************************************************************/

template <class TNodeData, class TArcData>
int Graph<TNodeData, TArcData>::fAddNode() {
	cN++;
	int lNewIndex = UNUSED;
	if (cHoles.size() != 0) {
		lNewIndex = cHoles.front();
		cHoles.pop_front();
	}
	else {
		lNewIndex = cNodes.size();
	}

	Node<TNodeData, TArcData> lNode;
	lNode.cIndex = lNewIndex;
	cNodes.push_back(lNode);

	DEB(LVL_GRAPH, "Graph::fAddNode: adding node index " << lNewIndex << std::endl);

	return lNewIndex;
}

template <class TNodeData, class TArcData>
TNodeData* Graph<TNodeData, TArcData>::fGetNodeData(int iX) {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		ERR("Graph::fGetNodeData: node index " << iX << " not present" << std::endl);
		return NULL;
	}

	return &(cNodes[iX].cData);
}

template <class TNodeData, class TArcData>
void Graph<TNodeData, TArcData>::fRemoveNode(int iX) {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		ERR("Graph::fRemoveNode: node index " << iX << " not present" << std::endl);
		return;
	}

	cNodes[iX].cIndex = UNUSED;
	cHoles.push_back(iX);
	cN--;

	DEB(LVL_GRAPH, "Graph::fRemoveNode: node index " << iX << " removed" << std::endl);
}

template <class TNodeData, class TArcData>
void Graph<TNodeData, TArcData>::fAddArc(int iX, int iY) {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		ERR("Graph::fAddArc: node index " << iX << " not present" << std::endl);
		return;
	}

	if (iY < 0 || iY > cNodes.size() - 1 || cNodes[iY].cIndex == UNUSED) {
		ERR("Graph::fAddArc: node index " << iY << " not present" << std::endl);
		return;
	}

	if (cNodes[iX].cArcs.find(iY) != cNodes[iX].cArcs.end()) {
		DEB(LVL_ALREADY, "Graph::fAddArc: arc already in the graph" << std::endl);
		return;
	}

	Arc<TArcData> lArc = Arc<TArcData>();
	lArc.cData = TArcData();
	lArc.cXIndex = iX;
	lArc.cYIndex = iY;
	cNodes[iX].cArcs.insert(std::pair<int, Arc<TArcData> >(iY, lArc));
	cM++;

	DEB(LVL_GRAPH, "Graph::fAddArc: arc (" << iX << ", " << iY << ") added" << std::endl);
}

template <class TNodeData, class TArcData>
TArcData* Graph<TNodeData, TArcData>::fGetArcData(int iX, int iY) {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		DEB(LVL_GRAPH, "Graph::fGetArcData: node index " << iX << " not present" << std::endl);
		return NULL;
	}

	if (iY < 0 || iY > cNodes.size() - 1 || cNodes[iY].cIndex == UNUSED ||
			cNodes[iX].cArcs.find(iY) == cNodes[iX].cArcs.end()) {
		DEB(LVL_GRAPH, "Graph::fGetArcData: node index " << iY << " not present or no arc from "
				<< iX << std::endl);
		return NULL;
	}

	return &(cNodes[iX].cArcs[iY].cData);
}

template <class TNodeData, class TArcData>
void Graph<TNodeData, TArcData>::fRemoveArc(int iX, int iY) {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		ERR("Graph::fRemoveArc: node index " << iX << " not present" << std::endl);
		return;
	}

	if (iY < 0 || iY > cNodes.size() - 1 || cNodes[iY].cIndex == UNUSED ||
			cNodes[iX].cArcs.find(iY) == cNodes[iX].cArcs.end()) {
		ERR("Graph::fRemoveArc: node index " << iY << " not present or no arc from " << iX << std::endl);
		return;
	}

	cNodes[iX].cArcs.erase(iY);
	cM--;

	DEB(LVL_GRAPH, "Graph::fRemoveArc: arc (" << iX << ", " << iY << ") removed" << std::endl);
}

/* Restructuralize the graph so that there are no holes in its inner data structures.
 * This may be necessary after many node removals from the graph
 *
 * Returned std::vector is a translation of new indices (key) to old ones (values)
 *
 * Complexity is O(p + m log n), where p is the total number of vertices added before
 * restructuralization (or simply the size of cNodes). n log n is a worst case bound
 * in arcs adjustment when we have a complete graph */
template <class TNodeData, class TArcData>
std::vector<int> Graph<TNodeData, TArcData>::fRestruct() {
	/* 1 hole hole 4 hole 6 7 8 hole ... hole 15 will become:
	 * 1 2 3 4 5 6
	 * 1 4 6 7 8 15 <- returned translations*/

	std::vector<int> lTranslations; // new index -> old index
	std::vector<int> lBackTranslations = std::vector<int>(cNodes.size(), -1); //old index -> new index

	//make new nodes array
	std::vector<Node<TNodeData, TArcData> > lNewNodes;
	for (typename std::vector<Node<TNodeData, TArcData> >::size_type i = 0; i < cNodes.size(); i++) {
		if (cNodes[i].cIndex != UNUSED) {
			Node<TNodeData, TArcData> lNode = cNodes[i];

			lTranslations.push_back(lNode.cIndex);
			lBackTranslations[lNode.cIndex] = lTranslations.size() - 1;

			lNode.cIndex = lNewNodes.size() - 1;
			lNewNodes.push_back(lNode);
		}
	}

	//adjust arcs
	for (typename std::vector<Node<TNodeData, TArcData> >::size_type i = 0; i < lNewNodes.size(); i++) {
		Node<TNodeData, TArcData> lNode = lNewNodes[i];
		boost::unordered_map<int, Arc<TArcData> > lNewTo;
		for (typename boost::unordered_map<int, Arc<TArcData> >::iterator i = lNode.cArcs.begin(); i != lNode.cArcs.end(); i++) {
			Arc<TArcData> lArc = i->second;
			lArc.cXIndex = lBackTranslations[i->first];
			lArc.cYIndex = lBackTranslations[lArc.cYIndex];

			lNewTo.insert(std::pair<int, Arc<TArcData> >(lArc.cXIndex, lArc));
			lNode.cArcs = lNewTo;
		}
	}

	//clean holes
	cHoles.clear();

	return lTranslations;
}

template <class TNodeData, class TArcData>
std::string Graph<TNodeData, TArcData>::fGetGraphString() const {
	std::stringstream lResult (std::stringstream::out);

	lResult << cN << std::endl;
	lResult << cM << std::endl;

	for (typename std::vector<Node<TNodeData, TArcData> >::size_type i = 0; i < cNodes.size(); i++) {
		if (cNodes[i].cIndex == UNUSED) {
			lResult << "	" << i << ": UNUSED";
		}
		else {
			lResult << "	" << i << ": " << fGetArcsString(i);
			lResult << std::endl;
		}
	}

	return lResult.str();
}

template <class TNodeData, class TArcData>
bool Graph<TNodeData, TArcData>::fNodeIndexExists(int iX) const {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		return false;
	}

	return true;
}

template <class TNodeData, class TArcData>
bool Graph<TNodeData, TArcData>::fArcExists(int iX, int iY) const {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		return false;
	}
	if (iY < 0 || iY > cNodes.size() - 1 || cNodes[iY].cIndex == UNUSED) {
		return false;
	}
	if (cNodes[iX].cArcs.find(iY) == cNodes[iX].cArcs.end()) {
		return false;
	}

	return true;
}

/* Returns the indices where actual nodes are stored */
template <class TNodeData, class TArcData>
std::vector<int> Graph<TNodeData, TArcData>::fGetNodeIndices() const {
	std::vector<int> lIndices;

	for (typename std::vector<Node<TNodeData, TArcData> >::size_type i = 0; i < cNodes.size(); i++) {
		if (cNodes[i].cIndex != UNUSED) {
			lIndices.push_back(i);
		}
	}

	return lIndices;
}

template <class TNodeData, class TArcData>
int Graph<TNodeData, TArcData>::fGetDeg(int iX) const {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		ERR("Graph::fGetDeg: node index " << iX << " not present" << std::endl);
		return -1;
	}

	return cNodes[iX].cArcs.size();
}

template <class TNodeData, class TArcData>
boost::unordered_map<int, Arc<TArcData> > Graph<TNodeData, TArcData>::fGetArcs(int iX) const {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		ERR("Graph::fGetArcs: node index " << iX << " not present" << std::endl);
		return boost::unordered_map<int, Arc<TArcData> >();
	}

	//DEB(LVL_GRAPH, "Graph::fGetTo: To size: " << cNodes[iX].cArcs.size() << std::endl);

	return cNodes[iX].cArcs;
}

template <class TNodeData, class TArcData>
void Graph<TNodeData, TArcData>::fSetArcs(int iX, boost::unordered_map<int, Arc<TArcData> > iArcs) {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		ERR("Graph::fSetTo: node index " << iX << " not present" << std::endl);
		return;
	}

	cNodes[iX].cArcs = iArcs;
}

template <class TNodeData, class TArcData>
void Graph<TNodeData, TArcData>::fClearTo(int iX) {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		ERR("Graph::fClearTo: node index " << iX << " not present" << std::endl);
		return;
	}

	cNodes[iX].cArcs.clear();
}

template <class TNodeData, class TArcData>
void Graph<TNodeData, TArcData>::fRemoveOrientation() {
	DEB(LVL_GRAPH, "Graph::fRemoveOrientation: Going to remove orientation" << std::endl);

	for (typename std::vector<Node<TNodeData, TArcData> >::size_type i = 0; i < cNodes.size(); i++) {
		Node<TNodeData, TArcData> *lNode = &(cNodes[i]);
		if (cNodes[i].cIndex != UNUSED) {
			for (typename boost::unordered_map<int, Arc<TArcData> >::iterator j = lNode->cArcs.begin();
					j != lNode->cArcs.end(); j++) {
				Arc<TArcData> lArc = j->second;
				if (fArcExists(lArc.cYIndex, lArc.cXIndex) == false) {
					fAddArc(lArc.cYIndex, lArc.cXIndex);
				}
			}
		}
	}
}

template <class TNodeData, class TArcData>
void Graph<TNodeData, TArcData>::fReverseOrientation() {
	DEB(LVL_GRAPH, "Graph::fReverseOrientation: Going to reverse orientation" << std::endl);

	//copy old nodes to new ones
	std::vector<Node<TNodeData, TArcData> > lNewNodes = cNodes;

	//clear new nodes' arcs
	for (typename std::vector<Node<TNodeData, TArcData> >::size_type i = 0; i < lNewNodes.size(); i++) {
		Node<TNodeData, TArcData> *lNode = &(lNewNodes[i]);

		//skip unused nodes
		if (lNode->cIndex == UNUSED) {
			continue;
		}

		lNode->cArcs.clear();
	}

	//add reversed arcs to new nodes
	for (typename std::vector<Node<TNodeData, TArcData> >::size_type i = 0; i < cNodes.size(); i++) {
		Node<TNodeData, TArcData> *lNode = &(cNodes[i]);

		//skip unused nodes
		if (lNode->cIndex == UNUSED) {
			continue;
		}

		for (typename boost::unordered_map<int, Arc<TArcData> >::iterator j = lNode->cArcs.begin();
				j != lNode->cArcs.end(); j++) {
			Arc<TArcData> lArc = j->second;
			int lTemp = lArc.cXIndex;
			lArc.cXIndex = lArc.cYIndex;
			lArc.cYIndex = lTemp;
			lNewNodes[lArc.cXIndex].cArcs.insert(std::pair<int, Arc<TArcData> >(lArc.cYIndex, lArc));
		}
	}

	//replace the nodes by new nodes
	cNodes = lNewNodes;
}

/* O(m' log n' + m) where n' is number of subgraph's nodes (similarly m') */
template <class TNodeData, class TArcData>
Graph<TNodeData, TArcData> Graph<TNodeData, TArcData>::fSubGraph(std::vector<int> iIndices) const {
	DEB(LVL_GRAPH, "Graph::fSubGraph: Indices: " << fGetVectorString(iIndices) << std::endl);

	Graph<TNodeData, TArcData> lGraph;
	std::vector<int> lTranslations = std::vector<int>(cNodes.size(), -1); //old vertex -> subgraph vertex
	std::vector<bool> lSubIndex(fGetN(), false);

	//add all nodes
	for (std::vector<int>::size_type i = 0; i < iIndices.size(); i++) {
		if (iIndices[i] < 0 || iIndices[i] >= cNodes.size() || cNodes[iIndices[i]].cIndex != iIndices[i]) {
			ERR("Index of subgraph node not present in original graph" << std::endl);
			continue;
		}

		lSubIndex[iIndices[i]] = true;

		//add new node
		int lNewIndex = lGraph.fAddNode();
		lTranslations[iIndices[i]] = lNewIndex;

		//copy node data
		Node<TNodeData, TArcData> lNode = cNodes[iIndices[i]];
		TNodeData *lNodeData = lGraph.fGetNodeData(lNewIndex);
		*lNodeData = lNode.cData;
	}

	DEB(LVL_GRAPH, "Graph::fSubGraph: translations: " << fGetVectorString(lTranslations) << std::endl);

	//add edges
	for (std::vector<int>::size_type i = 0; i < iIndices.size(); i++) {
		if (iIndices[i] < 0 || iIndices[i] >= cNodes.size() || cNodes[iIndices[i]].cIndex != iIndices[i]) {
			continue;
		}

		const boost::unordered_map<int, Arc<TArcData> > *lTo = &cNodes[iIndices[i]].cArcs;
		for (typename boost::unordered_map<int, Arc<TArcData> >::const_iterator j = lTo->begin(); j != lTo->end();
				j++) {
			if (lSubIndex[j->first] == false) {
				continue;
			}

			//add new edge
			int lFrom = lTranslations[iIndices[i]];
			int lTo = lTranslations[j->first];
			lGraph.fAddArc(lFrom, lTo);

			//copy arc data
			TArcData *lArcData = lGraph.fGetArcData(lFrom, lTo);
			*lArcData = j->second.cData;
		}
	}

	return lGraph;
}

template <class TNodeData, class TArcData>
std::vector<int> Graph<TNodeData, TArcData>::fShuffle() {
	std::vector<int> lTranslations = fRandomPermutation(cNodes.size()); // new index -> old index
	std::vector<int> lBackTranslations = std::vector<int>(cNodes.size(), -1); //old index -> new index

	DEB(LVL_GRAPH, "Graph::fShuffle: Starting shuffling with permutation: " <<
			fGetVectorString(lTranslations) << std::endl);

	//make new nodes array
	std::vector<Node<TNodeData, TArcData> > lNewNodes;
	for (typename std::vector<Node<TNodeData, TArcData> >::size_type i = 0; i < cNodes.size(); i++) {
		int lIndex = lTranslations[i];
		if (cNodes[lIndex].cIndex == UNUSED) {
			lNewNodes.push_back(cNodes[lIndex]);
			continue;
		}

		Node<TNodeData, TArcData> lNode = cNodes[lIndex];

		lBackTranslations[lIndex] = i;

		lNode.cIndex = i;
		lNewNodes.push_back(lNode);
	}

	//adjust arcs
	for (typename std::vector<Node<TNodeData, TArcData> >::size_type i = 0; i < lNewNodes.size(); i++) {
		Node<TNodeData, TArcData> *lNode = &(lNewNodes[i]);
		boost::unordered_map<int, Arc<TArcData> > lNewTo;
		for (typename boost::unordered_map<int, Arc<TArcData> >::iterator i = lNode->cArcs.begin();
				i != lNode->cArcs.end(); i++) {
			Arc<TArcData> lArc = i->second;
			lArc.cXIndex = lBackTranslations[lArc.cXIndex];
			lArc.cYIndex = lBackTranslations[lArc.cYIndex];

			lNewTo.insert(std::pair<int, Arc<TArcData> >(lArc.cYIndex, lArc));
		}
		lNode->cArcs = lNewTo;
	}

	cNodes = lNewNodes;

	return lTranslations;
}

template <class TNodeData, class TArcData>
std::string Graph<TNodeData, TArcData>::fGetArcsString(int iX) const {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		ERR("Graph::fGetArcsString: node index " << iX << " not present" << std::endl);
		return "";
	}

	std::stringstream lResult (std::stringstream::out);

	for (typename boost::unordered_map<int, Arc<TArcData> >::const_iterator i = cNodes[iX].cArcs.begin();
			i != cNodes[iX].cArcs.end(); i++) {
		if (i != cNodes[iX].cArcs.begin()) {
			lResult << ", ";
		}
		lResult << i->first << " [" << i->second.cXIndex << "," << i->second.cYIndex << "]";
	}

	return lResult.str();
}

template <class TNodeData, class TArcData>
int Graph<TNodeData, TArcData>::fGetSpace() const {
	int lBytes = 0;

	lBytes += fSizeOf<int>(cHoles);
	lBytes += fSizeOf(cNodes);
	for (typename std::vector<Node<TNodeData, TArcData> >::const_iterator i = cNodes.begin();
			i != cNodes.end(); i++) {
		lBytes += i->fGetSpace();
	}

	return lBytes;
}

template <class TNodeData, class TArcData>
std::vector<int> Graph<TNodeData, TArcData>::fGetTo(int iX) const {
	if (iX < 0 || iX > cNodes.size() - 1 || cNodes[iX].cIndex == UNUSED) {
		ERR("Graph::fGetTo: node index " << iX << " not present" << std::endl);
		return std::vector<int>();
	}

	std::vector<int> lResult;
	for (typename boost::unordered_map<int, Arc<TArcData> >::const_iterator i = cNodes[iX].cArcs.begin();
			i != cNodes[iX].cArcs.end(); i++) {
		lResult.push_back(i->first);
	}

	return lResult;
}
