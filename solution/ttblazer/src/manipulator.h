/* Manipulates with the objects:
 * - Underlying graphs
 * - Time-expanded graphs
 * - Time-dependent graphs
 * - Timetables
 *
 * Implements behavior invoked by commands from user */
#ifndef MANIPULATOR_H_
#define MANIPULATOR_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>
#include <vector>

#include "central.h"
#include "objects/object.h"
#include "objects/undergr.h"
#include "objects/timedep.h"
#include "objects/timeexp.h"
#include "objects/timetable.h"
#include "oracles/oracle.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define LS_PADWIDTH 40

//----- Constants used to output results of algorithms -----
/* RINFO - results info - are the strings to be looked for when analysing
 * log dumps from the program */
#define RINFO_MODIFYING "********** MODIFYING *********************"
#define RINFO_GENERATING "********** GENERATING ********************"
#define RINFO_POSTBOX "********** POSTING MAIL *****************"
#define RINFO_RMPOST "********** REMOVING MAIL ****************"
#define RINFO_ANALYSING "********** ANALYSING *********************"
#define RINFO_ORACLEPREP "********** ORACLE PREPROCESSING **********"
#define RINFO_ORACLERM "********** ORACLE REMOVING **********"
#define RINFO_SHPATH "********** SHORTEST PATH *****************"
#define RINFO_CONN "********** CONNECTION ********************"
#define RINFO_DIST "********** DISTANCE **********************"
#define RINFO_EA "********** EARLIEST ARRIVAL **************"
#define RINFO_SHOWING "********** SHOWING **********************"
#define RINFO_LISTING "********** LISTING **********************"
#define RINFO_METHODS "********** METHODS **********************"
#define RINFO_LOAD "********** LOADING **********************"
#define RINFO_SAVE "********** SAVING **********************"
#define RINFO_LOADPM "********** LOADING PM ********************"
#define RINFO_SAVEPM "********** SAVING PM ********************"
#define RINFO_RM "********** REMOVING ********************"
#define RINFO_RENAME "********** RENAMING ********************"

#define RINFO_FROM "From"
#define RINFO_ATTIME "At time"
#define RINFO_TO "To"
#define RINFO_ORACLE "Oracle"
#define RINFO_ORACLE_INDEX "Oracle index"
#define RINFO_ANALYSER "Analyser"
#define RINFO_GENERATOR "Generator"
#define RINFO_GENOTYPE "Generated object type"
#define RINFO_GENOINDEX "Generated object index"
#define RINFO_GENONAME "Generated object name"
#define RINFO_PBPOSTMAN "Postman"
#define RINFO_PBTYPE "Posted mail type"
#define RINFO_PBNAME "Posted mail name"
#define RINFO_PBDESC "Posted mail description"
#define RINFO_MODIFIER "Modifier"
#define RINFO_UG "UG"
#define RINFO_TE "TE"
#define RINFO_TD "TD"
#define RINFO_TT "TT"
#define RINFO_ARGS "Args"
#define RINFO_OTYPE "Object type"
#define RINFO_OINDEX "Object index"
#define RINFO_ONAME "Object name"
#define RINFO_DUR "Duration"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Struct - Info
/*****************************************************************/

/* Wrapper class (or struct) for a loaded object */
struct Info {
	//----- data -----
	std::string cName;
	std::string cFileName;
	std::vector<Oracle*> cOracles;
	Object* cObject;

	//----- constructors -----
	virtual ~Info() = 0;

	//----- abstract interface -----
	virtual ObjType fObjType() = 0;
	virtual void fCreateObject() = 0;

	//----- static interface -----
	static Info* fCreateInfo(ObjType iOT);

	//----- interface -----
	virtual Oracle* fGetOracle(int iIndex) {
		if (iIndex < 0 || iIndex >= cOracles.size()) {
			return NULL;
		}

		return cOracles[iIndex];
	}

	virtual void fRemoveOracle(int iIndex) {
		if (iIndex < 0 || iIndex >= cOracles.size()) {
			return;
		}

		delete cOracles[iIndex];
		cOracles.erase(cOracles.begin() + iIndex);
	}

	virtual int fGetOracleIndex(std::string iName) {
		for (std::vector<Oracle*>::size_type i = 0; i < cOracles.size(); i++) {
			if (cOracles[i]->fGetName() == iName) {
				return i;
			}
		}
		return -1;
	}

	virtual std::string fGetFileName() {return cFileName;};
	virtual void fSetFileName(std::string iFileName) {cFileName = iFileName;};
};
inline Info::~Info() {
	for (size_t i = 0; i < cOracles.size(); i++) {
		delete cOracles[i];
	}

	delete cObject;
};

/*****************************************************************/
/* Struct - GraphInfo
/*****************************************************************/

struct GraphInfo : virtual public Info {
	//----- constructors -----
	virtual ~GraphInfo() = 0;

	//----- abstract interface -----
	virtual GraphObject* fGetGraphObj() = 0;
};
inline GraphInfo::~GraphInfo() {};

/*****************************************************************/
/* Struct - TimetableInfo
/*****************************************************************/

struct TimetableInfo : virtual public Info {
	//----- constructors -----
	virtual ~TimetableInfo() = 0;

	//----- abstract interface -----
	virtual TimetableObject* fGetTimetableObj() = 0;
};
inline TimetableInfo::~TimetableInfo() {};

/*****************************************************************/
/* Struct - TimeGraphInfo
/*****************************************************************/

struct TimeGraphInfo : virtual public TimetableInfo, virtual public GraphInfo {
	//----- constructors -----
	virtual ~TimeGraphInfo() = 0;

	//----- abstract interface -----
	virtual TimeGraphObject* fGetTimeGraphObj() = 0;
};
inline TimeGraphInfo::~TimeGraphInfo() {};

/*****************************************************************/
/* Structs - Object infos
/*****************************************************************/

struct UgInfo : public GraphInfo {
	//----- required interface - Info -----
	virtual ObjType fObjType() {
		return OTUg;
	}

	virtual void fCreateObject() {
		cObject = new UgGraph();
	}

	//----- required interface - GraphInfo -----
	virtual GraphObject* fGetGraphObj() {
		return dynamic_cast<GraphObject*>(cObject);
	}
};

struct TeInfo : public TimeGraphInfo {
	//----- required interface - Info -----
	virtual ObjType fObjType() {
		return OTTe;
	}

	virtual void fCreateObject() {
		cObject = new TeGraph();
	}

	//----- required interface - GraphInfo -----
	virtual GraphObject* fGetGraphObj() {
		return dynamic_cast<GraphObject*>(cObject);
	}

	//----- required interface - TimetableInfo -----
	virtual TimetableObject* fGetTimetableObj() {
		return dynamic_cast<TimetableObject*>(cObject);
	}

	//----- required interface - TimetableInfo -----
	virtual TimeGraphObject* fGetTimeGraphObj() {
		return dynamic_cast<TimeGraphObject*>(cObject);
	}
};

struct TdInfo : public TimeGraphInfo {
	//----- required interface - Info -----
	virtual ObjType fObjType() {
		return OTTd;
	}

	virtual void fCreateObject() {
		cObject = new TdGraph();
	}

	//----- required interface - GraphInfo -----
	virtual GraphObject* fGetGraphObj() {
		return dynamic_cast<GraphObject*>(cObject);
	}

	//----- required interface - TimetableInfo -----
	virtual TimetableObject* fGetTimetableObj() {
		return dynamic_cast<TimetableObject*>(cObject);
	}

	//----- required interface - TimetableInfo -----
	virtual TimeGraphObject* fGetTimeGraphObj() {
		return dynamic_cast<TimeGraphObject*>(cObject);
	}
};

struct TtInfo : public TimetableInfo {
	//----- required interface - Info -----
	virtual ObjType fObjType() {
		return OTTt;
	}

	virtual void fCreateObject() {
		cObject = new Timetable();
	}

	//----- required interface - TimetableInfo -----
	virtual TimetableObject* fGetTimetableObj() {
		return dynamic_cast<TimetableObject*>(cObject);
	}
};

/*****************************************************************/
/* Class Manipulator
/*****************************************************************/

class Manipulator {
private:
	//----- data -----
	static std::map<ObjType, std::vector<Info*> > cInfos;

	//----- implementation -----
	static bool fRestructedCheck(ObjType iOT, Info *iInfo);

	static Info* fGetInfo(ObjType iOT, int iIndex);
	static std::string fGetTimeString(int iTime);
public:
	//----- static interface -----
	static void fInitialize();
	static int fTransformIndex(ObjType iOT, int iIndex);
	static int fGetInfoIndex(ObjType iOT, std::string iName);
	static int fGetOracleIndex(ObjType iOT, int iObjIndex, std::string iOrName);

	//Top-level manipulation
	static void fLoad(ObjType iOT, const char *iFileName);
	static void fSave(ObjType iOT, std::string iName, const char *iFileName);
	static void fSave(ObjType iOT, int iIndex, const char *iFileName);
	static Ret fProcessScript(const char *iFileName);
	static void fRemove(ObjType iOT, int iObjIndex);
	static void fRename(ObjType iOT, int iObjIndex, std::string iNewName);

	//Manipulation
	static void fBuildIdIndex(ObjType iOT, int iObjIndex);
	static void fChangeTimeFormat(TimeFormat iTF);

	//Listing
	static void fListAll(ObjType iOT);
	static void fListAll();
	static void fListObject(ObjType iOT, int iIndex);
	static void fShowNode(ObjType iOT, int iObjIndex, int iNodeIndex);
	static void fShowArc(ObjType iOT, int iObjIndex, int iFrom, int iTo);
	static void fShowAll(ObjType iOT, bool iWhole);
	static void fShowAll(bool iWhole);
	static void fShow(ObjType iOT, int iIndex, bool iWhole);
	static int fIdToIndex(ObjType iOT, int iObjIndex, std::string iId);

	//Oracles
	static void fListDoMethods();
	static void fOracleHelp(ObjType iOT, std::string iOracle);
	static void fCreateOracle(ObjType iOT, std::string iOracle, int iObjIndex,
			std::vector<std::string> iArgs);
	static void fRemoveOracle(ObjType iOT, int iOracleIndex, int iObjIndex,
			std::vector<std::string> iArgs);
	static void fSp(ObjType iOT, int iObjIndex, int iOracleIndex, int iFrom, int iTo);
	static void fDist(ObjType iOT, int iObjIndex, int iOracleIndex, int iFrom, int iTo);
	static void fConn(ObjType iOT, int iObjIndex, int iOracleIndex, std::string iFrom, int iTime,
			std::string iTo);
	static void fEa(ObjType iOT, int iObjIndex, int iOracleIndex, std::string iFrom, int iTime,
			std::string iTo);
	static void fCompareOracles(ObjType iOT, int iObjIndex, QueryType iQT, int iTestCases,
			double iExistRatio, std::vector<int> iOracles);

	//Analysers
	static void fListAnalyseMethods();
	static void fAnalyserHelp(ObjType iOT, std::string iAnalyser);
	static void fAnalyse(ObjType iOT, std::string iAnalyser, int iObjIndex,
			std::vector<std::string> iArgs);

	//Generators
	static void fListGeneratorMethods();
	static void fGeneratorHelp(ObjType iOT, std::string iGenerator);
	static void fGenerate(ObjType iOT, std::string iGenerator, int iObjIndex,
			std::vector<std::string> iArgs);

	//Modifiers
	static void fListModifierMethods();
	static void fModifierHelp(ObjType iOT, std::string iModifier);
	static void fModify(ObjType iOT, std::string iModifier, int iObjIndex,
			std::vector<std::string> iArgs);

	//Postmen
	static void fListPostmanMethods();
	static void fPostmanHelp(ObjType iOT, std::string iPostman);
	static void fPostman(ObjType iOT, std::string iPostman, std::string iPostName, int iObjIndex,
			std::vector<std::string> iArgs);
	static void fRmPostMail(std::string iPostName);
	static void fShowPostBox();
	static void fSaveAnSet(std::string iAnSet, std::string iFileName);
	static void fLoadAnSet(std::string iAnSet, std::string iFileName);
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* MANIPULATOR_H_ */
