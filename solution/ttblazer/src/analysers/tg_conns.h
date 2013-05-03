/* Determines properties of connections for given time graph:
 * - average connection length
 * - average connection size
 * - maximum connection length (and connection itself)
 * - maximum connection size (and connection itself)
 * .... */
#ifndef TG_CONNS_H_
#define TG_CONNS_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TG_CONNS_NAME "conns"
#define TG_CONNS_CMD_DET "-det"
#define TG_CONNS_CMD_APX "-apx"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TgConnsResult : public StdAnalyseResult {
public:
	virtual std::string fGetResultNameString() {
		return "*** Connections analysis results ***";
	}
};

class TgConns : public Analyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptConnsDet;
	double cOptApx;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TgConns(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};

	//----- required interface - TgAnalyser -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TG_CONNS_NAME;};
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTe);
		lSet.insert(OTTd);
		return lSet;
	};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TG_CONNS_H_ */
