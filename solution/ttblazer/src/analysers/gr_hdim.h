/* Analyses highway dimension of graph like objects */
#ifndef GR_HDIM_H_
#define GR_HDIM_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"
#include "hdim.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define GR_HDIM_NAME "hd"

#define GR_HDIM_CMD_PREC "-prec"
#define GR_HDIM_CMD_REP "-rep"
#define GR_HDIM_CMD_MULTI "-multi"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class GrHDimResult : public HDimResult {

};

class GrHDim : public Analyser {
private:
	//----- data -----
	double cPrec;
	int cRep;
	bool cMulti;

	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	GrHDim(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return GR_HDIM_NAME;};
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTUg);
		lSet.insert(OTTe);
		lSet.insert(OTTd);
		return lSet;
	};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* GR_HDIM_H_ */
