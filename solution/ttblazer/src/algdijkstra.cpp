/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"

#include "algdijkstra.h"
#include "algorithms.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Wrappers for UG graph SP search
/*****************************************************************/

int fDijkWrapDist(UgGraph *iGraph, int iFrom, int iTo, bool iHasWeights) {
	return fDijkstraDist(iGraph, iFrom, -1, iTo, "", iHasWeights);
}

Path fDijkWrapSp(UgGraph *iGraph, int iFrom, int iTo, bool iHasWeights) {
	unordered_set<string> lEmptySet;
	return fDijkstraSp(iGraph, iFrom, -1, iTo, "", iHasWeights, lEmptySet);
}

Path fDijkWrapSpRestr(UgGraph *iGraph, int iFrom, int iTo, bool iHasWeights,
		unordered_set<string> &iAllowedCities) {
	return fDijkstraSp(iGraph, iFrom, -1, iTo, "", iHasWeights, iAllowedCities);
}

vector<Path> fDijkWrapSpToAll(UgGraph *iGraph, int iFrom, bool iHasWeights) {
	return fDijkstraSpToAll(iGraph, iFrom, -1, iHasWeights);
}

vector<Path> fDijkWrapRandomSpToAll(UgGraph *iGraph, int iFrom, bool iHasWeights) {
	return fDijkstraRandomSpToAll(iGraph, iFrom, -1, iHasWeights);
}

vector<vector<Path> > fDijkWrapMultiSpToAll(UgGraph *iGraph, int iFrom, bool iHasWeights) {
	return fDijkstraMultiSpToAll(iGraph, iFrom, -1, iHasWeights);
}

vector<vector<vector<Path> > > fDijkWrapAllMultiSp(UgGraph *iGraph, bool iHasWeights) {
	return fDijkstraAllMultiSp(iGraph, iHasWeights);
}

/*****************************************************************/
/* Wrappers for TE graph SP search, we use node indices thus time is not necessary. Graph is always weighted
/*****************************************************************/

int fDijkWrapDist(TeGraph *iGraph, int iFrom, int iTo) {
	return fDijkstraDist(iGraph, iFrom, -1, iTo, "", true);
}

Path fDijkWrapSp(TeGraph *iGraph, int iFrom, int iTo) {
	unordered_set<string> lEmptySet;
	return fDijkstraSp(iGraph, iFrom, -1, iTo, "", true, lEmptySet);
}

Path fDijkWrapSpRestr(TeGraph *iGraph, int iFrom, int iTo,
		unordered_set<string> &iAllowedCities) {
	return fDijkstraSp(iGraph, iFrom, -1, iTo, "", true, iAllowedCities);
}

vector<Path> fDijkWrapSpToAll(TeGraph *iGraph, int iFrom) {
	return fDijkstraSpToAll(iGraph, iFrom, -1, true);
}

vector<Path> fDijkWrapRandomSpToAll(TeGraph *iGraph, int iFrom) {
	return fDijkstraRandomSpToAll(iGraph, iFrom, -1, true);
}

vector<vector<Path> > fDijkWrapMultiSpToAll(TeGraph *iGraph, int iFrom) {
	return fDijkstraMultiSpToAll(iGraph, iFrom, -1, true);
}

vector<vector<vector<Path> > > fDijkWrapAllMultiSp(TeGraph *iGraph) {
	return fDijkstraAllMultiSp(iGraph, true);
}

/*****************************************************************/
/* Wrappers for TD graph SP search, we use node indices, disregard time and consider each arc length = 1
/*****************************************************************/

int fDijkWrapDist(TdGraph *iGraph, int iFrom, int iTo) {
	return fDijkstraDist(iGraph, iFrom, -1, iTo, "", false);
}

Path fDijkWrapSp(TdGraph *iGraph, int iFrom, int iTo) {
	unordered_set<string> lEmptySet;
	return fDijkstraSp(iGraph, iFrom, -1, iTo, "", false, lEmptySet);
}

Path fDijkWrapSpRestr(TdGraph *iGraph, int iFrom, int iTo,
		unordered_set<string> &iAllowedCities) {
	return fDijkstraSp(iGraph, iFrom, -1, iTo, "", false, iAllowedCities);
}

vector<Path> fDijkWrapSpToAll(TdGraph *iGraph, int iFrom) {
	return fDijkstraSpToAll(iGraph, iFrom, -1, false);
}

vector<Path> fDijkWrapRandomSpToAll(TdGraph *iGraph, int iFrom) {
	return fDijkstraRandomSpToAll(iGraph, iFrom, -1, false);
}

vector<vector<Path> > fDijkWrapMultiSpToAll(TdGraph *iGraph, int iFrom) {
	return fDijkstraMultiSpToAll(iGraph, iFrom, -1, false);
}

vector<vector<vector<Path> > > fDijkWrapAllMultiSp(TdGraph *iGraph) {
	return fDijkstraAllMultiSp(iGraph, false);
}

/*****************************************************************/
/* Wrappers for TE graph EA search
/*****************************************************************/

int fDijkWrapEa(TeGraph *iGraph, string iFrom, int iTime, string iTo) {
	TENodeData lData;
	lData.cName = iFrom;
	lData.cTime = iTime;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId(iGraph->fGetTimeFormat()));
	if (lFrom == -1) {
		return -1;
	}

	return fDijkstraDist(iGraph, lFrom, -1, -1, iTo, true);
}

Connection fDijkWrapConn(TeGraph *iGraph, string iFrom, int iTime, string iTo) {
	TENodeData lData;
	lData.cName = iFrom;
	lData.cTime = iTime;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId(iGraph->fGetTimeFormat()));
	if (lFrom == -1) {
		return Connection();
	}

	unordered_set<string> lEmptySet;
	Path lPath = fDijkstraSp(iGraph, lFrom, -1, -1, iTo, true, lEmptySet);

	return iGraph->fMakeConnection(lPath, iTime);
}

Connection fDijkWrapConnRestr(TeGraph *iGraph, string iFrom, int iTime, string iTo,
		unordered_set<string> &iAllowedCities) {
	TENodeData lData;
	lData.cName = iFrom;
	lData.cTime = iTime;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId(iGraph->fGetTimeFormat()));
	if (lFrom == -1) {
		return Connection();
	}

	Path lPath = fDijkstraSp(iGraph, lFrom, -1, -1, iTo, true, iAllowedCities);

	return iGraph->fMakeConnection(lPath, iTime);
}

unordered_map<string, Connection> fDijkWrapConnToCities(TeGraph *iGraph, string iFrom, int iTime,
		unordered_set<string> &iToCities) {
	TENodeData lData;
	lData.cName = iFrom;
	lData.cTime = iTime;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId(iGraph->fGetTimeFormat()));
	if (lFrom == -1) {
		return unordered_map<string, Connection>();
	}

 	unordered_map<string, Path> lPaths = fDijkstraSpToCities(iGraph, lFrom, -1, iToCities, true);

	unordered_map<string, Connection> lResult;
	for (unordered_map<string, Path>::iterator i = lPaths.begin(); i != lPaths.end(); i++) {
		lResult[i->first] = iGraph->fMakeConnection(i->second, iTime);
	}

	return lResult;
}

unordered_map<string, Connection> fDijkWrapRandomConnToCities(TeGraph *iGraph, string iFrom, int iTime,
		unordered_set<string> &iToCities) {
	TENodeData lData;
	lData.cName = iFrom;
	lData.cTime = iTime;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId(iGraph->fGetTimeFormat()));
	if (lFrom == -1) {
		return unordered_map<string, Connection>();
	}

	unordered_map<string, Path> lPaths = fDijkstraRandomSpToCities(iGraph, lFrom, -1, iToCities, true);

	unordered_map<string, Connection> lResult;
	for (unordered_map<string, Path>::iterator i = lPaths.begin(); i != lPaths.end(); i++) {
		lResult[i->first] = iGraph->fMakeConnection(i->second, iTime);
	}

	return lResult;
}

unordered_map<string, vector<Connection> > fDijkWrapMultiConnToCities(TeGraph *iGraph,
		string iFrom, int iTime, vector<int> iToCityInds, vector<string> *iAllCities) {
	TENodeData lData;
	lData.cName = iFrom;
	lData.cTime = iTime;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId(iGraph->fGetTimeFormat()));
	if (lFrom == -1) {
		return unordered_map<string, vector<Connection> >();
	}

	vector<string> *lAllCitiesPtr;
	vector<string> lAllCities;
	if (iAllCities == NULL) {
		lAllCities = iGraph->fGetCities();
		lAllCitiesPtr = &lAllCities;
	}
	else {
		lAllCitiesPtr = iAllCities;
	}

	vector<vector<Path> > lPaths = fDijkstraMultiSpToCities(iGraph, lFrom, -1, iToCityInds,
			lAllCitiesPtr, false, true);

	unordered_map<string, vector<Connection> > lResult;
	for (size_t i = 0; i < lPaths.size(); i++) {
		string lCity = (*lAllCitiesPtr)[iToCityInds[i]];
		lResult[lCity] = vector<Connection>();
		lResult[lCity].reserve(lPaths[i].size());
		for (size_t j = 0; j < lPaths[i].size(); j++) {
			lResult[lCity].push_back(iGraph->fMakeConnection(lPaths[i][j], iTime));
		}
	}

	return lResult;
}

unordered_map<string, CityPath> fDijkWrapUgSpToCities(TeGraph *iGraph,
		string iFrom, int iTime, unordered_set<string> &iToCities) {
	TENodeData lData;
	lData.cName = iFrom;
	lData.cTime = iTime;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId(iGraph->fGetTimeFormat()));
	if (lFrom == -1) {
		return unordered_map<string, CityPath>();
	}

	return fDijkstraUgSpToCities(iGraph, lFrom, -1, iToCities, true);
}

unordered_map<string, vector<Path> > fDijkWrapMultiUgSpToCities(TeGraph *iGraph,
		string iFrom, int iTime, vector<int> iToCityInds, vector<string> *iAllCities) {
	TENodeData lData;
	lData.cName = iFrom;
	lData.cTime = iTime;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId(iGraph->fGetTimeFormat()));
	if (lFrom == -1) {
		return unordered_map<string, vector<Path> >();
	}

	vector<string> *lAllCitiesPtr;
	vector<string> lAllCities;
	if (iAllCities == NULL) {
		lAllCities = iGraph->fGetCities();
		lAllCitiesPtr = &lAllCities;
	}
	else {
		lAllCitiesPtr = iAllCities;
	}

	vector<vector<Path> > lPaths = fDijkstraMultiSpToCities(iGraph, lFrom, -1, iToCityInds,
			lAllCitiesPtr, true, true);

	unordered_map<string, vector<Path> > lResult;
	for (size_t i = 0; i < lPaths.size(); i++) {
		string lCity = (*lAllCitiesPtr)[iToCityInds[i]];
		lResult[lCity] = vector<Path>();
		lResult[lCity].reserve(lPaths[i].size());
		for (size_t j = 0; j < lPaths[i].size(); j++) {
			lResult[lCity].push_back(lPaths[i][j]);
		}
	}

	return lResult;
}

/*****************************************************************/
/* Wrappers for TD graph EA search
/*****************************************************************/

int fDijkWrapEa(TdGraph *iGraph, string iFrom, int iTime, string iTo) {
	TDNodeData lData;
	lData.cName = iFrom;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId());
	if (lFrom == -1) {
		return -1;
	}

	return fDijkstraDist(iGraph, lFrom, iTime, -1, iTo, true);
}

Connection fDijkWrapConn(TdGraph *iGraph, string iFrom, int iTime, string iTo) {
	TDNodeData lData;
	lData.cName = iFrom;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId());
	if (lFrom == -1) {
		return Connection();
	}

	unordered_set<string> lEmptySet;
	Path lPath = fDijkstraSp(iGraph, lFrom, iTime, -1, iTo, true, lEmptySet);

	return iGraph->fMakeConnection(lPath, iTime);
}

Connection fDijkWrapConnRestr(TdGraph *iGraph, string iFrom, int iTime, string iTo,
		unordered_set<string> &iAllowedCities) {
	TDNodeData lData;
	lData.cName = iFrom;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId());
	if (lFrom == -1) {
		return Connection();
	}

	Path lPath = fDijkstraSp(iGraph, lFrom, iTime, -1, iTo, true, iAllowedCities);

	return iGraph->fMakeConnection(lPath, iTime);
}

Connection fDijkWrapConnRestr(TdGraph *iGraph, int iFrom, int iTime, int iTo,
		unordered_set<int> &iAllowedCities) {
	Path lPath = fDijkstraRestr(iGraph, iFrom, iTime, iTo, iAllowedCities);

	return iGraph->fMakeConnection(lPath, iTime);
}

unordered_map<int, Connection> fDijkWrapConnToCitiesRestr(TdGraph *iGraph, int iFrom, int iTime,
		unordered_set<int> &iToCities, unordered_set<int> &iAllowedCities) {
	vector<int> lToCities = vector<int>(iToCities.begin(), iToCities.end());

	vector<Path> lPaths = fDijkstraToCitiesRestr(iGraph, iFrom, iTime, lToCities, iAllowedCities);

	unordered_map<int, Connection> lResult;
	for (size_t i = 0; i < lPaths.size(); i++) {
		lResult[lToCities[i]] = iGraph->fMakeConnection(lPaths[i], iTime);
	}

	return lResult;
}

unordered_map<string, Connection> fDijkWrapConnToCities(TdGraph *iGraph, string iFrom, int iTime,
		unordered_set<string> &iToCities) {
	TDNodeData lData;
	lData.cName = iFrom;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId());
	if (lFrom == -1) {
		return unordered_map<string, Connection>();
	}

	unordered_map<string, Path> lPaths = fDijkstraSpToCities(iGraph, lFrom, iTime, iToCities, true);

	unordered_map<string, Connection> lResult;
	for (unordered_map<string, Path>::iterator i = lPaths.begin(); i != lPaths.end(); i++) {
		lResult[i->first] = iGraph->fMakeConnection(i->second, iTime);
	}

	return lResult;
}

unordered_map<string, Connection> fDijkWrapRandomConnToCities(TdGraph *iGraph, string iFrom, int iTime,
		unordered_set<string> &iToCities) {
	TDNodeData lData;
	lData.cName = iFrom;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId());
	if (lFrom == -1) {
		return unordered_map<string, Connection>();
	}

	unordered_map<string, Path> lPaths = fDijkstraRandomSpToCities(iGraph, lFrom, iTime, iToCities, true);

	unordered_map<string, Connection> lResult;
	for (unordered_map<string, Path>::iterator i = lPaths.begin(); i != lPaths.end(); i++) {
		lResult[i->first] = iGraph->fMakeConnection(i->second, iTime);
	}

	return lResult;
}

unordered_map<string, vector<Connection> > fDijkWrapMultiConnToCities(TdGraph *iGraph,
		string iFrom, int iTime, vector<int> iToCityInds, vector<string> *iAllCities) {
	TDNodeData lData;
	lData.cName = iFrom;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId());
	if (lFrom == -1) {
		return unordered_map<string, vector<Connection> >();
	}

	vector<string> *lAllCitiesPtr;
	vector<string> lAllCities;
	if (iAllCities == NULL) {
		lAllCities = iGraph->fGetCities();
		lAllCitiesPtr = &lAllCities;
	}
	else {
		lAllCitiesPtr = iAllCities;
	}

	vector<vector<Path> > lPaths = fDijkstraMultiSpToCities(iGraph, lFrom, iTime, iToCityInds,
			lAllCitiesPtr, false, true);

	unordered_map<string, vector<Connection> > lResult;
	for (size_t i = 0; i < lPaths.size(); i++) {
		string lCity = (*lAllCitiesPtr)[iToCityInds[i]];
		lResult[lCity] = vector<Connection>();
		lResult[lCity].reserve(lPaths[i].size());
		for (size_t j = 0; j < lPaths[i].size(); j++) {
			lResult[lCity].push_back(iGraph->fMakeConnection(lPaths[i][j], iTime));
		}
	}

	return lResult;
}

unordered_map<string, CityPath> fDijkWrapUgSpToCities(TdGraph *iGraph,
		string iFrom, int iTime, unordered_set<string> &iToCities) {
	TDNodeData lData;
	lData.cName = iFrom;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId());
	if (lFrom == -1) {
		return unordered_map<string, CityPath>();
	}

	return fDijkstraUgSpToCities(iGraph, lFrom, iTime, iToCities, true);
}

unordered_map<string, vector<Path> > fDijkWrapMultiUgSpToCities(TdGraph *iGraph,
		string iFrom, int iTime, vector<int> iToCityInds, vector<string> *iAllCities) {
	TDNodeData lData;
	lData.cName = iFrom;
	int lFrom = iGraph->fGetNodeIndex(lData.fGetId());
	if (lFrom == -1) {
		return unordered_map<string, vector<Path> >();
	}

	vector<string> *lAllCitiesPtr;
	vector<string> lAllCities;
	if (iAllCities == NULL) {
		lAllCities = iGraph->fGetCities();
		lAllCitiesPtr = &lAllCities;
	}
	else {
		lAllCitiesPtr = iAllCities;
	}

	vector<vector<Path> > lPaths = fDijkstraMultiSpToCities(iGraph, lFrom, iTime, iToCityInds,
			lAllCitiesPtr, true, true);

	unordered_map<string, vector<Path> > lResult;
	for (size_t i = 0; i < lPaths.size(); i++) {
		string lCity = (*lAllCitiesPtr)[iToCityInds[i]];
		lResult[lCity] = vector<Path>();
		lResult[lCity].reserve(lPaths[i].size());
		for (size_t j = 0; j < lPaths[i].size(); j++) {
			lResult[lCity].push_back(lPaths[i][j]);
		}
	}

	return lResult;
}

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

/*****************************************************************/
/* Class - interface
/*****************************************************************/

/*****************************************************************/
/* Class - implementation
/*****************************************************************/
