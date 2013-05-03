/* Removes overtaking edges */
#ifndef TT_RMOVERTAKE_H_
#define TT_RMOVERTAKE_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "modifier.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TT_RMOVERTAKE_NAME "rmover"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TtRmOvertake: public TtModifier {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TtRmOvertake(std::vector<std::string> iArguments);

	//----- required interface - Generator -----
	virtual Ret fModify(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - TimetableGenerator -----
	//none

	//----- required interface - TtGenerator -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TT_RMOVERTAKE_NAME;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TT_RMOVERTAKE_H_ */
