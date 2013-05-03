/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"
#include "manipulator.h"
#include "tests.h"

#include "cmdlnproc.h"
#include "cmdproc.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Functions
/*****************************************************************/

void fPrintCmdlnHelp() {
	INFO(
			"TTBlazer **************** CMD LINE HELP *****************" << endl <<
			"	" << CLN_HELP << ": shows this help" << endl <<
			"	" << CLN_SCRIPT << " {filename}: puts command from the file into the command queue" <<
				endl <<
			"	" << CLN_TESTS << ": runs tests specified in tests.cpp" << endl <<
			"	" << CLN_LVLON << " {lvl}: the specified debug level would be turned on" << endl <<
			"	" << CLN_LVLOFF << " {lvl}: the specified debug level would be turned off" << endl <<
			"	" << CLN_DEB << " {" << CLN_DEB_ON << "| " << CLN_DEB_OFF << "}: runs program"
					"with debugging on of off" << endl <<
			"	" << CLN_LOGFILE << " {filename}: logs output on the screen also to the specified file"
				<< endl <<
			"	" << CLN_LOGPORT << " {port}: sends output of the log on the specified port" << endl <<
			"	" << CLN_PERFORM << " {args}: puts all following commands (separated by " << CLN_PERF_SEP <<
					") into the command queue" << endl <<
			"	" << CLN_CMD_SRC << " {" << CMD_SOURCE_QUE << "|" << CMD_SOURCE_STD << "}: "
					"setting the command source." << endl <<
	endl);
}

#define COMP(x) if (lItems[lPos] == x)
#define UNKNOWN ERR("Cmdlnproc::fProcessCmdln: Unknown command" << endl); \
	fPrintCmdlnHelp(); \
	return NOK;
#define MOVEARG lPos++; \
	if (lPos > lItems.size() - 1) { \
		ERR("Cmdlnproc::fProcessCmdln: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl); \
		fPrintCmdlnHelp(); \
		return NOK; \
	} \
	lCurArg = lItems[lPos];
Ret fProcessCmdln(int argc, char *argv[]) {
	vector<string> lItems;
	for (int i = 1; i < argc; i++) {
		lItems.push_back(fCharArrayToString(argv[i]));
	}
	DEB(LVL_CMDPROC, "Cmdlnproc::fProcessCmdln: Command vector:"
			<< fGetVectorString<string>(lItems) << endl);
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)lItems.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(CLN_HELP) {
			fPrintCmdlnHelp();
			continue;
		}
		else COMP(CLN_SCRIPT) {
			MOVEARG;
			Manipulator::fProcessScript(lCurArg.c_str());
			continue;
		}
		else COMP(CLN_TESTS) {
			//tests
			Ret lRet = fRunTests();
			if (lRet != OK) {
				ERR("Cmdlnproc::fProcessCmdln: Tests were not OK" << endl);
			}
			continue;
		}
		else COMP(CLN_DEB) {
			MOVEARG;
			COMP(CLN_DEB_ON) {
				gLogger->fDebOn(true);
				INFO("Debugging ON" << endl);
			} else COMP(CLN_DEB_OFF) {
				gLogger->fDebOn(false);
				INFO("Debugging OFF" << endl);
			} else {
				UNKNOWN;
			}
			continue;
		}
		else COMP(CLN_LVLON) {
			MOVEARG;
			int lLvl = fStrToInt(lCurArg);
			gLogger->fSetLvl(lLvl);
			INFO("Debug level " << lLvl << " ON" << endl);
			continue;
		}
		else COMP(CLN_LVLOFF) {
			MOVEARG;
			int lLvl = fStrToInt(lCurArg);
			gLogger->fUnsetLvl(lLvl);
			INFO("Debug level " << lLvl << " OFF" << endl);
			continue;
		}
		else COMP(CLN_LOGFILE) {
			MOVEARG;
			string lFileName = lCurArg;
			ofstream *lFile = new ofstream();
			lFile->open(lFileName.c_str());
			if (lFile->good() == false) {
				ERR("Cmdlnproc::fProcessCmdln: Error opening file " << lFileName << endl);
			}
			gLogger->fStartWriteToOfStream(lFile, LOG_ALL_MASK);
			INFO("Logging to file " << lFileName << endl);
			continue;
		}
		else COMP(CLN_LOGPORT) {
			MOVEARG;
			int lPort = fStrToInt(lCurArg);
			gLogger->fStartSendOnPort(lPort, LOG_ALL_MASK);
			INFO("Logging to stream " << lPort << endl);
			continue;
		}
		else COMP(CLN_PERFORM) {
			while (lPos < lItems.size() - 1){
				MOVEARG;
				string lCommand = "";
				while (lCurArg != CLN_PERF_SEP) {
					lCommand += lCurArg + " ";
					if (lPos >= lItems.size() - 1) {
						break;
					}
					MOVEARG;
				}
				lCommand = lCommand.substr(0, lCommand.size() - 1);
				INFO("Adding new command to the queue: \"" << lCommand << "\"" << endl);
				fQueueAdd(lCommand);
			}
		}
		else COMP(CLN_CMD_SRC) {
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
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CMDPROC, "Cmdlnproc::fProcessCmdln: Command line arguments processed" << endl);

	return OK;
}
