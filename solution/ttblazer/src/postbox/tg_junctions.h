/* outputs junction cities to the post box */
#ifndef TG_JUNC_H
#define TG_JUNC_H

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "postbox.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TG_JUNC_NAME "junc"
#define TG_JUNC_CMD_LOC "-loc"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TgJunctions: public Postman {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	int cOptLoc;

	static std::vector<int> cDegs;

	//----- implementation -----
	void fProcessArgs();

	std::vector<int> fGetJunctions(std::vector<std::vector<std::vector<Path> > > *iUsps,
		std::vector<int> iAllCities, int iLocality);
public:
	//----- constructors -----
	TgJunctions(std::vector<std::string> iArguments);

	//----- required interface - Postman -----
	virtual PostMail fPostMail(Object *iObject, std::string iMailName);

	virtual inline std::string fGenerates() {return fSGetGenerates();};
	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TG_JUNC_NAME;};
	static inline std::string fSGetGenerates() {return PB_TYPE_VECT_STRING;};
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTe);
		lSet.insert(OTTd);
		return lSet;
	};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TG_JUNC_H */
