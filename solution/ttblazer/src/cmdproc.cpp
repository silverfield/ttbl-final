/*****************************************************************/
/* Includes
/*****************************************************************/

#include <queue>
#include <pthread.h>
#include <fstream>
#include <iostream>

#include "central.h"
#include "manipulator.h"
#include "main.h"
#include "tests.h"
#include "postbox/postbox.h"

#include "cmdproc.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

CmdQueue gCmdQueue;
pthread_mutex_t gCmdQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gQueueEmptyCond;

/*****************************************************************/
/* CmdQueue
/*****************************************************************/

void CmdQueue::fAddHere(vector<string> iCommands) {
	for (vector<string>::size_type i = iCommands.size(); i > 0; i--) {
		cQueue.push_front(iCommands[i - 1]);
	}
}

/*****************************************************************/
/* Functions - From commands
/*****************************************************************/

void fCatLvls() {
	ifstream lFile;

	//COMMON
	char lFileCommon [] = "common/src/commonlvls.h";
	lFile.open(lFileCommon);

	if (!lFile) {
		ERR("Cmdproc::fCatLvls: failed to open file " << lFileCommon << endl);
		return;
	}

	INFO("LVL MEANINGS ***************************" << endl);

	string lLine = "";
	while (lFile && lLine != "//CAT_HERE_FROM") {
		getline(lFile, lLine);
	}
	getline(lFile, lLine);
	getline(lFile, lLine);
	while (lFile && lLine != "//CAT_HERE_TO") {
		INFO(lLine << endl);
		getline(lFile, lLine);
	}

	lFile.close();

	//TTBLAZER
	char lFileTtblazer [] = "ttblazer/src/ttblazerlvls.h";
	lFile.open(lFileTtblazer);

	if (!lFile) {
		ERR("Cmdproc::fCatLvls: failed to open file " << lFileTtblazer << endl);
		return;
	}

	lLine = "";
	while (lFile && lLine != "//CAT_HERE_FROM") {
		getline(lFile, lLine);
	}
	getline(lFile, lLine);
	getline(lFile, lLine);
	while (lFile && lLine != "//CAT_HERE_TO") {
		INFO(lLine << endl);
		getline(lFile, lLine);
	}

	lFile.close();
}

void fPrintLvls() {
	vector<int> lLevels = gLogger->fGetLevels();

	INFO("DEBUG LEVELS ***********************" << endl);
	for (vector<int>::size_type i = 0; i < lLevels.size(); i++) {
		INFO(lLevels[i] << " ");
	}

	INFO(endl);
}

/*****************************************************************/
/* Functions - Queue
/*****************************************************************/

void fQueueAddHere(string iCmd) {
	pthread_mutex_lock(&gCmdQueueMutex);

	gCmdQueue.fAdd(iCmd);
	DEB(LVL_CMDPROC, "CmdProc::fAddCmd: added new command to queue: \"" << iCmd << "\"" << endl);

	pthread_cond_broadcast(&gQueueEmptyCond);

	pthread_mutex_unlock(&gCmdQueueMutex);
}

void fQueueAdd(string iCmd) {
	pthread_mutex_lock(&gCmdQueueMutex);

	gCmdQueue.fAdd(iCmd);
	DEB(LVL_CMDPROC, "CmdProc::fAddCmd: added new command to queue: \"" << iCmd << "\"" << endl);

	pthread_cond_broadcast(&gQueueEmptyCond);

	pthread_mutex_unlock(&gCmdQueueMutex);
}

void fQueueAddHere(vector<string> iCommands) {
	pthread_mutex_lock(&gCmdQueueMutex);

	gCmdQueue.fAddHere(iCommands);
	DEB(LVL_CMDPROC, "CmdProc::fQueueAddHere: added new commands to queue's front: \""
			<< fGetVectorString(iCommands) << "\"" << endl);

	pthread_cond_broadcast(&gQueueEmptyCond);

	pthread_mutex_unlock(&gCmdQueueMutex);
}

string fQueueNext() {
	pthread_mutex_lock(&gCmdQueueMutex);

	if (gCmdQueue.fIsEmpty()) {
		pthread_mutex_unlock(&gCmdQueueMutex);
		return "";
	}

	string lReturnString = gCmdQueue.fPeek();
	gCmdQueue.fPop();

	pthread_mutex_unlock(&gCmdQueueMutex);

	INFO("Command obtained: " << lReturnString << endl);

	return lReturnString;
}

string fQueuePeek() {
	pthread_mutex_lock(&gCmdQueueMutex);

	string lReturnString = gCmdQueue.fPeek();

	pthread_mutex_unlock(&gCmdQueueMutex);

	return lReturnString;
}

void fQueuePop() {
	pthread_mutex_lock(&gCmdQueueMutex);

	gCmdQueue.fPop();

	pthread_mutex_unlock(&gCmdQueueMutex);
}

bool fQueueEmpty() {
	pthread_mutex_lock(&gCmdQueueMutex);

	bool lResult = gCmdQueue.fIsEmpty();

	pthread_mutex_unlock(&gCmdQueueMutex);

	return lResult;
}

/*****************************************************************/
/* Functions - Processing commands
/*****************************************************************/

#define COMP(x) if (lItems[lPos] == x)

#define UNKNOWN ERR("Cmdproc::fProcessCmd: Unknown command" << endl); \
	fPrintHelpAll();\
	return NOK;

#define MOVEARG lPos++; \
	if (lPos > lNoItems - 1) { \
		ERR("Cmdproc::fProcessCmd: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl); \
		fPrintHelp();\
		return NOK; \
	} \
	lCurArg = lItems[lPos];

#define GET_GRAPH_INDICES(iCount, iOT) MOVEARG \
	if (GraphObject::fIsGraphObject(iOT) == false) { \
		UNKNOWN; \
	} \
	bool lIndexing = false; \
	COMP(CMD_INDEX) { \
		lIndexing = true; \
	} \
	else COMP(CMD_NAME) { \
		lIndexing = false; \
	} \
	else { \
		UNKNOWN; \
	} \
	\
	vector<int> lIndices; \
	for (int k = 0; k < iCount; k++) { \
		MOVEARG \
		int lIndex = -1; \
		if (lIndexing) { \
			lIndex = fStrToInt(lCurArg); \
		} \
		else {\
			string lId = ""; \
			if (TimetableObject::fIsTimetableObject(iOT)) { \
				if (gOptions->cTimeFormat == TFOn) { \
					lId += lCurArg; MOVEARG; lId += " " + lCurArg; MOVEARG; lId += " " + lCurArg; \
				} \
				else { \
					lId += lCurArg; MOVEARG; lId += " " + lCurArg; \
				} \
			} \
			else { \
				lId = lCurArg; \
			} \
			lIndex = Manipulator::fIdToIndex(iOT, lObjIndex, lId); \
		} \
		lIndices.push_back(lIndex); \
	}

#define GET_OT MOVEARG \
	ObjType lOT = OTErr; \
	if (lCurArg == gObjTypeCmdStr[OTUg]) { \
		lOT = OTUg; \
	} \
	else if (lCurArg == gObjTypeCmdStr[OTTe]) { \
		lOT = OTTe; \
	} \
	else if (lCurArg == gObjTypeCmdStr[OTTd]) { \
		lOT = OTTd; \
	} \
	else if (lCurArg == gObjTypeCmdStr[OTTt]) { \
		lOT = OTTt; \
	} \
	if (lOT == OTErr) { \
		UNKNOWN; \
	}

#define OBJ_INDEX MOVEARG \
	int lObjIndex = -1; \
	if (gOptions->cObjIndexing == OIIndex) { \
		lObjIndex = fStrToInt(lCurArg); \
		lObjIndex = Manipulator::fTransformIndex(lOT, lObjIndex); \
	} \
	else { \
		if (lCurArg == CMD_LASTN) { \
			lObjIndex = Manipulator::fTransformIndex(lOT, CMD_LASTI); \
		} \
		else { \
			lObjIndex = Manipulator::fGetInfoIndex(lOT, lCurArg); \
		} \
	}

#define OR_INDEX MOVEARG \
	int lDoIndex = -1; \
	if (gOptions->cObjIndexing == OIIndex) { \
		lDoIndex = fStrToInt(lCurArg); \
	} \
	else { \
		lDoIndex = Manipulator::fGetOracleIndex(lOT, lObjIndex, lCurArg); \
	}

Ret fProcessCmd(string iCmd) {
	vector<string> lItems = fSplitString(iCmd, " ");
	int lNoItems = lItems.size();
	DEB(LVL_CMDPROC, "CmdProc::fProcessCmd: Command vector:" << fGetVectorString<string>(lItems) << endl);
	int lPos = -1;
	string lCurArg = "";
	MOVEARG;

	//----- Sub-helps -----
	COMP(CMD_HELP) {
		if (lNoItems == 1) {
			fPrintHelp();
		}
		else {
			MOVEARG;
			COMP(CMD_H_ALL) {
				fPrintHelpAll();
			}
			else COMP(CMD_H_LOGGING) {
				fPrintHelpLogging();
			}
			else COMP(CMD_H_PROGCMDS) {
				fPrintHelpProgramCommands();
			}
			else COMP(CMD_H_MANIP) {
				fPrintHelpManipulation();
			}
			else COMP(CMD_H_ORACLES) {
				fPrintHelpOracles();
			}
			else COMP(CMD_H_ANYSERS) {
				fPrintHelpAnalysers();
			}
			else COMP(CMD_H_MODIFIERS) {
				fPrintHelpModifiers();
			}
			else COMP(CMD_H_GENERS) {
				fPrintHelpGenerators();
			}
			else COMP(CMD_H_PB) {
				fPrintHelpPostmans();
			}
			else COMP(CMD_H_NOTES) {
				fPrintHelpNotes();
			}
			else {
				UNKNOWN;
			}
		}
	}

	//----- Program commands -----
	else COMP(CMD_LSOPTS) {
		INFO(gOptions->fGetOptionsString());
	}
	else COMP(CMD_SOURCE) {
		MOVEARG;
		COMP(CMD_SOURCE_STD) {
			gOptions->cCmdSource = CSStdIn;
			INFO("Setting the command source to " << gCmdSourceStr[CSStdIn] << endl);
		}
		else COMP(CMD_SOURCE_QUE) {
			gOptions->cCmdSource = CSQueue;
			INFO("Setting the command source to " << gCmdSourceStr[CSQueue] << endl);
		}
		else {
			UNKNOWN;
		}
	}
	else COMP(CMD_TIMING) {
		MOVEARG;
		COMP(CMD_ON) {
			gOptions->cTiming = TimOn;
			INFO("Setting the timing " << gTimingStr[TimOn] << endl);
		}
		else COMP(CMD_OFF) {
			gOptions->cTiming = TimOff;
			INFO("Setting the timing " << gTimingStr[TimOff] << endl);
		}
		else {
			UNKNOWN;
		}
	}
	else COMP(CMD_QUIT) {
		fExitProgram();
	}
	else COMP(CMD_SCRIPT) {
		MOVEARG;
		string lFileName = lCurArg;
		Manipulator::fProcessScript(lFileName.c_str());
	}
	else COMP(CMD_TIME_FORMAT) {
		MOVEARG;
		COMP(CMD_ON) {
			gOptions->cTimeFormat = TFOn;
			INFO("Setting the time format " << gTimeFormatStr[TFOn] << endl);
			Manipulator::fChangeTimeFormat(TFOn);
		}
		else COMP(CMD_OFF) {
			gOptions->cTimeFormat = TFOff;
			INFO("Setting the time format " << gTimeFormatStr[TFOff] << endl);
			Manipulator::fChangeTimeFormat(TFOff);
		}
		else {
			UNKNOWN;
		}
	}
	else COMP(CMD_OBJECT_INDEX) {
		MOVEARG;
		COMP(CMD_OBJECT_I_INDEX) {
			gOptions->cObjIndexing = OIIndex;
			INFO("Setting the object indexing to " << gObjIndexingStr[OIIndex] << endl);
		}
		else COMP(CMD_OBJECT_I_NAME) {
			gOptions->cObjIndexing = OIName;
			INFO("Setting the object indexing to " << gObjIndexingStr[OIName] << endl);
		}
		else {
			UNKNOWN;
		}
	}
	else COMP(CMD_TESTS) {
		fRunTests();
	}

	//----- Logging -----
	else COMP(CMD_DEB) {
		MOVEARG;
		COMP(CMD_ON) {
			gLogger->fDebOn(true);
			INFO("Debugging ON" << endl);
		}
		else COMP(CMD_OFF) {
			gLogger->fDebOn(false);
			INFO("Debugging OFF" << endl);
		}
		else {
			UNKNOWN;
		}
	}
	else COMP(CMD_LVLON) {
		MOVEARG;
		int lLvl = fStrToInt(lCurArg);
		gLogger->fSetLvl(lLvl);
		INFO("Debug level " << lLvl << " ON" << endl);
	}
	else COMP(CMD_LVLOFF) {
		MOVEARG;
		int lLvl = fStrToInt(lCurArg);
		gLogger->fUnsetLvl(lLvl);
		INFO("Debug level " << lLvl << " OFF" << endl);
	}
	else COMP(CMD_DEB_ALL_OFF) {
		gLogger->fUnsetAll();
		INFO("All debug levels OFF" << endl);
	}
	else COMP(CMD_CATLVLS) {
		fCatLvls();
	}
	else COMP(CMD_LVLS) {
		fPrintLvls();
	}

	//----- Manipulation -----
	else COMP(CMD_LOAD) {
		GET_OT;
		MOVEARG;
		string lFName = lCurArg;
		Manipulator::fLoad(lOT, lCurArg.c_str());
	}
	else COMP(CMD_SAVE) {
		GET_OT;
		OBJ_INDEX;
		MOVEARG;
		string lFName = lCurArg;
		Manipulator::fSave(lOT, lObjIndex, lCurArg.c_str());
	}
	else COMP(CMD_RM) {
		GET_OT;
		OBJ_INDEX
		Manipulator::fRemove(lOT, lObjIndex);
	}
	else COMP(CMD_RENAME) {
		GET_OT;
		OBJ_INDEX
		MOVEARG;
		string lNewName = lCurArg;
		Manipulator::fRename(lOT, lObjIndex, lNewName);
	}
	else COMP(CMD_LIST) {
		GET_OT;
		OBJ_INDEX
		Manipulator::fListObject(lOT, lObjIndex);
	}
	else COMP(CMD_LISTALL) {
		GET_OT;
		Manipulator::fListAll(lOT);
	}
	else COMP(CMD_LIST_EVERYTHING) {
		Manipulator::fListAll();
	}
	else COMP(CMD_SHOW) {
		GET_OT;
		OBJ_INDEX
		Manipulator::fShow(lOT, lObjIndex, false);
	}
	else COMP(CMD_SHOWALL) {
		GET_OT;
		Manipulator::fShowAll(lOT, false);
	}
	else COMP(CMD_SHOW_EVERYTHING) {
		Manipulator::fShowAll(false);
	}
	else COMP(CMD_WHOLE) {
		GET_OT;
		OBJ_INDEX;
		Manipulator::fShow(lOT, lObjIndex, true);
	}
	else COMP(CMD_SHNODE) {
		GET_OT;
		OBJ_INDEX;
		GET_GRAPH_INDICES(1, lOT);
		Manipulator::fShowNode(lOT, lObjIndex, lIndices[0]);
	}
	else COMP(CMD_SHARC) {
		GET_OT;
		OBJ_INDEX;
		GET_GRAPH_INDICES(2, lOT);
		Manipulator::fShowArc(lOT, lObjIndex, lIndices[0], lIndices[1]);
	}
	else COMP(CMD_BUILDII) {
		GET_OT;
		OBJ_INDEX;
		Manipulator::fBuildIdIndex(lOT, lObjIndex);
	}

	//----- Oracles -----
	else COMP(CMD_LS_ORACLES) {
		Manipulator::fListDoMethods();
	}
	else COMP(CMD_ORACLE_H) {
		GET_OT;
		MOVEARG;
		string lDoName = lCurArg;
		Manipulator::fOracleHelp(lOT, lCurArg);
	}
	else COMP(CMD_ORACLE) {
		GET_OT;
		MOVEARG;
		string lDoName = lCurArg;
		OBJ_INDEX;
		vector<string> lArgs = vector<string>(lItems.begin() + lPos + 1, lItems.end());
		Manipulator::fCreateOracle(lOT, lDoName, lObjIndex, lArgs);
	}
	else COMP(CMD_RMOR) {
		GET_OT;
		OBJ_INDEX;
		OR_INDEX;
		vector<string> lArgs = vector<string>(lItems.begin() + lPos + 1, lItems.end());
		Manipulator::fRemoveOracle(lOT, lDoIndex, lObjIndex, lArgs);
	}
	else COMP(CMD_SP) {
		GET_OT;
		OBJ_INDEX;
		OR_INDEX;
		GET_GRAPH_INDICES(2, lOT);
		Manipulator::fSp(lOT, lObjIndex, lDoIndex, lIndices[0], lIndices[1]);
	}
	else COMP(CMD_DIST) {
		GET_OT;
		OBJ_INDEX;
		OR_INDEX;
		GET_GRAPH_INDICES(2, lOT);
		Manipulator::fDist(lOT, lObjIndex, lDoIndex, lIndices[0], lIndices[1]);
	}
	else COMP(CMD_CONN) {
		GET_OT;
		OBJ_INDEX;
		OR_INDEX;
		MOVEARG;
		string lFrom = lCurArg;
		GET_TIME;
		MOVEARG;
		string lTo = lCurArg;
		Manipulator::fConn(lOT, lObjIndex, lDoIndex, lFrom, lTime, lTo);
	}
	else COMP(CMD_EA) {
		GET_OT;
		OBJ_INDEX;
		OR_INDEX;
		MOVEARG;
		string lFrom = lCurArg;
		GET_TIME;
		MOVEARG;
		string lTo = lCurArg;
		Manipulator::fEa(lOT, lObjIndex, lDoIndex, lFrom, lTime, lTo);
	}
	else COMP(CMD_COMP) {
		GET_OT;
		OBJ_INDEX;
		QueryType lQueryType = QTSp;
		MOVEARG;
		if (lCurArg == gQueryTypeStrSmall[QTSp]) {
			lQueryType = QTSp;
		}
		if (lCurArg == gQueryTypeStrSmall[QTDist]) {
			lQueryType = QTDist;
		}
		if (lCurArg == gQueryTypeStrSmall[QTEa]) {
			lQueryType = QTEa;
		}
		if (lCurArg == gQueryTypeStrSmall[QTConn]) {
			lQueryType = QTConn;
		}
		MOVEARG;
		int lTestCases = fStrToInt(lCurArg);
		vector<int> lOracles;
		while (lPos < lItems.size() - 1) {
			OR_INDEX;
			lOracles.push_back(lDoIndex);
		}
		if (lOracles.size() == 0) {
			ERR("At least one oracle must be set for comparison" << endl);
			UNKNOWN;
		}
		Manipulator::fCompareOracles(lOT, lObjIndex, lQueryType, lTestCases, 0, lOracles);
	}

	//----- Analysers -----
	else COMP(CMD_LS_ANYSERS) {
		Manipulator::fListAnalyseMethods();
	}
	else COMP(CMD_ANYSER_H) {
		GET_OT;
		MOVEARG;
		string lAnyserName = lCurArg;
		Manipulator::fAnalyserHelp(lOT, lAnyserName);
	}
	else COMP(CMD_ANYSER) {
		GET_OT;
		MOVEARG;
		string lAnName = lCurArg;
		OBJ_INDEX;
		vector<string> lArgs = vector<string>(lItems.begin() + lPos + 1, lItems.end());
		Manipulator::fAnalyse(lOT, lAnName, lObjIndex, lArgs);
	}

	//----- Generators -----
	else COMP(CMD_LS_GENERS) {
		Manipulator::fListGeneratorMethods();
	}
	else COMP(CMD_GENER_H) {
		GET_OT;
		MOVEARG;
		string lGenerName = lCurArg;
		Manipulator::fGeneratorHelp(lOT, lGenerName);
	}
	else COMP(CMD_GENER) {
		GET_OT;
		MOVEARG;
		string lGenName = lCurArg;
		OBJ_INDEX;
		vector<string> lArgs = vector<string>(lItems.begin() + lPos + 1, lItems.end());
		Manipulator::fGenerate(lOT, lGenName, lObjIndex, lArgs);
	}

	//----- Modifiers -----
	else COMP(CMD_LS_MODIFIERS) {
		Manipulator::fListModifierMethods();
	}
	else COMP(CMD_MODIFIER_H) {
		GET_OT;
		MOVEARG;
		string lModifierName = lCurArg;
		Manipulator::fModifierHelp(lOT, lModifierName);
	}
	else COMP(CMD_MODIFIER) {
		GET_OT;
		MOVEARG;
		string lModifierName = lCurArg;
		OBJ_INDEX;
		vector<string> lArgs = vector<string>(lItems.begin() + lPos + 1, lItems.end());
		Manipulator::fModify(lOT, lModifierName, lObjIndex, lArgs);
	}

	//----- Postbox -----
	else COMP(CMD_LS_PB) {
		Manipulator::fListPostmanMethods();
	}
	else COMP(CMD_SHOW_PB) {
		Manipulator::fShowPostBox();
	}
	else COMP(CMD_RM_PB) {
		MOVEARG;
		string lPmName = lCurArg;
		Manipulator::fRmPostMail(lPmName);
	}
	else COMP(CMD_PB_H) {
		GET_OT;
		MOVEARG;
		string lPmName = lCurArg;
		Manipulator::fPostmanHelp(lOT, lPmName);
	}
	else COMP(CMD_PB) {
		GET_OT;
		MOVEARG;
		string lPmName = lCurArg;
		OBJ_INDEX;
		MOVEARG
		string lMailName = lCurArg;
		vector<string> lArgs = vector<string>(lItems.begin() + lPos + 1, lItems.end());
		Manipulator::fPostman(lOT, lPmName, lMailName, lObjIndex, lArgs);
	}
	else COMP(CMD_PB_SAVE) {
		MOVEARG;
		string lMailName = lCurArg;
		MOVEARG
		string lFileName = lCurArg;
		Manipulator::fSaveAnSet(lMailName, lFileName);
	}
	else COMP(CMD_PB_LOAD) {
		MOVEARG;
		string lMailName = lCurArg;
		MOVEARG
		string lFileName = lCurArg;
		Manipulator::fLoadAnSet(lMailName, lFileName);
	}

	//----- Unknown -----
	else {
		UNKNOWN;
	}

	return OK;
}

void fProcessCommands() {
	while (1) {
		string lCommand;

		//get command
		if (gOptions->cCmdSource == CSStdIn) {
			INFO("Type in the next command:" << endl);

			getline(cin, lCommand);

			INFO("Command obtained: " << lCommand << endl);
		}
		else if (gOptions->cCmdSource == CSQueue) {
			lCommand = fQueueNext();

			if (lCommand == "") {
				INFO("Waiting for a command to arrive to queue" << endl);

				pthread_mutex_lock(&gCmdQueueMutex);
				pthread_cond_wait(&gQueueEmptyCond, &gCmdQueueMutex);
				pthread_mutex_unlock(&gCmdQueueMutex);

				lCommand = fQueueNext();
			}
		}

		//process command
		fProcessCmd(lCommand);
	}
}

void fPrintHelp() {
	INFO(
			"**************** TTBlazer HELP *****************" << endl <<
			"	" << CMD_HELP << ": print this help" << endl <<
			"	" << CMD_HELP << " [" << CMD_H_ALL << "|"
									 << CMD_H_MANIP << "|"
									 << CMD_H_PROGCMDS << "|"
									 << CMD_H_LOGGING << "|"
									 << CMD_H_ORACLES << "|"
									 << CMD_H_ANYSERS << "|"
									 << CMD_H_MODIFIERS << "|"
									 << CMD_H_GENERS << "|"
									 << CMD_H_PB << "|"
									 << CMD_H_NOTES << "]: shows specified part of the help" << endl <<
	endl);
}

void fPrintHelpNotes() {
	INFO(
			"**************** TTBlazer HELP NOTES *****************" << endl <<
			"- A node can be specified by its index (option " << CMD_INDEX << "; its a unique number) "
			"or ID (option " << CMD_NAME << "; can be string, integer...). In order to specify "
			"nodes by ID, ID->Index map must be built first." << endl <<
			"- Objects are referenced by index (e.g. ug 3 means 3rd underlying graph) or by name. "
			"A special index is " << CMD_LASTI << " which means the last loaded object of the given type. "
			"Similarly, special name is \"" << CMD_LASTN << "\" which means the same." << endl <<
	endl);
}

void fPrintHelpAll() {
	fPrintHelp();
	fPrintHelpProgramCommands();
	fPrintHelpLogging();
	fPrintHelpManipulation();
	fPrintHelpOracles();
	fPrintHelpAnalysers();
	fPrintHelpGenerators();
	fPrintHelpModifiers();
	fPrintHelpPostmans();
}

void fPrintHelpProgramCommands() {
	INFO(
			"***** Program Commands *****" << endl <<
			"	" << CMD_LSOPTS << ": gets current options of the program" << endl <<
			"	" << CMD_SOURCE << " {" << CMD_SOURCE_QUE << "|" << CMD_SOURCE_STD << "}: "
					"setting the command source: " << endl <<
			"	 - " << CMD_SOURCE_QUE << ": commands from command queue, commands could be sent "
					"to the program" << endl <<
			"	 - " << CMD_SOURCE_STD << ": commands are provided from standard input" << endl <<
			"	" << CMD_TIMING << " {" << CMD_ON << "|" << CMD_OFF << "}: "
					"specifies if the timing of algorithms should be on or off" << endl <<
			"	" << CMD_QUIT << ": quits the application" << endl <<
			"	" << CMD_SCRIPT << " {filename}: puts the commands from the script into the command"
					"queue" << endl <<
			"	" << CMD_TIME_FORMAT << " {" << CMD_ON << "|" << CMD_OFF << "}: specifies the time"
					"format in which the program operates. There are two options:" << endl <<
			"	 - " << CMD_ON << ": time is expressed as DAYS HH:MM" << endl <<
			"	 - " << CMD_OFF << ": time is expressed in minutes" << endl <<
			"		The time format influences how timetables are saved/loaded - the format in the file"
				"must agree with the current program format." << endl <<
			"	" << CMD_OBJECT_INDEX << " {" << CMD_OBJECT_I_INDEX << "|" << CMD_OBJECT_I_NAME << "}: "
					"specifies the indexing of objects (and oracles) - by index or by name" << endl <<
			"	" << CMD_TESTS << ": runs tests" << endl <<
	endl);
}

void fPrintHelpLogging() {
	INFO(
			"***** Logging *****" << endl <<
			"	" << CMD_DEB << " {" << CMD_ON << "|" << CMD_OFF << "}: turns debugging on or off"
				<< endl <<
			"	" << CMD_LVLON << " {lvl number}: turns specific debugging level on" << endl <<
			"	" << CMD_LVLOFF << " {lvl number}: turns specific debugging level off" << endl <<
			"	" << CMD_DEB_ALL_OFF << ": turns all debugging levels off" << endl <<
			"	" << CMD_LVLS << ": shows debugging levels that are currently on" << endl <<
			"	" << CMD_CATLVLS << ": shows the meaning of the individual debugging levels" << endl <<
	endl);
}

void fPrintHelpManipulation() {
	INFO(
			"***** Manipulation *****" << endl <<
			"	" << CMD_LOAD << " " << fObjTypeArgString() <<
				" {filename}: loads object from file" << endl <<
			"	" << CMD_SAVE << " " << fObjTypeArgString() <<
				" {object} {filename}: saves given object to file" << endl <<
			"	" << CMD_RM << " " << fObjTypeArgString() <<
				" {object}: removes the object from program memory" << endl <<
			"	" << CMD_RENAME << " " << fObjTypeArgString() <<
				" {object} {new name}: renames the object" << endl <<
			"	" << CMD_LIST << " " << fObjTypeArgString() << " {object}: provides basic information"
					"about the given object" << endl <<
			"	" << CMD_LISTALL << " " << fObjTypeArgString() << ": lists all objects of the given"
					"type" << endl <<
			"	" << CMD_LIST_EVERYTHING ": lists all objects" << endl <<
			"	" << CMD_SHOW << " " << fObjTypeArgString() << " {object}: shows some main properties"
					"of the object" << endl <<
			"	" << CMD_SHOWALL << " " << fObjTypeArgString() << ": shows some main properties"
					"of the objects of given type" << endl <<
			"	" << CMD_SHOW_EVERYTHING << ": shows some main properties for each loaded object"
				<< endl <<
			"	" << CMD_WHOLE << " " << fObjTypeArgString() << " {object}: shows the whole object"
					" (e.g. all the nodes and arcs of the graph)" << endl <<
			"	" << CMD_SHNODE << " {" << gObjTypeCmdStr[OTUg] << "|" << gObjTypeCmdStr[OTTe] <<
				"|" << gObjTypeCmdStr[OTTd] << "}" << " {object} {" << CMD_INDEX << " node-index|"
				<< CMD_NAME << " node-id}: shows given node of the graph-like object" << endl <<
			"	" << CMD_SHARC << " {" << gObjTypeCmdStr[OTUg] << "|" << gObjTypeCmdStr[OTTe] <<
				"|" << gObjTypeCmdStr[OTTd] << "}" << " {object} {" << CMD_INDEX <<
				" from-index, to-index|" << CMD_NAME << " from-id, to-id}: shows given "
						"arc of the graph-like object" << endl <<
			"	" << CMD_BUILDII << " {" << gObjTypeCmdStr[OTUg] << "|" << gObjTypeCmdStr[OTTe] <<
				"|" << gObjTypeCmdStr[OTTd] << "}: builds ID->Index map on the given graph-like"
						"object. This can be used to address nodes by their ID, not just their "
						"index" << endl <<
	endl);
}

void fPrintHelpOracles() {
	INFO(
			"***** Oracles *****" << endl <<
			"	" << CMD_LS_ORACLES << ": lists available oracle methods" << endl <<
			"	" << CMD_ORACLE_H << " " << fObjTypeArgString() << " {oracle} " << ": invokes"
					"help for target oracle method" << endl <<
			"	" << CMD_ORACLE << " " << fObjTypeArgString() << " {oracle} " <<
				"{object} [arguments]: builds target oracle on specified object" << endl <<
			"	" << CMD_RMOR << " " << fObjTypeArgString() << " {object} {oracle}" <<
				": removes target oracle" << endl <<
			"	" << CMD_SP << " {" << gObjTypeCmdStr[OTUg] << "|" << gObjTypeCmdStr[OTTe] <<
				"|" << gObjTypeCmdStr[OTTd] << "} {object} {oracle} {" << CMD_INDEX <<
				" from-index, to-index|" << CMD_NAME << " from-id, to-id}: answers a shortest"
						" path query on specified graph-like object using given oracle" << endl <<
			"	" << CMD_DIST << " {" << gObjTypeCmdStr[OTUg] << "|" << gObjTypeCmdStr[OTTe] <<
				"|" << gObjTypeCmdStr[OTTd] << "} {object} {oracle} {" << CMD_INDEX <<
				" from-index, to-index|" << CMD_NAME << " from-id, to-id}: answers a distance"
						" query on specified graph-like object using given oracle" << endl <<
			"	" << CMD_CONN << " {" << gObjTypeCmdStr[OTTe] << "|" << gObjTypeCmdStr[OTTd] <<
				"|" << gObjTypeCmdStr[OTTt] << "} {object} {oracle} {from-id, from-time, " <<
				"to-id}: gets the optimal connections using given oracle for the specified EAP "
					"query" << endl <<
			"	" << CMD_EA << " {" << gObjTypeCmdStr[OTTe] << "|" << gObjTypeCmdStr[OTTd] <<
				"|" << gObjTypeCmdStr[OTTt] << "} {object} {oracle} {from-id, from-time, " <<
				"to-id}: gets just the value of earliest arrival for the specified EAP "
					"query" << endl <<
			"	" << CMD_COMP << " " << fObjTypeArgString() << " {object} {sp|dist|conn|ea} {# of test"
					"cases} {exist. ratio} {oracles}: does the comparison of oracles (in randomly "
					"generated queries of type shortest-path (sp), distance (dist), connections (conn), "
					"earliest arrival (ea)) on a given graph. First oracle is referential. Exist ratio "
					"specifies the percentage of the test cases that must have solution"
					<< endl <<
	endl);
}

void fPrintHelpAnalysers() {
	INFO(
			"***** Analysers *****" << endl <<
			"	" << CMD_LS_ANYSERS << ": lists available analysers" << endl <<
			"	" << CMD_ANYSER_H << " " << fObjTypeArgString() << " {analyser-name} " << ": invokes"
					"help for target analyser" << endl <<
			"	" << CMD_ANYSER << " " << fObjTypeArgString() << " {analyser-name} " <<
				"{object} [arguments]: runs given analyser on the given object" << endl <<
	endl);
}

void fPrintHelpModifiers() {
	INFO(
			"***** Modifiers *****" << endl <<
			"	" << CMD_LS_MODIFIERS << ": lists available modifiers" << endl <<
			"	" << CMD_MODIFIER_H << " " << fObjTypeArgString() << " {modifier-name} " << ": invokes"
					"help for target modifier" << endl <<
			"	" << CMD_MODIFIER << " " << fObjTypeArgString() << " {modifier-name} " <<
				"{object} [arguments]: modifies given object with the specified modifier" << endl <<
	endl);
}

void fPrintHelpGenerators() {
	INFO(
			"***** Generators *****" << endl <<
			"	" << CMD_LS_GENERS << ": lists available generators" << endl <<
			"	" << CMD_GENER_H << " " << fObjTypeArgString() << " {generator-name} " << ": invokes"
					"help for target generator" << endl <<
			"	" << CMD_GENER << " " << fObjTypeArgString() << " {generator-name} " <<
				"{object} [arguments]: runs specified generator on the given object" << endl <<
	endl);
}

void fPrintHelpPostmans() {
	INFO(
			"***** Postmans *****" << endl <<
			"	" << CMD_LS_PB << ": lists available postmans" << endl <<
			"	" << CMD_SHOW_PB << ": shows the content of the post box" << endl <<
			"	" << CMD_RM_PB << " {name}: removes the post mail from the post box" << endl <<
			"	" << CMD_PB_H << " " << fObjTypeArgString() << " {postman-name} " << ": invokes"
					"help for target postman" << endl <<
			"	" << CMD_PB << " " << fObjTypeArgString() << " {postman-name} " <<
				"{object} {mail-name} [arguments]: runs specified postman on the given object to "
				"produce mail with given name" << endl <<
			"	" << CMD_PB_SAVE << " " << " {post mail} {file name} " << "saves given post mail of"
					" type " << PB_TYPE_VECT_STRING << " to file specified" << endl <<
			"	" << CMD_PB_LOAD << " " << " {post mail} {file name} " << "loads given post mail of"
					" type " << PB_TYPE_VECT_STRING << " from file specified" << endl <<
	endl);
}

/*****************************************************************/
/* Implementation
/*****************************************************************/

string fObjTypeArgString() {
	stringstream lResult (stringstream::out);

	lResult << "{" << gObjTypeCmdStr[OTUg] << "|" <<
				gObjTypeCmdStr[OTTe] << "|" << gObjTypeCmdStr[OTTd] << "|" << gObjTypeCmdStr[OTTt] <<
				"}";

	return lResult.str();
}
