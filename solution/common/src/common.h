/* Functions, macros, data types... Common to the whole project */

#ifndef COMMON_H_
#define COMMON_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>
#include <set>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macro definitions
/*****************************************************************/

#define HERE std::cout << "here" << std::endl;

/*****************************************************************/
/* Structs
/*****************************************************************/

struct Time {
	int cDays;
	int cHours;
	int cMinutes;
};

/*****************************************************************/
/* Functions - general
/*****************************************************************/

std::string fExec(const char* cmd);
std::string fCharArrayToString(const char *iCharArray);
std::string fIntToStr(int iInteger);
int fStrToIntNoCheck(std::string iString);
int fStrToInt(std::string iString);
std::string fDoubleToStr(double iDouble);
double fStrToDoubleNoCheck(std::string iString);
double fStrToDouble(std::string iString);
bool fIsInt(std::string iString);
bool fIsDouble(std::string iString);
std::vector<std::string> fSplitString(std::string iString, char iSeparator);
std::vector<std::string> fSplitString(std::string iString, std::string iSeparator);
std::string fCharRemove(std::string iString, char iChar);
std::string fJustFileName(std::string iFileName);
std::string fPadString(std::string iString, int iTotalWidth);
std::string fPadInt(int iInt, int iTotalWidth);
std::string fGetLongString(int iSize);
std::vector<int> fRandomPermutation(int iN);
std::vector<int> fIdentityVector(int iN);
int fRound(double iDouble);

/*****************************************************************/
/* Time related
/*****************************************************************/

int fTimeToMinutes(Time iTime);
Time fMinutesToTime(int iMinutes);
std::string fTimeToString(Time iTime);
Time fStringToTimeNoCheck(std::string iTimeString);
Time fStringToTime(std::string iTimeString);
inline int fStringToMinutesNoCheck(std::string iTimeString) {
	return fTimeToMinutes(fStringToTimeNoCheck(iTimeString));};
inline int fStringToMinutes(std::string iTimeString) {
	return fTimeToMinutes(fStringToTime(iTimeString));};
inline std::string fMinutesToString(int iMinutes) {return fTimeToString(fMinutesToTime(iMinutes));};

/*****************************************************************/
/* Functions to print set, vector, map
/*****************************************************************/

template <class T>
std::string fGetSetString(std::set<T> iSet, std::string iSep = ", ") {
	std::stringstream lSs (std::stringstream::out);

    for (typename std::set<T>::iterator i = iSet.begin(); i != iSet.end(); i++) {
        if (i != iSet.begin()) {
    		lSs << iSep;
    	}
        lSs << *i;
    }

    return lSs.str();
}

template <class T>
void fPrintSetString(std::set<T> iSet, std::string iSep = ", ") {
	std::cout << fGetSetString<T>(iSet, iSep) << "\n";
}

template <class T>
std::string fGetSetString(boost::unordered_set<T> iSet, std::string iSep = ", ") {
	std::stringstream lSs (std::stringstream::out);

    for (typename boost::unordered_set<T>::iterator i = iSet.begin(); i != iSet.end(); i++) {
        if (i != iSet.begin()) {
    		lSs << iSep;
    	}
        lSs << *i;
    }

    return lSs.str();
}

template <class T>
void fPrintSetString(boost::unordered_set<T> iSet, std::string iSep = ", ") {
	std::cout << fGetSetString<T>(iSet, iSep) << "\n";
}

template <class T>
std::string fGetListString(std::list<T> iList, std::string iSep = ", ") {
	std::stringstream lSs (std::stringstream::out);

    for (typename std::list<T>::iterator i = iList.begin(); i != iList.end(); i++) {
        if (i != iList.begin()) {
    		lSs << iSep;
    	}
        lSs << *i;
    }

    return lSs.str();
}

template <class T>
void fPrintListString(std::list<T> iList, std::string iSep = ", ") {
	std::cout << fGetListString<T>(iList, iSep) << "\n";
}

template <class T>
std::string fGetVectorString(std::vector<T> iVector, std::string iSep = ", ") {
	std::stringstream lSs (std::stringstream::out);

    for (typename std::vector<T>::iterator i = iVector.begin(); i != iVector.end(); i++) {
    	if (i != iVector.begin()) {
    		lSs << iSep;
    	}
        lSs << *i;
    }

    return lSs.str();
}

template <class T>
void fPrintVectorString(std::vector<T> iVector, std::string iSep = ", ") {
	std::cout << fGetVectorString<T>(iVector, iSep) << "\n";
}

template <class K, class T>
std::string fGetMapString(std::map<K, T> iMap, std::string iSep = ", ") {
	std::stringstream lSs (std::stringstream::out);

    for (typename std::map<K, T>::iterator i = iMap.begin(); i != iMap.end(); i++) {
    	if (i != iMap.begin()) {
    		lSs << iSep;
    	}
        lSs << "[" << i->first << ", " << i->second << "]";
    }

    return lSs.str();
}

template <class K, class T>
void fPrintMapString(std::map<K, T> iMap, std::string iSep = ", ") {
	std::cout << fGetMapString<K, T>(iMap, iSep) << "\n";
}

template <class K, class T>
std::string fGetMapString(boost::unordered_map<K, T> iMap, std::string iSep = ", ") {
	std::stringstream lSs (std::stringstream::out);

    for (typename boost::unordered_map<K, T>::iterator i = iMap.begin(); i != iMap.end(); i++) {
    	if (i != iMap.begin()) {
    		lSs << iSep;
    	}
        lSs << "[" << i->first << ", " << i->second << "]";
    }

    return lSs.str();
}

template <class K, class T>
void fPrintMapString(boost::unordered_map<K, T> iMap, std::string iSep = ", ") {
	std::cout << fGetMapString<K, T>(iMap, iSep) << "\n";
}

#define QS_SWAP(x, y) if (x != y) { \
	T lTemp = (*iVector)[x]; \
	(*iVector)[x] = (*iVector)[y]; \
	(*iVector)[y] = lTemp; \
	int lTempIndex = (*iTranslations)[x]; \
	(*iTranslations)[x] = (*iTranslations)[y]; \
	(*iTranslations)[y] = lTempIndex; \
	}

template <class T>
int fPartition(std::vector<T> *iVector, int iFrom, int iTo, bool iAsc,
		std::vector<int> *iTranslations) {
	int lPivotIndex = iFrom + (rand() % (iTo - iFrom + 1));
	T lPivot = (*iVector)[lPivotIndex];
	QS_SWAP(iTo, lPivotIndex);

	int lSwapPlace = iFrom;
	for (typename std::vector<T>::size_type i = iFrom; i < iTo; i++) {
		if ((iAsc && (*iVector)[i] < lPivot) ||
			(!iAsc && (*iVector)[i] > lPivot)) {
			QS_SWAP(lSwapPlace, i);
			lSwapPlace++;
		}
	}
	QS_SWAP(lSwapPlace, iTo);
	return lSwapPlace;
}

template <class T>
void fQuickSort(std::vector<T> *iVector, int iFrom, int iTo, bool iAsc,
		std::vector<int> *iTranslations) {
	if (iFrom >= iTo) {
		return;
	}
	int lMiddle = fPartition(iVector, iFrom, iTo, iAsc, iTranslations);
	fQuickSort(iVector, iFrom, lMiddle - 1, iAsc, iTranslations);
	fQuickSort(iVector, lMiddle + 1, iTo, iAsc, iTranslations);
}

/* returns the translations - translation[k] is the old position of the element
 * located at k-th place */
template <class T>
std::vector<int> fSortVector(std::vector<T> *iVector, bool iAsc) {
	std::vector<int> lTranslations;
	for (std::vector<int>::size_type i = 0; i < iVector->size(); i++) {
		lTranslations.push_back(i);
	}
	fQuickSort(iVector, 0, iVector->size() - 1, iAsc, &lTranslations);
	return lTranslations;
}


#endif /* COMMON_H_ */
