/* Oracle based on memorizing underlying shortest paths on access nodes
 *
 * timetables are segmented to days */
#ifndef TDUSPORA_SEG_H_
#define TDUSPORA_SEG_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>

#include "oracle.h"
#include "../central.h"
#include "../algusp.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TD_USPORA_SEG_NAME "usporaseg"

#define TD_USPORA_SEG_SET "-set"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TdUsporaSeg: public TdOracle {
private:
	//----- data -----
	TdGraph *cGraph;
	UgGraph *cUgGraph;

	boost::unordered_set<int> cAns; //the same, but as a set
	boost::unordered_map<int, int> cCity2AnIndex; //back-translations for cANs

	std::vector<boost::unordered_set<int> > cFLans; //for each UG node index -> set of UG node indices that are its LANs
	std::vector<boost::unordered_set<int> > cBLans; //the same, back LANs
	std::vector<UspTree> cFLocUspTrees;
	std::vector<boost::unordered_set<int> > cFNeighs;
	std::vector<boost::unordered_set<int> > cBNeighs;

	std::vector<std::vector<std::vector<Path> > > *cFAnUsps;
	std::vector<std::vector<std::vector<std::vector<size_t> > > > *cFAnUspInd;

	std::string cOptAnPm;

	int cFromDay;
	int cToDay;

	bool cLastQueryLocal;

	bool cPreprocessed;
	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();

	Connection fAnUspConn(int iFrom, int iTime, int iTo);
public:
	//----- Constructors -----
	TdUsporaSeg(std::vector<std::string> iArguments);
	~TdUsporaSeg();

	//----- required interface - Oracle -----
	virtual Ret fPreprocess(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline bool fAnswers(QueryType iQt) {return fSAnswers(iQt);};
	virtual inline std::set<QueryType> fQueryTypes() {return fSQueryTypes();};
	virtual inline std::vector<std::string> fGetArgs() {return cArgs;};
	virtual bool fLastQueryLocal() {
		return cLastQueryLocal;
	}
	virtual int fGetSpace();

	//----- required interface - TimetableOracle -----
	virtual Connection fQueryConn(std::string iFrom, int iTime, std::string iTo);
	virtual int fQueryEa(std::string iFrom, int iTime, std::string iTo);

	//----- Static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TD_USPORA_SEG_NAME;};
	static inline bool fSAnswers(QueryType iQt) {return (iQt == QTEa || iQt == QTConn);};
	static std::set<QueryType> fSQueryTypes();
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TDUSPORA_SEG_H_ */
