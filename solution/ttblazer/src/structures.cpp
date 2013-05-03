/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"

#include "structures.h"

using namespace std;

/*****************************************************************/
/* Variables
/*****************************************************************/

CityPathHash gCityPathHash;
PathHash gPathHash;

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class - Set
/*****************************************************************/

bool fCompareVectorSize(vector<int> iX, vector<int> iY) {
	return (iX.size() > iY.size());
}

void Partition::fSortBySize() {
	//we sort in DESCENDING order
	sort(cSets.begin(), cSets.end(), fCompareVectorSize);
}

int Partition::fAddSet() {
	vector<int> lNewSet;
	cSets.push_back(lNewSet);

	return cSets.size() - 1;
}

vector<int> Partition::fGetSet(int iIndex) {
	if (iIndex < 0 || iIndex > cSets.size()) {
		return vector<int>();
	}

	return cSets[iIndex];
}

int Partition::fGetVertexSet(int iVertex) {
	if (cVertex2SetIndex.find(iVertex) == cVertex2SetIndex.end()) {
		return -1;
	}

	return cVertex2SetIndex[iVertex];
}

void Partition::fAddToSet(int iIndex, int iVertex) {
	if (iIndex < 0 || iIndex > cSets.size()) {
		return;
	}

	cSets[iIndex].push_back(iVertex);
	cVertex2SetIndex.insert(pair<int, int>(iVertex, iIndex));
}

