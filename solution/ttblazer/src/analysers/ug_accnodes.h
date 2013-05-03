/* Determines access nodes properties for given UG and given access node set */
#ifndef UG_ACCNODES_H_
#define UG_ACCNODES_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"
#include "betwess.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_ACC_NODES_NAME "accn"

#define UG_ACC_NODES_LIM "-l"
#define UG_ACC_NODES_SET "-set"
#define UG_ACC_NODES_DET "-det"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class UgAccNodesResult : public StdAnalyseResult {
public:
	virtual std::string fGetResultNameString() {
		return "*** Access nodes set analysis results ***";
	}
};

class UgAccNodes : public UgAnalyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	std::string cOptionsAccNodesPm;
	bool cOptionsDet;
	int cOptionsLim;

	//----- implementation -----
	void fProcessArgs();

	bool fLanUsageComp (std::pair<std::string, int> i, std::pair<std::string, int> j) {
		return (i.second > j.second);
	}
public:
	//----- constructors -----
	UgAccNodes(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - UgAnalyser -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_ACC_NODES_NAME;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_ACCNODES_H_ */
