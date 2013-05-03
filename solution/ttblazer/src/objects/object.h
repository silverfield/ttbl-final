/* Common predecessor(s) for objects */
#ifndef OBJECT_H_
#define OBJECT_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <typeinfo>

#include "../central.h"
#include "../structures.h"
#include "graph.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class - Object
/*****************************************************************/

class Object {
protected:
	virtual void fWriteObject(std::ostream *iStringStream, bool iForSave) = 0;
public:
	//----- constructors -----
	virtual ~Object() = 0;

	//----- abstract interface -----
	virtual std::string fGetObjectString(bool iForSave) = 0;
	virtual Ret fSaveObject(const char *iFileName) = 0;
	virtual Ret fLoadObject(const char *iFileName) = 0;
	virtual ObjType fGetObjectType() = 0;
	virtual Object* fGetCopy() = 0;

	virtual std::string fGetBasicFactsString() = 0;
	virtual int fGetSpace() = 0;
};
inline Object::~Object() {};

/*****************************************************************/
/* Class - Graph Object
/*****************************************************************/

class GraphObject : virtual public Object {
private:
protected:
	boost::unordered_map<std::string, int> cIdIndexMap;
	bool cIdIndexMapValid;

	virtual GraphInterface* fGraphInterface() = 0; //this is non-const
public:
	//----- constructors -----
	virtual ~GraphObject() = 0;

	//----- abstract interface -----
	virtual std::string fGetNodeString(int iIndex) = 0;
	virtual std::string fGetArcString(int iFrom, int iTo) = 0;

	virtual std::string fGetNodeId(int iX) = 0; //Bratislava 14:50
	virtual std::string fGetNodeName(int iX) = 0; //Bratislava
	virtual std::string fGetArcsString(int iX) = 0;

	virtual int fGetPathLength(Path iPath) = 0;
	virtual std::string fGetPathString(Path iPath, std::string iSep = ", ") = 0;
	virtual bool fIsWeighted() = 0;

	virtual const GraphInterface* fGetGraphInterface() = 0;
	virtual std::vector<SPQuery> fGetSPInstances(int iCount) = 0;
	virtual int fGetLength(int iFrom, int iTo, int iTime) = 0;
	virtual std::vector<int> fGetTo(int iX) = 0;
	virtual GraphObject* fSubGraph(std::vector<int> iIndices) = 0;

	//----- interface -----
	inline int fAddNode() {return fGraphInterface()->fAddNode();};
	inline void fRemoveNode(int iX) {fGraphInterface()->fRemoveNode(iX);};
	inline void fAddArc(int iX, int iY) {fGraphInterface()->fAddArc(iX, iY);};
	inline void fRemoveArc(int iX, int iY) {fGraphInterface()->fRemoveArc(iX, iY);};

	inline std::vector<int> fRestruct() {return fGraphInterface()->fRestruct();};
	inline bool fIsRestructed() {return fGraphInterface()->fIsRestructed();};
	inline void fRemoveOrientation() {fGraphInterface()->fRemoveOrientation();};
	inline void fReverseOrientation() {fGraphInterface()->fReverseOrientation();};

	inline bool fNodeIndexExists(int iX) {return fGraphInterface()->fNodeIndexExists(iX);};
	inline bool fArcExists(int iX, int iY) {return fGraphInterface()->fArcExists(iX, iY);};

	inline int fGetN() {return fGraphInterface()->fGetN();};
	inline int fGetM() {return fGraphInterface()->fGetM();};

	int fGetNodeIndex(std::string iId);
	void fComputeIdIndexMap();
	inline void fInvalidateIdIndexMap() {cIdIndexMapValid = false;};
	void fAddIdIndex(std::string iId, int iIndex);
	void fRemIdIndex(std::string iId);

	int fGetSpace();
	std::vector<int> fGetDegrees();

	//----- static interface -----
	static inline bool fIsGraphObject(ObjType iOT) {
		return (iOT == OTUg || iOT == OTTe || iOT == OTTd);};
};
inline GraphObject::~GraphObject() {};

/*****************************************************************/
/* Class - Timetable Object
/*****************************************************************/

class UgGraph;
class TimetableObject : virtual public Object {
private:
protected:
	TimeFormat cTimeFormat;
public:
	//----- constructors -----
	virtual ~TimetableObject() = 0;

	//----- abstract interface -----
	virtual void fChangeTimeFormat(TimeFormat iTF) = 0;
	virtual TimeFormat fGetTimeFormat() = 0;

	virtual std::vector<EAQuery> fGetEAInstances(int iCount, double iExistRatio) = 0;
	virtual std::vector<std::string> fGetCities() = 0;
	virtual int fGetTimeRange() = 0;
	virtual int fGetMinTime() = 0;
	virtual int fGetMaxTime() = 0;
	virtual int fGetHeight() = 0;
	virtual double fGetAvgHeight() = 0;
	virtual std::vector<Event> fGetEvents(boost::unordered_set<std::string> iCities,
			std::pair<int, int> iRange = ANY_RANGE) = 0;
	virtual int fGetEventCount() = 0;
	virtual int fGetCityCount() = 0;

	virtual UgGraph* fToUgGraph() = 0;

	//----- static interface -----
	static inline bool fIsTimetableObject(ObjType iOT) {
		return (iOT == OTTt || iOT == OTTe || iOT == OTTd);};
};
inline TimetableObject::~TimetableObject() {};

/*****************************************************************/
/* Class - Timetable Object
/*****************************************************************/

class TimeGraphObject : public virtual TimetableObject, public virtual GraphObject {
private:
public:
	//----- constructors -----
	virtual ~TimeGraphObject() = 0;

	//----- abstract interface -----
	//makes connection from path in that graph
	virtual Connection fMakeConnection(Path iPath, int iStartTime) = 0;

	//makes connection from underlying path
	virtual Connection fMakeConnection(std::vector<std::string> iUsp, int iStartTime) = 0;

	//gets the time we can reach the to-city. we suppose it is the neighbor of from-city
	virtual int fGetReachingTime(std::string iFrom, int iTime, std::string iTo) = 0;

	//----- static interface -----
	static inline bool fIsTimeGraphObject(ObjType iOT) {
		return (iOT == OTTe || iOT == OTTd);};
};
inline TimeGraphObject::~TimeGraphObject() {};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* OBJECT_H_ */
