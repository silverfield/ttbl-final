/* outputs high-BC cities to the post box */
#ifndef UG_HIGH_BC_H
#define UG_HIGH_BC_H

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "postbox.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_HIGH_BC_NAME "hbc"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class UgHighBc: public UgPostman {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	double cThreshold;
	double cApxPrec;
	int cLimit;

	static std::vector<double> cBetw;

	//----- implementation -----
	void fProcessArgs();

	static bool fBetwsComp(const int &i, const int &j) {
		return (cBetw[i] > cBetw[j]);
	}
public:
	//----- constructors -----
	UgHighBc(std::vector<std::string> iArguments);

	//----- required interface - Postman -----
	virtual PostMail fPostMail(Object *iObject, std::string iMailName);

	virtual inline std::string fGenerates() {return fSGetGenerates();};
	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_HIGH_BC_NAME;};
	static inline std::string fSGetGenerates() {return PB_TYPE_VECT_STRING;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_HIGH_BC_H */
