/* analysis of timetable object properties:
 * - height
 * - time range
 * - number of cities
 * - number of UG arcs */
#ifndef TO_VARIOUS_H
#define TO_VARIOUS_H

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <sstream>

#include "analyser.h"

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TO_VARIOUS_NAME "var"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class ToVariousResult : public StdAnalyseResult {
public:
	virtual std::string fGetResultNameString() {
		return "*** VARIOUS analysis results ***";
	}
};

class ToVarious : public Analyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	ToVarious(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TO_VARIOUS_NAME;};
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTt);
		lSet.insert(OTTe);
		lSet.insert(OTTd);
		return lSet;
	};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TO_VARIOUS_H */
