/* Determines properties of paths for given UG:
 * - average path length
 * - average path size
 * - maximum path length (and path itself)
 * - maximum path size (and path itself)
 * .... */
#ifndef UG_PATHS_H_
#define UG_PATHS_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_PATHS_NAME "paths"
#define UG_PATHS_CMD_DET "-det"
#define UG_PATHS_CMD_APX "-apx"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class UgPathsResult : public StdAnalyseResult {
public:
	virtual std::string fGetResultNameString() {
		return "*** Paths analysis results ***";
	}
};

class UgPaths : public UgAnalyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptPathsDet;
	double cOptApx;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	UgPaths(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - UgAnalyser -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_PATHS_NAME;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_PATHS_H_ */
