/* Time-dependent graph - a graph with cost function on arcs
 * Its a representation of a timetable
 *
 * As it represents a timetable, we call it a timetable-like object
 * As it has the graph object as the underlying structure, we call it a graph-like object
 * Its a timegraph-object */
#ifndef TIMEDEP_H_
#define TIMEDEP_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <list>
#include <vector>
#include <map>
#include <algorithm>

#include "graph.h"
#include "object.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Class - ConstFunc
/*****************************************************************/

/* Interpolation point */
struct IntPoint {
	int64_t cDep; //departure time of connection
	int64_t cTravel; //travel time of connection

	static bool fCompare(IntPoint iX, IntPoint iY) {
		return iX.cDep < iY.cDep;
	}

	IntPoint(int iDep, int iTravel) {
		cDep = iDep;
		cTravel = iTravel;
	}

	IntPoint() {
		cDep = -1;
		cTravel = -1;
	}

	std::string fGetString() const {
		return fIntToStr(cDep) + " [" + fIntToStr(cTravel) + "]";
	}
};

class CostFunc {
private:
	std::vector<IntPoint> cIntPoints;
	bool cNeedRebuild; //need to rebuild the search structure to answer quickly queries
	std::vector<std::vector<int64_t> > cSearchStruct;
	int64_t cMaxRange; //the upper bound on the departures of interpolation points
	int64_t cMinRange; //the lower bound on the departures of interpolation points
	int cFromDay;
	int cToDay;
	int cDayPeriod;
	int cMinutePeriod;

	//----- inmplementation -----
	IntPoint *fGetExactIntPointPtr(int iTime);
	inline IntPoint fGetExactIntPoint(int iTime) {
		IntPoint *lIntPoint = fGetExactIntPointPtr(iTime);
		if (lIntPoint == NULL) {
			return IntPoint();
		}
		return *lIntPoint;
	}
public:
	//----- constructors -----
	CostFunc();

	//----- interface -----
	void fAddIntPoint(int iDep, int iTime);
	void fRemoveIntPoint(int iIndex);
	void fRebuildSearch(int iFromDay, int iToDay);
	int fGetHeight();
	int fGetLength(int iTime);
	std::string fGetSearchStructString();
	std::string fGetCostString(TimeFormat iTimeFormat);
	int fGetSpace() const;
	inline int fGetMaxRange() const { return cMaxRange;};
	inline int fGetMinRange() const { return cMinRange;};
	const std::vector<IntPoint>* fGetIntPoints();
	int fGetFirstDeparture(int iTime);
	IntPoint fGetIntPoint(int iTime);
	IntPoint fGetLatestIntPoint();
	IntPoint fGetEarliestIntPoint();
};

/*****************************************************************/
/* Structs - Node and Arc data
/*****************************************************************/

struct TDArcData {
	CostFunc cCostF;

	TDArcData() {
		cCostF = CostFunc();
	}

	int fGetSpace() const {
		int lBytes = 0;

		lBytes += cCostF.fGetSpace();

		return lBytes;
	}
};

struct TDNodeData {
	std::string cName;
	double cCoorX;
	double cCoorY;
	bool cCoor;

	TDNodeData() {
		cName = "";
		cCoorX = 0;
		cCoorY = 0;
		cCoor = false;
	}

	std::string fGetId() {
		return cName;
	}

	std::string fGetCoorString() {
		if (cCoor == false) {
			return NULL_STR;
		}

		return fDoubleToStr(cCoorX) + " " + fDoubleToStr(cCoorY);
	}

	int fGetSpace() const {
		int lBytes = 0;

		lBytes += cName.length();

		return lBytes;
	}
};

/*****************************************************************/
/* Class - TdGraph
/*****************************************************************/

class TdGraph : public TimeGraphObject {
private:
	//----- data -----
	Graph<TDNodeData, TDArcData> cGraph;

	int cFromDay;
	int cToDay;
	int cPeriod;
	int cSetPeriod;

	//----- implementation -----
	virtual inline GraphInterface* fGraphInterface() {return &cGraph;};
	void fWriteObject(std::ostream *iStringStream, bool iForSave);
	double fGetHeight(bool iMax);
public:
	//----- constructors -----
	TdGraph();

	//----- required interface - Object -----
	virtual std::string fGetObjectString(bool iForSave);
	virtual Ret fSaveObject(const char *iFileName);
	virtual Ret fLoadObject(const char *iFileName);

	virtual std::string fGetBasicFactsString();
	virtual int fGetSpace();
	virtual inline ObjType fGetObjectType() {return OTTd;};

	virtual Object* fGetCopy() {TdGraph *lGraph = new TdGraph(); *lGraph = *this; return lGraph;};

	//----- required interface - GraphObject -----
	virtual std::string fGetNodeString(int iIndex);
	virtual std::string fGetArcString(int iFrom, int iTo);

	virtual inline std::string fGetNodeId(int iX) {return cGraph.fGetNodeData(iX)->fGetId();};
	std::string fGetNodeName(int iX);
	virtual std::string fGetArcsString(int iX);

	bool fCheckPath(Path iPath);
	virtual int fGetPathLength(Path iPath);
	virtual std::string fGetPathString(Path iPath, std::string iSep);
	virtual inline bool fIsWeighted() {return true;};

	virtual inline const GraphInterface* fGetGraphInterface() {return &cGraph;};
	virtual std::vector<SPQuery> fGetSPInstances(int iCount);
	virtual int fGetLength(int iFrom, int iTo, int iTime);
	virtual inline std::vector<int> fGetTo(int iX) {return cGraph.fGetTo(iX);};
	GraphObject* fSubGraph(std::vector<int> iIndices);

	void fSetPeriod(int iDays) {cSetPeriod = iDays; fRebuildCostFunctions();};
	int fGetPeriod() {return cPeriod;};
	int fGetFromDay() {return cFromDay;};
	int fGetToDay() {return cToDay;};

	//----- required interface - TimetableObject -----
	virtual void fChangeTimeFormat(TimeFormat iTF);
	virtual inline TimeFormat fGetTimeFormat() {return cTimeFormat;};

	virtual std::vector<EAQuery> fGetEAInstances(int iCount, double iExistRatio);
	virtual std::vector<std::string> fGetCities();
	virtual int fGetTimeRange();
	virtual int fGetMinTime();
	virtual int fGetMaxTime();
	virtual double fGetAvgHeight();
	virtual int fGetHeight();
	virtual std::vector<Event> fGetEvents(boost::unordered_set<std::string> iCities,
			std::pair<int, int> iRange);
	virtual int fGetEventCount();
	virtual inline int fGetCityCount() {return fGetN();};

	virtual UgGraph* fToUgGraph();

	//----- required interface - TimeGraphObject -----
	virtual Connection fMakeConnection(Path iPath, int iStartTime);
	virtual Connection fMakeConnection(std::vector<std::string> iUsp, int iStartTime);
	virtual int fGetReachingTime(std::string iFrom, int iTime, std::string iTo);

	//----- interface -----
	void fRebuildCostFunctions();
	void fSetGraph(const Graph<TDNodeData, TDArcData> *iGraph);

	inline TDNodeData* fGetNodeData(int iX) {return cGraph.fGetNodeData(iX);};
	inline TDArcData* fGetArcData(int iX, int iY) {return cGraph.fGetArcData(iX, iY);};
	inline boost::unordered_map<int, Arc<TDArcData> > fGetArcs(int iX) {return cGraph.fGetArcs(iX);};
	inline const Graph<TDNodeData, TDArcData> * fGetGraph() {return &cGraph;};
};

#endif /* TIMEDEP_H_ */


