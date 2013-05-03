/* Determines betweenness properties for given TE
 * Algorithm used is exact, which may be very costly, or apx */
#ifndef TE_BETW_H_
#define TE_BETW_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"
#include "betwess.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TE_BETW_NAME "betw"
#define TE_BETW_CMD_APX "-apx"
#define TE_BETW_CMD_DET "-det"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TeBetwResult : public BetwResult {
public:
	TeBetwResult(GraphObject *iObject) : BetwResult(iObject) {};
};

class TeBetw : public TeAnalyser {
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
	TeBetw(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - TeAnalyser -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TE_BETW_NAME;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TE_BETW_H_ */
