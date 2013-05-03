/* Oracle based on memorizing underlying shortest paths (USP)
 *
 * Timetable is segmented to days */
#ifndef TDUSPORSEG_H_
#define TDUSPORSEG_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>

#include "oracle.h"
#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TD_USPORSEG_NAME "usporseg"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class - UsporSeg
/*****************************************************************/

class TdUsporSeg: public TdOracle {
private:
	//----- data -----
	TdGraph *cGraph;
	UgGraph *cUgGraph;
	bool cPreprocessed;

	std::vector<std::vector<std::vector<Path> > > *cUsps;
	std::vector<std::vector<std::vector<std::vector<size_t> > > > *cUspInd;

	int cFromDay;
	int cToDay;

	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- Constructors -----
	TdUsporSeg(std::vector<std::string> iArguments);
	~TdUsporSeg();

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
	static inline std::string fSGetName() {return TD_USPORSEG_NAME;};
	static inline bool fSAnswers(QueryType iQt) {return (iQt == QTEa || iQt == QTConn);};
	static std::set<QueryType> fSQueryTypes();
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TDUSPORSEG_H_ */
