/*****************************************************************/
/* Includes
/*****************************************************************/

#include <boost/heap/fibonacci_heap.hpp>

#include <algorithm>
#include <fstream>

#include "central.h"
#include "objects/undergr.h"
#include "objects/timedep.h"
#include "objects/timeexp.h"
#include "objects/timetable.h"
#include "neuralnet.h"
#include "fibheap.h"

#include "tests.h"

using namespace std;
using namespace boost;

#define STVEC(mType, mName, ...) vector<mType> mName; \
	if (1) { \
		mType ST_INIT [] = { __VA_ARGS__ }; \
		mName.resize(sizeof(ST_INIT) / sizeof(ST_INIT[0])); \
		copy(ST_INIT, ST_INIT + sizeof(ST_INIT) / sizeof(ST_INIT[0]), mName.begin()); \
	}

/*****************************************************************/
/* Functions
/*****************************************************************/

Ret fRunTests() {
	DEB(LVL_MAIN, "Main::main: Current directory is " << fExec("pwd") << std::endl);

	vector<int> lDistances(100, -1);
	FibHeap lDistancesHeap(lDistances);
	lDistances[20] = 0;

	INFO("top = " << lDistancesHeap.fTopIndex() << "/" << lDistancesHeap.fTopVal() << endl);



	STVEC(int, lVec, 100, 54, 143, 231, -1, 133, 10);

	FibHeap lFib (lVec);

	INFO("top = " << lFib.fTopIndex() << "/" << lFib.fTopVal() << endl);

	lVec[3] = 270;
	lFib.fUpdate(3);
	lVec[1] = 5;
	lFib.fUpdate(1);

	INFO("top = " << lFib.fTopIndex() << "/" << lFib.fTopVal() << endl);

//	UgGraph lUg;
//	lUg.fLoadObject("/home/ferrard/workspace/dipl/gendata/ug.ug");
//	INFO(lUg.fGetObjectString(false) << std::endl);
//	lUg.fSaveObject("bordel/ug.ug");
//
//	TdGraph lTd;
//	lTd.fChangeTimeFormat(gOptions->cTimeFormat);
//	lTd.fLoadObject("/home/ferrard/workspace/dipl/gendata/td.td");
//	lTd.fSaveObject("bordel/td_time.td");
//	lTd.fSaveObject("bordel/td.td");
//	INFO(lTd.fGetObjectString(true));
//
//	TeGraph lTe;
//	lTe.fChangeTimeFormat(gOptions->cTimeFormat);
//	lTe.fLoadObject("/home/ferrard/workspace/dipl/gendata/te.te");
//	lTe.fSaveObject("bordel/te_time.te");
//	lTe.fSaveObject("bordel/te.te");
//	INFO(lTe.fGetObjectString(true));
//
//	Timetable lTt;
//	lTt.fChangeTimeFormat(gOptions->cTimeFormat);
//	lTt.fLoadObject("/home/ferrard/workspace/dipl/gendata/tt.tt");
//	lTt.fSaveObject("bordel/tt_time.tt");
//	lTt.fSaveObject("bordel/tt.tt");
//	INFO(lTt.fGetObjectString(true));
//
//	string lLongString = "";
//	for (int i = 0; i < MSG_BUFF_SIZE / 2; i++) {
//		lLongString += fIntToStr(i) + "\n";
//	}
//	//INFO(lLongString << endl);

//	ifstream lFile;
//	char lFileName [] = "2d.trn.pac";
//	lFile.open(lFileName);
//
//	vector<pair<vector<double>, vector<double> > > lTrainSet;
//
//	string lLine;
//	int lPos = 0;
//	while (lFile.eof() == false) {
//		lPos++;
//		getline(lFile, lLine);
//		if (lFile.eof()) {
//			break;
//		}
//
//		vector<string> lItems = fSplitString(lLine, " ");
//		DEB(LVL_TESTS, "Training example " << lPos << " for neural network " <<
//				fGetVectorString(lItems) << endl);
//
//		STVEC(double, lInput, fStrToDouble(lItems[0]), fStrToDouble(lItems[1]));
//		vector<double> lOutput(3, 0);
//		lOutput[fStrToInt(lItems[2])] = 1;
//
//		lTrainSet.push_back(pair<vector<double>, vector<double> >(lInput, lOutput));
//	}
//
//
//	NeuralNet lNet(2, 3, 3, 3);
//
//	lNet.fTrain(lTrainSet, 80, 0.7, -1, -1);
//
//	NeuralNet lNet(3, 2, 2, 2);
//	vector<pair<vector<int>, vector<double> > > lEstSet;
//	if (1) {
//		STVEC(int, lInput, 1, 1, 0);
//		STVEC(double, lOutput, 1, 1);
//		lEstSet.push_back(pair<vector<int>, vector<double> >(lInput, lOutput));
//	}
//	if (1) {
//		STVEC(int, lInput, 1, 0, 1);
//		STVEC(double, lOutput, 1, 1);
//		lEstSet.push_back(pair<vector<int>, vector<double> >(lInput, lOutput));
//	}
//	if (1) {
//		STVEC(int, lInput, 1, 0, 0);
//		STVEC(double, lOutput, 0, 1);
//		lEstSet.push_back(pair<vector<int>, vector<double> >(lInput, lOutput));
//	}
//	if (1) {
//		STVEC(int, lInput, 0, 1, 0);
//		STVEC(double, lOutput, 0, 1);
//		lEstSet.push_back(pair<vector<int>, vector<double> >(lInput, lOutput));
//	}
//
//	vector<pair<vector<int>, vector<double> > > lValSet;
//	if (1) {
//		STVEC(int, lInput, 0, 0, 1);
//		STVEC(double, lOutput, 0, 1);
//		lValSet.push_back(pair<vector<int>, vector<double> >(lInput, lOutput));
//	}
//	if (1) {
//		STVEC(int, lInput, 0, 1, 1);
//		STVEC(double, lOutput, 1, 1);
//		lValSet.push_back(pair<vector<int>, vector<double> >(lInput, lOutput));
//	}
//
//	lNet.fTrain(lValSet, lEstSet, 0.1);

	return OK;
}
