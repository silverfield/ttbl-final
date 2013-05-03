/* common betweenness analysis stuff */
#ifndef BETWES_H_
#define BETWES_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class BetwResult : public AnalyseResult {
protected:
	//----- data -----
	GraphObject *cObject;

	double cAvgBetw;
	double cMaxBetw;
	int cMaxBetwIndex;
	double cMinBetw;
	int cMinBetwIndex;

	std::vector<double> cBetws;
	std::vector<double> cBetwPercents;

	bool cBetwDetDefined;
	bool cBetwDefined;
public:
	//----- constructors -----
	BetwResult(GraphObject *iObject) {
		cBetwDefined = true;
		cBetwDetDefined = false;

		cAvgBetw = -1;
		cMaxBetwIndex = -1;
		cMinBetwIndex = -1;
		cMaxBetw = -1;
		cMinBetw = -1;

		cObject = iObject;
	}

	//----- required interface -----
	virtual std::string fGetResultNameString() {
		return "*** Betweenness analysis results ***";
	}

	virtual std::string fGetResultString() {
		std::stringstream lResult (std::stringstream::out);

		if (cBetwDefined) {
			lResult << fGetBetwString();
		}
		if (cBetwDetDefined) {
			lResult << fGetBetwDetString();
		}

		return lResult.str();
	}

	//----- interface -----
	void fSetBetw(double iAvgBetw, int iMaxBetwIndex, int iMinBetwIndex, double iMaxBetw,
			double iMinBetw) {
		cAvgBetw = iAvgBetw;
		cMaxBetwIndex = iMaxBetwIndex;
		cMinBetwIndex = iMinBetwIndex;
		cMaxBetw = iMaxBetw;
		cMinBetw = iMinBetw;

		cBetwDefined = true;
	}

	void fSetBetwDet(std::vector<double> iBetws, std::vector<double> iBetwPercents) {
		cBetws = iBetws;
		cBetwPercents = iBetwPercents;

		cBetwDetDefined = true;
	}

	std::string fGetBetwString() {
		std::stringstream lResult (std::stringstream::out);

		lResult << "AVG BETWEENNESS: " << cAvgBetw << std::endl;
		lResult << "MAX BETWEENNESS: " << cMaxBetw << " (node " << cMaxBetwIndex << ")" << std::endl;
		lResult << "MIN BETWEENNESS: " << cMinBetw << " (node " << cMinBetwIndex << ")" << std::endl;

		return lResult.str();
	}

	std::string fGetBetwDetString() {
		std::stringstream lResult (std::stringstream::out);

		std::vector<int> lTranslations = fSortVector(&cBetws, false);

		lResult << "BETWEENNESS for each node [percents of shortest paths the node was on]:" << std::endl;
		for (std::vector<double>::size_type i = 0; i < cBetws.size(); i++) {
			lResult << "	" << lTranslations[i] << " (" << cObject->fGetNodeId(lTranslations[i]) << "): "
					<< cBetws[i] << " [" << cBetwPercents[lTranslations[i]] * 100 << " %]" << std::endl;
		}

		return lResult.str();
	}
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* BETWES_H_ */
