/* Options of the program */
#ifndef OPTIONS_H_
#define OPTIONS_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <sstream>
#include <string>

/*****************************************************************/
/* Enums
/*****************************************************************/

enum CmdSource {CSStdIn, CSQueue};
enum Timing {TimOn, TimOff};
enum TimeFormat {TFOn, TFOff};
enum ObjIndexing {OIName, OIIndex};

/*****************************************************************/
/* Variables
/*****************************************************************/

extern std::string gCmdSourceStr [];
extern std::string gTimingStr [];
extern std::string gTimeFormatStr[];
extern std::string gObjIndexingStr[];

/*****************************************************************/
/* Options
/*****************************************************************/

struct Options {
	CmdSource cCmdSource;
	Timing cTiming;
	TimeFormat cTimeFormat;
	ObjIndexing cObjIndexing;

	void fSetDefaultValues();
	std::string fGetOptionsString();
};

#endif /* OPTIONS_H_ */
