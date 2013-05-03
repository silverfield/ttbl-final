/* Definitions of abstract classes (interfaces) for analysers. */
#ifndef ANALYSER_H_
#define ANALYSER_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>
#include <vector>

#include "../central.h"
#include "../objects/object.h"
#include "../objects/undergr.h"
#include "../objects/timedep.h"
#include "../objects/timeexp.h"
#include "../objects/timetable.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class - AnalyseResult
/*****************************************************************/

class AnalyseResult {
public:
	//----- constructors -----
	virtual ~AnalyseResult();

	//----- abstract interface -----
	virtual std::string fGetResultNameString() = 0;
	virtual std::string fGetResultString() = 0;
};
inline AnalyseResult::~AnalyseResult() {};

#define ANRES (*(lAnRes->fSs()))
class StdAnalyseResult : public AnalyseResult {
	std::ostringstream cResult;

public:
	std::string fGetResultString() {
		return cResult.str();
	}

	std::ostringstream* fSs() {
		return &cResult;
	}
};

/*****************************************************************/
/* Class - Analyser
/*****************************************************************/

class Analyser {
public:
	//----- constructors -----
	virtual ~Analyser() = 0;

	//----- abstract interface -----
	virtual AnalyseResult* fAnalyse(Object *iObject) = 0;

	virtual std::string fGetHelp() = 0;
	virtual std::string fGetName() = 0;
	virtual std::set<ObjType> fMeantFor() = 0;
};
inline Analyser::~Analyser() {};

/*****************************************************************/
/* Classes - Object Analysers
/*****************************************************************/

class UgAnalyser: public Analyser {
public:
	//----- constructors -----
	virtual ~UgAnalyser() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTUg);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline UgAnalyser::~UgAnalyser() {};

class TeAnalyser: public Analyser {
public:
	//----- constructors -----
	virtual ~TeAnalyser() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTe);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TeAnalyser::~TeAnalyser() {};

class TdAnalyser: public Analyser {
public:
	//----- constructors -----
	virtual ~TdAnalyser() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTd);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TdAnalyser::~TdAnalyser() {};

class TtAnalyser: public Analyser {
public:
	//----- constructors -----
	virtual ~TtAnalyser() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTt);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TtAnalyser::~TtAnalyser() {};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* ANALYSER_H_ */
