/* common degrees analysis stuff */
#ifndef DEGREES_H_
#define DEGREES_H_

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

class DegreesResult : public AnalyseResult {
private:
	//----- data -----
	double cAvgDeg;
	int cMaxDeg;
	int cMinDeg;
	std::vector<int> cDegreesInf;
	std::map<int, std::vector<int> > cDegreesDet;

	bool cDegreesDefined;
	bool cDegreesInfDefined;
	bool cDegreesDetDefined;
public:
	//----- constructors -----
	DegreesResult() {
		cAvgDeg = 0;
		cMaxDeg = 0;
		cMinDeg = 0;

		cDegreesDefined = false;
		cDegreesInfDefined = false;
		cDegreesDetDefined = false;
	}

	//----- required interface -----
	virtual std::string fGetResultNameString() {
		return "*** Degrees analysis results ***";
	}

	virtual std::string fGetResultString() {
		std::stringstream lResult (std::stringstream::out);

		if (cDegreesDefined) {
			lResult << fGetDegreesString();
		}
		if (cDegreesInfDefined) {
			lResult << fGetDegreesInfString();
		}
		if (cDegreesDetDefined) {
			lResult << fGetDegreesDetString();
		}

		return lResult.str();
	}

	//----- interface -----
	void fSetDegrees(double iAvgDeg, int iMinDeg, int iMaxDeg) {
		cAvgDeg = iAvgDeg;
		cMaxDeg = iMaxDeg;
		cMinDeg = iMinDeg;

		cDegreesDefined = true;
	}

	void fSetDegreesInf(std::vector<int> iDegreesInf) {
		cDegreesInf = iDegreesInf;

		cDegreesInfDefined = true;
	}

	void fSetDegreesDet(std::map<int, std::vector<int> > iDegreesDet) {
		cDegreesDet = iDegreesDet;

		cDegreesDetDefined = true;
	}

	std::string fGetDegreesString() {
		std::stringstream lResult (std::stringstream::out);

		lResult << "AVG DEGREE: " << cAvgDeg << std::endl;
		lResult << "MIN DEGREE: " << cMinDeg << std::endl;
		lResult << "MAX DEGREE: " << cMaxDeg << std::endl;

		return lResult.str();
	}

	std::string fGetDegreesInfString() {
		std::stringstream lResult (std::stringstream::out);

		lResult << "UBIQUITY OF DEGREES: " << std::endl;
		for (std::vector<int>::size_type i = 0; i < cDegreesInf.size(); i++) {
			if (cDegreesInf[i] == 0) {
				continue;
			}
			lResult << "	" << i << ": " << cDegreesInf[i] << " x" << std::endl;
		}

		return lResult.str();
	}

	std::string fGetDegreesDetString() {
		std::stringstream lResult (std::stringstream::out);

		lResult << "DEGREE GROUPS: " << std::endl;
		for (std::map<int, std::vector<int> >::iterator i = cDegreesDet.begin();
				i != cDegreesDet.end(); i++) {
			lResult << "	" << i->first << ": " << fGetVectorString(i->second) << std::endl;
		}

		return lResult.str();
	}
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* DEGREES_H_ */
