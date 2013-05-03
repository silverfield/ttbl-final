/*****************************************************************/
/* Includes
/*****************************************************************/

#include <iostream>
#include <stdio.h>

#include "commonlvls.h"
#include "common.h"

#include "logger.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

Logger *gDefaultLogger;
pthread_mutex_t gLogMutex = PTHREAD_MUTEX_INITIALIZER;
ostringstream gLoggerStringStream;

/*****************************************************************/
/* Functions
/*****************************************************************/

void INFOF(const char *iFormat, ...) {
	va_list lArgptr;
	va_start(lArgptr, iFormat);
	INFOF_M(iFormat, lArgptr);
	va_end(lArgptr);
}

void IMPFOF(const char *iFormat, ...) {
	va_list lArgptr;
	va_start(lArgptr, iFormat);
	IMPFOF_M(iFormat, lArgptr);
	va_end(lArgptr);
}

void ERRF(const char *iFormat, ...) {
	va_list lArgptr;
	va_start(lArgptr, iFormat);
	ERRF_M(iFormat, lArgptr);
	va_end(lArgptr);
}

void DEBF(int iLvl, const char *iFormat, ...) {
	if (LOG_LOGGER->fIsDebOn() == false) {
		return;
	}

	va_list lArgptr;
	va_start(lArgptr, iFormat);
	DEBF_M(iLvl, iFormat, lArgptr);
	va_end(lArgptr);
}

/*****************************************************************/
/* Constructor(s)
/*****************************************************************/

Logger::Logger(int iInitDebLvls [], int iInitLvlsCount) {
	fInit(iInitDebLvls, iInitLvlsCount);

	cLastInfoNewLine = true;
}

/*****************************************************************/
/* Interface
/*****************************************************************/

void Logger::fLog(int iLvl, string iString, LogMask iLogTypes) {
	if (iLogTypes & LOG_INFO_MASK) {
		fInfo(iString);
	}
	if (iLogTypes & LOG_ERR_MASK) {
		fErr(iString);
	}
	if (iLogTypes & LOG_DEB_MASK) {
		fDeb(iLvl, iString);
	}
}

void Logger::fInfo(string iString) {
	if (cInfoOn == false) {
		return;
	}

	string lAdjustedStr = iString;
	if (cLastInfoNewLine) {
		lAdjustedStr = "$ " + iString;
	}

	if (iString[iString.size() - 1] == '\n') {
		cLastInfoNewLine = true;
	}
	else {
		cLastInfoNewLine = false;
	}

	fWrite(lAdjustedStr, LOG_INFO_MASK);
}

void Logger::fImpfo(string iString) {
	if (cInfoOn == false) {
		return;
	}

	//adjust the string
	string lString = fAdjustImpfoString(iString);

	fWrite(lString, LOG_INFO_MASK);
}

void Logger::fErr(string iString) {
	if (cErrOn == false) {
		return;
	}

	//adjust the string
	string lString = fAdjustErrString(iString);

	fWrite(lString, LOG_ERR_MASK);
}

void Logger::fDeb(int iLvl, string iString) {
	if (cDebOn == false) {
		gLoggerStringStream.str("");
		gLoggerStringStream.clear();
		return;
	}

	if (cDebLvls[iLvl] == false) {
		return;
	}

	//adjust the string
	string lString = fAdjustDebugString(iString);

	//do NOT send messages from sender on ports - this would do an infinite cycle
	if (iLvl == LVL_SENDER) {
		fWriteToStreams(lString, LOG_DEB_MASK);
	}
	else {
		fWrite(lString, LOG_DEB_MASK);
	}
}

void Logger::fStartSendOnPort(int iPort, LogMask iLogTypes) {
	int lPosition = -1;
	for (vector<Sender*>::size_type i = 0; i < cSenders.size(); i++) {
		if (cSenders[i]->fGetPort() == iPort) {
			lPosition = i;
			break;
		}
	}

	//sender exists but does not log required type
	if (lPosition != -1) {
		cSenderLogTypes[lPosition] |= iLogTypes;
	}
	else {
		Sender *lNewSender = new Sender(iPort, MSG_BUFF_SIZE);
		cSenders.push_back(lNewSender);
		cSenderLogTypes.push_back(iLogTypes);
	}

	DEB(LVL_LOGGER, "Logger::fStartSendOnPort: Starting logging (" << iLogTypes <<
			") on port " << iPort << endl);
}

void Logger::fStartWriteToStream(ostream* iStream, LogMask iLogTypes) {
	int lPosition = -1;
	for (vector<ostream*>::size_type i = 0; i < cStreams.size(); i++) {
		if (cStreams[i] == iStream) {
			lPosition = i;
			break;
		}
	}

	//stream exists but does not log required type
	if (lPosition != -1) {
		cStreamLogTypes[lPosition] |= iLogTypes;
	}
	else {
		cStreams.push_back(iStream);
		cStreamLogTypes.push_back(iLogTypes);
	}

	DEB(LVL_LOGGER, "Logger::fStartWriteToStream: Starting logging (" << iLogTypes <<
			") on stream with pointer " << iStream << endl);
}

void Logger::fStartWriteToOfStream(ofstream* iOfStream, LogMask iLogTypes) {
	int lPosition = -1;
	for (vector<ostream*>::size_type i = 0; i < cOfStreams.size(); i++) {
		if (cOfStreams[i] == iOfStream) {
			lPosition = i;
			break;
		}
	}

	//stream exists but does not log required type
	if (lPosition != -1) {
		cOfStreamLogTypes[lPosition] |= iLogTypes;
	}
	else {
		cOfStreams.push_back(iOfStream);
		cOfStreamLogTypes.push_back(iLogTypes);
	}

	DEB(LVL_LOGGER, "Logger::fStartWriteToOfStream: Starting logging (" << iLogTypes <<
			") on ofstream with pointer " << iOfStream << endl);
}

Ret Logger::fStopSendOnPort(int iPort, LogMask iLogTypes) {
	int lPosition = -1;
	for (vector<Sender*>::size_type i = 0; i < cSenders.size(); i++) {
		if (cSenders[i]->fGetPort() == iPort) {
			lPosition = i;
			break;
		}
	}

	if (lPosition == -1) {
		ERR("Logger::fStopSendOnPort: Sender not found");
		return NOK;
	}

	//erase only on given log types
	if ((cSenderLogTypes[lPosition] & ~iLogTypes) != 0) {
		cSenderLogTypes[lPosition] &= ~iLogTypes;
	}
	else {
		cSenders.erase(cSenders.begin() + lPosition);
	}

	DEB(LVL_LOGGER, "Logger::fStopSendOnPort: Stopping logging (" << iLogTypes <<
			") on port " << iPort << endl);

	return OK;
}

Ret Logger::fStopWriteToStream(ostream* iStream, LogMask iLogTypes) {
	int lPosition = -1;
	for (vector<ostream*>::size_type i = 0; i < cStreams.size(); i++) {
		if (cStreams[i] == iStream) {
			lPosition = i;
			break;
		}
	}

	if (lPosition == -1) {
		ERR("Logger::fStopWriteToStream: Stream not found");
		return NOK;
	}

	//erase only on given log types
	if ((cStreamLogTypes[lPosition] & ~iLogTypes) != 0) {
		cStreamLogTypes[lPosition] &= ~iLogTypes;
	}
	else {
		cStreams.erase(cStreams.begin() + lPosition);
	}

	DEB(LVL_LOGGER, "Logger::fStopWriteToStream: Stopping logging (" << iLogTypes <<
			") on stream with pointer " << iStream << endl);

	return OK;
}

Ret Logger::fStopWriteToOfStream(ofstream* iOfStream, LogMask iLogTypes) {
	int lPosition = -1;
	for (vector<ofstream*>::size_type i = 0; i < cOfStreams.size(); i++) {
		if (cOfStreams[i] == iOfStream) {
			lPosition = i;
			break;
		}
	}

	if (lPosition == -1) {
		ERR("Logger::fStopWriteToOfStream: OfStream not found");
		return NOK;
	}

	//erase only on given log types
	if ((cOfStreamLogTypes[lPosition] & ~iLogTypes) != 0) {
		cOfStreamLogTypes[lPosition] &= ~iLogTypes;
	}
	else {
		cOfStreams[lPosition]->flush();
		cOfStreams[lPosition]->close();
		cOfStreams.erase(cOfStreams.begin() + lPosition);
	}

	DEB(LVL_LOGGER, "Logger::fStopWriteToOfStream: Stopping logging (" << iLogTypes <<
			") on ofstream with pointer " << iOfStream << endl);

	return OK;
}

void Logger::fUnsetAll() {
	for (int i = 0; i < LOG_LVL_COUNT; i++) {
		cDebLvls[i] = false;
	}
}

vector<int> Logger::fGetLevels() {
	vector<int> lLevels;

	for (int i = 0; i < LOG_LVL_COUNT; i++) {
		if (cDebLvls[i] == true) {
			lLevels.push_back(i);
		}
	}

	return lLevels;
}

void Logger::fDeinitialize() {
	for (vector<ofstream*>::size_type i = 0; i < cOfStreams.size(); i++) {
		fStopWriteToOfStream(cOfStreams[i], LOG_ALL_MASK);
	}
}

/*****************************************************************/
/* Implementation
/*****************************************************************/

void Logger::fInit(int iInitDebLvls [], int iInitLvlsCount) {
	cInfoOn = true;
	cErrOn = true;
	cDebOn = true;

	//initialize all debug levels to false
	for (int i = 0; i < LOG_LVL_COUNT; i++) {
		cDebLvls[i] = false;
	}

	//set specified debug levels to true
	for (int i = 0; i < iInitLvlsCount; i++) {
		cDebLvls[iInitDebLvls[i]] = true;
	}
}

void Logger::fWrite(string iString, LogMask iLogTypes) {
	fWriteToStreams(iString, iLogTypes);
	fSendOnPorts(iString, iLogTypes);
}

void Logger::fWriteToStreams(string iString, LogMask iLogTypes) {
	pthread_mutex_lock(&gLogMutex);

	//send on all streams
	for (vector<ostream*>::size_type i = 0; i < cStreams.size(); i++) {
		if (cStreamLogTypes[i] & iLogTypes != 0) {
			(*cStreams[i]) << iString;
		}
	}

	for (vector<ofstream*>::size_type i = 0; i < cOfStreams.size(); i++) {
		if (cOfStreamLogTypes[i] & iLogTypes != 0) {
			(*cOfStreams[i]) << iString;
		}
	}

	pthread_mutex_unlock(&gLogMutex);
}

void Logger::fSendOnPorts(string iString, LogMask iLogTypes) {
	pthread_mutex_lock(&gLogMutex);

	//send by all senders
	for (vector<Sender*>::size_type i = 0; i < cSenders.size(); i++) {
		if (cSenderLogTypes[i] & iLogTypes != 0) {
			cSenders[i]->fSend(iString);
		}
	}

	pthread_mutex_unlock(&gLogMutex);
}

string Logger::fAdjustDebugString(string iString) {
	int lDoubleColon = iString.find("::");

	//no :: -> no adjustment
	if (lDoubleColon == string::npos) {
		return iString;
	}

	//we must have "something::something2: message"
	int lColon = iString.find(": ");
	if (lColon < lDoubleColon) {
		return iString;
	}
	int lPaddingSize = max(LOG_PADDING - lColon, 1);

	string lFuncPart = iString.substr(0, lColon);
	string lPaddingPart = string(lPaddingSize, ' ');
	string lMsgPart = iString.substr(lColon + 2);

	return lFuncPart + lPaddingPart + lMsgPart;
}

string Logger::fAdjustErrString(string iString) {
	int lDoubleColon = iString.find("::");

	//no :: -> no adjustment
	if (lDoubleColon == string::npos) {
		return "ERR " + iString;
	}

	//we must have "something::something2: message"
	int lColon = iString.find(": ");
	if (lColon < lDoubleColon) {
		return iString;
	}
	int lPaddingSize = max(LOG_PADDING - lColon, 1);

	string lFuncPart = iString.substr(0, lColon);
	string lPaddingPart = string(lPaddingSize, ' ');
	string lMsgPart = "ERR " + iString.substr(lColon + 2);

	return lFuncPart + lPaddingPart + lMsgPart;
}
string Logger::fAdjustImpfoString(string iString) {
	vector<string> lRows = fSplitString(iString, "\n");

	stringstream lResult (stringstream::out);

	for (vector<string>::size_type i = 0; i < lRows.size(); i++) {
		if (lRows[i] == "") {
			continue;
		}
		lResult << IMPFO_STR << lRows[i] << endl;
	}

	return lResult.str();
}
