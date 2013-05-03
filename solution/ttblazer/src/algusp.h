/* Algorithms related to USP */
#ifndef ALGUSP_H_
#define ALGUSP_H_

/*****************************************************************/
/* Constants
/*****************************************************************/

#define DAY_MIN 1440
#define WEEK_MIN 10080
#define INF_MIN INT_MAX

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <vector>

#include "central.h"
#include "objects/undergr.h"
#include "objects/object.h"

/*****************************************************************/
/* Structs
/*****************************************************************/

struct UspTree {
	boost::unordered_map<int, boost::unordered_set<int> > cAllowed;
	boost::unordered_set<int> cRestr;

	bool fContains(int iX) {
		return (cRestr.find(iX) != cRestr.end());
	}

	void fAdd(int iFrom, int iTo) {
		cRestr.insert(iFrom);
		cRestr.insert(iTo);

		boost::unordered_map<int, boost::unordered_set<int> >::iterator i = cAllowed.find(iFrom);
		if (i == cAllowed.end()) {
			boost::unordered_set<int> lNewSet;
			lNewSet.insert(iTo);
			cAllowed.insert(std::pair<int, boost::unordered_set<int> >(iFrom, lNewSet));
			return;
		}

		i->second.insert(iTo);
	}

	boost::unordered_set<int>* fGetRestr() {
		return &cRestr;
	}

	void fAdd(Path &iPath) {
		for (int i = 0; i < iPath.fGetSize() - 1; i++) {
			fAdd(iPath.fGet(i), iPath.fGet(i + 1));
		}
	}

	boost::unordered_set<int>* fGet(int iFrom) {
		boost::unordered_map<int, boost::unordered_set<int> >::iterator i = cAllowed.find(iFrom);
		if (i == cAllowed.end()) {
			return NULL;
		}

		return &(i->second);
	}

	bool fGet(int iFrom, int iTo) {
		boost::unordered_map<int, boost::unordered_set<int> >::iterator i = cAllowed.find(iFrom);
		if (i == cAllowed.end()) {
			return false;
		}

		return (i->second.count(iTo) != 0);
	}

	int fGetSpace() {
		int lBytes = 0;

		lBytes += fSizeOf(cAllowed);
		for (boost::unordered_map<int, boost::unordered_set<int> >::iterator i = cAllowed.begin();
				i != cAllowed.end(); i++) {
			lBytes += fSizeOf(i->second);
		}
		lBytes += fSizeOf(cRestr);

		return lBytes;
	}

	bool fIsEmpty() {
		return cAllowed.empty();
	}

	bool fCheck(TdGraph *iGraph) {
		for (boost::unordered_map<int, boost::unordered_set<int> >::iterator i = cAllowed.begin();
				i != cAllowed.end(); i++) {
			for (boost::unordered_set<int>::iterator j = i->second.begin();
					j != i->second.end(); j++) {
				if (iGraph->fArcExists(i->first, *j) == false) {
					return false;
				}
			}
		}

		return true;
	}

	std::string fGetString(UgGraph *iUgGraph) {
		std::ostringstream lSs;

		for (boost::unordered_map<int, boost::unordered_set<int> >::iterator i = cAllowed.begin();
				i != cAllowed.end(); i++) {
			lSs << "	" << i->first;
			if (iUgGraph != NULL) {
				lSs << "	" << fPadString(iUgGraph->fGetNodeName(i->first), 40);
			}
			if (iUgGraph != NULL) {
				lSs << std::endl;
				for (boost::unordered_set<int>::iterator j = i->second.begin();
						j != i->second.end(); j++) {
					lSs << "		" << *j << " " << iUgGraph->fGetNodeName(*j) << std::endl;
				}
			}
			else {
				lSs << fGetSetString(i->second) << std::endl;
			}
		}

		return lSs.str();
	}
};

/*****************************************************************/
/* Enums
/*****************************************************************/

//USP analysis type
enum UspAnType {UTNormal, UTCount, UTUsage};
extern std::string gUspAnTypeStr [];

/*****************************************************************/
/* Inline functions
/*****************************************************************/

inline Path fCityPathToPath(CityPath &iCityPath, UgGraph* iUgGraph) {
	std::vector<int> lNodeIndices;
	lNodeIndices.reserve(iCityPath.fGetSize());
	for (int i = 0; i < iCityPath.fGetSize(); i++) {
		lNodeIndices.push_back(iUgGraph->fGetNodeIndex(iCityPath.fGet(i)));
	}

	return Path(lNodeIndices);
};

/*****************************************************************/
/* Functions
/*****************************************************************/

std::string fAnalyzeSegUsps(UgGraph *iUgGraph,
		std::vector<std::vector<std::vector<Path> > > *iPaths,
		std::vector<std::vector<std::vector<std::vector<size_t> > > > *iIndices,
		std::vector<int> &iFromIndices, std::vector<int> &iToIndices, bool iDet);

std::string fAnalyzeUsps(UgGraph *iUgGraph, std::vector<std::vector<std::vector<Path> > > *iPaths,
		std::vector<int> &iFromIndices, std::vector<int> &iToIndices, bool iDet);

double fGetUspCoef(UgGraph *iUgGraph, std::vector<std::vector<int> > *iCounts,
		std::vector<int> &iFromIndices, std::vector<int> &iToIndices);

std::string fAnalyzeUspCounts(UgGraph *iUgGraph, std::vector<std::vector<int> > *iCounts,
		std::vector<int> &iFromIndices, std::vector<int> &iToIndices, bool iDet);

std::string fAnalyzeUspUsage(UgGraph *iUgGraph,
		std::vector<std::vector<std::vector<std::pair<Path, int> > > > *iUsage,
		std::vector<int> &iFromIndices, std::vector<int> &iToIndices, bool iDet);

Connection fConnUsps(int iTime, path_set &iUsps, TdGraph *iGraph);
Connection fConnUsps(int iTime, std::vector<Path> &iUsps, TdGraph *iGraph);
Connection fConnUsps(int iTime, std::vector<Path> &iUsps, std::vector<size_t> &iUspInd, TdGraph *iGraph);

Connection fConnUspTree(int iFrom, int iTime, int iTo, UspTree &iUspTree, TdGraph *iGraph);

boost::unordered_map<int, Connection> fConnUspTreeToCities(int iFrom, int iTime,
		boost::unordered_set<int> &iToCities, UspTree &iUspTree, TdGraph *iGraph);

std::vector<std::vector<std::vector<Path> > >* fUSP(UgGraph *iUgGraph, TimeGraphObject *iGraph,
	std::vector<int> &iUgCitiesFrom, std::vector<int> &iUgCitiesTo);

std::vector<std::vector<std::vector<Path> > >* fTdUSP(TdGraph *iGraph,
	std::vector<int> &iUgCitiesFrom, std::vector<int> &iUgCitiesTo);

std::vector<std::vector<Path> >* fRestrUSP(TdGraph *iGraph,
	int iFrom, std::vector<int> &iUgCitiesTo, boost::unordered_set<int> *iNeigh,
	std::pair<int, int> iRange = ANY_RANGE);

int fTimeToSegment(int iSegSize, int iFromDay, int iToDay, int iTime);
int fSegments(int iSegSize, int iFromDay, int iToDay);
std::pair<int, int> fSegmentRange(int iSegSize, int iSegment, int iFromDay);

//from - at segment - to - paths
std::vector<std::vector<std::vector<Path> > >* fTdSegUSP(TdGraph *iGraph,
	std::vector<int> &iUgCitiesFrom, std::vector<int> &iUgCitiesTo, int iSegSize,
	std::vector<std::vector<std::vector<std::vector<size_t> > > > **oPathIndices);

std::vector<std::vector<UspTree> >* fUspTrees(UgGraph *iUgGraph, TimeGraphObject *iGraph,
	std::vector<int> &iUgCitiesFrom, std::vector<int> &iUgCitiesTo);

std::vector<std::vector<std::vector<std::pair<Path, int> > > >* fUSPUsageAlso(
		UgGraph *iUgGraph, TimeGraphObject *iGraph,
		std::vector<int> &iUgCitiesFrom, std::vector<int> &iUgCitiesTo);

std::vector<std::vector<int> >* fUSPCountOnly(UgGraph *iUgGraph, TimeGraphObject *iGraph,
	std::vector<int> &iUgCitiesFrom, std::vector<int> &iUgCitiesTo);

std::vector<std::vector<std::vector<Path> > >* fMultiUSP(UgGraph *iUgGraph, TimeGraphObject *iGraph,
	std::vector<int> &iUgCitiesFrom, std::vector<int> &iUgCitiesTo);

#endif /* ALGUSP_H_ */
