/* Determines density of the graph (max m/n for subgraphs) */
#ifndef UG_DENSITY_H_
#define UG_DENSITY_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_DENSITY_NAME "density"
#define UG_DENSITY_CMD_APX "-apx"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class UgDensityResult : public StdAnalyseResult {
public:
	virtual std::string fGetResultNameString() {
		return "*** Density analysis results ***";
	}
};

class UgDensity : public UgAnalyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptDensityDet;
	double cOptApx;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	UgDensity(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - UgAnalyser -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_DENSITY_NAME;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_DENSITY_H_ */
