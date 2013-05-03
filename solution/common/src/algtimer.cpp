/*****************************************************************/
/* Includes
/*****************************************************************/

#include "logger.h"

#include "algtimer.h"
#include "commonlvls.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

AlgTimer::AlgTimer() {
	cLastStart.tv_sec = 0;
	cLastStart.tv_usec = 0;
	cResultMs = 0;
	cState = TSStop;
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

void AlgTimer::fForceStartTimer() {
	cState = TSRun;
	gettimeofday(&cLastStart, NULL);
	cResultMs = 0;
}

Ret AlgTimer::fStartNewTimer() {
	if (cState == TSPause || cState == TSRun) {
		DEB(LVL_TIMER, "AlgTimer::fStartNewTimer: Timer busy ATM" << endl);
		return NOK;
	}

	cState = TSRun;
	gettimeofday(&cLastStart, NULL);
	cResultMs = 0;

	return OK;
}

void AlgTimer::fPauseTimer() {
	cState = TSPause;
	fAddToResult();
}

void AlgTimer::fContinueTimer() {
	cState = TSRun;
	gettimeofday(&cLastStart, NULL);
}

void AlgTimer::fStopTimer() {
	cState = TSStop;
	fAddToResult();
}

long long int AlgTimer::fGetResult() {
	return cResultMs;
}

string AlgTimer::fGetResultString() {
	return fGetResultString(cResultMs);
}

string AlgTimer::fGetResultString(long long int iResult) {
	stringstream lResult (stringstream::out);

	long long int lSec = (iResult)/1000;
	int lMs = iResult % 1000;

	lResult << lSec << "." << fPadInt(lMs, 3) << "s";

	return lResult.str();
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

void AlgTimer::fAddToResult() {
	struct timeval lEnd;
	gettimeofday(&lEnd, NULL);
	long long int lSecs = lEnd.tv_sec  - cLastStart.tv_sec;
    long long int lUSecs = lEnd.tv_usec - cLastStart.tv_usec;
    long long int lMs = ((lSecs) * 1000 + lUSecs/1000.0) + 0.5;
	cResultMs += lMs;
}
