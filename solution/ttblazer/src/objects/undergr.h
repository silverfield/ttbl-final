/* Underlying graph - an oriented weighted graph.
 *
 * As it has the graph object as the underlying structure, we call it a graph-like object. */
#ifndef UNDERGR_H_
#define UNDERGR_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <list>
#include <vector>
#include <map>
#include <sstream>

#include "../structures.h"
#include "graph.h"
#include "object.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define UG_NULL_LENGTH -1

/*****************************************************************/
/* Structs - Node and Arc data
/*****************************************************************/

struct UGArcData {
	std::vector<std::string> cLines;
	int cLength;

	UGArcData() {
		cLength = UG_NULL_LENGTH;
	}

	std::string fGetLengthString() {
		if (cLength == UG_NULL_LENGTH) {
			return NULL_STR;
		}

		return fIntToStr(cLength);
	}

	std::string fGetLinesString() {
		if (cLines.size() == 0) {
			return NULL_STR;
		}

		std::stringstream lResult (std::stringstream::out);
		for (std::vector<std::string>::iterator line = cLines.begin(); line != cLines.end(); line++) {
			if (line != cLines.begin()) {
				lResult << " ";
			}
			lResult << *line;
		}

		return lResult.str();
	}

	int fGetSpace() const {
		int lBytes = 0;

		lBytes += fSizeOf(cLines);
		for (std::vector<std::string>::const_iterator i = cLines.begin(); i != cLines.end(); i++) {
			lBytes += i->length();
		}

		return lBytes;
	}
};

struct UGNodeData {
	std::string cName;
	double cCoorX;
	double cCoorY;
	bool cCoor;

	UGNodeData() {
		cName = "";
		cCoorX = 0;
		cCoorY = 0;
		cCoor = false;
	}

	std::string fGetId() {
		return cName;
	}

	std::string fGetCoorString() {
		if (cCoor == false) {
			return NULL_STR;
		}

		return fDoubleToStr(cCoorX) + " " + fDoubleToStr(cCoorY);
	}

	int fGetSpace() const {
		int lBytes = 0;

		lBytes += cName.length();

		return lBytes;
	}
};

/*****************************************************************/
/* Class - UgGraph
/*****************************************************************/

class UgGraph : public GraphObject {
private:
	//----- data -----
	Graph<UGNodeData, UGArcData> cGraph;

	//----- implementation -----
	virtual inline GraphInterface* fGraphInterface() {return &cGraph;};
	void fWriteObject(std::ostream *iStringStream, bool iForSave);
public:
	//----- constructors -----
	UgGraph();

	//----- required interface - Object -----
	virtual std::string fGetObjectString(bool iForSave);
	virtual Ret fSaveObject(const char *iFileName);
	virtual Ret fLoadObject(const char *iFileName);

	virtual std::string fGetBasicFactsString();
	virtual int fGetSpace();
	virtual inline ObjType fGetObjectType() {return OTUg;};
	virtual Object* fGetCopy() {UgGraph *lGraph = new UgGraph(); *lGraph = *this; return lGraph;};

	//----- required interface - GraphObject -----
	virtual std::string fGetNodeString(int iIndex);
	virtual std::string fGetArcString(int iFrom, int iTo);

	virtual inline std::string fGetNodeId(int iX) {return cGraph.fGetNodeData(iX)->fGetId();};
	std::string fGetNodeName(int iX);
	virtual std::string fGetArcsString(int iX);

	virtual int fGetPathLength(Path iPath);
	virtual std::string fGetPathString(Path iPath, std::string iSep);
	virtual bool fIsWeighted();

	virtual inline const GraphInterface* fGetGraphInterface() {return &cGraph;};
	virtual std::vector<SPQuery> fGetSPInstances(int iCount);
	virtual int fGetLength(int iFrom, int iTo, int iTime);
	virtual inline std::vector<int> fGetTo(int iX) {return cGraph.fGetTo(iX);};
	GraphObject* fSubGraph(std::vector<int> iIndices);

	//----- interface -----
	void fSetGraph(const Graph<UGNodeData, UGArcData> *iGraph);
	std::vector<std::string> fGetNodeNames();
	Path fMakePath(Connection &iConnection);

	//----- inline interface -----
	inline UGNodeData* fGetNodeData(int iX) {return cGraph.fGetNodeData(iX);};
	inline UGArcData* fGetArcData(int iX, int iY) {return cGraph.fGetArcData(iX, iY);};
	inline boost::unordered_map<int, Arc<UGArcData> > fGetArcs(int iX) {return cGraph.fGetArcs(iX);};
	inline const Graph<UGNodeData, UGArcData> * fGetGraph() {return &cGraph;};
};

#endif /* UNDERGR_H_ */
