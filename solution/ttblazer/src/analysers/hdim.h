/* common highway dimension analysis stuff */
#ifndef HDIM_H_
#define HDIM_H_

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

class HDimResult : public AnalyseResult {
private:
	//----- data -----
	int cHDim;
	double cNormHDim;

	bool cHDimDefined;
public:
	//----- constructors -----
	HDimResult() {
		cHDim = -1;
		cNormHDim = -1;

		cHDimDefined = false;
	}

	//----- required interface -----
	virtual std::string fGetResultNameString() {
		return "*** Highway dimension analysis results ***";
	}

	virtual std::string fGetResultString() {
		std::stringstream lResult (std::stringstream::out);

		if (cHDimDefined) {
			lResult << fGetHDimString();
		}

		return lResult.str();
	}

	//----- interface -----
	void fSetHDim(int iHDim, double iNormHDim) {
		cHDim = iHDim;
		cNormHDim = iNormHDim;

		cHDimDefined = true;
	}

	std::string fGetHDimString() {
		std::stringstream lResult (std::stringstream::out);

		lResult << "HDIM: " << cHDim << " (" << cNormHDim << " normalized)" << std::endl;

		return lResult.str();
	}
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* HDIM_H_ */
