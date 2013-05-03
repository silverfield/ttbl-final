/* Timetable - a set of elementary connections.
 *
 * As it represents a timetable, we call it a timetable-like object */
#ifndef TIMETABLE_H_
#define TIMETABLE_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <set>
#include <boost/unordered_set.hpp>

#include "../central.h"
#include "object.h"
#include "timedep.h"
#include "timeexp.h"
#include "undergr.h"

/*****************************************************************/
/* Struct - ElCon
/*****************************************************************/

struct ElCon {
	std::string cFrom;
	std::string cTo;
	int cDep;
	int cArr;

	int fGetLength() const {
		return abs(cArr - cDep);
	}

	std::string fGetString(TimeFormat iTimeFormat) const {
		if (iTimeFormat == TFOn) {
			return cFrom + " " + cTo + " " + fMinutesToString(cDep) + " " + fMinutesToString(cArr);
		}

		return cFrom + " " + cTo + " " + fIntToStr(cDep) + " " + fIntToStr(cArr);
	}

	int fGetSpace() const {
		int lBytes = 0;

		lBytes += cFrom.length();
		lBytes += cTo.length();

		return lBytes;
	}
};

struct ElConEqual : std::binary_function<ElCon, ElCon, bool> {
    bool operator()(ElCon const& iX, ElCon const& iY) const {
    	if (iX.cArr != iY.cArr) {
    		return false;
    	}
    	if (iX.cDep != iY.cDep) {
    		return false;
    	}
    	if (iX.cFrom != iY.cFrom) {
    		return false;
    	}
    	if (iX.cTo != iY.cTo) {
    		return false;
    	}

        return true;
    }
};

struct ElConHash : std::unary_function<ElCon, size_t> {
    size_t operator()(ElCon const& iElCon) const {
        size_t lSeed = 0;

        boost::hash_combine(lSeed, iElCon.cArr);
        boost::hash_combine(lSeed, iElCon.cDep);
        boost::hash_combine(lSeed, iElCon.cFrom);
        boost::hash_combine(lSeed, iElCon.cTo);

        return lSeed;
    }
};

struct ElConComp {
    inline bool operator()(const ElCon& iLeft, const ElCon& iRight) {
    	std::string lLeftString = iLeft.cFrom + iLeft.cTo + fPadInt(iLeft.cDep, 3) + fIntToStr(iLeft.cArr);
    	std::string lRightString = iRight.cFrom + iRight.cTo + fPadInt(iRight.cDep, 3) + fIntToStr(iRight.cArr);
        return lLeftString < lRightString;
    }
};

/*****************************************************************/
/* Class - Timetable
/*****************************************************************/

class Timetable : public TimetableObject {
private:
	//----- data -----
	boost::unordered_set<ElCon, ElConHash, ElConEqual> cElCons;

	//----- implementation -----
	void fWriteObject(std::ostream *iStringStream, bool iForSave);
	double fGetHeight(bool iMax);
public:
	//----- required interface - Object -----
	virtual std::string fGetObjectString(bool iForSave);
	virtual Ret fSaveObject(const char *iFileName);
	virtual Ret fLoadObject(const char *iFileName);

	virtual std::string fGetBasicFactsString();
	virtual int fGetSpace();
	virtual inline ObjType fGetObjectType() {return OTTt;};

	virtual Object* fGetCopy() {Timetable *lTt = new Timetable(); *lTt = *this; return lTt;};

	//----- required interface - TimetableObject -----
	virtual void fChangeTimeFormat(TimeFormat iTF);
	virtual inline TimeFormat fGetTimeFormat() {return cTimeFormat;};

	virtual std::vector<EAQuery>fGetEAInstances(int iCount, double iExistRatio);
	virtual std::vector<std::string> fGetCities();
	virtual int fGetTimeRange();
	virtual int fGetMinTime();
	virtual int fGetMaxTime();
	virtual int fGetHeight();
	virtual double fGetAvgHeight();
	virtual std::vector<Event> fGetEvents(boost::unordered_set<std::string> iCities,
			std::pair<int, int> iRange);
	virtual inline int fGetEventCount() {return fGetN();};
	virtual inline int fGetCityCount() {return fGetCities().size();};

	virtual UgGraph* fToUgGraph();

	//----- interface -----
	Ret fAdd(ElCon iElCon);
	Ret fRemove(ElCon iElCon);

	TeGraph* fToTeGraph();
	TdGraph* fToTdGraph();

	//creates sub-timetable considering only connections on the underlying graph
	//max height determines the maximum number of events per city
	//min/max range determines the maximum time the event can have
	Timetable* fSubTimetable(UgGraph* iUgGraph, int iMaxHeight, int iMinRange, int iMaxRange);

	//returns c x c array of sorted elementary connections for a given citypair
	//
	std::vector<std::vector<std::vector<ElCon> > > fGetSortedElCons(std::vector<std::string> *iCities);

	//----- inline interface -----
	inline int fGetN() {return cElCons.size();};
};

#endif /* TIMETABLE_H_ */
