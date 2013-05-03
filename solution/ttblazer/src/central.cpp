/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>

#include "central.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

string gObjTypeCmdStr [] = {"ug", "te", "td", "tt", "ERR"};
string gObjTypeCmdStrs [] = {"ugs", "tes", "tds", "tts", "ERR"};
string gObjTypeStr [] = {"UG", "TE", "TD", "TT", "ERR"};
string gObjTypeStrings [] = {"underlying graph", "time-expanded graph", "time-dependent graph",
		"timetable", "ERROR object"};
string gObjTypeStringS [] = {"Underlying graph", "Time-expanded graph", "Time-dependent graph",
		"Timetable", "ERROR object"};
string gQueryTypeStr [] = {"SP", "DIST", "CONN", "EA"};
string gQueryTypeStrSmall [] = {"sp", "dist", "conn", "ea"};
string fGetQueryTypeSetString(set<QueryType> iSet) {
	string lResult = "";

	for (set<QueryType>::iterator i = iSet.begin(); i != iSet.end(); i++) {
		if (i != iSet.begin()) {
			lResult += ", ";
		}
		lResult += gQueryTypeStr[*i];
	}

	return lResult;
}
string gConnStr [] = {"CONNStrong", "CONNWeak", "CONNNone"};
string gBoolStr [] = {"False", "True"};

/*****************************************************************/
/* Global object definitions
/*****************************************************************/

Logger *gLogger;
Communicator *gCommunicator;
Options *gOptions;
AlgTimer* gAlgTimer;

/*****************************************************************/
/* Functions
/*****************************************************************/

std::string fTimeFormatToString(int iMinutes) {
	if (gOptions->cTimeFormat == TFOn) {
		return fMinutesToString(iMinutes);
	}
	else {
		return fIntToStr(iMinutes);
	}
}

int fStringToTimeFormat(std::string iString) {
	if (gOptions->cTimeFormat == TFOn) {
		return fStringToMinutes(iString);
	}
	else {
		return fStrToInt(iString);
	}
}
