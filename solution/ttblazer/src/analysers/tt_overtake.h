/* Analyses of TT overtaking property */
#ifndef TT_OVERTAKE_H
#define TT_OVERTAKE_H

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "analyser.h"
#include "../objects/timetable.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TT_OVERTAKE_NAME "overtake"

#define TT_OVERTAKE_CMD_DET "-det"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class OvertakeResult : public StdAnalyseResult {
private:
public:
	//----- constructors -----

	//----- required interface -----
	virtual std::string fGetResultNameString() {
		return "*** Overtake analysis results ***";
	}
};

class TtOvertake : public TtAnalyser {
private:
	//----- data -----
	bool cOptionsDet;

	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TtOvertake(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};

	//----- required interface - UgAnalyser -----
	//none

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TT_OVERTAKE_NAME;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TT_OVERTAKE_H */
