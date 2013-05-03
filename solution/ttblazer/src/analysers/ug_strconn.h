/* Determines strong connectivity for given UG */
#ifndef UG_STRCONN_H_
#define UG_STRCONN_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"
#include "strconn.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_STRCONN_NAME "strconn"

#define UG_STRCONN_CMD_PARTINF "-inf"
#define UG_STRCONN_CMD_PARTDET "-det"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class UgStrConnResult : public StrConnResult {

};

class UgStrConn: public UgAnalyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptPartition;
	bool cOptPartitionDet;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	UgStrConn(std::vector<std::string> iArguments);

	//----- required interface - Analyser-----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - UgAnalyser -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return UG_STRCONN_NAME;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* UG_STRCONN_H_ */
