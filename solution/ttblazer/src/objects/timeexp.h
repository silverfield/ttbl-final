/* Time-expanded graph - a graph whose edges represent elementary connections of the timetable.
 * We can say it's a true graph representation of a timetable
 *
 * As it represents a timetable, we call it a timetable-like object
 * As it has the graph object as the underlying structure, we call it a graph-like object
 * Its a timegraph-object */
#ifndef TIMEEXP_H_
#define TIMEEXP_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <list>
#include <vector>
#include <map>
#include <algorithm>

#include "graph.h"
#include "object.h"
#include "undergr.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TE_NULL_LENGTH -1

/*****************************************************************/
/* Structs - Node and Arc data
/*****************************************************************/

struct TEArcData {
	int cLength;

	TEArcData() {
		cLength = TE_NULL_LENGTH;
	}

	std::string fGetLengthString() {
		if (cLength == TE_NULL_LENGTH) {
			return NULL_STR;
		}

		return fIntToStr(cLength);
	}

	int fGetSpace() const {
		int lBytes = 0;

		return lBytes;
	}
};

struct TENodeData {
	std::string cName;
	int cTime;

	TENodeData() {
		cName = "";
		cTime = -1;
	}

	std::string fGetId(TimeFormat iTimeFormat) {
		if (iTimeFormat == TFOn) {
			return cName + " " + fMinutesToString(cTime);
		}

		return cName + " " + fIntToStr(cTime);
	}

	int fGetSpace() const {
		int lBytes = 0;

		lBytes += cName.length();

		return lBytes;
	}
};

/*****************************************************************/
/* Class - TeGraph
/*****************************************************************/

class TeGraph : public TimeGraphObject {
private:
	//----- data -----
	Graph<TENodeData, TEArcData> cGraph;

	//----- implementation -----
	virtual inline GraphInterface* fGraphInterface() {return &cGraph;};
	virtual void fWriteObject(std::ostream *iStringStream, bool iForSave);
public:
	//----- constructors -----
	TeGraph();

	//----- required interface - Object -----
	virtual std::string fGetObjectString(bool iForSave);
	virtual Ret fSaveObject(const char *iFileName);
	virtual Ret fLoadObject(const char *iFileName);

	virtual std::string fGetBasicFactsString();
	virtual int fGetSpace();
	virtual inline ObjType fGetObjectType() {return OTTe;};
	virtual Object* fGetCopy() {TeGraph *lGraph = new TeGraph(); *lGraph = *this; return lGraph;};

	//----- required interface - GraphObject -----
	virtual std::string fGetNodeString(int iIndex);
	virtual std::string fGetArcString(int iFrom, int iTo);

	virtual inline std::string fGetNodeId(int iX) {
		return cGraph.fGetNodeData(iX)->fGetId(cTimeFormat);};
	std::string fGetNodeName(int iX);
	virtual std::string fGetArcsString(int iX);

	virtual int fGetPathLength(Path iPath);
	virtual std::string fGetPathString(Path iPath, std::string iSep);
	virtual inline bool fIsWeighted() {return true;};

	virtual inline const GraphInterface* fGetGraphInterface() {return &cGraph;};
	virtual std::vector<SPQuery> fGetSPInstances(int iCount);
	virtual int fGetLength(int iFrom, int iTo, int iTime);
	virtual inline std::vector<int> fGetTo(int iX) {return cGraph.fGetTo(iX);};
	GraphObject* fSubGraph(std::vector<int> iIndices);

	//----- required interface - TimetableObject -----
	virtual void fChangeTimeFormat(TimeFormat iTF);
	virtual inline TimeFormat fGetTimeFormat() {return cTimeFormat;};

	virtual std::vector<EAQuery> fGetEAInstances(int iCount, double iExistRatio);
	virtual std::vector<std::string> fGetCities();
	virtual int fGetTimeRange();
	virtual int fGetMinTime();
	virtual int fGetMaxTime();
	virtual int fGetHeight();
	virtual double fGetAvgHeight() {return -1;}; //dummy
	virtual std::vector<Event> fGetEvents(boost::unordered_set<std::string> iCities,
			std::pair<int, int> iRange);
	virtual inline int fGetEventCount() {return fGetN();};
	virtual inline int fGetCityCount() {return fGetCities().size();};

	virtual UgGraph* fToUgGraph();

	//----- required interface - TimeGraphObject -----
	virtual Connection fMakeConnection(Path iPath, int iStartTime);
	virtual Connection fMakeConnection(std::vector<std::string> iUsp, int iStartTime);
	virtual int fGetReachingTime(std::string iFrom, int iTime, std::string iTo);

	//----- interface -----
	int fGetNodeTime(int iX);
	//gets the earliest index of TE node (with city = iCity) we can reach when being in the city
	//at time iTime
	int fGetEarliestRealIndex(std::string iCity, int iTime);
	//gets the waiting time we have to undergo in the city of the TE node (specified by index iFromIndex)
	//in order to have option of a connection to iToCity
	int fGetWaitTime(std::string iToCity, int iFromIndex);
	//gets the earliest reachable index of TE node (with city = iToCity) we can reach from node specified
	//by index iFromIndex
	int fGetEarliestIndex(std::string iToCity, int iFromIndex);
	//the same, but it returns the sequence of indices that have to be traversed (not including the
	//iFromIndex, but including the destination) to reach iToCity
	std::vector<int> fGetEarliestSequence(std::string iToCity, int iFromIndex);
	void fSetGraph(const Graph<TENodeData, TEArcData> *iGraph);

	inline TENodeData* fGetNodeData(int iX) {return cGraph.fGetNodeData(iX);};
	inline TEArcData* fGetArcData(int iX, int iY) {return cGraph.fGetArcData(iX, iY);};
	inline boost::unordered_map<int, Arc<TEArcData> > fGetArcs(int iX) {return cGraph.fGetArcs(iX);};
	inline const Graph<TENodeData, TEArcData> * fGetGraph() {return &cGraph;};
};

#endif /* TIMEEXP_H_ */


