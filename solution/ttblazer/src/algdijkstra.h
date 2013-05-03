/* Wrappers for DijkWrap's algorithm */
#ifndef ALGDIJKSTRA_H_
#define ALGDIJKSTRA_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"
#include "structures.h"
#include "objects/graph.h"
#include "objects/timedep.h"
#include "objects/timeexp.h"
#include "objects/undergr.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Functions
/*****************************************************************/

//Wrappers for UG graph SP search
int fDijkWrapDist(UgGraph *iGraph, int iFrom, int iTo, bool iHasWeights);
Path fDijkWrapSp(UgGraph *iGraph, int iFrom, int iTo, bool iHasWeights);
Path fDijkWrapSpRestr(UgGraph *iGraph, int iFrom, int iTo, bool iHasWeights,
		boost::unordered_set<std::string> &iAllowedCities);
std::vector<Path> fDijkWrapSpToAll(UgGraph *iGraph, int iFrom, bool iHasWeights);
std::vector<Path> fDijkWrapRandomSpToAll(UgGraph *iGraph, int iFrom, bool iHasWeights);
std::vector<std::vector<Path> > fDijkWrapMultiSpToAll(UgGraph *iGraph, int iFrom, bool iHasWeights);
std::vector<std::vector<std::vector<Path> > > fDijkWrapAllMultiSp(UgGraph *iGraph, bool iHasWeights);

//Wrappers for TE graph SP search, we use node indices thus time is not necessary. Graph is always weighted
int fDijkWrapDist(TeGraph *iGraph, int iFrom, int iTo);
Path fDijkWrapSp(TeGraph *iGraph, int iFrom, int iTo);
Path fDijkWrapSpRestr(TeGraph *iGraph, int iFrom, int iTo,
		boost::unordered_set<std::string> &iAllowedCities);
std::vector<Path> fDijkWrapSpToAll(TeGraph *iGraph, int iFrom);
std::vector<Path> fDijkWrapRandomSpToAll(TeGraph *iGraph, int iFrom);
std::vector<std::vector<Path> > fDijkWrapMultiSpToAll(TeGraph *iGraph, int iFrom);
std::vector<std::vector<std::vector<Path> > > fDijkWrapAllMultiSp(TeGraph *iGraph);

//Wrappers for TD graph SP search, we use node indices, disregard time and consider each arc length = 1
int fDijkWrapDist(TdGraph *iGraph, int iFrom, int iTo);
Path fDijkWrapSp(TdGraph *iGraph, int iFrom, int iTo);
Path fDijkWrapSpRestr(TdGraph *iGraph, int iFrom, int iTo,
		boost::unordered_set<std::string> &iAllowedCities);
std::vector<Path> fDijkWrapSpToAll(TdGraph *iGraph, int iFrom);
std::vector<Path> fDijkWrapRandomSpToAll(TdGraph *iGraph, int iFrom);
std::vector<std::vector<Path> > fDijkWrapMultiSpToAll(TdGraph *iGraph, int iFrom);
std::vector<std::vector<std::vector<Path> > > fDijkWrapAllMultiSp(TdGraph *iGraph);

//Wrappers for TE graph EA search
int fDijkWrapEa(TeGraph *iGraph, std::string iFrom, int iTime, std::string iTo);
Connection fDijkWrapConn(TeGraph *iGraph, std::string iFrom, int iTime, std::string iTo);
Connection fDijkWrapConnRestr(TeGraph *iGraph, std::string iFrom, int iTime, std::string iTo,
		boost::unordered_set<std::string> &iAllowedCities);
boost::unordered_map<std::string, Connection> fDijkWrapConnToCities(TeGraph *iGraph, std::string iFrom, int iTime,
		boost::unordered_set<std::string> &iToCities);
boost::unordered_map<std::string, Connection> fDijkWrapRandomConnToCities(TeGraph *iGraph, std::string iFrom, int iTime,
		boost::unordered_set<std::string> &iToCities);
boost::unordered_map<std::string, std::vector<Connection> > fDijkWrapMultiConnToCities(TeGraph *iGraph,
		std::string iFrom, int iTime, std::vector<int> iToCityInds, std::vector<std::string> *iAllCities);
boost::unordered_map<std::string, CityPath> fDijkWrapUgSpToCities(TeGraph *iGraph,
		std::string iFrom, int iTime, boost::unordered_set<std::string> &iToCities);
boost::unordered_map<std::string, std::vector<Path> > fDijkWrapMultiUgSpToCities(TeGraph *iGraph,
		std::string iFrom, int iTime, std::vector<int> iToCityInds, std::vector<std::string> *iAllCities);

//Wrappers for TD graph EA search
int fDijkWrapEa(TdGraph *iGraph, std::string iFrom, int iTime, std::string iTo);
Connection fDijkWrapConn(TdGraph *iGraph, std::string iFrom, int iTime, std::string iTo);
Connection fDijkWrapConnRestr(TdGraph *iGraph, std::string iFrom, int iTime, std::string iTo,
		boost::unordered_set<std::string> &iAllowedCities);
Connection fDijkWrapConnRestr(TdGraph *iGraph, int iFrom, int iTime, int iTo,
		boost::unordered_set<int> &iAllowedCities);
boost::unordered_map<int, Connection> fDijkWrapConnToCitiesRestr(TdGraph *iGraph, int iFrom, int iTime,
		boost::unordered_set<int> &iToCities, boost::unordered_set<int> &iAllowedCities);
boost::unordered_map<std::string, Connection> fDijkWrapConnToCities(TdGraph *iGraph, std::string iFrom, int iTime,
		boost::unordered_set<std::string> &iToCities);
boost::unordered_map<std::string, Connection> fDijkWrapRandomConnToCities(TdGraph *iGraph, std::string iFrom, int iTime,
		boost::unordered_set<std::string> &iToCities);
boost::unordered_map<std::string, std::vector<Connection> > fDijkWrapMultiConnToCities(TdGraph *iGraph,
		std::string iFrom, int iTime, std::vector<int> iToCityInds, std::vector<std::string> *iAllCities);
boost::unordered_map<std::string, CityPath> fDijkWrapUgSpToCities(TdGraph *iGraph,
		std::string iFrom, int iTime, boost::unordered_set<std::string> &iToCities);
boost::unordered_map<std::string, std::vector<Path> > fDijkWrapMultiUgSpToCities(TdGraph *iGraph,
		std::string iFrom, int iTime, std::vector<int> iToCityInds, std::vector<std::string> *iAllCities);

//Wrappers for TimeGraphObject EA search
inline int fDijkWrapEa(TimeGraphObject *iGraph, std::string iFrom, int iTime, std::string iTo) {
	if (iGraph->fGetObjectType() == OTTe) {
		return fDijkWrapEa(dynamic_cast<TeGraph*>(iGraph), iFrom, iTime, iTo);
	}
	return fDijkWrapEa(dynamic_cast<TdGraph*>(iGraph), iFrom, iTime, iTo);
}
inline Connection fDijkWrapConn(TimeGraphObject *iGraph, std::string iFrom, int iTime, std::string iTo) {
	if (iGraph->fGetObjectType() == OTTe) {
		return fDijkWrapConn(dynamic_cast<TeGraph*>(iGraph), iFrom, iTime, iTo);
	}
	return fDijkWrapConn(dynamic_cast<TdGraph*>(iGraph), iFrom, iTime, iTo);
}
inline Connection fDijkWrapConnRestr(TimeGraphObject *iGraph, std::string iFrom, int iTime, std::string iTo,
		boost::unordered_set<std::string> &iAllowedCities) {
	if (iGraph->fGetObjectType() == OTTe) {
		return fDijkWrapConnRestr(dynamic_cast<TeGraph*>(iGraph), iFrom, iTime, iTo, iAllowedCities);
	}
	return fDijkWrapConnRestr(dynamic_cast<TdGraph*>(iGraph), iFrom, iTime, iTo, iAllowedCities);
}

inline boost::unordered_map<std::string, Connection> fDijkWrapConnToCities(TimeGraphObject *iGraph, std::string iFrom,
		int iTime, boost::unordered_set<std::string> &iToCities) {
	if (iGraph->fGetObjectType() == OTTe) {
		return fDijkWrapConnToCities(dynamic_cast<TeGraph*>(iGraph), iFrom, iTime, iToCities);
	}
	return fDijkWrapConnToCities(dynamic_cast<TdGraph*>(iGraph), iFrom, iTime, iToCities);
}

inline boost::unordered_map<std::string, Connection> fDijkWrapRandomConnToCities(TimeGraphObject *iGraph,
		std::string iFrom, int iTime, boost::unordered_set<std::string> &iToCities) {
	if (iGraph->fGetObjectType() == OTTe) {
		return fDijkWrapRandomConnToCities(dynamic_cast<TeGraph*>(iGraph), iFrom, iTime, iToCities);
	}
	return fDijkWrapRandomConnToCities(dynamic_cast<TdGraph*>(iGraph), iFrom, iTime, iToCities);
}

inline boost::unordered_map<std::string, std::vector<Connection> > fDijkWrapMultiConnToCities(TimeGraphObject *iGraph,
		std::string iFrom, int iTime, std::vector<int> iToCityInds, std::vector<std::string> *iAllCities) {
	if (iGraph->fGetObjectType() == OTTe) {
		return fDijkWrapMultiConnToCities(dynamic_cast<TeGraph*>(iGraph), iFrom, iTime, iToCityInds, iAllCities);
	}
	return fDijkWrapMultiConnToCities(dynamic_cast<TdGraph*>(iGraph), iFrom, iTime, iToCityInds, iAllCities);
}

inline boost::unordered_map<std::string, CityPath> fDijkWrapUgSpToCities(TimeGraphObject *iGraph,
		std::string iFrom, int iTime, boost::unordered_set<std::string> &iToCities) {
	if (iGraph->fGetObjectType() == OTTe) {
		return fDijkWrapUgSpToCities(dynamic_cast<TeGraph*>(iGraph), iFrom, iTime, iToCities);
	}
	return fDijkWrapUgSpToCities(dynamic_cast<TdGraph*>(iGraph), iFrom, iTime, iToCities);
}

inline boost::unordered_map<std::string, std::vector<Path> > fDijkWrapMultiUgSpToCities(TimeGraphObject *iGraph,
		std::string iFrom, int iTime, std::vector<int> iToCityInds, std::vector<std::string> *iAllCities) {
	if (iGraph->fGetObjectType() == OTTe) {
		return fDijkWrapMultiUgSpToCities(dynamic_cast<TeGraph*>(iGraph), iFrom, iTime, iToCityInds, iAllCities);
	}
	return fDijkWrapMultiUgSpToCities(dynamic_cast<TdGraph*>(iGraph), iFrom, iTime, iToCityInds, iAllCities);
}

#endif /* ALGDIJKSTRA_H_ */
