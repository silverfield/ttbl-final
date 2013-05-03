/* used to time execution of algorithms */
#ifndef ALGTIMER_H_
#define ALGTIMER_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

/*****************************************************************/
/* Constants
/*****************************************************************/

#define CLOCKS_PER_MS 1000

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Types
/*****************************************************************/

enum TimerState {TSRun, TSPause, TSStop};

/*****************************************************************/
/* Class
/*****************************************************************/

class AlgTimer {
private:
	//----- data -----
	TimerState cState;
	struct timeval cLastStart;
	long long int cResultMs;

	void fAddToResult();
public:
	//----- constructors -----
	AlgTimer();

	//----- interface -----
	void fForceStartTimer();
	Ret fStartNewTimer(); //will not start if timing is running or paused
	void fPauseTimer();
	void fContinueTimer();
	void fStopTimer();

	long long int fGetResult();
	std::string fGetResultString();

	static std::string fGetResultString(long long int iResult);
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* ALGTIMER_H_ */
