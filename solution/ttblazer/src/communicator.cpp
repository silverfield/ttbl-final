/*****************************************************************/
/* Includes
/*****************************************************************/

#include <pthread.h>
#include <iostream>

#include "../../common/src/receiver.h"

#include "central.h"
#include "cmdproc.h"

#include "communicator.h"

using namespace std;

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class Constructor(s)
/*****************************************************************/

Communicator::Communicator(int iPort) {
	cPort = iPort;
}

/*****************************************************************/
/* Static interface
/*****************************************************************/

/* Creates a new communicator in a new thread, the communicator starts communication */
Communicator* Communicator::fCreateCommunicator(int iPort) {
	Communicator *lNewCommunicator = new Communicator(iPort);

	DEB(LVL_COMMUNICATOR, "Communicator::fCreateCommunicator: Going to create thread for new communicator" << endl);

	pthread_t lNewThread;
	pthread_create(&lNewThread, NULL, &Communicator::fStartCommunicator, (void*)lNewCommunicator);

	DEB(LVL_COMMUNICATOR, "Communicator::fCreateCommunicator: Communicator initialized" << endl);

	return lNewCommunicator;
}

/*****************************************************************/
/* Static implementation
/*****************************************************************/

void* Communicator::fStartCommunicator(void* iCommunicator) {
	((Communicator*)iCommunicator)->fCommunicate();

	return NULL;
}

/*****************************************************************/
/* Implementation
/*****************************************************************/

void Communicator::fCommunicate() {
	//create receiver
	Receiver *lReceiver = new Receiver(cPort, COMMAND_BUFF_SIZE);

	DEB(LVL_COMMUNICATOR, "Communicator::fCommunicate: Communicator starting communication on port " << cPort << endl);

	//listen
	while (1) {
		string lCommand = lReceiver->fReceive();
		DEB(LVL_COMMUNICATOR, "Communicator::fCommunicate: Received command: " << lCommand << endl);

		lCommand = lCommand.substr(0, lCommand.size() - 1);
		fQueueAdd(lCommand);
	}
}
