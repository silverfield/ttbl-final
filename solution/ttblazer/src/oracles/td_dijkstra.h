/* Dijkstra's algorithm for TD graphs */
#ifndef TDDIJKSTRA_H_
#define TDDIJKSTRA_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>

#include "oracle.h"
#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TD_DIJKSTRA_NAME "tddijkstra"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class - Dijkstra
/*****************************************************************/

class TdDijkstra: public TdOracle {
private:
	//----- data -----
	TdGraph *cGraph;
	std::map<std::string, int> cCities;
	bool cPreprocessed;

	std::vector<std::string> cArgs;
public:
	//----- Constructors -----
	TdDijkstra(std::vector<std::string> iArguments);

	//----- required interface - Oracle -----
	virtual Ret fPreprocess(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline bool fAnswers(QueryType iQt) {return fSAnswers(iQt);};
	virtual inline std::set<QueryType> fQueryTypes() {return fSQueryTypes();};
	virtual inline std::vector<std::string> fGetArgs() {return cArgs;};
	virtual int fGetSpace();

	//----- required interface - TimetableOracle -----
	virtual int fQueryEa(std::string iFrom, int iTime, std::string iTo);
	virtual Connection fQueryConn(std::string iFrom, int iTime, std::string iTo);

	//----- required interface - TdOracle -----
	//none

	//----- Static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TD_DIJKSTRA_NAME;};
	static inline bool fSAnswers(QueryType iQt) {return (iQt == QTEa || iQt == QTConn);};
	static std::set<QueryType> fSQueryTypes();
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TDDIJKSTRA_H_ */
