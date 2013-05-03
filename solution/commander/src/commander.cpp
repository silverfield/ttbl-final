/*****************************************************************/
/* Includes
/*****************************************************************/

#include <iostream>
#include <netinet/in.h>
#include <string>
#include <stdlib.h>

#include "../../common/src/logger.h"
#include "../../common/src/sender.h"
#include "../../common/src/commondefs.h"

#include "commanderlvls.h"

using namespace std;

/*****************************************************************/
/* Functions
/*****************************************************************/

/* main
 *
 * Argument is the command */
int main(int argc, char *argv[]) {
	//set logger
	int lDebLvls [] = LVL_INIT;
	int lInitLevelsCount = sizeof(lDebLvls) / sizeof(int);
	Logger *lLogger = new Logger(lDebLvls, lInitLevelsCount);
	fSetDefaultLogger(lLogger);
	//lLogger->fStartWriteToStream(&cout, LOG_ALL_MASK);
	//lLogger->fDebOn(false);

	//check command line arguments
	if (argc < 2) {
		ERR("There should be at least one argument (command)" << endl);
		return 1;
	}

	//get the port number
	int lPort = COMMAND_PORT;

	//create receiver
	Sender lSender(lPort, COMMAND_BUFF_SIZE);

	//compose command
	string lCommand = "";
	for (int i = 1; i < argc; i++) {
		lCommand += argv[i];
		if (i != argc - 1) {
			lCommand += " ";
		}
	}

	lSender.fSend(lCommand);

	return 0;
}
