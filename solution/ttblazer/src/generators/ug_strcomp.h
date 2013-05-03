/* Generates the largest strongly connected subgraph of underlying graph */
#ifndef UG_STRCOMP_H_
#define UG_STRCOMP_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>

#include "generator.h"
#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_STRCOMP "strcomp"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class - UgStrComp
/*****************************************************************/

class UgStrComp: public UgGenerator {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- Constructors -----
	UgStrComp(std::vector<std::string> iArguments);

	//----- required interface - Generator -----
	virtual Object* fGenerate(Object *iObject);

	virtual inline ObjType fGenerates() {return fSGetGenerates();};
	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline std::vector<std::string> fGetArgs() {return cArgs;};

	//----- required interface - GraphGenerator -----
	//none

	//----- required interface - UgGenerator -----
	//none

	//----- Static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_STRCOMP;};
	static inline ObjType fSGetGenerates() {return OTUg;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_STRCOMP_H_ */
