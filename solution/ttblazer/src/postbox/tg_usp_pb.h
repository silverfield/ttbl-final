/* outputs USPs to the post box */
#ifndef TG_USP_PB_H
#define TG_USP_PB_H

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "postbox.h"

#include "../central.h"
#include "../algusp.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TG_USP_PB_NAME "usp"

#define TG_USP_PB_CMD_TYPE "-type"
#define TG_USP_PB_CMD_FROM "-from"
#define TG_USP_PB_CMD_TO "-to"
#define TG_USP_PB_CMD_CITY_ALL "all"
#define TG_USP_PB_CMD_CITY_PB "pb"
#define TG_USP_PB_CMD_SET "-set"
#define TG_USP_PB_CMD_LIM "-l"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TgUspPb: public Postman {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	UspAnType cOptionsUspAnType;
	int cOptionsLim;
	bool cOptionsFromCityAll;
	bool cOptionsToCityAll;
	std::string cOptionsFromCityPm;
	std::string cOptionsToCityPm;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TgUspPb(std::vector<std::string> iArguments);

	//----- required interface - Postman -----
	virtual PostMail fPostMail(Object *iObject, std::string iMailName);

	virtual inline std::string fGenerates() {return fSGetGenerates();};
	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TG_USP_PB_NAME;};
	static inline std::string fSGetGenerates() {
		std::string lResult = "";
		lResult += PB_TYPE_USP_NORMAL;
		lResult += ", ";
		lResult += PB_TYPE_USP_COUNT;
		lResult += ", ";
		lResult += PB_TYPE_USP_USAGE;
		return lResult;
	};
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTe);
		lSet.insert(OTTd);
		return lSet;
	};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TG_USP_PB_H */
