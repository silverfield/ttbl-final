/*****************************************************************/
/* Includes
/*****************************************************************/

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "../../../common/src/structsizer.h"

#include "../central.h"

#include "undergr.h"

using namespace std;
using namespace boost;

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

UgGraph::UgGraph() {
	cGraph = Graph<UGNodeData, UGArcData>();

	cIdIndexMapValid = false;
}

/*****************************************************************/
/* Interface
/*****************************************************************/

string UgGraph::fGetObjectString(bool iForSave) {
	stringstream lResult (stringstream::out);

	DEB(LVL_UNDERGR, "UgGraph::fGetObjectString: going to get the graph string" << endl);

	fWriteObject(&lResult, iForSave);

	return lResult.str();
}

Ret UgGraph::fSaveObject(const char *iFileName) {
	ofstream lFile;
	lFile.open(iFileName);

	if (!lFile) {
		ERR("UgGraph::fSaveObject: failed to open file " << iFileName << endl);
		return NOK;
	}

	fWriteObject(&lFile, true);
	lFile.close();

	return OK;
}

Ret UgGraph::fLoadObject(const char *iFileName) {
	ifstream lFile;
	lFile.open(iFileName);

	if (!lFile) {
		ERR("UgGraph::fLoadObject: failed to open file " << iFileName << endl);
		return NOK;
	}

	int lN = 0;
	int lM = 0;

	lFile >> lN;
	lFile >> lM;
	string lLine;
	getline(lFile, lLine);

	//nodes
	unordered_map<string, int> lNameToIndex;
	for (int i = 0; i < lN; i++) {
		//read
		getline(lFile, lLine);
		vector<string> lItems = fSplitString(lLine, " ");
		//fPrintVectorString(lItems);

		//add node
		DEBF(LVL_UNDERGR, "UgGraph::fLoadObject: adding node %s\n", lItems[0].c_str());
		int lNodeIndex = fAddNode();

		//add node data
		UGNodeData *lData = fGetNodeData(lNodeIndex);
		lData->cName = lItems[0];
		if (lItems[1] != NULL_STR) {
			if (lItems.size() != 3) {
				ERR("UgGraph::fLoadObject: Incorrect format" << endl);
				return NOK;
			}
			//we have coordintes
			lData->cCoor = true;
			lData->cCoorX = fStrToDouble(lItems[1]);
			lData->cCoorY = fStrToDouble(lItems[2]);
		}

		lNameToIndex.insert(pair<string, int>(lData->cName, lNodeIndex));
	}

	DEB(LVL_UNDERGR, "UgGraph::fLoadObject: nodes loaded" << endl);
	DEB(LVL_UNDERGR, "UgGraph::fLoadObject: name-to-index " << fGetMapString(lNameToIndex) << endl);

	//edges
	for (int i = 0; i < lM; i++) {
		//read
		getline(lFile, lLine);
		vector<string> lItems = fSplitString(lLine, " ");

		if (lItems.size() < 4) {
			ERR("UgGraph::fLoadObject: Incorrect format" << endl);
			return NOK;
		}

		//add arc
		int lFromI = lNameToIndex[lItems[0]];
		int lToI = lNameToIndex[lItems[1]];
		DEBF(LVL_UNDERGR, "UgGraph::fLoadObject: adding arc %d->%d\n", lFromI, lToI);
		fAddArc(lFromI, lToI);

		//add arc data
		UGArcData *lArcData = fGetArcData(lFromI, lToI);
		if (lItems[2] != NULL_STR) {
			//we have length
			lArcData->cLength = fStrToInt(lItems[2]);
		}
		if (lItems[3] != NULL_STR) {
			//we have lines
			lArcData->cLines.insert(lArcData->cLines.begin(), lItems.begin() + 3, lItems.end());
		}
	}

	DEB(LVL_UNDERGR, "UgGraph::fLoadObject: edges loaded" << endl);

	cIdIndexMapValid = false;

	lFile.close();

	return OK;
}

std::string UgGraph::fGetBasicFactsString() {
	stringstream lResult (stringstream::out);

	lResult << "Nodes: " << fGetN() << endl;
	lResult << "Edges: " << fGetM() << endl;
	lResult << "II map valid: " << gBoolStr[cIdIndexMapValid] << endl;
	lResult << "Size: " << fGetSizeString(sizeof(*this) + fGetSpace()) << endl;

	return lResult.str();
}

int UgGraph::fGetSpace() {
	int lBytes = 0;

	lBytes += GraphObject::fGetSpace();
	lBytes += cGraph.fGetSpace();

	return lBytes;
}

string UgGraph::fGetNodeString(int iIndex) {
	if (cGraph.fNodeIndexExists(iIndex) == false) {
		ERR("UgGraph::fGetNodeString: Node with specified index does not exist" << endl);
		return "";
	}

	stringstream lSs (stringstream::out);
	lSs << iIndex << ": " << cGraph.fGetNodeData(iIndex)->fGetId() << endl;
	lSs << "	Coor: " << cGraph.fGetNodeData(iIndex)->fGetCoorString() << endl;
	lSs << "	Deg: " << cGraph.fGetDeg(iIndex) << endl;
	lSs << "	Neighs: " << fGetArcsString(iIndex) << endl;

	return lSs.str();
}

string UgGraph::fGetArcString(int iFrom, int iTo) {
	if (cGraph.fArcExists(iFrom, iTo) == false) {
		ERR("UgGraph::fGetArcString: Arc does not exist" << endl);
		return "";
	}

	UGArcData *lArcData = cGraph.fGetArcData(iFrom, iTo);

	string lArcString = "";
	lArcString += cGraph.fGetNodeData(iFrom)->fGetId() + " ";
	lArcString += cGraph.fGetNodeData(iTo)->fGetId() + " ";
	lArcString += lArcData->fGetLengthString() + " ";
	lArcString += lArcData->fGetLinesString();

	return lArcString;
}

std::string UgGraph::fGetNodeName(int iX) {
	if (cGraph.fNodeIndexExists(iX) == false) {
		ERR("UgGraph::fGetNodeName: Node with specified index does not exist" << endl);
		return "";
	}

	return fGetNodeData(iX)->cName;
}

string UgGraph::fGetArcsString(int iX) {
	if (cGraph.fNodeIndexExists(iX) == false) {
		ERR("UgGraph::fGetArcsString: Node with specified index does not exist" << endl);
		return "";
	}

	unordered_map<int, Arc<UGArcData> > lArcs = fGetArcs(iX);
	string lResult = "";
	for (unordered_map<int, Arc<UGArcData> >::iterator i = lArcs.begin(); i != lArcs.end(); i++) {
		if (i != lArcs.begin()) {
			lResult += ", ";
		}
		lResult += fIntToStr(i->first) + " [" + fGetNodeId(i->first) + "]";
	}

	return lResult;
}

int UgGraph::fGetPathLength(Path iPath) {
	if (iPath.fGetSize() == 0) {
		return -1;
	}
	int lLength = 0;
	for (int i = 0; i < iPath.fGetSize() - 1; i++) {
		UGArcData *lArcData = fGetArcData(iPath.fGet(i), iPath.fGet(i + 1));
		if (lArcData->cLength == -1) {
			return iPath.fGetSize() - 1;
		}
		lLength += lArcData->cLength;
	}

	return lLength;
}

string UgGraph::fGetPathString(Path iPath, std::string iSep = ", ") {
	string lPathString = "";
	for (int i = 0; i < iPath.fGetSize(); i++) {
		string lNodeString = fGetNodeId(iPath.fGet(i));
		lPathString += lNodeString;
		if (i != iPath.fGetSize() - 1) {
			lPathString += iSep;
		}
	}

	return lPathString;
}

bool UgGraph::fIsWeighted() {
	for (size_t i = 0; i < fGetN(); i++) {
		unordered_map<int, Arc<UGArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<UGArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			Arc<UGArcData> *lArc = &(j->second);
			if (lArc->cData.cLength == UG_NULL_LENGTH || lArc->cData.cLength < 0) {
				return false;
			}
		}
	}

	return true;
}

vector<SPQuery> UgGraph::fGetSPInstances(int iCount) {
	vector<SPQuery> lResult;

	while (lResult.size() != iCount) {
		SPQuery lQuery;
		lQuery.cFrom = rand() % fGetN();
		lQuery.cTo = rand() % fGetN();
		if (lQuery.cFrom == lQuery.cTo) {
			continue;
		}

		lResult.push_back(lQuery);
	}

	return lResult;
}

int UgGraph::fGetLength(int iFrom, int iTo, int iTime = -1) {
	if (fArcExists(iFrom, iTo) == false) {
		ERR("UgGraph::fGetLength: Arc does not exist" << endl);
		return -1;
	}

	return fGetArcData(iFrom, iTo)->cLength;
}

GraphObject* UgGraph::fSubGraph(vector<int> iIndices) {
	UgGraph *lSubUgGraph = new UgGraph();

	Graph<UGNodeData, UGArcData> lSubGraph = cGraph.fSubGraph(iIndices);
	lSubUgGraph->fSetGraph(&lSubGraph);

	return lSubUgGraph;
}

void UgGraph::fSetGraph(const Graph<UGNodeData, UGArcData> *iGraph) {
	cGraph = *iGraph;

	cIdIndexMapValid = false;
}

vector<string> UgGraph::fGetNodeNames() {
	vector<string> lResult;
	lResult.reserve(fGetN());

	for (int i = 0; i < fGetN(); i++) {
		lResult.push_back(fGetNodeName(i));
	}

	return lResult;
}

Path UgGraph::fMakePath(Connection &iConnection) {
	vector<int> lPathIndices;

	string lLastCity = "";
	for (int i = 0; i < iConnection.fGetSize(); i++) {
		if (iConnection.fGetCity(i) != lLastCity) {
			lLastCity = iConnection.fGetCity(i);
			lPathIndices.push_back(fGetNodeIndex(lLastCity));
		}
	}

	return Path(lPathIndices);
}

/*****************************************************************/
/* Implementation
/*****************************************************************/

void UgGraph::fWriteObject(ostream *iStringStream, bool iForSave) {
	if (!iForSave) {
		(*iStringStream) << "N: ";
	}
	(*iStringStream) << fGetN() << endl;
	if (!iForSave) {
		(*iStringStream) << "M: ";
	}
	(*iStringStream) << fGetM() << endl;

	//nodes
	if (!iForSave) {
		(*iStringStream) << "NODES" << endl;
	}
	for (size_t i = 0; i < fGetN(); i++) {
		if (!iForSave) {
			(*iStringStream) << i << ": ";
		}
		(*iStringStream) << cGraph.fGetNodeData(i)->fGetId() << " ";
		(*iStringStream) << cGraph.fGetNodeData(i)->fGetCoorString() << endl;
	}

	//edges
	if (!iForSave) {
		(*iStringStream) << "EDGES" << endl;
	}
	int lEdgeCount = 0;
	for (size_t i = 0; i < fGetN(); i++) {
		int lNodeEdgeCount = 0;
		unordered_map<int, Arc<UGArcData> > lArcs = cGraph.fGetArcs(i);
		for (unordered_map<int, Arc<UGArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			lNodeEdgeCount++;
			lEdgeCount++;
			if (!iForSave) {
				(*iStringStream) << lEdgeCount << " (" << lNodeEdgeCount << "/" << i << "): ";
			}

			Arc<UGArcData> *lArc = &(j->second);
			string lFrom = cGraph.fGetNodeData(i)->fGetId();
			string lTo = cGraph.fGetNodeData(j->first)->fGetId();
			(*iStringStream) << lFrom << " " << lTo << " ";
			(*iStringStream) << lArc->cData.fGetLengthString() << " ";
			(*iStringStream) << lArc->cData.fGetLinesString();
			(*iStringStream) << endl;
		}
	}
}
