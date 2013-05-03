/* analysis of underlying shortest paths (USP):
 * - those are paths, that are traversed upon carrying out an optimal connection in a timetable
 *
 * parameters:
 * - det - gives details for each pair of cities
 * - multi - considers ALL optimal connections and their USP
 * - from-cities [all, postbox #]
 * - to-cities [all, postbox #]
 *  */
#ifndef TG_USP_H
#define TG_USP_H

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <sstream>

#include "analyser.h"

#include "../central.h"
#include "../postbox/postbox.h"
#include "../algusp.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TG_USP_NAME "usp"

#define TG_USP_CMD_DET "-det"
#define TG_USP_CMD_TYPE "-type"
#define TG_USP_CMD_USPS "-usps"
#define TG_USP_CMD_FROM "-from"
#define TG_USP_CMD_TO "-to"
#define TG_USP_CMD_CITY_ALL "all"
#define TG_USP_CMD_CITY_PB "pb"
#define TG_USP_CMD_SET "-set"
#define TG_USP_CMD_LIM "-l"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class TgUspResult : public StdAnalyseResult {
public:
	virtual std::string fGetResultNameString() {
		return "*** USP analysis results ***";
	}
};

enum CitySet {CSetAll, CSetPb};

class TgUsp : public Analyser {
private:
	//----- data -----
	std::vector<std::string> cArgs;

	bool cOptionsDet;
	UspAnType cOptionsUspAnType;
	int cOptionsLim;
	CitySet cOptionsFromCitySet;
	CitySet cOptionsToCitySet;
	std::string cOptionsFromCityPm;
	std::string cOptionsToCityPm;
	std::string cOptionsUspsPm;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- constructors -----
	TgUsp(std::vector<std::string> iArguments);

	//----- required interface - Analyser -----
	virtual AnalyseResult* fAnalyse(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};

	//----- static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TG_USP_NAME;};
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

#endif /* TG_USP_H */
