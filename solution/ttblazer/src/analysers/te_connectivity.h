/* Determines connectivity (weak) for given TE */
#ifndef TE_CONNECTIVITY_H_
#define TE_CONNECTIVITY_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"
#include "connectivity.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TE_CONNECTIVITY_NAME "conn"

#define TE_CONN_CMD_PARTINF "-inf"
#define TE_CONN_CMD_PARTDET "-det"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TeConnResult : public ConnResult {

};

class TeConnectivity : public TeAnalyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptPartition;
	bool cOptPartitionDet;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TeConnectivity(std::vector<std::string> iArguments);

	//----- required interface - Analyser-----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - TeAnalyser -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TE_CONNECTIVITY_NAME;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TE_CONNECTIVITY_H_ */
