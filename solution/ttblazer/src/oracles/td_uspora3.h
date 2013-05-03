/* Oracle based on memorizing underlying shortest path trees (USPTrees) and access nodes */
#ifndef TDUSPORA3_H_
#define TDUSPORA3_H_

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

#define TD_USPORA3_NAME "uspora3"

#define TD_USPORA3_SET "-set"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class - Uspor
/*****************************************************************/

class TdUspora3: public TdOracle {
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

	std::vector<std::vector<UspTree> > cFAnUspTrees;

	std::string cOptAnPm;

	bool cLastQueryLocal;

	bool cPreprocessed;
	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();

	Connection fAnUspTreeConn(int iFrom, int iTime, int iTo);
public:
	//----- Constructors -----
	TdUspora3(std::vector<std::string> iArguments);
	~TdUspora3();

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
	static inline std::string fSGetName() {return TD_USPORA3_NAME;};
	static inline bool fSAnswers(QueryType iQt) {return (iQt == QTEa || iQt == QTConn);};
	static std::set<QueryType> fSQueryTypes();
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TDUSPORA3_H_ */
