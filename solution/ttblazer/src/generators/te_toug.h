/* Builds a UG graph from given time-expanded graph */
#ifndef TE_TOUG_H_
#define TE_TOUG_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "generator.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TE_TOUG_NAME "2ug"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TeToUg: public TeGenerator {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TeToUg(std::vector<std::string> iArguments);

	//----- required interface - Generator -----
	virtual Object* fGenerate(Object *iObject);

	virtual inline ObjType fGenerates() {return fSGetGenerates();};
	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - TeGenerator -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TE_TOUG_NAME;};
	static inline ObjType fSGetGenerates() {return OTUg;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TE_TOUG_H_ */
