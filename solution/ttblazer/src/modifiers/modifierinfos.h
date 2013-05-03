/* Collecting all the modifier methods at one place */
#ifndef MODIFIERINFOS_H_
#define MODIFIERINFOS_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "../central.h"
#include "modifier.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class ModifierInfos {
private:
	//----- data -----
	static std::vector<std::string> cNames;
	static std::vector<std::string> cHelps;
	static std::vector<std::set<ObjType> > cMeantFors;

	//----- implementation -----
	static int fGetIndexByName(std::string iMName, ObjType iOT);
public:
	//----- static interface -----
	static std::vector<std::string> fGetNames();
	static std::vector<std::set<ObjType> > fGetMeantFors();
	static std::string fGetHelp(std::string iMName, ObjType iOT);
	static std::set<ObjType> fGetMeantFor(std::string iMName, ObjType iOT);
	static Modifier* fCreateModifier(std::string iMName, ObjType iOT, std::vector<std::string> iArgs);
	inline static bool fExists(std::string iMName, ObjType iOT) {
		return fGetIndexByName(iMName, iOT) != -1;};

	static void fInitializeInfos();
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* MODIFIERINFOS_H_ */
