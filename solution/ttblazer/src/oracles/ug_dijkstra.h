/* Dijkstra's algorithm for underlying graphs */
#ifndef UGDIJKSTRA_H_
#define UGDIJKSTRA_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>

#include "oracle.h"
#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_DIJKSTRA_NAME "dijkstra"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class - Dijkstra
/*****************************************************************/

class UgDijkstra: public UgOracle {
private:
	//----- data -----
	UgGraph *cGraph;
	bool cPreprocessed;

	bool cHasWeights;

	std::vector<std::string> cArgs;
public:
	//----- Constructors -----
	UgDijkstra(std::vector<std::string> iArguments);

	//----- required interface - Oracle -----
	virtual Ret fPreprocess(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline bool fAnswers(QueryType iQt) {return fSAnswers(iQt);};
	virtual inline std::set<QueryType> fQueryTypes() {return fSQueryTypes();};
	virtual inline std::vector<std::string> fGetArgs() {return cArgs;};
	virtual int fGetSpace();

	//----- required interface - GraphOracle -----
	virtual Path fQueryPath(int iFrom, int iTo);
	virtual int fQueryDistance(int iFrom, int iTo);

	//----- required interface - UgOracle -----
	//none

	//----- Static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_DIJKSTRA_NAME;};
	static inline bool fSAnswers(QueryType iQt) {return (iQt == QTSp || iQt == QTDist);};
	static std::set<QueryType> fSQueryTypes();
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UGDIJKSTRA_H_ */
