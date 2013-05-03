/* Determines connectivity (weak) for given UG */
#ifndef UG_CONNECTIVITY_H_
#define UG_CONNECTIVITY_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"
#include "connectivity.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_CONNECTIVITY_NAME "conn"

#define UG_CONN_CMD_PARTINF "-inf"
#define UG_CONN_CMD_PARTDET "-det"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class UgConnResult : public ConnResult {

};

class UgConnectivity : public UgAnalyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptPartition;
	bool cOptPartitionDet;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	UgConnectivity(std::vector<std::string> iArguments);

	//----- required interface - Analyser-----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - UgAnalyser -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_CONNECTIVITY_NAME;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_CONNECTIVITY_H_ */
