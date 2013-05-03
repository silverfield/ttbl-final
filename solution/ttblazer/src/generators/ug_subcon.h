/* Generates a connected subgraph of underlying graph with the specified size,
 * or a maximum one, if specified size is too large*/
#ifndef UG_SUBCON_H_
#define UG_SUBCON_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>

#include "generator.h"
#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_SUBCON "subcon"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class - UgSubCon
/*****************************************************************/

class UgSubCon: public UgGenerator {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	int cN;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- Constructors -----
	UgSubCon(std::vector<std::string> iArguments);

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
	static inline std::string fSGetName() {return UG_SUBCON;}
	static inline ObjType fSGetGenerates() {return OTUg;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_SUBCON_H_ */
