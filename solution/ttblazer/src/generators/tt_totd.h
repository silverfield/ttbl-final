/* Builds a TD graph from given timetable */
#ifndef TT_TOTD_H_
#define TT_TOTD_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "generator.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TT_TOTD_NAME "2td"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TtToTd: public TtGenerator {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TtToTd(std::vector<std::string> iArguments);

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
	static inline std::string fSGetName() {return TT_TOTD_NAME;};
	static inline ObjType fSGetGenerates() {return OTTd;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TT_TOTD_H_ */
