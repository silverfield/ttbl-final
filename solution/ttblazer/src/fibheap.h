/* Wrapper for Fibonnachi heap */
#ifndef FIBHEAP_T
#define FIBHEAP_T

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"

#include <boost/heap/fibonacci_heap.hpp>
#include <vector>

/*****************************************************************/
/* Class
/*****************************************************************/

struct DijkstraComp : public std::binary_function<std::pair<int, int*>, std::pair<int, int*>, bool>
{
    bool operator()(std::pair<int, int*> iX, std::pair<int, int*> iY) const {
    	return (*(iX.second) == -1 || (*(iY.second) != -1 && *(iX.second) > *(iY.second)));
    }
};

class FibHeap {
	typedef boost::heap::fibonacci_heap<std::pair<int, int*>, boost::heap::compare<DijkstraComp> > heap_t;
	typedef boost::heap::fibonacci_heap<std::pair<int, int*>,
			boost::heap::compare<DijkstraComp> >::handle_type handle_t;
	heap_t cHeap;

	std::vector<handle_t*> cHandles;
	std::vector<bool> cHandleUsed;
public:
	FibHeap(std::vector<int> &iValues);
	~FibHeap();

	void fUpdate(int iIndex);
	int fTopIndex();
	int fTopVal();
	void fPop();
	bool fEmpty() {return cHeap.empty();};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* FIBHEAP_T */
