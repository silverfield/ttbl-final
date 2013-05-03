/*****************************************************************/
/* Includes
/*****************************************************************/

#include "string.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Functions - general
/*****************************************************************/

string fExec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}

string fCharArrayToString(const char *iCharArray) {
	string lResult(iCharArray);
	//for (int i = 0; iCharArray[i] != 0; i++) {
	//   lResult += iCharArray[i];
	//}

	return lResult;
}

vector<string> fSplitString(string iString, char iSeparator) {
	vector<string> lItems;

	string lItem = "";
	for (string::iterator i = iString.begin(); i != iString.end(); i++) {
		if (*i == iSeparator) {
			lItems.push_back(lItem);
			lItem = "";
			continue;
		}
		lItem += *i;
	}
	lItems.push_back(lItem);

	return lItems;
}

vector<string> fSplitString(string iString, string iSeparator) {
	vector<string> lItems;
	split(lItems, iString, is_any_of(iSeparator)); //TODO change to exact

	return lItems;
}

string fIntToStr(int iInteger) {
    //stringstream lStream;
    //lStream << iInteger;
	char lBuffer [30];
	sprintf(lBuffer, "%d", iInteger);

	return fCharArrayToString(lBuffer);
}

int fStrToIntNoCheck(string iString) {
	return atoi(iString.c_str());
}

int fStrToInt(string iString) {
	if (fIsInt(iString) == false) {
		return -1;
	}

	return atoi(iString.c_str());
}

string fDoubleToStr(double iDouble) {
	char lBuffer[30];
	snprintf(lBuffer, sizeof(lBuffer), "%g", iDouble);

	return fCharArrayToString(lBuffer);
}

double fStrToDouble(string iString) {
	if (fIsDouble(iString) == false) {
		return 0;
	}

	return atof(iString.c_str());
}

double fStrToDoubleNoCheck(string iString) {
	return atof(iString.c_str());
}

bool fIsDouble(string iString) {
	bool lGotDot = false;
	for (string::size_type i = 0; i < iString.size(); i++) {
		if (i == 0 && iString[i] == '-') {
			continue;
		}

		if (iString[i] == '.' && (lGotDot || i == 0)) {
			return false;
		}
		else if (iString[i] == '.') {
			lGotDot = true;
			continue;
		}

		if (isdigit(iString[i]) == false) {
			return false;
		}
	}

	return true;
}

bool fIsInt(string iString) {
	for (string::size_type i = 0; i < iString.size(); i++) {
		if (i == 0 && iString[i] == '-') {
			continue;
		}
		if (isdigit(iString[i]) == false) {
			return false;
		}
	}

	return true;
}

string fCharRemove(string iString, char iChar) {
	string lResult = "";
	for (int i = 0; i < iString.size(); i++) {
		if (iString[i] != iChar) {
			lResult += iString[i];
		}
	}

	return lResult;
}

string fJustFileName(string iFileName) {
	vector<string> lParts = fSplitString(iFileName, "/");
	vector<string> lNameExt = fSplitString(lParts[lParts.size() - 1], ".");

	return lNameExt[0];
}

string fPadString(string iString, int iTotalWidth) {
	string lResult = iString;
	int lPadding = max(0, iTotalWidth - (int)iString.size());
	string lPaddingString(lPadding, ' ');
	lResult += lPaddingString;

	return lResult;
}

string fPadInt(int iInt, int iTotalWidth) {
	string lResult = fIntToStr(iInt);
	int lPadding = max(0, iTotalWidth - (int)lResult.size());
	string lPaddingString(lPadding, '0');
	lResult = lPaddingString + lResult;

	return lResult;
}

string fGetLongString(int iSize) {
	string lResult = "";
	for (int i = 0; i < iSize; i += 100) {
		lResult += string(100, '-');
	}
	return lResult;
}

vector<int> fRandomPermutation(int iN) {
	vector<int> lResult = fIdentityVector(iN);

	for (int i = 1; i < iN; i++) {
		int lPos = rand() % (i + 1);
		int lTemp = lResult[i];
		lResult[i] = lResult[lPos];
		lResult[lPos] = lTemp;
	}

	return lResult;
}

vector<int> fIdentityVector(int iN) {
	vector<int> lResult;
	lResult.reserve(iN);
	for (int i = 0; i < iN; i++) {
		lResult.push_back(i);
	}

	return lResult;
}

int fRound(double iDouble) {
	if ((iDouble - (double)((int)iDouble)) >= 0.5) {
		return 1 + (int)iDouble;
	}

	return (int)iDouble;
}

/*****************************************************************/
/* Time related
/*****************************************************************/

int fTimeToMinutes(Time iTime) {
	return iTime.cDays * 24 * 60 + iTime.cHours * 60 + iTime.cMinutes;
}

Time fMinutesToTime(int iMinutes) {
	Time lTime;
	lTime.cMinutes = iMinutes % 60;
	iMinutes /= 60;
	lTime.cHours = iMinutes % 24;
	iMinutes /= 24;
	lTime.cDays = iMinutes;

	return lTime;
}

string fTimeToString(Time iTime) {
	return fIntToStr(iTime.cDays) + " " + fPadInt(iTime.cHours, 2) + ":" + fPadInt(iTime.cMinutes, 2);
}

Time fStringToTimeNoCheck(string iTimeString) {
	//eg. 134 23:59
	Time lTime;

	//lTime.cDays = fStrToInt(lDayHours[0]);
	lTime.cDays = fStrToIntNoCheck(iTimeString.substr(0, iTimeString.size() - 6));
	//vector<string> lHourMinutes = fSplitString(lDayHours[1], ":");
	//lTime.cHours = fStrToInt(lHourMinutes[0]);
	lTime.cHours = fStrToIntNoCheck(iTimeString.substr(iTimeString.size() - 5, 2));
	//lTime.cMinutes = fStrToInt(lHourMinutes[1]);
	lTime.cMinutes = fStrToIntNoCheck(iTimeString.substr(iTimeString.size() - 2, 2));

	return lTime;
}

Time fStringToTime(string iTimeString) {
	//eg. 134 23:59
	Time lTime;

	if (iTimeString.size() < 7) {
		return lTime;
	}

	//lTime.cDays = fStrToInt(lDayHours[0]);
	lTime.cDays = fStrToInt(iTimeString.substr(0, iTimeString.size() - 6));
	//vector<string> lHourMinutes = fSplitString(lDayHours[1], ":");
	//lTime.cHours = fStrToInt(lHourMinutes[0]);
	lTime.cHours = fStrToInt(iTimeString.substr(iTimeString.size() - 5, 2));
	//lTime.cMinutes = fStrToInt(lHourMinutes[1]);
	lTime.cMinutes = fStrToInt(iTimeString.substr(iTimeString.size() - 2, 2));

	return lTime;
}
