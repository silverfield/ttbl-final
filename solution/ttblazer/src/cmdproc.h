/* Process commands sent by user */

#ifndef CMDPROC_H_
#define CMDPROC_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"

#include <queue>

/*****************************************************************/
/* Macros
/*****************************************************************/

#define GET_TIME MOVEARG \
	int lTime; \
	if (gOptions->cTimeFormat == TFOn) { \
		string lTimeString = ""; \
		lTimeString += lCurArg; MOVEARG; lTimeString += " " + lCurArg; \
		lTime = fStringToMinutes(lTimeString); \
	} \
	else { \
		lTime = fStrToInt(lCurArg); \
	} \

/*****************************************************************/
/* Constants
/*****************************************************************/

/* These are constants representing commands */

//----- General -----
#define CMD_ON "on"
#define CMD_OFF "off"
#define CMD_INDEX "-i"
#define CMD_LASTI -2
#define CMD_NAME "-n"
#define CMD_LASTN "last"

//----- Sub-helps -----
#define CMD_HELP "h"
#define CMD_H_ALL "all"
#define CMD_H_PROGCMDS "program"
#define CMD_H_LOGGING "logging"
#define CMD_H_ORACLES "ors"
#define CMD_H_ANYSERS "ans"
#define CMD_H_MODIFIERS "mods"
#define CMD_H_GENERS "gens"
#define CMD_H_MANIP "manip"
#define CMD_H_NOTES "notes"
#define CMD_H_PB "pb"

//----- Program commands -----
#define CMD_LSOPTS "options"
#define CMD_SOURCE "cmd-source"
#define CMD_SOURCE_STD "cmd"
#define CMD_SOURCE_QUE "queue"
#define CMD_TIMING "timing"
#define CMD_QUIT "quit"
#define CMD_SCRIPT "script"
#define CMD_TIME_FORMAT "tf"
#define CMD_OBJECT_INDEX "oi"
#define CMD_OBJECT_I_INDEX "index"
#define CMD_OBJECT_I_NAME "name"
#define CMD_TESTS "tests"

//----- Logging -----
#define CMD_DEB "deb"
#define CMD_LVLON "lvl-on"
#define CMD_LVLOFF "lvl-off"
#define CMD_DEB_ALL_OFF "deb-all-off"
#define CMD_LVLS "lvls"
#define CMD_CATLVLS "cat-lvls"

//----- Manipulation -----
#define CMD_LOAD "load"
#define CMD_SAVE "save"
#define CMD_RM "rm"
#define CMD_RENAME "mv"
#define CMD_LIST "ls"
#define CMD_LISTALL "lsal"
#define CMD_LIST_EVERYTHING "lsall"
#define CMD_SHOW "show"
#define CMD_SHOWALL "showal"
#define CMD_SHOW_EVERYTHING "showall"
#define CMD_WHOLE "whole"
#define CMD_SHNODE "sh-nd"
#define CMD_SHARC "sh-arc"
#define CMD_BUILDII "build-ii"

//----- Oracles -----
//do {ug...} {do-name} args
#define CMD_LS_ORACLES "ls-ors"
#define CMD_ORACLE_H "or-h"
#define CMD_ORACLE "or"
#define CMD_RMOR "rmor"
//sp {ug|te|td} {index} {do-index} {from} {to}
#define CMD_SP gQueryTypeStrSmall[QTSp]
#define CMD_DIST gQueryTypeStrSmall[QTDist]
//ea {te|td|tt} {index} {do-index} {from} {at} {to}
#define CMD_EA gQueryTypeStrSmall[QTEa]
#define CMD_CONN gQueryTypeStrSmall[QTConn]
//compor {ug|te|td|tt} {index} {query-type} {#of test-cases} {oracle-indices}
#define CMD_COMP "orcomp"

//----- Analysers -----
#define CMD_LS_ANYSERS "ls-ans"
#define CMD_ANYSER_H "an-h"
#define CMD_ANYSER "an"

//----- Modifiers -----
#define CMD_LS_MODIFIERS "ls-mods"
#define CMD_MODIFIER_H "mod-h"
#define CMD_MODIFIER "mod"

//----- Generators -----
#define CMD_LS_GENERS "ls-gens"
#define CMD_GENER_H "gen-h"
#define CMD_GENER "gen"

//----- Postman -----
#define CMD_LS_PB "ls-pbs"
#define CMD_RM_PB "rm-pb"
#define CMD_SHOW_PB "show-pb"
#define CMD_PB_H "pb-h"
#define CMD_PB "pb"
#define CMD_PB_SAVE "save-pb"
#define CMD_PB_LOAD "load-pb"

/*****************************************************************/
/* Queue
/*****************************************************************/

/* This is a command queue. If the command source of the program is set to
 * queue, every time program is idle, it looks for next command in this queue.
 * Commands can be put to the queue through port on which communicator listens. */
class CmdQueue {
private:
	std::list<std::string> cQueue;
public:
	inline void fAdd(std::string iCommand) {cQueue.push_back(iCommand);};
	inline std::string fPeek() {return cQueue.front();};
	inline void fPop() {return cQueue.pop_front();};
	inline bool fIsEmpty() {return cQueue.empty();};

	void fAddHere(std::vector<std::string> iCommands);
};

/*****************************************************************/
/* Variables
/*****************************************************************/

extern CmdQueue gCmdQueue;

/*****************************************************************/
/* Functions - interface
/*****************************************************************/

//----- From commands -----
void fCatLvls();
void fPrintLvls();

//----- Queue -----
void fQueueAdd(std::string iCmd);
void fQueueAddHere(std::vector<std::string> iCommands);
std::string fQueueNext(); //does also pop
std::string fQueuePeek();
void fQueuePop();
bool fQueueEmpty();

//----- Processing commands -----
Ret fProcessCmd(std::string iCmd);
void fProcessCommands();
void fPrintHelp();
void fPrintHelpAll();
void fPrintHelpProgramCommands();
void fPrintHelpLogging();
void fPrintHelpManipulation();
void fPrintHelpOracles();
void fPrintHelpAnalysers();
void fPrintHelpModifiers();
void fPrintHelpGenerators();
void fPrintHelpPostmans();
void fPrintHelpNotes();

/*****************************************************************/
/* Functions - implementation
/*****************************************************************/

/* used with printing of help */
std::string fObjTypeArgString();

#endif /* CMDPROC_H_ */
