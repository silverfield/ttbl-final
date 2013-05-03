/* Makes sufficient AN set based on high degree values so that the
 * neighborhoods are sufficiently small */
#ifndef UG_ANHDEG_H
#define UG_ANHDEG_H

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "postbox.h"

#include "../central.h"
#include "../algaccn.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_AN_HDEG "anhdeg"
#define UG_AN_HDEG_CMD_MAX "-max"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class UgAnHDeg: public UgPostman {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptionsMax;
	double cMaxCoef;

	static std::vector<int> cDegs;

	//----- implementation -----
	void fProcessArgs();

	static bool fDegsComp(const int &i, const int &j) {
		return (cDegs[i] > cDegs[j]);
	}

	bool fNeighOk(AnDiag iDiag, int iReqSize, int iN);
public:
	//----- constructors -----
	UgAnHDeg(std::vector<std::string> iArguments);

	//----- required interface - Postman -----
	virtual PostMail fPostMail(Object *iObject, std::string iMailName);

	virtual inline std::string fGenerates() {return fSGetGenerates();};
	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_AN_HDEG;};
	static inline std::string fSGetGenerates() {return PB_TYPE_VECT_STRING;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_ANHDEG_H */
