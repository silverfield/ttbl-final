/* analysing the sizes of structures */
#ifndef STRUCTSIZER_H_
#define STRUCTSIZER_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <vector>
#include <queue>
#include <set>
#include <map>
#include <list>
#include <string>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Functions
/*****************************************************************/

std::string fGetSizeString(int iBytes);

/*****************************************************************/
/* Template functions
/*****************************************************************/

template <class A, class B>
int fSizeOf(const std::map<A, B> &iMap) {
	int lBytes = iMap.size() * (sizeof(A) + sizeof(B));
	return lBytes;
}

template <class A>
int fSizeOf(const std::vector<A> &iVector) {
	int lBytes = iVector.size() * (sizeof(A));
	return lBytes;
}

template <class A>
int fSizeOf(const std::list<A> &iList) {
	int lBytes = iList.size() * (sizeof(A));
	return lBytes;
}

template <class A, class B>
int fSizeOf(const std::set<A, B> &iSet) {
	int lBytes = iSet.size() * (sizeof(A) + sizeof(B));
	return lBytes;
}

template <class A>
int fSizeOf(const std::set<A> &iSet) {
	int lBytes = iSet.size() * (sizeof(A));
	return lBytes;
}

template <class A>
int fSizeOf(const std::queue<A> &iQueue) {
	int lBytes = iQueue.size() * (sizeof(A));
	return lBytes;
}

template <class A, class B, class C>
int fSizeOf(const boost::unordered_set<A, B, C> &iSet) {
	int lBytes = iSet.size() * (sizeof(A));
	return lBytes;
}

template <class A, class B>
int fSizeOf(const boost::unordered_map<A, B> &iMap) {
	int lBytes = iMap.size() * (sizeof(A) + sizeof(B));
	return lBytes;
}

#endif /* STRUCTSIZER_H_ */
