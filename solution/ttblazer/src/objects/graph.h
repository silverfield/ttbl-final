/* Implements generic (template) graph that is then used in implementation of
 * time-dependent graph, underlying graph...
 *
 * Template parameter specifies the type of data stored with nodes and edges
 *
 * The graph is implemented as a vector of nodes (thus each node has an index)
 * and each node has a map of its neighbors */
#ifndef GRAPH_H_
#define GRAPH_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <list>
#include <vector>
#include <map>
#include <string>

#include <boost/unordered_map.hpp>

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

//unused node has this index
#define UNUSED -1

/*****************************************************************/
/* Class
/*****************************************************************/

/* Specifies non-template functions implemented by Graph */
class GraphInterface {
private:
public:
	//----- constructors -----
	virtual ~GraphInterface() = 0;

	//----- interface -----
	virtual int fAddNode() = 0;
	virtual void fRemoveNode(int iX) = 0;
	virtual void fAddArc(int iX, int iY) = 0;
	virtual void fRemoveArc(int iX, int iY) = 0;
	virtual std::vector<int> fRestruct() = 0;

	virtual std::string fGetGraphString() const = 0;
	virtual bool fNodeIndexExists(int iX) const = 0;
	virtual bool fArcExists(int iX, int iY) const = 0;
	virtual std::vector<int> fGetNodeIndices() const = 0;
	virtual int fGetDeg(int iX) const = 0;
	virtual void fClearTo(int iX) = 0;
	virtual void fRemoveOrientation() = 0;
	virtual void fReverseOrientation() = 0;
	virtual std::vector<int> fShuffle() = 0;
	virtual std::string fGetArcsString(int iX) const = 0;
	virtual std::vector<int> fGetTo(int iX) const = 0;

	virtual bool fIsRestructed() const = 0;
	virtual int fGetN() const = 0;
	virtual int fGetM() const = 0;
	virtual int fGetSpace() const = 0;
};
inline GraphInterface::~GraphInterface() {};

template <class TArcData>
struct Arc {
	TArcData cData;
	int cXIndex;
	int cYIndex;

	Arc() {
		cData = TArcData();
		cXIndex = -1;
		cYIndex = -1;
	}

	int fGetSpace() const {
		int lBytes = 0;

		lBytes += cData.fGetSpace();

		return lBytes;
	}
};

template <class TNodeData, class TArcData>
struct Node {
	TNodeData cData;
	int cIndex;
	boost::unordered_map<int, Arc<TArcData> > cArcs;

	Node() {
		cIndex = -1;
		cData = TNodeData();
	}

	std::string fGetId() {
		if (cData.fGetId() == "") {
			return fIntToStr(cIndex);
		}

		return cData.fGetId();
	}

	int fGetSpace() const {
		int lBytes = 0;

		lBytes += cData.fGetSpace();
		lBytes += fSizeOf(cArcs);
		for (typename boost::unordered_map<int, Arc<TArcData> >::const_iterator i = cArcs.begin();
				i != cArcs.end(); i++) {
			lBytes += i->second.fGetSpace();
		}

		return lBytes;
	}
};

/* We implement the graph as a vector of nodes, each having its
 * own "dictionary" of type (neighbor index->arc). So e.g.:
 * 1 -> (2, arc-1-2), (4, arc-1-4)...
 * 2
 * hole //there may be holes in the vector of nodes made by removal
 * 4
 * hole
 * 6 */
template<class TNodeData, class TArcData>
class Graph : public GraphInterface {
private:
	//----- data -----
	std::vector<Node<TNodeData, TArcData> > cNodes;
	std::list<int> cHoles;
	int cM;	//number of edges
	int cN;	//number of nodes
public:
	//----- constructors -----
	Graph();

	//----- interface -----
	virtual int fAddNode(); //O(1) amortized (does not hold only when vector of nodes is extended in memory)
	TNodeData* fGetNodeData(int iX) ; //O(1)
	virtual void fRemoveNode(int iX); //O(1)
	virtual void fAddArc(int iX, int iY); //O(log k), where k = # of neighbors of node indexed by iX
	TArcData* fGetArcData(int iX, int iY); //O(log k)
	virtual void fRemoveArc(int iX, int iY); //O(log k)
	virtual std::vector<int> fRestruct(); //O(p + m log n), see source for explanation

	virtual std::string fGetGraphString() const;
	virtual bool fNodeIndexExists(int iX) const;
	virtual bool fArcExists(int iX, int iY) const;
	virtual std::vector<int> fGetNodeIndices() const;
	virtual int fGetDeg(int iX) const;
	boost::unordered_map<int, Arc<TArcData> > fGetArcs(int iX) const;
	void fSetArcs(int iX, boost::unordered_map<int, Arc<TArcData> > iArcs);
	virtual void fClearTo(int iX);
	virtual void fRemoveOrientation(); //O(p + m log n)
	virtual void fReverseOrientation();
	Graph<TNodeData, TArcData> fSubGraph(std::vector<int> iIndices) const; //O(m' log n' + m)
	virtual std::vector<int> fShuffle();
	virtual std::string fGetArcsString(int iX) const;
	virtual int fGetSpace() const;
	virtual std::vector<int> fGetTo(int iX) const;

	//----- inline interface -----
	virtual inline bool fIsRestructed() const {return cHoles.size() == 0;};
	virtual inline int fGetN() const {return cN;};
	virtual inline int fGetM() const {return cM;};
};

#include "graph.cpp"

#endif /* GRAPH_H_ */
