/* Definitions of abstract classes (interfaces) for oracles */
#ifndef ORACLE_H_
#define ORACLE_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>
#include <vector>

#include "../../../common/src/structsizer.h"

#include "../central.h"
#include "../objects/undergr.h"
#include "../objects/timedep.h"
#include "../objects/timeexp.h"
#include "../objects/timetable.h"
#include "../objects/object.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Types
/*****************************************************************/

/*****************************************************************/
/* Class - Oracle
/*****************************************************************/

class Oracle {
public:
	//----- constructors -----
	virtual ~Oracle() = 0;

	//----- abstract interface -----
	virtual Ret fPreprocess(Object *iObject) = 0;

	virtual std::string fGetHelp() = 0;
	virtual std::string fGetName() = 0;
	virtual std::set<ObjType> fMeantFor() = 0;
	virtual bool fAnswers(QueryType iQt) = 0;
	virtual std::set<QueryType> fQueryTypes() = 0;
	virtual std::vector<std::string> fGetArgs() = 0;
	virtual inline bool fLastQueryLocal() {
		return true;
	}
	virtual int fGetSpace() = 0;
};
inline Oracle::~Oracle() {};

/*****************************************************************/
/* Class - GraphOracle
/*****************************************************************/

class GraphOracle : virtual public Oracle {
public:
	//----- constructors -----
	virtual ~GraphOracle() = 0;

	//----- abstract interface -----
	virtual Path fQueryPath(int iFrom, int iTo) = 0;
	virtual int fQueryDistance(int iFrom, int iTo) = 0;
};
inline GraphOracle::~GraphOracle() {};

/*****************************************************************/
/* Class - TimetableOracle
/*****************************************************************/

class TimetableOracle : virtual public Oracle {
public:
	//----- constructors -----
	virtual ~TimetableOracle() = 0;

	//----- abstract interface -----
	virtual Connection fQueryConn(std::string iFrom, int iTime, std::string iTo) = 0;
	virtual int fQueryEa(std::string iFrom, int iTime, std::string iTo) = 0;
};
inline TimetableOracle::~TimetableOracle() {};

/*****************************************************************/
/* Classes - Object oracles
/*****************************************************************/

class UgOracle: public GraphOracle {
public:
	//----- constructors -----
	virtual ~UgOracle() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTUg);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline UgOracle::~UgOracle() {};

class TeOracle: public TimetableOracle {
public:
	//----- constructors -----
	virtual ~TeOracle() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTe);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TeOracle::~TeOracle() {};

class TdOracle: public TimetableOracle {
public:
	//----- constructors -----
	virtual ~TdOracle() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTd);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TdOracle::~TdOracle() {};

class TtOracle: public TimetableOracle {
public:
	//----- constructors -----
	virtual ~TtOracle() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTt);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TtOracle::~TtOracle() {};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* ORACLE_H_ */
