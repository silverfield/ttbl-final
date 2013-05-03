/*****************************************************************/
/* Includes
/*****************************************************************/
#include <fstream>
#include <iostream>
#include <typeinfo>

#include "central.h"
#include "cmdproc.h"
#include "oracles/oracleinfos.h"
#include "oracles/oracle.h"
#include "analysers/analyserinfos.h"
#include "analysers/analyser.h"
#include "generators/generatorinfos.h"
#include "generators/generator.h"
#include "modifiers/modifierinfos.h"
#include "modifiers/modifier.h"
#include "postbox/postboxinfos.h"
#include "postbox/postbox.h"

#include "manipulator.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

map<ObjType, vector<Info*> > Manipulator::cInfos;

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Struct - Infos
/*****************************************************************/

Info* Info::fCreateInfo(ObjType iOT) {
	switch (iOT) {
	case OTUg:
		return new UgInfo();
		break;
	case OTTe:
		return new TeInfo();
		break;
	case OTTd:
		return new TdInfo();
		break;
	case OTTt:
		return new TtInfo();
		break;
	}

	return NULL;
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

void Manipulator::fInitialize() {
	cInfos.insert(pair<ObjType, vector<Info*> >(OTUg, vector<Info*>()));
	cInfos.insert(pair<ObjType, vector<Info*> >(OTTe, vector<Info*>()));
	cInfos.insert(pair<ObjType, vector<Info*> >(OTTd, vector<Info*>()));
	cInfos.insert(pair<ObjType, vector<Info*> >(OTTt, vector<Info*>()));
}

int Manipulator::fTransformIndex(ObjType iOT, int iIndex) {
	if (iIndex == CMD_LASTI) {
		return cInfos[iOT].size() - 1;
	}

	return iIndex;
}

int Manipulator::fGetInfoIndex(ObjType iOT, string iName) {
	vector<Info*> lInfos = cInfos[iOT];
	for (vector<Info*>::size_type i = 0; i < lInfos.size(); i++) {
		if (lInfos[i]->cName == iName) {
			return i;
		}
	}

	ERR("Manipulator::fGetInfoIndex: No " << gObjTypeStringS[iOT] << " with such a name" << endl);
	return -1;
}

int Manipulator::fGetOracleIndex(ObjType iOT, int iObjIndex, string iOrName) {
	Info* lInfo = fGetInfo(iOT, iObjIndex);
	if (lInfo == NULL) {
		return -1;
	}

	return lInfo->fGetOracleIndex(iOrName);
}

/*****************************************************************/
/* Top-level manipulation
/*****************************************************************/

void Manipulator::fLoad(ObjType iOT, const char *iFileName) {
	Info *lInfo = Info::fCreateInfo(iOT);

	Ret lRetTimer = gAlgTimer->fStartNewTimer();

	//load
	lInfo->fCreateObject();
	Ret lRet = NOK;
	if (TimetableObject::fIsTimetableObject(iOT)) {
		dynamic_cast<TimetableInfo*>(lInfo)->fGetTimetableObj()->fChangeTimeFormat(gOptions->cTimeFormat);
		lRet = dynamic_cast<TimetableInfo*>(lInfo)->fGetTimetableObj()->fLoadObject(iFileName);
	}
	else {
		lRet = lInfo->cObject->fLoadObject(iFileName);
	}
	if (lRet != OK) {
		ERR("Manipulator::fLoad: Loading unsuccessful" << endl);
		gAlgTimer->fStopTimer();
		return;
	}

	//get name
	lInfo->cFileName = fCharArrayToString(iFileName);
	lInfo->cName = fJustFileName(lInfo->cFileName);

	if (lRetTimer == OK) {
		gAlgTimer->fStopTimer();
		INFO(endl);
		IMPFO(RINFO_LOAD << endl);
		IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
		IMPFO("Filename: " << lInfo->cFileName << endl);
		IMPFO(RINFO_OINDEX << ": " << cInfos[iOT].size() << endl);
		IMPFO(RINFO_ONAME << ": " << lInfo->cName << endl);
		IMPFO("Loading took " << gAlgTimer->fGetResultString());
		INFO(endl);
	}

	//store
	cInfos[iOT].push_back(lInfo);
}

void Manipulator::fSave(ObjType iOT, string iName, const char *iFileName) {
	//get the graph
	int lIndex = fGetInfoIndex(iOT, iName);
	fSave(iOT, lIndex, iFileName);
}

void Manipulator::fSave(ObjType iOT, int iIndex, const char *iFileName) {
	//get the object
	Info *lInfo = fGetInfo(iOT, iIndex);
	if (lInfo == NULL) {
		ERR("Manipulator::fSave: " << gObjTypeStrings[iOT] << " not found" << endl);
		return;
	}

	Ret lRetTimer = gAlgTimer->fStartNewTimer();

	//save
	Ret lRet = NOK;
	lRet = lInfo->cObject->fSaveObject(iFileName);
	if (lRet != OK) {
		ERR("Manipulator::fSave: Saving unsuccessful" << endl);
		gAlgTimer->fStopTimer();
		return;
	}

	if (lRetTimer == OK) {
		gAlgTimer->fStopTimer();
		INFO(endl);
		IMPFO(RINFO_SAVE << endl);
		IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
		IMPFO("Filename: " << iFileName << endl);
		IMPFO(RINFO_OINDEX << ": " << iIndex << endl);
		IMPFO(RINFO_ONAME << ": " << lInfo->cName << endl);
		IMPFO("Saving took " << gAlgTimer->fGetResultString());
		INFO(endl);
	}

	//adjust file name
	lInfo->cFileName = fCharArrayToString(iFileName);
}

Ret Manipulator::fProcessScript(const char *iFileName) {
	ifstream lFile;
	lFile.open(iFileName);

	if (!lFile) {
		ERR("Manipulator::fProcessScript: failed to open file " << iFileName << endl);
		return NOK;
	}

	vector<string> lCommands;
	string lLine;
	while (lFile) {
		getline(lFile, lLine);
		if (lLine == "") {
			continue;
		}
		if (lLine[0] != '#') {
			lCommands.push_back(lLine);
		}
	}

	fQueueAddHere(lCommands);

	lFile.close();
	return OK;
}

void Manipulator::fRemove(ObjType iOT, int iObjIndex) {
	//get the object
	Info *lInfo = fGetInfo(iOT, iObjIndex);
	if (lInfo == NULL) {
		ERR("Manipulator::fRemove: " << gObjTypeStrings[iOT] << " not found" << endl);
		return;
	}

	Ret lRetTimer = gAlgTimer->fStartNewTimer();

	string lObjName = lInfo->cName;

	delete lInfo;
	cInfos[iOT].erase(cInfos[iOT].begin() + iObjIndex);

	if (lRetTimer == OK) {
		gAlgTimer->fStopTimer();
		INFO(endl);
		IMPFO(RINFO_RM << endl);
		IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
		IMPFO(RINFO_OINDEX << ": " << iObjIndex << endl);
		IMPFO(RINFO_ONAME << ": " << lObjName << endl);
		INFO(endl);
	}
}

void Manipulator::fRename(ObjType iOT, int iObjIndex, string iNewName) {
	//get the object
	Info *lInfo = fGetInfo(iOT, iObjIndex);
	if (lInfo == NULL) {
		ERR("Manipulator::fRename: " << gObjTypeStrings[iOT] << " not found" << endl);
		return;
	}

	string lOldName = lInfo->cName;
	lInfo->cName = iNewName;

	gAlgTimer->fStopTimer();
	INFO(endl);
	IMPFO(RINFO_RENAME << endl);
	IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
	IMPFO(RINFO_OINDEX << ": " << iObjIndex << endl);
	IMPFO(RINFO_ONAME << ": " << lOldName << endl);
	IMPFO("New name" << ": " << iNewName << endl);
	INFO(endl);
}


/*****************************************************************/
/* Manipulation
/*****************************************************************/

void Manipulator::fBuildIdIndex(ObjType iOT, int iObjIndex) {
	if (GraphObject::fIsGraphObject(iOT) == false) {
		ERR("Object is not a graph object" << endl);
		return;
	}

	GraphInfo *lInfo = dynamic_cast<GraphInfo*>(fGetInfo(iOT, iObjIndex));
	if (lInfo == NULL) {
		ERR("Manipulator::fBuildIdIndex: " << gObjTypeStrings[iOT] << " not found" << endl);
		return;
	}

	Ret lRetTimer = gAlgTimer->fStartNewTimer();

	lInfo->fGetGraphObj()->fComputeIdIndexMap();

	if (lRetTimer == OK) {
		gAlgTimer->fStopTimer();
		INFO("Building ID->Index map for " << gObjTypeStrings[iOT] << " " << lInfo->cName << " took:"
				<< endl);
		INFO(gAlgTimer->fGetResultString());
	}
}

void Manipulator::fChangeTimeFormat(TimeFormat iTF) {
	for (map<ObjType, vector<Info*> >::iterator i = cInfos.begin(); i != cInfos.end(); i++) {
		if (TimetableObject::fIsTimetableObject(i->first) == false) {
			continue;
		}

		vector<Info*> lInfos = i->second;
		for (vector<Info*>::size_type j = 0; j < lInfos.size(); j++) {
			dynamic_cast<TimetableInfo*>(lInfos[j])->fGetTimetableObj()->fChangeTimeFormat(iTF);
		}
	}
}

/*****************************************************************/
/* Listing
/*****************************************************************/

void Manipulator::fListAll(ObjType iOT) {
	INFO("Listing " << gObjTypeStrings[iOT] << "s *******************" << endl);
	for (vector<Info*>::size_type i = 0; i < cInfos[iOT].size(); i++) {
		fListObject(iOT, i);
	}
}

void Manipulator::fListAll() {
	INFO("*********************************************" << endl);
	INFO("Listing EVERYTHING **************************" << endl);
	INFO("*********************************************" << endl);
	fListAll(OTUg);
	fListAll(OTTe);
	fListAll(OTTd);
	fListAll(OTTt);
}

void Manipulator::fListObject(ObjType iOT, int iIndex) {
	Info *lInfo = fGetInfo(iOT, iIndex);
	if (lInfo == NULL) {
		ERR("Manipulator::fListObject: " << gObjTypeStrings[iOT] << " not found" << endl);
		return;
	}

	INFO(endl);
	IMPFO(RINFO_LISTING << endl);
	IMPFO(RINFO_OTYPE << ": " << RINFO_UG << endl);
	IMPFO("Index: " << iIndex << endl);
	IMPFO("Name: " << lInfo->cName << endl);
	IMPFO("File name: " << lInfo->cFileName << endl);
	if (lInfo->cOracles.size() == 0) {
		IMPFO("Oracles: none" << endl);
	}
	else {
		IMPFO("Oracles: " << endl);
		IMPFO(fPadString("	Index", LS_PADWIDTH) << fPadString("Name", LS_PADWIDTH) <<
				fPadString("Query types", LS_PADWIDTH))
		vector<Oracle*> lOracles = lInfo->cOracles;
		for (vector<Oracle*>::size_type i = 0; i < lOracles.size(); i++) {
			string lName = lOracles[i]->fGetName();
			string lQueryTypes = fGetQueryTypeSetString(lOracles[i]->fQueryTypes());
			IMPFO(fPadString("	" + fIntToStr(i), LS_PADWIDTH) << fPadString(lName, LS_PADWIDTH) <<
					fPadString(lQueryTypes, LS_PADWIDTH) << endl);
		}
	}
	INFO(endl);
}

void Manipulator::fShowNode(ObjType iOT, int iObjIndex, int iNodeIndex) {
	if (GraphObject::fIsGraphObject(iOT) == false) {
		ERR("Cannot show node in " << gObjTypeStrings[iOT] << endl);
		return;
	}

	GraphInfo *lInfo = dynamic_cast<GraphInfo*>(fGetInfo(iOT, iObjIndex));
	if (lInfo == NULL) {
		ERR("Manipulator::fShowNode: " << gObjTypeStrings[iOT] << " not found" << endl);
		return;
	}

	if (lInfo->fGetGraphObj()->fNodeIndexExists(iNodeIndex) == false) {
		ERR("Manipulator::fShowNode: Node index not found" << endl);
		return;
	}

	string lNodeString = "";
	if (TimeGraphObject::fIsTimeGraphObject(iOT)) {
		lNodeString = dynamic_cast<TimeGraphInfo*>(lInfo)->fGetTimeGraphObj()->fGetNodeString(
				iNodeIndex);
	}
	else {
		lNodeString = lInfo->fGetGraphObj()->fGetNodeString(iNodeIndex);
	}

	INFO(endl);
	IMPFO(RINFO_SHOWING << endl);
	IMPFO(lNodeString << endl);
	INFO(endl);
}

void Manipulator::fShowArc(ObjType iOT, int iObjIndex, int iFrom, int iTo) {
	if (GraphObject::fIsGraphObject(iOT) == false) {
		ERR("Cannot show arc in " << gObjTypeStrings[iOT] << endl);
		return;
	}

	GraphInfo *lInfo = dynamic_cast<GraphInfo*>(fGetInfo(iOT, iObjIndex));
	if (lInfo == NULL) {
		ERR("Manipulator::fShowArc: " << gObjTypeStrings[iOT] << " not found" << endl);
		return;
	}

	if (lInfo->fGetGraphObj()->fArcExists(iFrom, iTo) == false) {
		ERR("Manipulator::fShowArc: Arc not found" << endl);
		return;
	}

	string lArcString = "";
	if (TimeGraphObject::fIsTimeGraphObject(iOT)) {
		lArcString = dynamic_cast<TimeGraphInfo*>(lInfo)->fGetTimeGraphObj()->fGetArcString(
				iFrom, iTo);
	}
	else {
		lArcString = lInfo->fGetGraphObj()->fGetArcString(iFrom, iTo);
	}

	INFO(endl);
	IMPFO(RINFO_SHOWING << endl);
	IMPFO(lArcString << endl);
	INFO(endl);
}

void Manipulator::fShowAll(ObjType iOT, bool iWhole) {
	INFO("Showing " << gObjTypeStrings[iOT] << "s *******************" << endl);
	for (vector<Info*>::size_type i = 0; i < cInfos[iOT].size(); i++) {
		fShow(iOT, i, iWhole);
	}
}

void Manipulator::fShowAll(bool iWhole) {
	INFO("*********************************************" << endl);
	INFO("Showing EVERYTHING **************************" << endl);
	INFO("*********************************************" << endl);
	fShowAll(OTUg, iWhole);
	fShowAll(OTTe, iWhole);
	fShowAll(OTTd, iWhole);
	fShowAll(OTTt, iWhole);
}

void Manipulator::fShow(ObjType iOT, int iIndex, bool iWhole) {
	Info *lInfo = fGetInfo(iOT, iIndex);
	if (lInfo == NULL) {
		ERR("Manipulator::fShow: " << gObjTypeStrings[iOT] << " not found" << endl);
		return;
	}

	INFO(endl);
	IMPFO(RINFO_SHOWING << endl);
	IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
	IMPFO("Index: " << iIndex << endl);
	IMPFO("Name: " << lInfo->cName << endl);
	IMPFO(lInfo->cObject->fGetBasicFactsString());
	if (iWhole) {
		if (TimetableObject::fIsTimetableObject(iOT)) {
			TimetableInfo *lTtInfo = dynamic_cast<TimetableInfo*>(lInfo);
			IMPFO(lTtInfo->fGetTimetableObj()->fGetObjectString(false) << endl);
		}
		else {
			IMPFO(lInfo->cObject->fGetObjectString(false) << endl);
		}
	}
	INFO(endl);
}

int Manipulator::fIdToIndex(ObjType iOT, int iObjIndex, string iId) {
	if (GraphObject::fIsGraphObject(iOT) == false) {
		ERR("Object is not a graph object" << endl);
		return -1;
	}

	GraphInfo *lInfo = dynamic_cast<GraphInfo*>(fGetInfo(iOT, iObjIndex));
	if (lInfo == NULL) {
		ERR("Manipulator::fIdToIndex: " << gObjTypeStrings[iOT] << " not found" << endl);
		return -1;
	}

	int lIndex = lInfo->fGetGraphObj()->fGetNodeIndex(iId);
	if (lIndex == -1) {
		ERR("Manipulator::fIdToIndex: Problem retrieving index" << endl);
		return -1;
	}

	return lIndex;
}

/*****************************************************************/
/* Oracles
/*****************************************************************/

void Manipulator::fListDoMethods() {
	IMPFO(RINFO_METHODS << endl);
	IMPFO("Available ORACLE Methods:" << endl <<
		 fPadString("Name", LS_PADWIDTH) << fPadString("Meant for", LS_PADWIDTH) <<
		 fPadString("Answers", LS_PADWIDTH) << endl);
	vector<string> lOracleMethods = OracleInfos::fGetNames();
	vector<set<ObjType> > lOracleObjTypes = OracleInfos::fGetMeantFors();
	for (vector<string>::size_type i = 0; i < lOracleMethods.size(); i++) {
		string lName = lOracleMethods[i];
		ObjType lOT = *(lOracleObjTypes[i].begin());
		string lMeantFor = "";
		for (set<ObjType>::iterator j = lOracleObjTypes[i].begin(); j != lOracleObjTypes[i].end(); j++) {
			lMeantFor += gObjTypeStr[*j] + " ";
		}
		string lQueryTypes = fGetQueryTypeSetString(OracleInfos::fGetQueryTypes(lName, lOT));

		IMPFO(fPadString(lName, LS_PADWIDTH) << fPadString(lMeantFor, LS_PADWIDTH) <<
			  fPadString(lQueryTypes, LS_PADWIDTH) << endl);
	}
}

void Manipulator::fOracleHelp(ObjType iOT, string iOracle) {
	if (OracleInfos::fExists(iOracle, iOT) == false) {
		ERR("Oracle method with specified name not found" << endl);
		return;
	}

	INFO(OracleInfos::fGetHelp(iOracle, iOT));
}

void Manipulator::fCreateOracle(ObjType iOT, string iOracle, int iObjIndex, vector<string> iArgs) {
	DEB(LVL_MANIPULATOR, "Manipulator::fCreateOracle: Going to get the " << gObjTypeStrings[iOT] <<
			" information" << endl);
	Info *lInfo = fGetInfo(iOT, iObjIndex);
	if (lInfo == NULL) {
		ERR(gObjTypeStrings[iOT] << " with specified index not found" << endl);
		return;
	}

	if (fRestructedCheck(iOT, lInfo) == false) {
		return;
	}

	if (OracleInfos::fExists(iOracle, iOT) == false) {
		ERR("Oracle method with specified name not found" << endl);
		return;
	}

	srand(time(NULL));

	INFO("Going to create oracle \"" << iOracle << "\" for " << gObjTypeStrings[iOT] <<
			" \"" << lInfo->cName << "\" (" << iObjIndex << ")" << endl);

	DEB(LVL_MANIPULATOR, "Manipulator::fCreateOracle: Going to create the oracle" << endl);
	Oracle* lOracle = OracleInfos::fCreateOracle(iOracle, iOT, iArgs);
	if (lOracle == NULL) {
		ERR("Oracle could not be created" << endl);
		return;
	}
	lInfo->cOracles.push_back(lOracle);
	DEB(LVL_MANIPULATOR, "Manipulator::fCreateOracle: Going to preprocess the oracle" << endl);
	Ret lPrepRet = lOracle->fPreprocess(lInfo->cObject);
	if (lPrepRet != OK) {
		ERR("Error preprocessing oracle" << endl);
		return;
	}
	DEB(LVL_MANIPULATOR, "Manipulator::fCreateOracle: Preprocessing OK" << endl);

	INFO("Oracle successfully created" << endl);
	INFO(endl);
	IMPFO(RINFO_ORACLEPREP << endl);
	IMPFO(RINFO_ORACLE << ": " << lOracle->fGetName() << endl);
	IMPFO(RINFO_ORACLE_INDEX << ": " << lInfo->cOracles.size() - 1 << endl);
	IMPFO(RINFO_ARGS << ": " << fGetVectorString(lOracle->fGetArgs()) << endl);
	IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
	IMPFO(RINFO_OINDEX << ": " << iObjIndex << endl);
	IMPFO(RINFO_ONAME << ": " << lInfo->cName << endl);
	if (gOptions->cTiming == TimOn) {IMPFO(RINFO_DUR << ": " << gAlgTimer->fGetResultString());}
	IMPFO(fGetSizeString(lOracle->fGetSpace()) << endl);
	INFO(endl);
}

void Manipulator::fRemoveOracle(ObjType iOT, int iOracleIndex, int iObjIndex, vector<string> iArgs) {
	DEB(LVL_MANIPULATOR, "Manipulator::fRemoveOracle: Going to get the " << gObjTypeStrings[iOT] <<
			" information" << endl);
	Info *lInfo = fGetInfo(iOT, iObjIndex);
	if (lInfo == NULL) {
		ERR(gObjTypeStrings[iOT] << " with specified index not found" << endl);
		return;
	}

	if (fRestructedCheck(iOT, lInfo) == false) {
		return;
	}

	if (lInfo->fGetOracle(iOracleIndex) == NULL) {
		ERR("Oracle not found" << endl);
		return;
	}

	string lOrName = lInfo->fGetOracle(iOracleIndex)->fGetName();
	INFO("Removing oracle " << lOrName << endl);

	lInfo->fRemoveOracle(iOracleIndex);

	INFO("Oracle successfully removed" << endl);
	INFO(endl);
	IMPFO(RINFO_ORACLERM << endl);
	IMPFO(RINFO_ORACLE << ": " << lOrName << endl);
	IMPFO(RINFO_ORACLE_INDEX << ": " << iOracleIndex << endl);
	IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
	IMPFO(RINFO_OINDEX << ": " << iObjIndex << endl);
	IMPFO(RINFO_ONAME << ": " << lInfo->cName << endl);
	INFO(endl);
}

void Manipulator::fSp(ObjType iOT, int iObjIndex, int iOracleIndex, int iFrom, int iTo) {
	if (GraphObject::fIsGraphObject(iOT) == false) {
		ERR("Cannot search for shortest paths in " << gObjTypeStrings[iOT] << endl);
		return;
	}

	GraphInfo *lInfo = dynamic_cast<GraphInfo*>(fGetInfo(iOT, iObjIndex));
	if (lInfo == NULL) {
		ERR(gObjTypeStringS[iOT] << " with specified index not found" << endl);
		return;
	}

	if (!lInfo->fGetGraphObj()->fNodeIndexExists(iFrom) || !lInfo->fGetGraphObj()->fNodeIndexExists(iTo)) {
		ERR("Endpoint(s) not found in the graph" << endl);
		return;
	}

	GraphOracle *lOracle = dynamic_cast<GraphOracle*>(lInfo->fGetOracle(iOracleIndex));
	if (lOracle == NULL) {
		ERR("Oracle with index " << iOracleIndex << " was not found" << endl);
		return;
	}

	if (lOracle->fAnswers(QTSp) == false) {
		ERR("Oracle " << lOracle->fGetName() << " does not answer queries for"
				"shortest paths" << endl);
		return;
	}

	Path lSp = lOracle->fQueryPath(iFrom, iTo);

	INFO(endl);
	IMPFO(RINFO_SHPATH << endl);
	IMPFO(RINFO_FROM << ": " << iFrom << endl);
	IMPFO(RINFO_TO << ": " << iTo << endl);
	IMPFO(RINFO_ORACLE << ": " << lOracle->fGetName() << endl);
	IMPFO(RINFO_ORACLE_INDEX << ": " << iOracleIndex << endl);
	IMPFO(RINFO_ARGS << ": " << fGetVectorString(lOracle->fGetArgs()) << endl);
	IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
	IMPFO(RINFO_OINDEX << ": " << iObjIndex << endl);
	IMPFO(RINFO_ONAME << ": " << lInfo->cName << endl);
	IMPFO("SHORTEST PATH: " << lInfo->fGetGraphObj()->fGetPathString(lSp) << endl);
	IMPFO("SHORTEST PATH SIZE: " << lSp.fGetSize() - 1 << endl);
	IMPFO("SHORTEST PATH LENGTH: " << lInfo->fGetGraphObj()->fGetPathLength(lSp) << endl);
	if (gOptions->cTiming == TimOn) {IMPFO(RINFO_DUR << ": " << gAlgTimer->fGetResultString());}
	INFO(endl);
}

void Manipulator::fDist(ObjType iOT, int iObjIndex, int iOracleIndex, int iFrom, int iTo) {
	if (GraphObject::fIsGraphObject(iOT) == false) {
		ERR("Cannot search for shortest paths in " << gObjTypeStrings[iOT] << endl);
		return;
	}

	GraphInfo *lInfo = dynamic_cast<GraphInfo*>(fGetInfo(iOT, iObjIndex));
	if (lInfo == NULL) {
		ERR(gObjTypeStringS[iOT] << " with specified index not found" << endl);
		return;
	}

	if (!lInfo->fGetGraphObj()->fNodeIndexExists(iFrom) || !lInfo->fGetGraphObj()->fNodeIndexExists(iTo)) {
		ERR("Endpoint(s) not found in the graph" << endl);
		return;
	}

	GraphOracle *lOracle = dynamic_cast<GraphOracle*>(lInfo->fGetOracle(iOracleIndex));
	if (lOracle == NULL) {
		ERR("Oracle with index " << iOracleIndex << " was not found" << endl);
		return;
	}

	if (lOracle->fAnswers(QTSp) == false) {
		ERR("Oracle " << lOracle->fGetName() << " does not answer queries for"
				"shortest paths" << endl);
		return;
	}

	int lDist = lOracle->fQueryDistance(iFrom, iTo);

	INFO(endl);
	IMPFO(RINFO_DIST << endl);
	IMPFO(RINFO_FROM << ": " << iFrom << endl);
	IMPFO(RINFO_TO << ": " << iTo << endl);
	IMPFO(RINFO_ORACLE << ": " << lOracle->fGetName() << endl);
	IMPFO(RINFO_ORACLE_INDEX << ": " << iOracleIndex << endl);
	IMPFO(RINFO_ARGS << ": " << fGetVectorString(lOracle->fGetArgs()) << endl);
	IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
	IMPFO(RINFO_OINDEX << ": " << iObjIndex << endl);
	IMPFO(RINFO_ONAME << ": " << lInfo->cName << endl);
	IMPFO("DISTANCE: " << lDist << endl);
	if (gOptions->cTiming == TimOn) {IMPFO(RINFO_DUR << ": " << gAlgTimer->fGetResultString());}
	INFO(endl);
}

void Manipulator::fConn(ObjType iOT, int iObjIndex, int iOracleIndex, string iFrom, int iTime,
		string iTo) {
	if (TimetableObject::fIsTimetableObject(iOT) == false) {
		ERR("Cannot search for earliest arrival in " << gObjTypeStrings[iOT] << endl);
		return;
	}

	TimetableInfo *lInfo = dynamic_cast<TimetableInfo*>(fGetInfo(iOT, iObjIndex));
	if (lInfo == NULL) {
		ERR(gObjTypeStringS[iOT] << " with specified index not found" << endl);
		return;
	}

	TimetableOracle *lOracle = dynamic_cast<TimetableOracle*>(lInfo->fGetOracle(iOracleIndex));
	if (lOracle == NULL) {
		ERR("Oracle with index " << iOracleIndex << " was not found" << endl);
		return;
	}

	if (lOracle->fAnswers(QTConn) == false) {
		ERR("Oracle " << lOracle->fGetName() << " does not answer queries for"
				"connections" << endl);
		return;
	}

	Connection lConn = lOracle->fQueryConn(iFrom, iTime, iTo);

	INFO(endl);
	IMPFO(RINFO_CONN << endl);
	IMPFO(RINFO_FROM << ": " << iFrom << endl);
	IMPFO(RINFO_ATTIME << ": " << fMinutesToString(iTime) << endl);
	IMPFO(RINFO_TO << ": " << iTo << endl);
	IMPFO(RINFO_ORACLE << ": " << lOracle->fGetName() << endl);
	IMPFO(RINFO_ORACLE_INDEX << ": " << iOracleIndex << endl);
	IMPFO(RINFO_ARGS << ": " << fGetVectorString(lOracle->fGetArgs()) << endl);
	IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
	IMPFO(RINFO_OINDEX << ": " << iObjIndex << endl);
	IMPFO(RINFO_ONAME << ": " << lInfo->cName << endl);
	IMPFO("OPT. CONNECTIONS: " << lConn.fGetString("\n	") << endl);
	IMPFO("NUMBER OF EL. CONNS: " << lConn.fGetSize() - 1 << endl);
	IMPFO("EARLIEST ARRIVAL: " << lConn.fGetLength() << endl);
	if (gOptions->cTiming == TimOn) {IMPFO(RINFO_DUR << ": " << gAlgTimer->fGetResultString());}
	INFO(endl);
}
void Manipulator::fEa(ObjType iOT, int iObjIndex, int iOracleIndex, string iFrom, int iTime,
		string iTo) {
	if (TimetableObject::fIsTimetableObject(iOT) == false) {
		ERR("Cannot search for earliest arrival in " << gObjTypeStrings[iOT] << endl);
		return;
	}

	TimetableInfo *lInfo = dynamic_cast<TimetableInfo*>(fGetInfo(iOT, iObjIndex));
	if (lInfo == NULL) {
		ERR(gObjTypeStringS[iOT] << " with specified index not found" << endl);
		return;
	}

	TimetableOracle *lOracle = dynamic_cast<TimetableOracle*>(lInfo->fGetOracle(iOracleIndex));
	if (lOracle == NULL) {
		ERR("Oracle with index " << iOracleIndex << " was not found" << endl);
		return;
	}

	if (lOracle->fAnswers(QTEa) == false) {
		ERR("Oracle " << lOracle->fGetName() << " does not answer queries for earliest arrival" << endl);
		return;
	}

	int lEa = lOracle->fQueryEa(iFrom, iTime, iTo);

	INFO(endl);
	IMPFO(RINFO_EA << endl);
	IMPFO(RINFO_FROM << ": " << iFrom << endl);
	IMPFO(RINFO_ATTIME << ": " << fMinutesToString(iTime) << endl);
	IMPFO(RINFO_TO << ": " << iTo << endl);
	IMPFO(RINFO_ORACLE << ": " << lOracle->fGetName() << endl);
	IMPFO(RINFO_ORACLE_INDEX << ": " << iOracleIndex << endl);
	IMPFO(RINFO_ARGS << ": " << fGetVectorString(lOracle->fGetArgs()) << endl);
	IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
	IMPFO(RINFO_OINDEX << ": " << iObjIndex << endl);
	IMPFO(RINFO_ONAME << ": " << lInfo->cName << endl);
	IMPFO("EARLIEST ARRIVAL: " << lEa << endl);
	if (gOptions->cTiming == TimOn) {IMPFO(RINFO_DUR << ": " << gAlgTimer->fGetResultString());}
	INFO(endl);
}

void Manipulator::fCompareOracles(ObjType iOT, int iObjIndex, QueryType iQT, int iTestCases,
		double iExistRatio, vector<int> iOracles) {
	//----- checks -----
	Info *lInfo = fGetInfo(iOT, iObjIndex);
	if (lInfo == NULL) {
		ERR(gObjTypeStrings[iOT] << " with specified index not found" << endl);
		return;
	}

	if (fRestructedCheck(iOT, lInfo) == false) {
		return;
	}

	for (vector<int>::size_type i = 0; i < iOracles.size(); i++) {
		if (iOracles[i] < 0 || iOracles[i] >= lInfo->cOracles.size()) {
			ERR("Oracle index out of bounds" << endl);
			return;
		}
		else if (lInfo->cOracles[iOracles[i]]->fAnswers(iQT) == false) {
			ERR("Oracle " << iOracles[i] << " does not answer query type " << gQueryTypeStr[iQT] << endl);
			return;
		}
	}
	//----- shortest paths -----

	if (iQT == QTSp || iQT == QTDist) {
		//----- get test set -----
		GraphInfo* lGraphInfo = dynamic_cast<GraphInfo*>(lInfo);
		vector<SPQuery> lTestSet = lGraphInfo->fGetGraphObj()->fGetSPInstances(iTestCases);
	}
	//----- earliest arrival -----
	else {
		//----- get test set -----
		TimetableInfo* lTimeInfo = dynamic_cast<TimetableInfo*>(lInfo);

		INFO("Getting EA instances" << endl);
		vector<EAQuery> lTestSet = lTimeInfo->fGetTimetableObj()->fGetEAInstances(iTestCases, iExistRatio);
		INFO("EA instances created" << endl);

		//----- run tests -----
		vector<vector<bool> > lFounds(lTestSet.size(), vector<bool>()); //[test-case][oracle]
		vector<vector<bool> > lFoundSomethings(lTestSet.size(), vector<bool>());
		vector<vector<bool> > lLocalQuery(lTestSet.size(), vector<bool>());
		vector<vector<Connection> > lConnections(lTestSet.size(), vector<Connection>());
		vector<vector<int> > lConnSizes(lTestSet.size(), vector<int>());
		vector<vector<int> > lEas(lTestSet.size(), vector<int>());
		vector<vector<long long int> > lQTimes(lTestSet.size(), vector<long long int>());

		vector<long long int> lTotalQTimes;

		gLogger->fInfoOn(false);
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			AlgTimer lAlgTimer;
			lAlgTimer.fStartNewTimer();

			for (vector<EAQuery>::size_type i = 0; i < lTestSet.size(); i++) {
				IMPFO("Test case " << i << ". From " << lTestSet[i].cFrom << " at " <<
					fTimeFormatToString(lTestSet[i].cTime) << " to " << lTestSet[i].cTo <<
					"-------------------------------------------------------" << endl);

				//obtain results
				TimetableOracle *lOracle = dynamic_cast<TimetableOracle*>(lInfo->cOracles[iOracles[j]]);
				Connection lConn;
				int lConnSize = -1;
				int lEa = -1;
				bool lFound;
				bool lFoundSomething;
				if (iQT == QTConn) {
					lConn = lOracle->fQueryConn(lTestSet[i].cFrom, lTestSet[i].cTime,
						lTestSet[i].cTo);
					lFoundSomething = (lConn.fGetSize() != 0);
					lFound = false;
					if (lFoundSomething) {
						if (lConn.fGetCity(0) == lTestSet[i].cFrom &&
							lConn.fGetTime(0) == lTestSet[i].cTime &&
							lConn.fGetLastCity() == lTestSet[i].cTo) {
							lFound = true;
						}
					}
					if (lFound) {
						lEa = lConn.fGetLength();
						lConnSize = lConn.fGetSize();
					}
				}
				else {
					lEa = lOracle->fQueryEa(lTestSet[i].cFrom, lTestSet[i].cTime,
						lTestSet[i].cTo);
					lFound = (lEa != -1);
					lFoundSomething = lFound;
				}
				long long int lQTime = gAlgTimer->fGetResult();
				bool lLocal = lOracle->fLastQueryLocal();

				//report results
				INFO(endl);
				IMPFO("	Oracle " << lOracle->fGetName() << " (" << iOracles[j] << ") results" << endl);
				IMPFO("		Local query: " << gBoolStr[lOracle->fLastQueryLocal()] << endl);
				IMPFO("		Connection: " << lConn.fGetString("\n	") << endl);
				IMPFO("		Connection size: " << lConnSize << endl);
				IMPFO("		Earliest arrival: " << fTimeFormatToString(lEa) << endl);
				IMPFO("		Query time: " << AlgTimer::fGetResultString(lQTime) << endl);
				INFO(endl);

				lFounds[i].push_back(lFound);
				lFoundSomethings[i].push_back(lFoundSomething);
				lLocalQuery[i].push_back(lLocal);
				lConnections[i].push_back(lConn);
				lConnSizes[i].push_back(lConnSize);
				lEas[i].push_back(lEa);
				lQTimes[i].push_back(lQTime);
			}

			lAlgTimer.fStopTimer();
			lTotalQTimes.push_back(lAlgTimer.fGetResult());
		}
		gLogger->fInfoOn(true);

		//----- comparison -----
		int lOrCount = iOracles.size();
		vector<int> lFoundConn(lOrCount, 0);
		vector<int> lFoundSomeConn(lOrCount, 0);
		vector<int> lLocalQCount(lOrCount, 0);

		vector<int> lBestConnSize(lOrCount, 0);
		vector<double> lAvgConnSize(lOrCount, 0);
		vector<int> lMaxConnSize(lOrCount, 0);
		vector<int> lMaxConnSizeTestCase(lOrCount, 0);
		vector<int> lMinConnSize(lOrCount, -1);
		vector<int> lMinConnSizeTestCase(lOrCount, 0);

		vector<int> lBestEa(lOrCount, 0);
		vector<int> lAvgEa(lOrCount, 0);
		vector<int> lMaxEa(lOrCount, 0);
		vector<int> lMaxEaTestCase(lOrCount, 0);
		vector<int> lMinEa(lOrCount, -1);
		vector<int> lMinEaTestCase(lOrCount, 0);

		vector<int> lBestQTime(lOrCount, 0);
		vector<long long int> lAvgQTime(lOrCount, 0);
		vector<long long int> lMaxQTime(lOrCount, 0);
		vector<int> lMaxQTimeTestCase(lOrCount, 0);
		vector<long long int> lMinQTime(lOrCount, 0);
		vector<int> lMinQTimeTestCase(lOrCount, 0);

		vector<int> lCorrectEa(lOrCount, 0);
		vector<double> lMaxStretch(lOrCount, 0);
		vector<int> lMaxStretchTestCase(lOrCount, 0);
		vector<double> lAvgStretch(lOrCount, 0);
		vector<int> lCorrectEaSolv(lOrCount, 0);
		vector<double> lMaxStretchSolv(lOrCount, 0);
		vector<int> lMaxStretchSolvTestCase(lOrCount, 0);
		vector<double> lAvgStretchSolv(lOrCount, 0);

		vector<int> lOracleSize(lOrCount, 0);

		for (vector<EAQuery>::size_type i = 0; i < lTestSet.size(); i++) {
			int lBestConnSizeValue = -1;
			int lBestEaValue = -1;
			long long int lBestQTimeValue = -1;

			for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
				if (lBestConnSizeValue == -1 || (lConnSizes[i][j] < lBestConnSizeValue &&
					lConnSizes[i][j] != -1)) {
					lBestConnSizeValue = lConnSizes[i][j];
				}
				if (lBestEaValue == -1 || (lEas[i][j] < lBestEaValue && lEas[i][j] != -1)) {
					lBestEaValue = lEas[i][j];
				}
				if (lBestQTimeValue == -1 || (lQTimes[i][j] < lBestQTimeValue &&
					lQTimes[i][j] != -1)) {
					lBestQTimeValue = lQTimes[i][j];
				}

				if (lFounds[i][j] == true) {
					lFoundConn[j]++;
				}

				if (lFounds[i][j] && lLocalQuery[i][j] == true) {
					lLocalQCount[j]++;
				}

				if (lFoundSomethings[i][j] == true) {
					lFoundSomeConn[j]++;
				}

				if (lFounds[i][j] && lConnSizes[i][j] != -1) {
					lAvgConnSize[j] += lConnSizes[i][j];
				}

				if (lMaxConnSize[j] < lConnSizes[i][j]) {
					lMaxConnSize[j] = lConnSizes[i][j];
					lMaxConnSizeTestCase[j] = i;
				}

				if (lMinConnSize[j] == -1 || (lMinConnSize[j] > lConnSizes[i][j] && lConnSizes[i][j] != -1)) {
					lMinConnSize[j] = lConnSizes[i][j];
					lMinConnSizeTestCase[j] = i;
				}

				if (lFounds[i][j] && lEas[i][j] != -1) {
					lAvgEa[j] += lEas[i][j];
				}

				if (lMaxEa[j] < lEas[i][j]) {
					lMaxEa[j] = lEas[i][j];
					lMaxEaTestCase[j] = i;
				}

				if (lMinEa[j] == -1 || (lMinEa[j] > lEas[i][j] && lEas[i][j] != -1)) {
					lMinEa[j] = lEas[i][j];
					lMinEaTestCase[j] = i;
				}

				lAvgQTime[j] += lQTimes[i][j];

				if (lMaxQTime[j] < lQTimes[i][j]) {
					lMaxQTime[j] = lQTimes[i][j];
					lMaxQTimeTestCase[j] = i;
				}

				if (lMinQTime[j] == -1 || lMinQTime[j] > lQTimes[i][j]) {
					lMinQTime[j] = lQTimes[i][j];
					lMinQTimeTestCase[j] = i;
				}

				if (lEas[i][j] == lEas[i][0]) {
					lCorrectEa[j]++;
					if (lEas[i][0] != -1) {
						lCorrectEaSolv[j]++;
					}
				}

				double lStretch;
				if (lEas[i][0] == 0) {
					if (j == 0) {
						lStretch = 1;
					}
					else {
						lStretch = abs(lEas[i][j] - lEas[i][0]);
					}
				}
				else {
					lStretch = (double)(lEas[i][0] + abs(lEas[i][j] - lEas[i][0])) /
						(double)lEas[i][0];
				}
				lStretch = abs(lStretch);
				if (lStretch > lMaxStretch[j]) {
					lMaxStretch[j] = lStretch;
					lMaxStretchTestCase[j] = i;
				}
				if (lEas[i][0] != -1 && lStretch > lMaxStretchSolv[j]) {
					lMaxStretchSolv[j] = lStretch;
					lMaxStretchSolvTestCase[j] = i;
				}

				lAvgStretch[j] += lStretch;
				if (lEas[i][0] != -1) {
					lAvgStretchSolv[j] += lStretch;
				}
			}

			for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
				if (lConnSizes[i][j] != -1 && lConnSizes[i][j] == lBestConnSizeValue) {
					lBestConnSize[j]++;
				}
				if (lEas[i][j] != -1 && lEas[i][j] == lBestEaValue) {
					lBestEa[j]++;
				}
				if (lQTimes[i][j] == lBestQTimeValue) {
					lBestQTime[j]++;
				}
			}
		}

		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			lOracleSize[j] = lInfo->cOracles[iOracles[j]]->fGetSpace();
			if (lFoundSomeConn[j] == 0) {
				lAvgConnSize[j] = lAvgConnSize[j] / lFoundSomeConn[j];
				lAvgEa[j] = lAvgEa[j] / lFoundSomeConn[j];
			}
			else {
				lAvgConnSize[j] = lAvgConnSize[j] / lFoundSomeConn[j];
				lAvgEa[j] = lAvgEa[j] / lFoundSomeConn[j];
			}
			lAvgQTime[j] = lAvgQTime[j] / lTestSet.size();
			lAvgStretch[j] = lAvgStretch[j] / lTestSet.size();
			if (lCorrectEaSolv[0] != 0) {
				lAvgStretchSolv[j] = lAvgStretchSolv[j] / lCorrectEaSolv[0];
			}
			else {
				lAvgStretchSolv[j] = -1;
			}
		}

		//----- results -----
		int lFieldPad = 25;
		int lDescPad = 25;
		int lResultPad = 20;

		int lObjSize = lTimeInfo->fGetTimetableObj()->fGetSpace();
		INFO(IMPFO_STR << endl << "Results of comparison (reference oracle is the first one)" << endl);
		INFO("Dataset: " << lTimeInfo->cName << endl <<
				"	n = " << lTimeInfo->fGetTimetableObj()->fGetCityCount() << endl <<
				"	Obj. size = " << fGetSizeString(lObjSize) << endl);

		INFO(IMPFO_STR << fPadString("Measure", lFieldPad) << fPadString("Description", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			TimetableOracle *lOracle = dynamic_cast<TimetableOracle*>(lInfo->cOracles[iOracles[j]]);
			INFO(fPadString(lOracle->fGetName() + " (" + fIntToStr(iOracles[j]) + ")", lResultPad));
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Oracle size", lFieldPad)
				<< fPadString("B, KB, ...", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			INFO(fPadString(fGetSizeString(lOracleSize[j]), lResultPad));
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Size up", lFieldPad)
				<< fPadString("ratio", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			double lSzp = (double)lOracleSize[j] / (double)lObjSize;
			INFO(fPadString(fDoubleToStr(lSzp), lResultPad));
		}
		INFO(endl);

		if (iQT == QTConn) {
			INFO(IMPFO_STR << fPadString("Found some conn.", lFieldPad)
					<< fPadString("#, % of cases", lDescPad));
			for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
				int lPercentage = lFoundSomeConn[j] * 100 / iTestCases;
				INFO(fPadString(fIntToStr(lFoundSomeConn[j]) + " = " + fIntToStr(lPercentage) + "%", lResultPad));
			}
			INFO(endl);
		}

		INFO(IMPFO_STR << fPadString("Found conn./EA", lFieldPad)
				<< fPadString("#, % of cases", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			int lPercentage = lFoundConn[j] * 100 / iTestCases;
			INFO(fPadString(fIntToStr(lFoundConn[j]) + " = " + fIntToStr(lPercentage) + "%", lResultPad));
		}
		INFO(endl);

		if (iQT == QTConn) {
			INFO(IMPFO_STR << fPadString("Best conn. sizes", lFieldPad)
				<< fPadString("#, % of cases", lDescPad));
			for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
				int lPercentage = lBestConnSize[j] * 100 / iTestCases;
				INFO(fPadString(fIntToStr(lBestConnSize[j]) + " = " + fIntToStr(lPercentage) + "%", lResultPad));
			}
			INFO(endl);

			INFO(IMPFO_STR << fPadString("Avg conn. sizes", lFieldPad)
				<< fPadString("# of hops", lDescPad));
			for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
				INFO(fPadString(fDoubleToStr(lAvgConnSize[j]), lResultPad));
			}
			INFO(endl);

			INFO(IMPFO_STR << fPadString("Min conn. size", lFieldPad)
				<< fPadString("# of hops [test case #]", lDescPad));
			for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
				INFO(fPadString(fIntToStr(lMinConnSize[j]) + " [" + fIntToStr(lMinConnSizeTestCase[j]) + "]", lResultPad));
			}
			INFO(endl);

			INFO(IMPFO_STR << fPadString("Max conn. size", lFieldPad)
				<< fPadString("# of hops [test case #]", lDescPad));
			for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
				INFO(fPadString(fIntToStr(lMaxConnSize[j]) + " [" + fIntToStr(lMaxConnSizeTestCase[j]) + "]", lResultPad));
			}
			INFO(endl);
		}

		INFO(IMPFO_STR << fPadString("Best EA", lFieldPad)
				<< fPadString("#, % of cases", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			int lPercentage = lBestEa[j] * 100 / iTestCases;
			INFO(fPadString(fIntToStr(lBestEa[j]) + " = " + fIntToStr(lPercentage) + "%", lResultPad));
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Avg EA", lFieldPad)
				<< fPadString("minutes", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			INFO(fPadString(fIntToStr(lAvgEa[j]), lResultPad));
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Min EA", lFieldPad)
				<< fPadString("minutes [test case #]", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			INFO(fPadString(fIntToStr(lMinEa[j]) + " (" + fIntToStr(lMinEaTestCase[j]) + ")", lResultPad));
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Max EA", lFieldPad)
				<< fPadString("minutes [test case #]", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			INFO(fPadString(fIntToStr(lMaxEa[j]) + " (" + fIntToStr(lMaxEaTestCase[j]) + ")", lResultPad));
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Best query time", lFieldPad)
				<< fPadString("#, % of cases", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			int lPercentage = lBestQTime[j] * 100 / iTestCases;
			INFO(fPadString(fIntToStr(lBestQTime[j]) + " = " + fIntToStr(lPercentage) + "%", lResultPad));
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Avg query time", lFieldPad)
				<< fPadString("seconds", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			INFO(fPadString(AlgTimer::fGetResultString(lAvgQTime[j]), lResultPad));
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Min query time", lFieldPad)
				<< fPadString("seconds [test case #]", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			INFO(fPadString(AlgTimer::fGetResultString(lMinQTime[j]) +
					" (" + fIntToStr(lMinQTimeTestCase[j]) + ")", lResultPad));
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Max query time", lFieldPad)
				<< fPadString("seconds [test case #]", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			INFO(fPadString(AlgTimer::fGetResultString(lMaxQTime[j]) +
					" (" + fIntToStr(lMaxQTimeTestCase[j]) + ")", lResultPad));
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Total query time", lFieldPad)
				<< fPadString("seconds", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			INFO(fPadString(AlgTimer::fGetResultString(lTotalQTimes[j]), lResultPad));
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Speed-up", lFieldPad)
				<< fPadString("fraction", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			double lSpeedUp = (double)lTotalQTimes[0] / (double)lTotalQTimes[j];
			INFO(fPadString(fDoubleToStr(lSpeedUp), lResultPad));
		}
		INFO(endl);

		//correct EA
		INFO(IMPFO_STR << fPadString("Correct EA", lFieldPad)
				<< fPadString("#, % of cases", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			int lPercentage = lCorrectEa[j] * 100 / iTestCases;
			INFO(fPadString(fIntToStr(lCorrectEa[j]) + " = " + fIntToStr(lPercentage) + "%", lResultPad));
		}
		INFO(endl);

				//correct EA (solvable query)
		INFO(IMPFO_STR << fPadString("Corr. EA when solvable", lFieldPad)
				<< fPadString("#, % of cases", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			if (lCorrectEaSolv[0] == 0) {
				INFO(fPadString("N/A", lResultPad));
			}
			else {
				int lPercentage = lCorrectEaSolv[j] * 100 / lCorrectEaSolv[0];
				INFO(fPadString(fIntToStr(lCorrectEaSolv[j]) + " = " + fIntToStr(lPercentage) +
						"%", lResultPad));
			}
		}
		INFO(endl);

		//maximum stretch
		INFO(IMPFO_STR << fPadString("Max stretch", lFieldPad)
				<< fPadString("ratio [test case #]", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			INFO(fPadString(fDoubleToStr(lMaxStretch[j]) +
					" (" + fIntToStr(lMaxStretchTestCase[j]) + ")", lResultPad));
		}
		INFO(endl);

		//maximum stretch (solvable query)
		INFO(IMPFO_STR << fPadString("Max str. when solvable", lFieldPad)
				<< fPadString("ratio [test case #]", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			INFO(fPadString(fDoubleToStr(lMaxStretchSolv[j]) +
					" (" + fIntToStr(lMaxStretchSolvTestCase[j]) + ")", lResultPad));
		}
		INFO(endl);

		//avg stretch
		INFO(IMPFO_STR << fPadString("Avg stretch", lFieldPad)
				<< fPadString("ratio", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			INFO(fPadString(fDoubleToStr(lAvgStretch[j]), lResultPad));
		}
		INFO(endl);

		//avg stretch (solvable query)
		INFO(IMPFO_STR << fPadString("Avg str. when solvable", lFieldPad)
				<< fPadString("ratio", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			if (lCorrectEaSolv[0] == 0) {
				INFO(fPadString("N/A", lResultPad));
			}
			else {
				INFO(fPadString(fDoubleToStr(lAvgStretchSolv[j]), lResultPad));
			}
		}
		INFO(endl);

		INFO(IMPFO_STR << fPadString("Local query", lFieldPad)
				<< fPadString("#, % of cases", lDescPad));
		for (vector<int>::size_type j = 0; j < iOracles.size(); j++) {
			if (lFoundConn[j] == 0) {
				INFO(fPadString("N/A", lResultPad));
			}
			else {
				int lPercentage = lLocalQCount[j] * 100 / lFoundConn[j];
				INFO(fPadString(fIntToStr(lLocalQCount[j]) + " = " + fIntToStr(lPercentage) +
						"%", lResultPad));
			}
		}
		INFO(endl);

		INFO(endl);
		INFO(endl);
	}

}

/*****************************************************************/
/* Analysers
/*****************************************************************/

void Manipulator::fListAnalyseMethods() {
	IMPFO(RINFO_METHODS << endl);
	IMPFO("Available ANALYSER Methods:" << endl <<
		fPadString("Name", LS_PADWIDTH) << fPadString("Meant for", LS_PADWIDTH) << endl);
	vector<string> lAnalyserMethods = AnalyserInfos::fGetNames();
	vector<set<ObjType> > lAnalyserObjTypes = AnalyserInfos::fGetMeantFors();
	for (vector<string>::size_type i = 0; i < lAnalyserMethods.size(); i++) {
		string lName = lAnalyserMethods[i];
		ObjType lOT = *(lAnalyserObjTypes[i].begin());
		string lMeantFor = "";
		for (set<ObjType>::iterator j = lAnalyserObjTypes[i].begin(); j != lAnalyserObjTypes[i].end(); j++) {
			lMeantFor += gObjTypeStr[*j] + " ";
		}

		IMPFO(fPadString(lName, LS_PADWIDTH) << fPadString(lMeantFor, LS_PADWIDTH) << endl);
	}
}

void Manipulator::fAnalyserHelp(ObjType iOT, string iAnalyser) {
	if (AnalyserInfos::fExists(iAnalyser, iOT) == false) {
		ERR("Analyser method with specified name not found" << endl);
		return;
	}

	INFO(AnalyserInfos::fGetHelp(iAnalyser, iOT));
}

void Manipulator::fAnalyse(ObjType iOT, string iAnalyser, int iObjIndex, vector<string> iArgs) {
	Info *lInfo = fGetInfo(iOT, iObjIndex);
	if (lInfo == NULL) {
		ERR(gObjTypeStringS[iOT] << " with specified index not found" << endl);
		return;
	}

	if (fRestructedCheck(iOT, lInfo) == false) {
		return;
	}

	if (AnalyserInfos::fExists(iAnalyser, iOT) == false) {
		ERR("Analyser method with specified name not found" << endl);
		return;
	}

	srand(time(NULL));

	INFO("Going to run analyser \"" << iAnalyser << "\" on " << gObjTypeStrings[iOT] << " "
			"\"" << lInfo->cName << "\" (" << iObjIndex << ")" << endl);
	INFO("Arguments of the analyser are: " << fGetVectorString(iArgs) << endl);

	DEB(LVL_MANIPULATOR, "Manipulator::fAnalyse: Going to analyse" << endl);
	Analyser* lAnalyser = AnalyserInfos::fCreateAnalyser(iAnalyser, iOT, iArgs);
	if (lAnalyser == NULL) {
		ERR("Analyser could not be created" << endl);
		return;
	}
	AnalyseResult *lResult = lAnalyser->fAnalyse(lInfo->cObject);
	if (lResult == NULL) {
		ERR("Could not obtain analyser result" << endl);
		return;
	}
	DEB(LVL_MANIPULATOR, "Manipulator::fAnalyse: Analysing OK" << endl);

	INFO("Analysing was successful. Here are the results:" << endl);
	INFO(endl);
	IMPFO(RINFO_ANALYSING << endl);
	IMPFO(RINFO_ANALYSER << ": " << iAnalyser << endl);
	IMPFO(RINFO_ARGS << ": " << fGetVectorString(iArgs) << endl);
	IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
	IMPFO(RINFO_OINDEX << ": " << iObjIndex << endl);
	IMPFO(RINFO_ONAME << ": " << lInfo->cName << endl);
	IMPFO(lResult->fGetResultString());
	if (gOptions->cTiming == TimOn) {IMPFO(RINFO_DUR << ": " << gAlgTimer->fGetResultString());}
	INFO(endl);
}

/*****************************************************************/
/* Generators
/*****************************************************************/

void Manipulator::fListGeneratorMethods() {
	IMPFO(RINFO_METHODS << endl);
	IMPFO("Available GENERATOR Methods:" << endl <<
		fPadString("Name", LS_PADWIDTH) << fPadString("Meant for", LS_PADWIDTH) <<
		fPadString("Generates", LS_PADWIDTH) << endl);
	vector<string> lGeneratorMethods = GeneratorInfos::fGetNames();
	vector<ObjType> lGeneratorGenerates = GeneratorInfos::fGetGenerates();
	vector<set<ObjType> > lGeneratorObjTypes = GeneratorInfos::fGetMeantFors();
	for (vector<string>::size_type i = 0; i < lGeneratorMethods.size(); i++) {
		string lName = lGeneratorMethods[i];
		ObjType lOT = *(lGeneratorObjTypes[i].begin());
		string lMeantFor = "";
		for (set<ObjType>::iterator j = lGeneratorObjTypes[i].begin(); j != lGeneratorObjTypes[i].end(); j++) {
			lMeantFor += gObjTypeStr[*j] + " ";
		}
		string lGenerates = gObjTypeStr[lGeneratorGenerates[i]];

		IMPFO(fPadString(lName, LS_PADWIDTH) << fPadString(lMeantFor, LS_PADWIDTH) <<
				fPadString(lGenerates, LS_PADWIDTH) << endl);
	}
}

void Manipulator::fGeneratorHelp(ObjType iOT, string iGenerator) {
	if (GeneratorInfos::fExists(iGenerator, iOT) == false) {
		ERR("Generator method with specified name not found" << endl);
		return;
	}

	INFO(GeneratorInfos::fGetHelp(iGenerator, iOT));
}

void Manipulator::fGenerate(ObjType iOT, string iGenerator, int iObjIndex, vector<string> iArgs) {
	Info *lInfo = fGetInfo(iOT, iObjIndex);
	if (lInfo == NULL) {
		ERR(gObjTypeStringS[iOT] << " with specified index not found" << endl);
		return;
	}

	if (fRestructedCheck(iOT, lInfo) == false) {
		return;
	}

	if (GeneratorInfos::fExists(iGenerator, iOT) == false) {
		ERR("Generator with specified name not found" << endl);
		return;
	}

	srand(time(NULL));

	INFO("Going to run generator \"" << iGenerator << "\" on " << gObjTypeStrings[iOT] << " "
			"\"" << lInfo->cName << "\" (" << iObjIndex << ")" << endl);
	INFO("Arguments of the generator are: " << fGetVectorString(iArgs) << endl);

	DEB(LVL_MANIPULATOR, "Manipulator::fGenerate: Going to generate" << endl);
	Generator* lGenerator = GeneratorInfos::fCreateGenerator(iGenerator, iOT, iArgs);
	if (lGenerator == NULL) {
		ERR("Generator could not be created" << endl);
		return;
	}
	Object *lCreatedObject = lGenerator->fGenerate(lInfo->cObject);
	if (lCreatedObject == NULL) {
		ERR("Generation failed" << endl);
		return;
	}

	Info *lNewInfo = Info::fCreateInfo(lGenerator->fGenerates());
	lNewInfo->cObject = lCreatedObject;
	lNewInfo->cName = lInfo->cName + "_" + lGenerator->fGetName();
	if (TimetableObject::fIsTimetableObject(lGenerator->fGenerates())) {
		dynamic_cast<TimetableObject*>(lCreatedObject)->fChangeTimeFormat(gOptions->cTimeFormat);
	}
	DEB(LVL_MANIPULATOR, "Manipulator::fGenerate: Generating OK" << endl);

	INFO("Generating was successful:" << endl);
	INFO(endl);
	IMPFO(RINFO_GENERATING << endl);
	IMPFO(RINFO_GENERATOR << ": " << iGenerator << endl);
	IMPFO(RINFO_ARGS << ": " << fGetVectorString(iArgs) << endl);
	IMPFO(RINFO_GENOTYPE << ": " << gObjTypeStr[lGenerator->fGenerates()] << endl);
	IMPFO(RINFO_GENOINDEX << ": " << cInfos[lGenerator->fGenerates()].size() << endl);
	IMPFO(RINFO_GENONAME << ": " << lNewInfo->cName << endl);
	if (gOptions->cTiming == TimOn) {IMPFO(RINFO_DUR << ": " << gAlgTimer->fGetResultString());}
	INFO(endl);

	cInfos[lGenerator->fGenerates()].push_back(lNewInfo);
}

/*****************************************************************/
/* Modifiers
/*****************************************************************/

void Manipulator::fListModifierMethods() {
	IMPFO(RINFO_METHODS << endl);
	IMPFO("Available MODIFIER Methods:" << endl <<
		fPadString("Name", LS_PADWIDTH) << fPadString("Meant for", LS_PADWIDTH) << endl);
	vector<string> lModifierMethods = ModifierInfos::fGetNames();
	vector<set<ObjType> > lModifierObjTypes = ModifierInfos::fGetMeantFors();
	for (vector<string>::size_type i = 0; i < lModifierMethods.size(); i++) {
		string lName = lModifierMethods[i];
		ObjType lOT = *(lModifierObjTypes[i].begin());
		string lMeantFor = "";
		for (set<ObjType>::iterator j = lModifierObjTypes[i].begin(); j != lModifierObjTypes[i].end(); j++) {
			lMeantFor += gObjTypeStr[*j] + " ";
		}

		IMPFO(fPadString(lName, LS_PADWIDTH) << fPadString(lMeantFor, LS_PADWIDTH) << endl);
	}
}

void Manipulator::fModifierHelp(ObjType iOT, string iModifier) {
	if (ModifierInfos::fExists(iModifier, iOT) == false) {
		ERR("Modifier method with specified name not found" << endl);
		return;
	}

	INFO(ModifierInfos::fGetHelp(iModifier, iOT));
}

void Manipulator::fModify(ObjType iOT, string iModifier, int iObjIndex, vector<string> iArgs) {
	Info *lInfo = fGetInfo(iOT, iObjIndex);
	if (lInfo == NULL) {
		ERR(gObjTypeStringS[iOT] << " with specified index not found" << endl);
		return;
	}

	if (fRestructedCheck(iOT, lInfo) == false) {
		return;
	}

	if (ModifierInfos::fExists(iModifier, iOT) == false) {
		ERR("Modifier method with specified name not found" << endl);
		return;
	}

	srand(time(NULL));

	INFO("Going to run modifier \"" << iModifier << "\" on " << gObjTypeStrings[iOT] << " "
			"\"" << lInfo->cName << "\" (" << iObjIndex << ")" << endl);
	INFO("Arguments of the modifier are: " << fGetVectorString(iArgs) << endl);

	DEB(LVL_MANIPULATOR, "Manipulator::fModify: Going to modify" << endl);
	Modifier* lModifier = ModifierInfos::fCreateModifier(iModifier, iOT, iArgs);
	if (lModifier == NULL) {
		ERR("Modifier could not be created" << endl);
		return;
	}
	Ret lRet = lModifier->fModify(lInfo->cObject);
	if (lRet != OK) {
		ERR("Modifying failed" << endl);
		return;
	}
	DEB(LVL_MANIPULATOR, "Manipulator::fModify: Modifying OK" << endl);

	INFO("Modifying was successful. Here are the results:" << endl);
	INFO(endl);
	IMPFO(RINFO_MODIFYING << endl);
	IMPFO(RINFO_MODIFIER << ": " << iModifier << endl);
	IMPFO(RINFO_ARGS << ": " << fGetVectorString(iArgs) << endl);
	IMPFO(RINFO_OTYPE << ": " << gObjTypeStr[iOT] << endl);
	IMPFO(RINFO_OINDEX << ": " << iObjIndex << endl);
	IMPFO(RINFO_ONAME << ": " << lInfo->cName << endl);
	if (gOptions->cTiming == TimOn) {IMPFO(RINFO_DUR << ": " << gAlgTimer->fGetResultString());}
	INFO(endl);
}

/*****************************************************************/
/* Postmans
/*****************************************************************/

void Manipulator::fListPostmanMethods() {
	IMPFO(RINFO_METHODS << endl);
	IMPFO("Available POSTMAN Methods:" << endl <<
		fPadString("Name", LS_PADWIDTH) << fPadString("Meant for", LS_PADWIDTH) <<
		fPadString("Generates", LS_PADWIDTH) << endl);
	vector<string> lPostmanMethods = PostmanInfos::fGetNames();
	vector<string> lPostmanGenerates = PostmanInfos::fGetGenerates();
	vector<set<ObjType> > lPostmanObjTypes = PostmanInfos::fGetMeantFors();
	for (vector<string>::size_type i = 0; i < lPostmanMethods.size(); i++) {
		string lName = lPostmanMethods[i];
		ObjType lOT = *(lPostmanObjTypes[i].begin());
		string lMeantFor = "";
		for (set<ObjType>::iterator j = lPostmanObjTypes[i].begin(); j != lPostmanObjTypes[i].end(); j++) {
			lMeantFor += gObjTypeStr[*j] + " ";
		}
		string lGenerates = lPostmanGenerates[i];

		IMPFO(fPadString(lName, LS_PADWIDTH) << fPadString(lMeantFor, LS_PADWIDTH) <<
				fPadString(lGenerates, LS_PADWIDTH) << endl);
	}
}

void Manipulator::fPostmanHelp(ObjType iOT, string iPostman) {
	if (PostmanInfos::fExists(iPostman, iOT) == false) {
		ERR("Postman method with specified name not found" << endl);
		return;
	}

	INFO(PostmanInfos::fGetHelp(iPostman, iOT));
}

void Manipulator::fPostman(ObjType iOT, string iPostman, string iPostName, int iObjIndex,
		vector<string> iArgs) {
	Info *lInfo = fGetInfo(iOT, iObjIndex);
	if (lInfo == NULL) {
		ERR(gObjTypeStringS[iOT] << " with specified index not found" << endl);
		return;
	}

	if (fRestructedCheck(iOT, lInfo) == false) {
		return;
	}

	if (PostmanInfos::fExists(iPostman, iOT) == false) {
		ERR("Postman with specified name not found" << endl);
		return;
	}

	srand(time(NULL));

	INFO("Going to run postman \"" << iPostman << "\" on " << gObjTypeStrings[iOT] << " "
			"\"" << lInfo->cName << "\" (" << iObjIndex << ")" << endl);
	INFO("Arguments of the postman are: " << fGetVectorString(iArgs) << endl);

	DEB(LVL_MANIPULATOR, "Manipulator::fPostman: Going to post a mail " << iPostName << endl);
	Postman* lPostman = PostmanInfos::fCreatePostman(iPostman, iOT, iArgs);
	if (lPostman == NULL) {
		ERR("Postman could not be created" << endl);
		return;
	}
	PostMail lCreatedMail = lPostman->fPostMail(lInfo->cObject, iPostName);
	if (lCreatedMail.fGetData() == NULL) {
		ERR("Posting failed" << endl);
		return;
	}
	PostBox::fAddMail(iPostName, lCreatedMail);

	DEB(LVL_MANIPULATOR, "Manipulator::fPostman: Posting OK" << endl);

	INFO("Posting a mail was successful:" << endl);
	INFO(endl);
	IMPFO(RINFO_POSTBOX << endl);
	IMPFO(RINFO_PBPOSTMAN << ": " << iPostman << endl);
	IMPFO(RINFO_ARGS << ": " << fGetVectorString(iArgs) << endl);
	IMPFO(RINFO_PBTYPE << ": " << lPostman->fGenerates() << endl);
	IMPFO(RINFO_PBNAME << ": " << iPostName << endl);
	IMPFO(RINFO_PBDESC << ": " << lCreatedMail.fGetDesc() << endl);
	if (gOptions->cTiming == TimOn) {IMPFO(RINFO_DUR << ": " << gAlgTimer->fGetResultString());}
	INFO(endl);
}

void Manipulator::fRmPostMail(string iPostName) {
	Ret lRet = PostBox::fRmMail(iPostName);

	if (lRet != OK) {
		ERR("Removing post mail failed" << endl);
		return;
	}

	INFO(endl);
	IMPFO(RINFO_RMPOST << endl);
	IMPFO(RINFO_PBNAME << ": " << iPostName << endl);
	INFO(endl);
}

void Manipulator::fShowPostBox() {
	INFO(endl);
	IMPFO("Showing MAILS: " << endl);
	IMPFO(PostBox::fGetMailsString());
	INFO(endl);
}

void Manipulator::fSaveAnSet(string iAnSet, string iFileName) {
	PostMail *lMail = PostBox::fGetMail(iAnSet, PB_TYPE_VECT_STRING);

	if (lMail == NULL) {
		ERR("Post mail " << iAnSet << " of type " << PB_TYPE_VECT_STRING << " not found" << endl);
		return;
	}

	ofstream lFile;
	lFile.open(iFileName.c_str());

	if (!lFile) {
		ERR("Failed to open file " << iFileName << endl);
		return;
	}

	vector<string> lSet = *((vector<string>*)lMail->fGetData());
	for (size_t i = 0; i < lSet.size(); i++) {
		lFile << lSet[i] << endl;
	}

	lFile.close();

	INFO(endl);
	IMPFO(RINFO_SAVEPM << endl);
	IMPFO("Filename: " << iFileName << endl);
	IMPFO(RINFO_PBNAME << ": " << iAnSet << endl)
	IMPFO(RINFO_PBDESC << ": " << lMail->fGetDesc() << endl);
	INFO(endl);
}

void Manipulator::fLoadAnSet(string iAnSet, string iFileName) {
	ifstream lFile;
	lFile.open(iFileName.c_str());

	if (!lFile) {
		ERR("Failed to open file " << iFileName << endl);
		return;
	}

	vector<string> *lSet = new vector<string>();

	while (lFile) {
		string lLine;
		getline(lFile, lLine);
		if (lLine == "") {
			break;
		}
		lSet->push_back(lLine);
	}
	lFile.close();

	PostMail lMail(lSet, "Loaded set (" + iFileName + ": " + fGetVectorString(*lSet, "\n	"),
			PB_TYPE_VECT_STRING);
	PostBox::fAddMail(iAnSet, lMail);

	INFO(endl);
	IMPFO(RINFO_LOADPM << endl);
	IMPFO("Filename: " << iFileName << endl);
	IMPFO(RINFO_PBNAME << ": " << iAnSet << endl)
	IMPFO(RINFO_PBDESC << ": " << lMail.fGetDesc() << endl);
	INFO(endl);
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

bool Manipulator::fRestructedCheck(ObjType iOT, Info *iInfo) {
	if (GraphObject::fIsGraphObject(iOT)) {
		if (dynamic_cast<GraphInfo*>(iInfo)->fGetGraphObj()->fIsRestructed() == false) {
			INFO(gObjTypeStringS[iOT] << " is not restructed." << endl);
			return false;
		}
	}

	return true;
}

Info* Manipulator::fGetInfo(ObjType iOT, int iIndex) {
	vector<Info*> lInfos = cInfos[iOT];

	if ((iIndex < 0 || iIndex >= cInfos[iOT].size())) {
		ERR("Manipulator::fGetInfo: Index out of bounds" << endl);
		return NULL;
	}

	return cInfos[iOT][iIndex];
}

string Manipulator::fGetTimeString(int iTime) {
	if (gOptions->cTimeFormat == TFOn) {
		return fMinutesToString(iTime);
	}

	return fIntToStr(iTime);
}

