/* Collecting all the generator methods at one place */
#ifndef GENERATORINFOS_H_
#define GENERATORINFOS_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "../central.h"
#include "generator.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class GeneratorInfos {
private:
	//----- data -----
	static std::vector<std::string> cNames;
	static std::vector<std::string> cHelps;
	static std::vector<std::set<ObjType> > cMeantFors;
	static std::vector<ObjType> cGenerates;

	//----- implementation -----
	static int fGetIndexByName(std::string iGName, ObjType iOT);
public:
	//----- static interface -----
	static std::vector<std::string> fGetNames();
	static std::vector<std::set<ObjType> > fGetMeantFors();
	static std::vector<ObjType> fGetGenerates();
	static std::string fGetHelp(std::string iGName, ObjType iOT);
	static std::set<ObjType> fGetMeantFor(std::string iGName, ObjType iOT);
	static ObjType fGetGenerates(std::string iGName, ObjType iOT);
	static Generator* fCreateGenerator(std::string iGName, ObjType iOT, std::vector<std::string> iArgs);
	inline static bool fExists(std::string iGName, ObjType iOT) {
		return fGetIndexByName(iGName, iOT) != -1;};

	static void fInitializeInfos();
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* GENERATORINFOS_H_ */
