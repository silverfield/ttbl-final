/* Definitions of abstract classes (interfaces) for modifiers */
#ifndef MODIFIER_H
#define MODIFIER_H

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
/* Class - Modifier
/*****************************************************************/

class Modifier {
public:
	//----- constructors -----
	virtual ~Modifier() = 0;

	//----- abstract interface -----
	virtual Ret fModify(Object *iObject) = 0;

	virtual std::string fGetHelp() = 0;
	virtual std::string fGetName() = 0;
	virtual std::set<ObjType> fMeantFor() = 0;
};
inline Modifier::~Modifier() {};

/*****************************************************************/
/* Class - GraphModifier
/*****************************************************************/

class GraphModifier : virtual public Modifier {
public:
	//----- constructors -----
	virtual ~GraphModifier() = 0;
};
inline GraphModifier::~GraphModifier() {};

/*****************************************************************/
/* Class - TimetableModifier
/*****************************************************************/

class TimetableModifier : virtual public Modifier {
public:
	//----- constructors -----
	virtual ~TimetableModifier() = 0;
};
inline TimetableModifier::~TimetableModifier() {};

/*****************************************************************/
/* Classes - Object Modifiers
/*****************************************************************/

class UgModifier: public Modifier {
public:
	//----- constructors -----
	virtual ~UgModifier() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTUg);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline UgModifier::~UgModifier() {};

class TeModifier: public Modifier {
public:
	//----- constructors -----
	virtual ~TeModifier() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTe);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TeModifier::~TeModifier() {};

class TdModifier: public Modifier {
public:
	//----- constructors -----
	virtual ~TdModifier() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTd);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TdModifier::~TdModifier() {};

class TtModifier: public Modifier {
public:
	//----- constructors -----
	virtual ~TtModifier() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTt);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TtModifier::~TtModifier() {};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* MODIFIER_H */
