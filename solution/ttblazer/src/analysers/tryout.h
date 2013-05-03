/* Tryout analyser where u can have some fun */
#ifndef UG_TRYOUT
#define UG_TRYOUT

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TRYOUT_NAME "tryout"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TryOutResult : public AnalyseResult {
	std::string cResult;
public:
	virtual void fSetResult(std::string iResult) {
		cResult = iResult;
	}

	virtual std::string fGetResultNameString() {
		return "*** Tryout analysis results ***";
	}

	virtual std::string fGetResultString() {
		return cResult;
	}
};

class TryOut : public Analyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TryOut(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline std::set<ObjType> fMeantFor() {
		return fSMeantFor();
	};

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TRYOUT_NAME;};
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTUg);
		lSet.insert(OTTe);
		lSet.insert(OTTd);
		lSet.insert(OTTt);
		return lSet;
	};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_TRYOUT */
