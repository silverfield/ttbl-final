/* Makes sufficient AN set based on high BC values so that the
 * neighborhoods are sufficiently small */
#ifndef UG_ANHBC_H
#define UG_ANHBC_H

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "postbox.h"

#include "../central.h"
#include "../algaccn.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_AN_HBC "anhbc"
#define UG_AN_HBC_CMD_APX "-apx"
#define UG_AN_HBC_CMD_MAX "-max"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class UgAnHBc: public UgPostman {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptionsMax;
	double cMaxCoef;
	double cApxPrec;

	static std::vector<double> cBcs;

	//----- implementation -----
	void fProcessArgs();

	static bool fBcsComp(const int &i, const int &j) {
		return (cBcs[i] > cBcs[j]);
	}

	bool fNeighOk(AnDiag iDiag, int iReqSize, int iN);
public:
	//----- constructors -----
	UgAnHBc(std::vector<std::string> iArguments);

	//----- required interface - Postman -----
	virtual PostMail fPostMail(Object *iObject, std::string iMailName);

	virtual inline std::string fGenerates() {return fSGetGenerates();};
	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_AN_HBC;};
	static inline std::string fSGetGenerates() {return PB_TYPE_VECT_STRING;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_ANHBC_H */
