/* Makes AN set based on following heuristics:
 * - good AN node is such that has small intersection of
 * neighborhood and back-neighborhood */
#ifndef UG_LOCSEP_H
#define UG_LOCSEP_H

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <queue>

#include "postbox.h"

#include "../central.h"
#include "../algaccn.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_LOCSEP_NAME "locsep"
#define UG_LOCSEP_CMD_MAX "-max"
#define UG_LOCSEP_CMD_SET "-set"
#define UG_LOCSEP_CMD_NEIGH "-neigh"

#define UG_LOCSEP_LIST_TOP 30

/*****************************************************************/
/* Class
/*****************************************************************/

class UgLocsep: public UgPostman {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptMax;
	double cMaxCoef;
	std::string cOptSetPm;

	//Locsep specific
	bool cMax;
	UgGraph *cGraph;
	boost::unordered_set<int> cANs;
	int cReqSize;

	bool cFNeighOk;
	bool cBNeighOk;
	AnDiag cAnDiag;
	bool cTuningPhase;

	UgGraph cBackGraph;
	UgGraph cNorientGraph;
	boost::unordered_map<int, boost::unordered_set<int>* > cFNeighs;
	boost::unordered_map<int, boost::unordered_set<int>* > cFLans;
	boost::unordered_map<int, boost::unordered_set<int>* > cBNeighs;
	boost::unordered_map<int, boost::unordered_set<int>* > cBLans;

	std::vector<double> cPotentials;

	int cNeighCount;

	//----- implementation -----
	void fProcessArgs();

	boost::unordered_set<int> fLocsep(UgGraph *iGraph, boost::unordered_set<int> iInitSet, bool iMax);

	void fSelect(int iX);
	void fDeselect(int iX);

	void fUpdateArea(int iX);

	void fUpdateNeighLans(int iX);
	void fFUpdateNeighLans(int iX);
	void fBUpdateNeighLans(int iX);

	void fUpdatePotential(int iX);

	bool fFinishCrit(bool iMax);

	//best non-AN node
	int fBestPotential();

	//worst AN node
	int fWorstPotential();

	//size of the intersection of the two sets
	//O(min(|iX|, |iY|))
	int fIntersecSize(boost::unordered_set<int> *iX, boost::unordered_set<int> *iY);

	//size of the difference of two sets: |X\Y|
	int fSubtractSize(boost::unordered_set<int> *iX, boost::unordered_set<int> *iY);

	double fPotentialMax(int iX, boost::unordered_set<int> &iRestr);
	double fPotentialAvg(int iX, boost::unordered_set<int> &iRestr);
	double fPotentialAvg2(int iX, boost::unordered_set<int> &iRestr);
	double fPotentialCount(int iX, int iCount);

	//get the iCount nearest (in terms of hops) nodes around iX
	boost::unordered_set<int> fNeigh(UgGraph *iGraph, int iX, int iCount);

	//search for reachable nodes from iX in an area restricted by iRestr (NOT via ANs)
	boost::unordered_set<int> fNeighRestr(UgGraph *iGraph, int iX, boost::unordered_set<int> *iRestr);

	//the same, but start the search with the arc iX->iTo (NOT via ANs)
	boost::unordered_set<int> fArcNeighRestr(UgGraph *iGraph, int iX, int iTo,
			boost::unordered_set<int> *iRestr);

	//searches for nodes that could be reached from some node in iX set (NOT via ANs)
	//The search area is restricted
	boost::unordered_set<int> fSetSearchRestr(UgGraph *iGraph, boost::unordered_set<int> &iX,
			boost::unordered_set<int> *iRestr);

	//comparator function for descending pair<vertex-id, neigh-size> vector
	inline static bool fCompNeighSize(const std::pair<int, int> &iX, const std::pair<int, int> &iY) {
		return iX.second > iY.second;
	}

	int fGetMaxFNeighSize(boost::unordered_set<int> &iX);
	int fGetMaxBNeighSize(boost::unordered_set<int> &iX);

	//gets the f-neigh sizes for vertices in iX, sorted in descending order
	//returns pairs (vertex, neigh-size)
	std::vector<std::pair<int, int> > fGetFNeighSizes(boost::unordered_set<int> &iX);

	//gets the b-neigh sizes for vertices in iX, sorted in descending order
	//returns pairs (vertex, neigh-size)
	std::vector<std::pair<int, int> > fGetBNeighSizes(boost::unordered_set<int> &iX);

	template <class T>
	std::vector<T> fGetSorted(boost::unordered_set<T> iSet) {
		std::vector<T> lVec(iSet.begin(), iSet.end());
		std::sort(lVec.begin(), lVec.end());
		return lVec;
	}

	//get the top iTop cities' potentials, neighborhood sizes...
	void fList(int iTop);

	void fParameters();

	inline bool fIsAn(int iX) {
		return (cANs.find(iX) != cANs.end());
	}
public:
	//----- constructors -----
	UgLocsep(std::vector<std::string> iArguments);

	//----- required interface - Postman -----
	virtual PostMail fPostMail(Object *iObject, std::string iMailName);

	virtual inline std::string fGenerates() {return fSGetGenerates();};
	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_LOCSEP_NAME;};
	static inline std::string fSGetGenerates() {return PB_TYPE_VECT_STRING;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_LOCSEP_H */
