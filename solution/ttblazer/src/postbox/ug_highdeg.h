/* outputs high-degree cities to the post box */
#ifndef UG_HIGH_DEG_H
#define UG_HIGH_DEG_H

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "postbox.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_HIGH_DEG_NAME "hdeg"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class UgHighDeg: public UgPostman {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	int cThreshold;
	int cLimit;

	static std::vector<int> cDegs;

	//----- implementation -----
	void fProcessArgs();

	static bool fDegsComp(const int &i, const int &j) {
		return (cDegs[i] > cDegs[j]);
	}
public:
	//----- constructors -----
	UgHighDeg(std::vector<std::string> iArguments);

	//----- required interface - Postman -----
	virtual PostMail fPostMail(Object *iObject, std::string iMailName);

	virtual inline std::string fGenerates() {return fSGetGenerates();};
	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_HIGH_DEG_NAME;};
	static inline std::string fSGetGenerates() {return PB_TYPE_VECT_STRING;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_HIGH_DEG_H */
