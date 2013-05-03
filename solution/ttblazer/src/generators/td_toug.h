/* Builds a UG graph from given time-dependent graph */
#ifndef TD_TOUG_H_
#define TD_TOUG_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "generator.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TD_TOUG_NAME "2ug"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TdToUg: public TdGenerator {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TdToUg(std::vector<std::string> iArguments);

	//----- required interface - Generator -----
	virtual Object* fGenerate(Object *iObject);

	virtual inline ObjType fGenerates() {return fSGetGenerates();};
	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - TdGenerator -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TD_TOUG_NAME;};
	static inline ObjType fSGetGenerates() {return OTUg;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TD_TOUG_H_ */
