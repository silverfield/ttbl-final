/* Collecting all the postman methods at one place */
#ifndef POSTBOXINFO_H_
#define POSTBOXINFO_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "../central.h"
#include "postbox.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class PostmanInfos {
private:
	//----- data -----
	static std::vector<std::string> cNames;
	static std::vector<std::string> cHelps;
	static std::vector<std::set<ObjType> > cMeantFors;
	static std::vector<std::string> cGenerates;

	//----- implementation -----
	static int fGetIndexByName(std::string iName, ObjType iOT);
public:
	//----- static interface -----
	static std::vector<std::string> fGetNames();
	static std::vector<std::set<ObjType> > fGetMeantFors();
	static std::vector<std::string> fGetGenerates();
	static std::string fGetHelp(std::string iName, ObjType iOT);
	static std::set<ObjType> fGetMeantFor(std::string iName, ObjType iOT);
	static std::string fGetGenerates(std::string iName, ObjType iOT);
	static Postman* fCreatePostman(std::string iName, ObjType iOT, std::vector<std::string> iArgs);
	inline static bool fExists(std::string iName, ObjType iOT) {
		return fGetIndexByName(iName, iOT) != -1;};

	static void fInitializeInfos();
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* POSTBOXINFO_H_ */
