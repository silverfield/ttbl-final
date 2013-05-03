/*****************************************************************/
/* Includes
/*****************************************************************/

#include "options.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

string gCmdSourceStr [] = {"StdIn", "Queue"};
string gTimingStr [] = {"On", "Off"};
string gTimeFormatStr[] = {"On", "Off"};
string gObjIndexingStr[] = {"Name", "Index"};

/*****************************************************************/
/* Options
/*****************************************************************/

void Options::fSetDefaultValues() {
	cCmdSource = CSQueue;
	cTiming = TimOn;
	cTimeFormat = TFOn;
	cObjIndexing = OIName;
}

string Options::fGetOptionsString() {
	stringstream lSs (stringstream::out);

	lSs << "Options:" << endl;
	lSs << "	Command source: " << gCmdSourceStr[cCmdSource] << endl;
	lSs << "	Algorithm timing: " << gTimingStr[cTiming] << endl;
	lSs << "	Time format: " << gTimeFormatStr[cTimeFormat] << endl;
	lSs << "	Object indexing: by " << gObjIndexingStr[cObjIndexing] << endl;

	return lSs.str();
}
