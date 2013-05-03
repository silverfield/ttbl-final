/* Collecting all the analyser methods at one place */
#ifndef ANALYSERINFOS_H_
#define ANALYSERINFOS_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "../central.h"
#include "analyser.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class AnalyserInfos {
private:
	//----- data -----
	static std::vector<std::string> cNames;
	static std::vector<std::string> cHelps;
	static std::vector<std::set<ObjType> > cMeantFors;

	//----- implementation -----
	static int fGetIndexByName(std::string iAName, ObjType iOT);
public:
	//----- static interface -----
	static std::vector<std::string> fGetNames();
	static std::vector<std::set<ObjType> > fGetMeantFors();
	static std::string fGetHelp(std::string iAName, ObjType iOT);
	static std::set<ObjType> fGetMeantFor(std::string iAName, ObjType iOT);
	static Analyser* fCreateAnalyser(std::string iAName, ObjType iOT, std::vector<std::string> iArgs);
	inline static bool fExists(std::string iAName, ObjType iOT) {
		return fGetIndexByName(iAName, iOT) != -1;};

	static void fInitializeInfos();
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* ANALYSERINFOS_H_ */
