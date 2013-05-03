/* Dijkstra's algorithm for time graphs */
#ifndef TGDIJKSTRA_H_
#define TGDIJKSTRA_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>

#include "oracle.h"
#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TG_DIJKSTRA_NAME "dijkstra"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class - Dijkstra
/*****************************************************************/

class TgDijkstra: public TimetableOracle {
private:
	//----- data -----
	TimeGraphObject *cGraph;
	std::map<std::string, int> cCities;
	bool cPreprocessed;

	std::vector<std::string> cArgs;
public:
	//----- Constructors -----
	TgDijkstra(std::vector<std::string> iArguments);

	//----- required interface - Oracle -----
	virtual Ret fPreprocess(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline bool fAnswers(QueryType iQt) {return fSAnswers(iQt);};
	virtual inline std::set<QueryType> fQueryTypes() {return fSQueryTypes();};
	virtual inline std::vector<std::string> fGetArgs() {return cArgs;};
	virtual int fGetSpace();
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};

	//----- required interface - TimetableOracle -----
	virtual int fQueryEa(std::string iFrom, int iTime, std::string iTo);
	virtual Connection fQueryConn(std::string iFrom, int iTime, std::string iTo);

	//----- required interface - TgOracle -----
	//none

	//----- Static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TG_DIJKSTRA_NAME;};
	static inline bool fSAnswers(QueryType iQt) {return (iQt == QTEa || iQt == QTConn);};
	static std::set<QueryType> fSQueryTypes();
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

#endif /* TGDIJKSTRA_H_ */
