/* Builds a sub-TT from given timetable whose underlying graph is at most given constant and
 * connected */
#ifndef TT_SUBTT_H_
#define TT_SUBTT_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "generator.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TT_SUBTT_NAME "subtt"
#define TT_SUBTT_H "-h"
#define TT_SUBTT_MINR "-minr"
#define TT_SUBTT_MAXR "-maxr"
#define TT_SUBTT_N "-n"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TtSubTt: public TtGenerator {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	int cN;
	int cMaxHeight;
	int cMinRange;
	int cMaxRange;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TtSubTt(std::vector<std::string> iArguments);

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
	static inline std::string fSGetName() {return TT_SUBTT_NAME;};
	static inline ObjType fSGetGenerates() {return OTTt;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TT_SUBTT_H_ */
