/* Postbox - various generated stuff (e.g. a vector of high-BC cities) gets stored
 * here and can later be retrieved in other algorithms to carry on computations */
#ifndef POSTBOX_H_
#define POSTBOX_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>
#include <vector>

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
/* Class
/*****************************************************************/

#define PB_TYPE_ANY ""
#define PB_TYPE_SET_STRING "set<string>"
#define PB_TYPE_VECT_INT "vector<int>"
#define PB_TYPE_VECT_STRING "vector<string>"
#define PB_TYPE_USP_NORMAL "vector<vector<vector<Path> > >"
#define PB_TYPE_USP_COUNT "vector<vector<int> >"
#define PB_TYPE_USP_USAGE "vector<vector<vector<pair<Path, int> > > >"

#define MAX_COEF 3

class PostMail {
	void *cData;
	std::string cDesc;
	std::string cType;
public:
	static void fDeleteData(void *iData, std::string iType) {
		if (iType == PB_TYPE_SET_STRING) {
			std::set<std::string>* lData = (std::set<std::string>*)iData;
			delete lData;
		}
		else if (iType == PB_TYPE_VECT_INT) {
			std::vector<int>* lData = (std::vector<int>*)iData;
			delete lData;
		}
		else if (iType == PB_TYPE_VECT_STRING) {
			std::vector<std::string>* lData = (std::vector<std::string>*)iData;
			delete lData;
		}
		else if (iType == PB_TYPE_USP_NORMAL) {
			std::vector<std::vector<std::vector<Path> > >* lData =
					(std::vector<std::vector<std::vector<Path> > >*)iData;
			delete lData;
		}
		else if (iType == PB_TYPE_USP_COUNT) {
			std::vector<std::vector<int> >* lData =
					(std::vector<std::vector<int> >*)iData;
			delete lData;
		}
		else if (iType == PB_TYPE_USP_USAGE) {
			std::vector<std::vector<std::vector<std::pair<Path, int> > > >* lData =
					(std::vector<std::vector<std::vector<std::pair<Path, int> > > >*)iData;
			delete lData;
		}
		else {
			ERR("Unable to delete data of type \"any\" from post box -> memory leak possible" << std::endl);
		}
	}

	PostMail() {
		cData = NULL;
	}

	PostMail(void *iData, std::string iDesc, std::string iType) {
		cData = iData;
		cDesc = iDesc;
		cType = iType;
	}

	void fDeleteData() {
		fDeleteData(cData, cType);
	}

	void* fGetData() {
		return cData;
	}

	std::string fGetDesc() {
		return cDesc;
	}

	std::string fGetType() {
		return cType;
	}
};

class PostBox {
	static std::map<std::string, PostMail> cMails;
public:
	static PostMail* fGetMail(std::string iName, std::string iType) {
		if (cMails.find(iName) == cMails.end()) {
			ERR("Mail \"" << iName << "\" not found" << std::endl);
			return NULL;
		}

		if (iType == PB_TYPE_ANY) {
			return &(cMails[iName]);
		}

		if (cMails[iName].fGetType() != iType) {
			ERR("Incorrect mail type" << std::endl);
		}

		return &(cMails[iName]);
	}

	static Ret fAddMail(std::string iName, PostMail iMail) {
		if (cMails.find(iName) != cMails.end()) {
			INFO("Mail with the name " << iName << " already exists" << std::endl);
			return NOK;
		}

		cMails[iName] = iMail;

		return OK;
	}

	static Ret fRmMail(std::string iName) {
		if (cMails.find(iName) == cMails.end()) {
			ERR("Mail with the name " << iName << " not found" << std::endl);
			return NOK;
		}

		cMails[iName].fDeleteData();
		cMails.erase(iName);

		return OK;
	}

	static std::string fGetMailsString() {
		std::ostringstream lSs;

		for (std::map<std::string, PostMail>::iterator i = cMails.begin(); i != cMails.end(); i++) {
			lSs << "Mail " << i->first << std::endl;
			lSs << "	" << i->second.fGetType() << ": " << i->second.fGetDesc() << std::endl;
		}

		return lSs.str();
	}
};

/*****************************************************************/
/* Postman
/*****************************************************************/

class Postman {
public:
	//----- constructors -----
	virtual ~Postman() = 0;

	//----- abstract interface -----
	virtual PostMail fPostMail(Object *iObject, std::string iMailName) = 0;

	virtual std::string fGenerates() = 0;
	virtual std::string fGetHelp() = 0;
	virtual std::string fGetName() = 0;
	virtual std::set<ObjType> fMeantFor() = 0;
};
inline Postman::~Postman() {};

/*****************************************************************/
/* Object - postmans
/*****************************************************************/

class UgPostman: public Postman {
public:
	//----- constructors -----
	virtual ~UgPostman() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTUg);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline UgPostman::~UgPostman() {};

class TePostman: public Postman {
public:
	//----- constructors -----
	virtual ~TePostman() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTe);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TePostman::~TePostman() {};

class TdPostman: public Postman {
public:
	//----- constructors -----
	virtual ~TdPostman() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTd);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TdPostman::~TdPostman() {};

class TtPostman: public Postman {
public:
	//----- constructors -----
	virtual ~TtPostman() = 0;

	//----- static interface -----
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTt);
		return lSet;
	};

	//----- interface -----
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};
};
inline TtPostman::~TtPostman() {};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* POSTBOX_H_ */
