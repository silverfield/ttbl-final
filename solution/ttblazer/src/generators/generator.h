/* Definitions of abstract classes (interfaces) for generators */
#ifndef GENERATOR_H_
#define GENERATOR_H_

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
/* Class - Generator
/*****************************************************************/

class Generator {
public:
	//----- constructors -----
	virtual ~Generator() = 0;

	//----- abstract interface -----
	virtual Object* fGenerate(Object *iObject) = 0;

	virtual ObjType fGenerates() = 0;
	virtual std::string fGetHelp() = 0;
	virtual std::string fGetName() = 0;
	virtual std::set<ObjType> fMeantFor() = 0;
};
inline Generator::~Generator() {};

/*****************************************************************/
/* Class - GraphGenerator
/*****************************************************************/

class GraphGenerator : virtual public Generator {
public:
	//----- constructors -----
	virtual ~GraphGenerator() = 0;
};
inline GraphGenerator::~GraphGenerator() {};

/*****************************************************************/
/* Class - TimetableGenerator
/*****************************************************************/

class TimetableGenerator : virtual public Generator {
public:
	//----- constructors -----
	virtual ~TimetableGenerator() = 0;
};
inline TimetableGenerator::~TimetableGenerator() {};

/*****************************************************************/
/* Classes - Object Generators
/*****************************************************************/

class UgGenerator: public Generator {
public:
	//----- constructors -----
	virtual ~UgGenerator() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTUg);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline UgGenerator::~UgGenerator() {};

class TeGenerator: public Generator {
public:
	//----- constructors -----
	virtual ~TeGenerator() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTe);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TeGenerator::~TeGenerator() {};

class TdGenerator: public Generator {
public:
	//----- constructors -----
	virtual ~TdGenerator() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTd);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TdGenerator::~TdGenerator() {};

class TtGenerator: public Generator {
public:
	//----- constructors -----
	virtual ~TtGenerator() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTt);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TtGenerator::~TtGenerator() {};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* GENERATOR_H_ */
