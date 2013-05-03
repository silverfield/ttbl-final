/* provides logging  */

#ifndef LOGGER_H_
#define LOGGER_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>

#include "common.h"
#include "sender.h"

/*****************************************************************/
/* Types
/*****************************************************************/

typedef int LogMask;

/*****************************************************************/
/* Constants
/*****************************************************************/

#define LOG_MAX_SIZE 5000

#define LOG_INFO_MASK 0x1
#define LOG_ERR_MASK 0x2
#define LOG_DEB_MASK 0x4
#define LOG_ALL_MASK LOG_INFO_MASK | LOG_ERR_MASK | LOG_DEB_MASK

//number of debug levels
#define LOG_LVL_COUNT 100

//padding between function name and actual debug message
#define LOG_PADDING 40

#define IMPFO_STR ">>> "

#define LOG_LOGGER gDefaultLogger

/*****************************************************************/
/* Variables
/*****************************************************************/

//set default logger for use with macros INFO, DEB...
class Logger;
extern Logger *gDefaultLogger;
extern std::ostringstream gLoggerStringStream;

extern pthread_mutex_t gLogMutex;

/*****************************************************************/
/* Macro definitions
/*****************************************************************/

#define IMPFO(imX) if (1) {\
	gLoggerStringStream.clear(); \
	gLoggerStringStream.str(""); \
	gLoggerStringStream << imX; \
	LOG_LOGGER->fImpfo(gLoggerStringStream.str()); \
}

#define IMPFOF_M(imFormat, imVaList) if (1) {\
	char mBuffer [LOG_MAX_SIZE]; \
	vsnprintf (mBuffer, LOG_MAX_SIZE, imFormat, imVaList);\
	std::string mString = fCharArrayToString(mBuffer); \
	LOG_LOGGER->fImpfo(mString); \
}

#define INFO(imX) if (1) {\
	gLoggerStringStream.clear(); \
	gLoggerStringStream.str(""); \
	gLoggerStringStream << imX; \
	LOG_LOGGER->fInfo(gLoggerStringStream.str()); \
}

#define INFOF_M(imFormat, imVaList) if (1) {\
	char mBuffer [LOG_MAX_SIZE]; \
	vsnprintf (mBuffer, LOG_MAX_SIZE, imFormat, imVaList);\
	std::string mString = fCharArrayToString(mBuffer); \
	LOG_LOGGER->fInfo(mString); \
}

#define ERR(imX) if (1) {\
	gLoggerStringStream.clear(); \
	gLoggerStringStream.str(""); \
	gLoggerStringStream << imX; \
	LOG_LOGGER->fErr(gLoggerStringStream.str()); \
}

#define ERRF_M(imFormat, imVaList) if (1) {\
	char mBuffer [LOG_MAX_SIZE]; \
	vsnprintf(mBuffer, LOG_MAX_SIZE, imFormat, imVaList);\
	std::string mString = fCharArrayToString(mBuffer); \
	LOG_LOGGER->fErr(mString); \
}

#define DEB(imLvl, imX) if (LOG_LOGGER->fIsDebOn()) {\
	gLoggerStringStream.clear(); \
	gLoggerStringStream.str(""); \
	gLoggerStringStream << imX; \
	LOG_LOGGER->fDeb(imLvl, gLoggerStringStream.str()); \
}

#define DEBF_M(imLvl, imFormat, imVaList) if (LOG_LOGGER->fIsDebOn()) {\
	char mBuffer [LOG_MAX_SIZE]; \
	vsnprintf(mBuffer, LOG_MAX_SIZE, imFormat, imVaList);\
	std::string mString = fCharArrayToString(mBuffer); \
	LOG_LOGGER->fDeb(imLvl, mString); \
}

/*****************************************************************/
/* Functions
/*****************************************************************/

/* INFOF, DEBF...
 *
 * This functions are necessary since a macro cannot take variable number of arguments */
void INFOF(const char *iFormat, ...);
void IMPFOF(const char *iFormat, ...);
void ERRF(const char *iFormat, ...);
void DEBF(int iLvl, const char *iFormat, ...);

inline void fSetDefaultLogger(Logger* iLogger) {	gDefaultLogger = iLogger;};

/*****************************************************************/
/* Class declaration
/*****************************************************************/

class Logger {
private:
	//----- data -----
	std::vector<std::ostream*> cStreams;
	std::vector<LogMask> cStreamLogTypes; //what kind of logging (info, err, deb) is sent to particular stream
	std::vector<std::ofstream*> cOfStreams;
	std::vector<LogMask> cOfStreamLogTypes; //what kind of logging (info, err, deb) is sent to particular ofstream
	std::vector<Sender*> cSenders;
	std::vector<LogMask> cSenderLogTypes; //what kind of logging (info, err, deb) is sent by particular sender

	bool cInfoOn;
	bool cErrOn;
	bool cDebOn;
	bool cDebLvls [LOG_LVL_COUNT];

	bool cLastInfoNewLine;
public:
	//----- constructors -----
	Logger(int iInitDebLvls [], int iInitLvlsCount);

	//----- interface -----
	/* chooses appropriate function(s) according to log types */
	void fLog(int iLvl, std::string iString, LogMask iLogTypes);
	/* these 4 functions prepare the message and send it to function fWrite */
	void fInfo(std::string iString); //does not do new-lines
	void fImpfo(std::string iString); //just enhanced info - used for results of algs
	void fErr(std::string iString); //does not do new-lines
	void fDeb(int iLvl, std::string iString); //does not do new-lines

	void fStartSendOnPort(int iPort, LogMask iLogTypes);
	void fStartWriteToStream(std::ostream* iStream, LogMask iLogTypes);
	void fStartWriteToOfStream(std::ofstream* iOfStream, LogMask iLogTypes);
	Ret fStopSendOnPort(int iPort, LogMask iLogTypes);
	Ret fStopWriteToStream(std::ostream* iStream, LogMask iLogTypes);
	Ret fStopWriteToOfStream(std::ofstream* iOfStream, LogMask iLogTypes);

	void fUnsetAll();
	std::vector<int> fGetLevels();
	void fDeinitialize();

	//----- inline -----
	void fSetLvl(int iLvl) {if (iLvl > -1 && iLvl < LOG_LVL_COUNT) {cDebLvls[iLvl] = true;}};
	void fUnsetLvl(int iLvl) {if (iLvl > -1 && iLvl < LOG_LVL_COUNT) {cDebLvls[iLvl] = false;}};
	void fInfoOn(bool iOn) {cInfoOn = iOn;};
	void fErrOn(bool iOn) {cErrOn = iOn;};
	void fDebOn(bool iOn) {cDebOn = iOn;};
	bool fIsDebOn() {return cDebOn;};
private:
	void fInit(int iInitDebLvls [], int iInitLvlsCount);
	/* writes given string to streams/sockets supporting given type of logging */
	void fWrite(std::string iString, LogMask iLogTypes);
	void fWriteToStreams(std::string iString, LogMask iLogTypes);
	void fSendOnPorts(std::string iString, LogMask iLogTypes);
	std::string fAdjustDebugString(std::string iString);
	std::string fAdjustErrString(std::string iString);
	std::string fAdjustImpfoString(std::string iString);
};

#endif
