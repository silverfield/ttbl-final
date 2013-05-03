/* Determines betweenness properties for given UG
 * Algorithm used is exact, which may be very costly*/
#ifndef UG_BETW_H_
#define UG_BETW_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"
#include "betwess.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_BETW_NAME "betw"
#define UG_BETW_CMD_APX "-apx"
#define UG_BETW_CMD_DET "-det"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class UgBetwResult : public BetwResult {
public:
	UgBetwResult(GraphObject *iObject) : BetwResult(iObject) {};
};

class UgBetw : public UgAnalyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptBetwDet;
	bool cOptApx;
	double cApxPrec;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	UgBetw(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - UgAnalyser -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_BETW_NAME;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_BETW_H_ */
