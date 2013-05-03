/*****************************************************************/
/* Includes
/*****************************************************************/

#include "../central.h"

#include "object.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Variables
/*****************************************************************/

/*****************************************************************/
/* Functions
/*****************************************************************/

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

/*****************************************************************/
/* Class - interface
/*****************************************************************/

int GraphObject::fGetNodeIndex(string iId) {
	if (cIdIndexMapValid == false) {
		ERR("GraphObject::fGetNodeIndex: ID->Index map invalid" << endl);
		return -1;
	}

	unordered_map<string, int>::iterator lFound = cIdIndexMap.find(iId);
	if (lFound == cIdIndexMap.end()) {
		ERR("GraphObject::fGetNodeIndex: ID not found" << endl);
		return -1;
	}

	return lFound->second;
}

void GraphObject::fComputeIdIndexMap() {
	cIdIndexMap.clear();
	cIdIndexMapValid = false;
	vector<int> lIndices = fGetGraphInterface()->fGetNodeIndices();

	for (vector<int>::iterator i = lIndices.begin(); i != lIndices.end(); i++) {
		string lId = fGetNodeId(*i);
		pair<unordered_map<std::string, int>::iterator, bool> lReturn = cIdIndexMap.insert(
				pair<string, int>(lId, *i));
		if (lReturn.second == false) {
			ERR("GraphObject::fComputeIdIndexMap: Unable to compute ID -> Index map. Duplicate ID: " <<
					lId << endl);
			return;
		}
	}

	cIdIndexMapValid = true;

	DEB(LVL_UNDERGR, "GraphObject::fComputeIdIndexMap: Id -> Index map computed" << endl);
}

void GraphObject::fAddIdIndex(string iId, int iIndex) {
	unordered_map<string, int>::iterator lFound = cIdIndexMap.find(iId);
	if (lFound != cIdIndexMap.end()) {
		ERR("GraphObject::fAddIdIndex: ID already in the map" << endl);
		return;
	}

	cIdIndexMap.insert(pair<string, int>(iId, iIndex));
}

void GraphObject::fRemIdIndex(string iId) {
	unordered_map<string, int>::iterator lFound = cIdIndexMap.find(iId);

	if (lFound == cIdIndexMap.end()) {
		ERR("GraphObject::fRemIdIndex: ID not in the map" << endl);
		return;
	}

	cIdIndexMap.erase(lFound);
}

int GraphObject::fGetSpace() {
	int lBytes = 0;

	lBytes += fSizeOf(cIdIndexMap);
	for (unordered_map<string, int>::iterator i = cIdIndexMap.begin();
			i != cIdIndexMap.end(); i++) {
		lBytes += i->first.length();
	}

	return lBytes;
}

vector<int> GraphObject::fGetDegrees() {
	vector<int> lDegrees;

	lDegrees.reserve(fGetGraphInterface()->fGetN());
	for (size_t i = 0; i < fGetN(); i++) {
		lDegrees.push_back(fGetGraphInterface()->fGetDeg(i));
	}

	return lDegrees;
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/
