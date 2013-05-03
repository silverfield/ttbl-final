/* Builds a UG graph from given timetable */
#ifndef TT_TOUG_H_
#define TT_TOUG_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "generator.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TT_TOUG_NAME "2ug"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TtToUg: public TtGenerator {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TtToUg(std::vector<std::string> iArguments);

	//----- required interface - Generator -----
	virtual Object* fGenerate(Object *iObject);

	virtual inline ObjType fGenerates() {return fSGetGenerates();};
	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - TimetableGenerator -----
	//none

	//----- required interface - TtGenerator -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TT_TOUG_NAME;};
	static inline ObjType fSGetGenerates() {return OTUg;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TT_TOUG_H_ */
