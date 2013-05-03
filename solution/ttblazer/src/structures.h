/* Common structures used in the project */
#ifndef STRUCTURES_H_
#define STRUCTURES_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"
#include "../../common/src/structsizer.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Struct - EA query
/*****************************************************************/

struct EAQuery {
	std::string cFrom;
	int cTime;
	std::string cTo;
};

/*****************************************************************/
/* Struct - Event
/*****************************************************************/

struct Event {
	int cCityIndex; //this is an optional, auxiliary parameter
	std::string cCity;
	int cTime;

	Event(std::string iCity, int iTime) {
		cCity = iCity;
		cTime = iTime;

		cCityIndex = -1;
	}

	Event() {
		cTime = -1;
		cCityIndex = -1;
		cCity = "";
	}

	bool operator==(const Event &iOther) const {
		return (cCity == iOther.cCity && cTime == iOther.cTime);
	}

	std::string fGetString() {
		return cCity + " [" + fTimeFormatToString(cTime) + "]";
	}

	inline int fGetSpace() {
		return cCity.length();
	}
};

/*****************************************************************/
/* Struct - SP query
/*****************************************************************/

struct SPQuery {
	int cFrom;
	int cTo;
};

/*****************************************************************/
/* Class - Partition
/*****************************************************************/

/* Set of sets that do not intersect but whose union gives the whole vertex set */
class Partition {
private:
	//----- data -----
	std::vector< std::vector<int> > cSets;
	std::map<int, int> cVertex2SetIndex;
public:
	//----- interface -----
	void fSortBySize(); //sorts sets in descending order by size. Vertex2SetIndex is corrupted after this
	int fAddSet();
	std::vector<int> fGetSet(int iIndex);
	int fGetVertexSet(int iVertex);
	void fAddToSet(int iIndex, int iVertex);
	inline int fGetSetCount() {return cSets.size();};
};

/*****************************************************************/
/* Class - CityPath
/*****************************************************************/

class CityPath {
private:
	//----- data -----
	std::vector<std::string> cCities;
public:
	//----- constructors -----
	CityPath(std::vector<std::string> iCities) {cCities = iCities;};
	CityPath() {};

	//----- interface -----
	inline int fGetSize() const {return cCities.size();};
	inline void fAdd(std::string iCity) {cCities.push_back(iCity);};
	inline std::string fGet(int iIndex) const {return cCities[iIndex];};
	inline void fSet(int iIndex, std::string iValue) {cCities[iIndex] = iValue;};
	inline std::string fGetLast() {
		if (cCities.size() == 0) {return "";}
		else {return cCities[cCities.size() - 1];}};
	inline std::string fGetFirst() {
		if (cCities.size() == 0) {return "";}
		else {return cCities[0];}};
	inline bool fIsEmpty() {return cCities.size() == 0;};
	inline std::vector<std::string> fGetCities() {return cCities;};
	inline std::string fGetString() const {return fGetVectorString(cCities);};

	inline int fGetSpace() {
		int lBytes = 0;

		lBytes += fSizeOf(cCities);

		return lBytes;
	}
};

struct CityPathEqual : std::binary_function<CityPath, CityPath, bool> {
    bool operator()(CityPath const& iX, CityPath const& iY) const {
    	if (iX.fGetSize() != iY.fGetSize()) {
    		return false;
    	}
    	for (int i = 0; i < iX.fGetSize(); i++) {
            if (iX.fGet(i) != iY.fGet(i)) {
            	return false;
            }
        }
        return true;
    }
};

struct CityPathHash : std::unary_function<CityPath, size_t> {
    size_t operator()(CityPath const& iCityPath) const {
        size_t lSeed = 0;

        for (int i = 0; i < iCityPath.fGetSize(); i++) {
            boost::hash_combine(lSeed, iCityPath.fGet(i));
        }

        return lSeed;
    }
};

extern CityPathHash gCityPathHash;

typedef boost::unordered_set<CityPath, CityPathHash, CityPathEqual> citypath_set;

/*****************************************************************/
/* Class - Path
/*****************************************************************/

class Path {
private:
	//----- data -----
	std::vector<int> cNodeIndices;
public:
	//----- constructors -----
	Path(std::vector<int> iIndices) {cNodeIndices = iIndices;};
	Path() {};

	//----- interface -----
	inline int fGetSize() const {return cNodeIndices.size();};
	inline void fAdd(int iNodeIndex) {cNodeIndices.push_back(iNodeIndex);};
	inline int fGet(int iIndex) const {return cNodeIndices[iIndex];};
	inline void fSet(int iIndex, int iValue) {cNodeIndices[iIndex] = iValue;};
	inline int fGetLast() {
		if (cNodeIndices.size() == 0) {return -1;}
		else {return cNodeIndices[cNodeIndices.size() - 1];}};
	inline int fGetFirst() {
		if (cNodeIndices.size() == 0) {return -1;}
		else {return cNodeIndices[0];}};
	inline bool fIsEmpty() {return cNodeIndices.size() == 0;};
	inline std::vector<int> fGetIndices() {return cNodeIndices;};
	inline std::string fGetString() const {return fGetVectorString(cNodeIndices);};

	inline int fGetSpace() {
		int lBytes = 0;

		lBytes += fSizeOf(cNodeIndices);

		return lBytes;
	};
};

struct PathEqual : std::binary_function<Path, Path, bool> {
    bool operator()(Path const& iX, Path const& iY) const {
    	if (iX.fGetSize() != iY.fGetSize()) {
    		return false;
    	}
    	for (int i = 0; i < iX.fGetSize(); i++) {
            if (iX.fGet(i) != iY.fGet(i)) {
            	return false;
            }
        }
        return true;
    }
};

struct PathHash : std::unary_function<Path, size_t> {
    size_t operator()(Path const& iPath) const {
        size_t lSeed = 0;

        for (int i = 0; i < iPath.fGetSize(); i++) {
            boost::hash_combine(lSeed, iPath.fGet(i));
        }

        return lSeed;
    }
};

extern PathHash gPathHash;

typedef boost::unordered_set<Path, PathHash, PathEqual> path_set;

/*****************************************************************/
/* Class - Connection
/*****************************************************************/

class Connection {
private:
	//----- data -----
	std::vector<Event> cEvents;
public:
	//----- constructors -----
	Connection(std::vector<Event> iEvents) {
		cEvents = iEvents;
	};
	Connection() {};

	//----- interface -----
	inline int fGetSize() {return cEvents.size();};
	inline int fGetLength() {
		if (cEvents.size() == 0) {return -1;};
		return cEvents[cEvents.size() - 1].cTime - cEvents[0].cTime;
	};
	inline void fAdd(std::string iCity, int iTime) {
		Event lEvent;
		lEvent.cCity = iCity;
		lEvent.cTime = iTime;
		cEvents.push_back(lEvent);
	};
	inline void fAdd(Event iEvent) {
		cEvents.push_back(iEvent);
	};
	inline void fConcat(Connection iConnection) {
		std::vector<Event> lNewEvents = iConnection.fGetEvents();
		cEvents.insert(cEvents.end(), lNewEvents.begin(), lNewEvents.end());
	};
	inline std::string fGetCity(int iIndex) {return cEvents[iIndex].cCity;};
	inline int fGetTime(int iIndex) {return cEvents[iIndex].cTime;};
	inline Event fGetEvent(int iIndex) {return cEvents[iIndex];};
	inline std::string fGetLastCity() {
		if (cEvents.size() == 0) {return "";}
		else {return cEvents[cEvents.size() - 1].cCity;}
	};
	inline int fGetLastTime() {
		if (cEvents.size() == 0) {return -1;}
		else {return cEvents[cEvents.size() - 1].cTime;}
	};
	inline Event fGetLastEvent() {
		if (cEvents.size() == 0) {return Event();}
		else {return cEvents[cEvents.size() - 1];}
	};
	inline std::string fGetFirstCity() {
		if (cEvents.size() == 0) {return "";}
		else {return cEvents[0].cCity;}
	};
	inline int fGetFirstTime() {
		if (cEvents.size() == 0) {return -1;}
		else {return cEvents[0].cTime;}
	};
	inline Event fGetFirstEvent() {
		if (cEvents.size() == 0) {return Event();}
		else {return cEvents[0];}
	};
	inline std::vector<Event> fGetEvents() {return cEvents;};
	inline bool fIsEmpty() {return cEvents.size() == 0;};
	inline void fRemoveDuplicates() {
		std::vector<Event> lNewEvents;
		for (size_t i = 0; i < cEvents.size(); i++) {
			if (i < cEvents.size() - 1 && cEvents[i] == cEvents[i + 1]) {
				continue;
			}
			lNewEvents.push_back(cEvents[i]);
		}

		cEvents = lNewEvents;
	}
	inline int fGetSpace() {
		int lBytes = 0;

		lBytes += fSizeOf(cEvents);
		for (std::vector<Event>::iterator i = cEvents.begin(); i != cEvents.end(); i++) {
			lBytes += i->fGetSpace();
		}

		return lBytes;
	}

	std::string fGetString(std::string iSep = ", ") {
		std::string lResult = "";
		for (size_t i = 0; i < cEvents.size(); i++) {
			if (i != 0) {
				lResult += iSep;
			}
			lResult += cEvents[i].cCity + " [" + fTimeFormatToString(cEvents[i].cTime) + "]";
		}

		return lResult;
	};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* STRUCTURES_H_ */
