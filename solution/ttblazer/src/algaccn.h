/* Algorithms related to Access nodes */
#ifndef ALGACCN_H_
#define ALGACCN_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <vector>

#include "central.h"
#include "objects/undergr.h"

/*****************************************************************/
/* Structs
/*****************************************************************/

struct AnDiag {
	double cAvgLANSize;
	double cAvgNeighSize;
	int cMaxLANSize;
	int cMaxNeighSize;
	double cAvgBackLANSize;
	double cAvgBackNeighSize;
	int cMaxBackLANSize;
	int cMaxBackNeighSize;

	std::string fGetString() {
		std::stringstream lSs;

		lSs << "AVG SQR LAN size: " << cAvgLANSize << std::endl;
		lSs << "AVG SQR NEIGH size: " << cAvgNeighSize << std::endl;
		lSs << "AVG SQR BACK LAN size: " << cAvgBackLANSize << std::endl;
		lSs << "AVG SQR BACK NEIGH size: " << cAvgBackNeighSize << std::endl;
		lSs << "MAX LAN size: " << cMaxLANSize << std::endl;
		lSs << "MAX NEIGH size: " << cMaxNeighSize << std::endl;
		lSs << "MAX BACK LAN size: " << cMaxBackLANSize << std::endl;
		lSs << "MAX BACK NEIGH size: " << cMaxBackNeighSize << std::endl;

		return lSs.str();
	}
};

/*****************************************************************/
/* Functions
/*****************************************************************/

void s(UgGraph *iGraph, int iNode, boost::unordered_set<int> &iANs,
		boost::unordered_set<int> *oLANs, boost::unordered_set<int> *oNeighborhood);

boost::unordered_set<int> fGetLANsForNode(UgGraph *iGraph, int iFrom, boost::unordered_set<int> &iANs,
		boost::unordered_set<int> *oNeighborhood);

std::vector<boost::unordered_set<int> > fGetLANs(UgGraph *iGraph, boost::unordered_set<int> &iANs,
		std::vector<int> *oNeighborhoodSizes, std::vector<boost::unordered_set<int> > *oNeighborhoods);

std::vector<boost::unordered_set<int> > fGetBackLANs(UgGraph *iGraph, boost::unordered_set<int> &iANs,
		std::vector<int> *oNeighborhoodSizes, std::vector<boost::unordered_set<int> > *oNeighborhoods);

AnDiag fDiagnoseAnSet(UgGraph* iGraph, boost::unordered_set<int> &iANs);
std::string fStringDiagAnSet(UgGraph *iGraph, boost::unordered_set<int> &iANs, bool iDet);

#endif /* ALGACCN_H_ */
