/* Oracle based on memorizing underlying shortest paths (USP) */
#ifndef TDUSPOR_H_
#define TDUSPOR_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>

#include "oracle.h"
#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TD_USPOR_NAME "uspor"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class - Uspor
/*****************************************************************/

class TdUspor: public TdOracle {
private:
	//----- data -----
	TdGraph *cGraph;
	UgGraph *cUgGraph;
	bool cPreprocessed;

	std::vector<std::vector<std::vector<Path> > > *cUsps;

	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- Constructors -----
	TdUspor(std::vector<std::string> iArguments);
	~TdUspor();

	//----- required interface - Oracle -----
	virtual Ret fPreprocess(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline bool fAnswers(QueryType iQt) {return fSAnswers(iQt);};
	virtual inline std::set<QueryType> fQueryTypes() {return fSQueryTypes();};
	virtual inline std::vector<std::string> fGetArgs() {return cArgs;};
	virtual int fGetSpace();
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};

	//----- required interface - TdOracle -----
	virtual Connection fQueryConn(std::string iFrom, int iTime, std::string iTo);
	virtual int fQueryEa(std::string iFrom, int iTime, std::string iTo);

	//----- Static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TD_USPOR_NAME;};
	static inline bool fSAnswers(QueryType iQt) {return (iQt == QTEa || iQt == QTConn);};
	static std::set<QueryType> fSQueryTypes();
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TDUSPOR_H_ */
