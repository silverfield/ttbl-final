/* Collecting all the oracle methods at one place */
#ifndef ORACLEINFOS_H_
#define ORACLEINFOS_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "../central.h"
#include "oracle.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class OracleInfos {
private:
	//----- data -----
	static std::vector<std::string> cNames;
	static std::vector<std::string> cHelps;
	static std::vector<std::set<ObjType> > cMeantFors;
	static std::vector<std::set<QueryType> > cQueryTypes;

	//----- implementation -----
	static int fGetIndexByName(std::string iDoName, ObjType iOT);
public:
	//----- static interface -----
	static std::vector<std::string> fGetNames();
	static std::vector<std::set<ObjType> > fGetMeantFors();
	static std::string fGetHelp(std::string iDoName, ObjType iOT);
	static std::set<ObjType> fGetMeantFor(std::string iDoName, ObjType iOT);
	static bool fAnwers(std::string iDoName, ObjType iOT, QueryType iQT);
	static std::set<QueryType> fGetQueryTypes(std::string iDoName, ObjType iOT);
	static Oracle* fCreateOracle(std::string iDoName, ObjType iOT, std::vector<std::string> iArgs);
	inline static bool fExists(std::string iDoName, ObjType iOT) {
		return fGetIndexByName(iDoName, iOT) != -1;};

	static void fInitializeInfos();
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* ORACLEINFOS_H_ */
