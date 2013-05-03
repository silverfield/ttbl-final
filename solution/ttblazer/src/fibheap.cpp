/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"

#include "fibheap.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

FibHeap::FibHeap(vector<int> &iValues) {
	cHandles.insert(cHandles.end(), iValues.size(), NULL);
	cHandleUsed.insert(cHandleUsed.end(), iValues.size(), false);
	cHeap = heap_t();

	for (size_t i = 0; i < iValues.size(); i++) {
		handle_t lHandle = cHeap.push(pair<int, int*>(i, &(iValues[i])));
		cHandles[i] = new handle_t();
		*(cHandles[i]) = lHandle;
		cHandleUsed[i] = true;
	}
}

FibHeap::~FibHeap() {
	for (size_t i = 0; i < cHandles.size(); i++) {
		if (cHandles[i] == NULL) {
			continue;
		}

		delete cHandles[i];
	}
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

void FibHeap::fUpdate(int iIndex) {
	if (iIndex < 0 || iIndex > cHandles.size() - 1) {
		ERR("FibHeap::fUpdate: Index out of range" << endl);
		return;
	}

	if (cHandleUsed[iIndex] == false) {
		return;
	}

	cHeap.update(*(cHandles[iIndex]));
}

int FibHeap::fTopIndex() {
	if (cHeap.empty()) {
		ERR("FibHeap::fTopIndex: Heap is empty" << endl);
		return -1;
	}

	pair<int, int*> lTop = cHeap.top();
	if (*(lTop.second) == -1) {
		return -1;
	}
	return lTop.first;
}

int FibHeap::fTopVal() {
	if (cHeap.empty()) {
		ERR("FibHeap::fTopVal: Heap is empty" << endl);
		return -1;
	}

	pair<int, int*> lTop = cHeap.top();
	return *(lTop.second);
}

void FibHeap::fPop() {
	pair<int, int*> lTop = cHeap.top();
	cHeap.pop();
	cHandleUsed[lTop.first] = false;
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/
