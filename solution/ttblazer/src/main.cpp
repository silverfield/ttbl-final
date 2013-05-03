/*****************************************************************/
/* Include
/*****************************************************************/

#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "oracles/oracleinfos.h"
#include "analysers/analyserinfos.h"
#include "generators/generatorinfos.h"
#include "modifiers/modifierinfos.h"
#include "postbox/postboxinfos.h"
#include "central.h"
#include "communicator.h"
#include "tests.h"
#include "cmdproc.h"
#include "cmdlnproc.h"
#include "manipulator.h"

#include "main.h"

using namespace std;

/*****************************************************************/
/* Functions
/*****************************************************************/

void fExitProgram() {
	gLogger->fDeinitialize();

	INFO("Thank you, BYE!" << endl);
	exit(0);
}

/*****************************************************************/
/* Implementation functions
/*****************************************************************/

void fInitLogger() {
	//create the logger
	int lInitLevels [] = LVL_INIT;
	int lInitLevelsCount = sizeof(lInitLevels) / sizeof(int);
	gLogger = new Logger(lInitLevels, lInitLevelsCount);
	fSetDefaultLogger(gLogger);
	gLogger->fDebOn(false);

	//register cout
	gLogger->fStartWriteToStream(&cout, LOG_ALL_MASK);
	//gLogger->fStartSendOnPort(PRINT_PORT, LOG_ALL_MASK);
}

void fInitOptions() {
	gOptions = new Options();
	gOptions->fSetDefaultValues();
}

void fInitCommunicator() {
	gCommunicator = Communicator::fCreateCommunicator(COMMAND_PORT);
}

void fInitAlgTimer() {
	gAlgTimer = new AlgTimer();
}

void fInitVarious() {
	srand(time(NULL));
}

int main(int argc, char *argv[]) {
	cout << "-----------------------------------------------------------------" << endl;
	cout << "          Welcome to TTBlazer by František Hajnovič" << endl;
	cout << "-----------------------------------------------------------------" << endl;

	//various initializations
	fInitVarious();

	//initialize logger
	fInitLogger();
	DEB(LVL_MAIN, "Main::main: Seeing this message means that logging is successfully initialized" << endl);

	//initialize oracles, analysers...
	OracleInfos::fInitializeInfos();
	AnalyserInfos::fInitializeInfos();
	GeneratorInfos::fInitializeInfos();
	ModifierInfos::fInitializeInfos();
	PostmanInfos::fInitializeInfos();

	//initialize options
	fInitOptions();
	DEB(LVL_MAIN, "Main::main: Options initialized: " << endl << gOptions->fGetOptionsString() << endl);

	//initialize communicator
	fInitCommunicator();
	DEB(LVL_MAIN, "Main::main: Communicator initialized" << endl);

	//initialize alg-timer
	fInitAlgTimer();
	DEB(LVL_MAIN, "Main::main: Algorithm timer initialized" << endl);

	//initialize postbox
	//no initialization
	DEB(LVL_MAIN, "Main::main: Postbox initialized" << endl);

	//initialize manipulator
	Manipulator::fInitialize();

	//process command line arguments
	fProcessCmdln(argc, argv);

	//start main loop - process commands
	fProcessCommands();

	return 0;
}
