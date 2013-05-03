/* Analyses degrees of UG graph */
#ifndef UG_DEGREES_H_
#define UG_DEGREES_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"
#include "degrees.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_DEGREES_NAME "degs"

#define UG_DEG_CMD_INF "-inf"
#define UG_DEG_CMD_DET "-det"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class UgDegreesResult : public DegreesResult {

};

class UgDegrees : public UgAnalyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptDegreesInf;
	bool cOptDegreesDet;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	UgDegrees(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - UgAnalyser -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_DEGREES_NAME;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_DEGREES_H_ */
