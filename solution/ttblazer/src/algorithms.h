/* Algorithms used by more DO methods and/or analysers, modifiers... */
#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"
#include "structures.h"
#include "objects/graph.h"
#include "objects/timedep.h"
#include "objects/timeexp.h"
#include "objects/undergr.h"
#include "objects/timetable.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Simple functions
/*****************************************************************/

//Complexity O(n)
double fAvgDegree(std::vector<int> iDegreeFreqs);
int fFirstNonZeroValue(std::vector<int> iVector);
int fLastNonZeroValue(std::vector<int> iVector);
std::vector<double> fNormalize(std::vector<double> iVector, double iRangeMin, double iRangeMax);

/*****************************************************************/
/* Dijkstra implementations
/*****************************************************************/

//Complexity O(n^2)
int fDijkstraDist(GraphObject *iGraph, int iFrom, int iTime, int iTo, std::string iToCity,
		bool iHasWeights);
Path fDijkstraSp(GraphObject *iGraph, int iFrom, int iTime, int iTo, std::string iToCity,
		bool iHasWeights, boost::unordered_set<std::string> &iAllowedCities);
Connection fDijkstraConn(TdGraph *iGraph, int iFrom, int iTime, int iTo);
std::vector<Path> fDijkstraSpToAll(GraphObject *iGraph, int iFrom, int iTime, bool iHasWeights);
std::vector<Path> fDijkstraRandomSpToAll(GraphObject *iGraph, int iFrom, int iTime, bool iHasWeights);

//Complexity O(n^2 * #paths), #paths = max(number of paths from i to j), can be exponential in extreme cases
std::vector<std::vector<Path> > fDijkstraMultiSpToAll(GraphObject *iGraph, int iFrom, int iTime,
		bool iHasWeights);

//Complexity O(n^3 * #paths)
std::vector<std::vector<std::vector<Path> > > fDijkstraAllMultiSp(GraphObject *iGraph, bool iHasWeights);

//Complexity O(n^2)
boost::unordered_map<std::string, Path> fDijkstraSpToCities(TimeGraphObject *iGraph, int iFrom, int iTime,
		boost::unordered_set<std::string> &iToCities, bool iHasWeights);
boost::unordered_map<std::string, CityPath> fDijkstraUgSpToCities(TimeGraphObject *iGraph, int iFrom, int iTime,
		boost::unordered_set<std::string> &iToCities, bool iHasWeights);
boost::unordered_map<std::string, Path> fDijkstraRandomSpToCities(TimeGraphObject *iGraph, int iFrom, int iTime,
		boost::unordered_set<std::string> &iToCities, bool iHasWeights);

//Complexity O(n^2 * #paths)
std::vector<std::vector<Path> > fDijkstraMultiSpToCities(TimeGraphObject *iGraph, int iFrom, int iTime,
		std::vector<int> iToCityInd, std::vector<std::string> *iAllCities, bool iWannaUgPaths,
		bool iHasWeights);

Path fDijkstraRestr(TdGraph *iGraph, int iFrom, int iTime, int iTo, boost::unordered_set<int> &iRestr);
std::vector<Path> fDijkstraToCitiesRestr(TdGraph *iGraph, int iFrom, int iTime,
		std::vector<int> &iToCities, boost::unordered_set<int> &iRestr);

/*****************************************************************/
/* Floyd Warshall
/*****************************************************************/

//Complexity O(n^3)
std::vector<std::vector<Path> > fFloydWarshall(GraphObject *iGraph, bool iHasWeights);

//Complexity O(n^3), but it depends also on paths reconstruction, which may get exponential in extreme cases
std::vector<std::vector<std::vector<Path> > > fMultiFloydWarshall(GraphObject *iGraph, bool iHasWeights);

/*****************************************************************/
/* Betweenness
/*****************************************************************/

//Complexity O(n^3 * #paths) - see fDijkstraAllMultiSp
std::vector<double> fBetweenness(GraphObject *iGraph, std::vector<double> *oBetwPercents, bool iHasWeights);

//Complexity O(precision * n^2)
std::vector<double> fApxBetweenness(GraphObject *iGraph, std::vector<double> *oBetwPercents,
		double iPrecision, bool iHasWeights);

/*****************************************************************/
/* Radius and HD
/*****************************************************************/

//Complexity O(n^2) in case the radius covers the whole graph
boost::unordered_set<int> fNodesInRadius(GraphObject *iGraph, int iNode, int iTime, int iRadius,
		bool iHasWeights);

//Complexity O(n^3) where n is the number of nodes in the radius (subset). Works like fMultiFloydWarshall
std::vector<Path> fSpsInRadius(GraphObject *iGraph, int iNode, int iTime,
		int iRadius, int iMinPathLength, bool iMulti, double iPrecision, bool iHasWeights);
std::vector<Path> fSpsForSubset(GraphObject *iGraph, std::vector<int> &iSubset,
		int iMinPathLength, bool iMulti, double iPrecision, bool iHasWeights);

//Complexity O(p * n * log n) where p = number of paths (O(n^2), but in extreme cases exponential), n is the
//size of the subset of nodes on which we look for the cover
//log n - priority queue operation ..?
std::vector<int> fApxMinSPCover(GraphObject *iGraph, GraphObject *iReversed, int iNode, int iTime,
		int iRadius, int iMinPathLength, bool iMulti, double iPrecision, bool iHasWeights);
std::vector<int> fApxMinSPCover(std::vector<Path> &iPaths, int iN);

//Complexity O(n^3) since we use Floyd Warshall alg.
int fRadius(GraphObject *iGraph, bool iHasWeights);

int fApxRadius(GraphObject *iGraph, bool iHasWeights, double iPrecision);

int fApxHighwayDim(GraphObject *iGraph, bool iHasWeights, bool iMulti, double iPrecision, int iRepetitions);

/*****************************************************************/
/* Density
/*****************************************************************/

double fDensity(UgGraph *iGraph, int iRepetitions);

/*****************************************************************/
/* Others
/*****************************************************************/

std::vector<Path> fGetSps(GraphObject* iGraph, double iFracOfAll, bool iHasWeights);

std::vector<Connection> fGetConns(TimeGraphObject* iGraph, double iFracOfAll);

//Complexity O(m)
bool fHasCycle(GraphObject *iGraph, int iArcMinLength, int iArcMaxLength);

//
std::vector<std::pair<ElCon, ElCon> > fGetOvertakenEdges(Timetable *iTimetable);

/*****************************************************************/
/* Template functions
/*****************************************************************/

//Complexity O(m log n)
template<class TNodeData, class TArcData>
Partition fConn(const Graph<TNodeData, TArcData> *iGraph);

//Complexity O(m log n)
template<class TNodeData, class TArcData>
Graph<TNodeData, TArcData> fSubCon(const Graph<TNodeData, TArcData> *iGraph, int iSize);

//Complexity O(m + n) - Tarjan's Algorithm
template<class TNodeData, class TArcData>
Partition fStrongConn(const Graph<TNodeData, TArcData> *iGraph);

//Complexity O(m' log n' + m + n)
template<class TNodeData, class TArcData>
Graph<TNodeData, TArcData> fStrongComp(const Graph<TNodeData, TArcData> *iGraph, int iSize);

//Complexity O(n) (in most cases - there would have to be O(n) different degrees for superlinear time)
template<class TNodeData, class TArcData>
std::map<int, std::vector<int> > fDegreeGroups(const Graph<TNodeData, TArcData> *iGraph);

//Complexity O(m)
template<class TNodeData, class TArcData>
std::vector<int> fDegreeFreqs(const Graph<TNodeData, TArcData> *iGraph);

/* template functions are implemented here */
#include "algorithms_template.cpp"

#endif /* ALGORITHMS_H_ */
