/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>
#include <set>

#include "../central.h"
#include "../algdijkstra.h"

#include "tg_neural.h"

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

TgNeural::TgNeural(vector<string> iArguments) {
	cPreprocessed = false;

	cUgGraph = NULL;
	cNeuralNet = NULL;

	cLayers = -1;
	cLayerSize = -1;
	cTrainEx = -1;
	cEstValRatio = TG_NEURAL_DEF_ESTVAL;
	cAlpha = TG_NEURAL_DEF_ALPHA;
	cMaxIt = -1;
	cMinIt = -1;

	cArgs = iArguments;

	fProcessArgs();
}

TgNeural::~TgNeural() {
	delete cNeuralNet;
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Ret TgNeural::fPreprocess(Object *iObject) {
	cGraph = dynamic_cast<TimeGraphObject*>(iObject);

	//----- prerequisites -----
	if (cLayers == -1) {
		ERR("TgNeural::fPreprocess: Number of hidden layers not set" << endl);
		return NOK;
	}
	if (cLayerSize == -1) {
		ERR("TgNeural::fPreprocess: Hidden layer size not set" << endl);
		return NOK;
	}
	if (cTrainEx == -1) {
		ERR("TgNeural::fPreprocess: Number of training examples not set" << endl);
		return NOK;
	}
	if (cEstValRatio == -1) {
		ERR("TgNeural::fPreprocess: Estimation ratio is not set" << endl);
		return NOK;
	}
	if (cAlpha == -1) {
		ERR("TgNeural::fPreprocess: Alpha is not set" << endl);
		return NOK;
	}

	//----- preprocessing -----
	INFO("Starting preprocessing of Neural network oracle (for TE graphs)" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//build Id->Index map
	INFO("Building ID->Index map" << endl);
	cGraph->fComputeIdIndexMap();

	//get all cities
	INFO("Getting cities" << endl);
	vector<string> cIndexToCity = cGraph->fGetCities();
	int c = cIndexToCity.size();
	for (size_t i = 0; i < c; i++) {
		cCitiesToIndex[cIndexToCity[i]] = i;
	}

	int n = cGraph->fGetN();

	//get the underlying graph of the TE graph
	INFO("Gonna make the underlying graph of the given TE graph" << endl);
	cUgGraph = cGraph->fToUgGraph();
	DEB(LVL_OR_TGNEURAL, "TgNeural::fPreprocess: UG graph: " << cUgGraph->fGetObjectString(false)
			<< endl);

	//make edge->index and index->edge translations
	INFO("Making edge->index and index->edge translations" << endl);
	cIndexToEdge.reserve(cUgGraph->fGetM());
	cEdgeToIndex = vector<map<int, int> >(cUgGraph->fGetN(), map<int, int>());
	for (int i = 0; i < cUgGraph->fGetN(); i++) {
		unordered_map<int, Arc<UGArcData> > lArcs = cUgGraph->fGetArcs(i);
		for (unordered_map<int, Arc<UGArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			cIndexToEdge.push_back(pair<int, int>(i, j->first));
			cEdgeToIndex[i][j->first] = cIndexToEdge.size() - 1;
		}
	}

	//get events
	INFO("Getting events" << endl);
	vector<Event> lEvents = cGraph->fGetEvents(unordered_set<string>(), ANY_RANGE);
	cEventCount = lEvents.size();
	for (size_t i = 0; i < cEventCount; i++) {
		if (cEventToIndex.count(lEvents[i].cCity) == 0) {
			cEventToIndex[lEvents[i].cCity] = map<int, int>();
		}

		cEventToIndex[lEvents[i].cCity][lEvents[i].cTime] = i;
	}

	//create neural network
	INFO("Create neural network" << endl);
	cNeuralNet = new NeuralNet(cEventCount + c, cIndexToEdge.size(), cLayers, cLayerSize);
	cNeuralNet->fSetOutputAct(FUNC_SIGM);

	//get train set
	INFO("Going to get the train set" << endl);
	vector<pair<vector<double>, vector<double> > > lTrainSet;
	for (int i = 0; i < cTrainEx; i++) {
		int lDepartEvent = rand() % cEventCount;
		int lArriveCity = rand() % c;

		Connection lConn = fDijkWrapConn(cGraph, lEvents[lDepartEvent].cCity, lEvents[lDepartEvent].cTime,
				cIndexToCity[lArriveCity]);
		Path lUsp = cUgGraph->fMakePath(lConn);
		if (lUsp.fGetSize() == 0) {
			continue;
		}

		lTrainSet.push_back(pair<vector<double>, vector<double> >(
				vector<double>(cEventCount + c, 0),
				vector<double>(cIndexToEdge.size(), 0))
		);

		int lTrainExample = lTrainSet.size() - 1;

		//input
		vector<double> *lInput = &(lTrainSet[lTrainExample].first);
		(*lInput)[lDepartEvent] = 1;
		(*lInput)[cEventCount + lArriveCity] = 1;
		INFO("	" << lTrainExample << ". From " << lEvents[lDepartEvent].fGetString() << ". To "
				<< cIndexToCity[lArriveCity] << endl);

		//output
		vector<double> *lOutput = &(lTrainSet[lTrainExample].second);
		for (int j = 0; j < lUsp.fGetSize() - 1; j++) {
			string lFromCityName = cGraph->fGetNodeName(lUsp.fGet(j));
			string lToCityName = cGraph->fGetNodeName(lUsp.fGet(j + 1));
			int lFromUgIndex = cUgGraph->fGetNodeIndex(lFromCityName);
			int lToUgIndex = cUgGraph->fGetNodeIndex(lToCityName);

			//disregard waiting in a city
			if (lFromUgIndex == lToUgIndex) {
				continue;
			}

			int lEdgeIndex = cEdgeToIndex[lFromUgIndex][lToUgIndex];
			(*lOutput)[lEdgeIndex] = 1;
		}
	}
	DEB(LVL_OR_TGNEURAL, "TgNeuralEa::fPreprocess: TRAIN set: " << endl);
	for (vector<pair<vector<double>, vector<double> > >::size_type i = 0; i < lTrainSet.size(); i++) {
		DEB(LVL_OR_TGNEURAL, "	" << i << endl);
		DEB(LVL_OR_TGNEURAL, "		" << fGetVectorString(lTrainSet[i].first) << endl);
		DEB(LVL_OR_TGNEURAL, "		" << fGetVectorString(lTrainSet[i].second) << endl);
	}
	INFO("Actual size of the train set is " << lTrainSet.size() << " after discarding unreachable "
			"pairs" << endl);

	//train the network
	INFO("Going to train the neural network" << endl);
	int lIter = -1;
	double lValWa = -1;
	double lEstWa = -1;
	double lEstErr = -1;
	cNeuralNet->fTrain(lTrainSet, cEstValRatio, cAlpha, cMaxIt, cMinIt, &lValWa, &lEstWa, &lEstErr, &lIter);
	INFO("Neural network was trained after " << lIter << " iterations" << endl);
	INFO("Estimation error: " << lEstErr << endl);
	INFO("Wrong answers on validation data: " << lValWa * 100 << "%" << endl);
	INFO("Wrong answers on estimation data: " << lEstWa * 100 << "%" << endl);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending preprocessing of Neural network oracle (for TE graphs)" << endl);

	cPreprocessed = true;

	return OK;
}

int TgNeural::fGetSpace() {
	int lBytes = 0;

	lBytes += fSizeOf(cArgs);
	for (vector<string>::iterator i = cArgs.begin(); i != cArgs.end(); i++) {
		lBytes += i->length();
	}

	lBytes += cUgGraph->fGetSpace();
	lBytes += fSizeOf(cCitiesToIndex);
	lBytes += fSizeOf(cIndexToCity);
	for (map<string, int>::iterator i = cCitiesToIndex.begin(); i != cCitiesToIndex.end(); i++) {
		lBytes += i->first.length();
	}
	for (vector<string>::iterator i = cIndexToCity.begin(); i != cIndexToCity.end(); i++) {
		lBytes += i->length();
	}
	lBytes += fSizeOf(cEventToIndex);
	for (map<string, map<int, int> >::iterator i = cEventToIndex.begin(); i != cEventToIndex.end(); i++) {
		lBytes += i->first.length();
		lBytes += fSizeOf(i->second);
	}
	lBytes += cNeuralNet->fGetSpace();
	lBytes += fSizeOf(cIndexToEdge);
	lBytes += fSizeOf(cEdgeToIndex);
	for (vector<map<int, int> >::iterator i = cEdgeToIndex.begin(); i != cEdgeToIndex.end(); i++) {
		lBytes += fSizeOf(*i);
	}

	lBytes += sizeof(*this);

	return lBytes;
}

Connection TgNeural::fQueryConn(string iFrom, int iTime, string iTo) {
	if (cPreprocessed == false) {
		ERR("TgNeural::fQueryConn: Preprocessing not carried out" << endl);
		return Connection();
	}

	//----- compute -----
	INFO("Starting querying Neural network oracle (for Time graphs) for the connections" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//get indices
	map<string, map<int, int> >::iterator lFromCityIt = cEventToIndex.find(iFrom);
	if (lFromCityIt == cEventToIndex.end()) {
		ERR("City " << iFrom << " not found" << endl);
		return Connection();
	}
	map<int, int>::iterator lFromEventIt = lFromCityIt->second.find(iTime);
	if (lFromEventIt == lFromCityIt->second.end()) {
		ERR("Event " << iFrom << "/" << iTime << " not found" << endl);
		return Connection();
	}
	int lFromEvent = lFromEventIt->second;
	int lToCity = cCitiesToIndex[iTo];

	INFO("Indices: From event = " << lFromEvent << ". To city (index) = " << lToCity << endl);

	//adjust for input to the neural network
	INFO("Adjusting for input to the neural network" << endl);
	vector<double> lInput(cEventCount + cIndexToCity.size(), 0);
	lInput[lFromEvent] = 1;
	lInput[cEventCount + lToCity] = 1;

	//run computation in neural network
	INFO("Computation by neural network" << endl);
	vector<double> lOutput = cNeuralNet->fCompute(lInput);
	if (lOutput.size() == 0) {
		ERR("Unable to compute the EA value" << endl);
		return Connection();
	}
	DEB(LVL_OR_TGNEURAL, "Computed output: " << fGetVectorString(lOutput) << endl);

	//obtain UG path from the result
	INFO("Obtaining the UG path" << endl);
	vector<string> lUgCities;
	vector<bool> lUsedInPath(cUgGraph->fGetN(), false);
	int lCurUgIndex = cUgGraph->fGetNodeIndex(iFrom);
	int lToUgIndex = cUgGraph->fGetNodeIndex(iTo);
	while (lCurUgIndex != lToUgIndex) {
		lUgCities.push_back(cUgGraph->fGetNodeName(lCurUgIndex));
		lUsedInPath[lCurUgIndex] = true;

		unordered_map<int, Arc<UGArcData> > lArcs = cUgGraph->fGetArcs(lCurUgIndex);

		//get next vertex for the path - based on maximum value on the output layer
		int lNewCurUgIndex = -1;
		double lMaxNeuron = 0;
		for (unordered_map<int, Arc<UGArcData> >::iterator j = lArcs.begin(); j != lArcs.end(); j++) {
			int lEdgeIndex = cEdgeToIndex[lCurUgIndex][j->first];
			if (lUsedInPath[j->first] == false && lOutput[lEdgeIndex] > lMaxNeuron) {
				lMaxNeuron = lOutput[lEdgeIndex];
				lNewCurUgIndex = j->first;
			}
		}

		lCurUgIndex = lNewCurUgIndex;
		if (lCurUgIndex == -1) {
			break;
		}
	}

	//it might be we did not even found some path
	if (lCurUgIndex == -1) {
		INFO("Network was unable to find the path to the destination" << endl);
		DEB(LVL_OR_TGNEURAL, "TgNeural::fPreprocess: Incomplete path found: " <<
				fGetVectorString(lUgCities) << endl);
		if (lRet == OK) {gAlgTimer->fStopTimer();}
		return Connection();
	}

	lUgCities.push_back(cUgGraph->fGetNodeName(lToUgIndex));
	INFO("UG path obtained: " << fGetVectorString(lUgCities) << endl);

	//get the connection in TG graph
	INFO("Converting the UG path to the connection" << endl);
	Connection lConn = cGraph->fMakeConnection(lUgCities, iTime);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying Neural network oracle (for TE graphs) for the connections" << endl);

	return lConn;
}

int TgNeural::fQueryEa(string iFrom, int iTime, string iTo) {
	INFO("Starting querying Neural network oracle (for Time graphs) for the earliest arrival" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Connection lConn = fQueryConn(iFrom, iTime, iTo);

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying Neural network oracle (for Time graphs) for the earliest arrival" << endl);

	return lConn.fGetLength();
}

string TgNeural::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP Neural network oracle (for TE graphs)" << endl <<
			"	" << TG_NEURAL_CMD_LAYERS << " {layer count, layer size}: set the number of hidden layers"
					" and the size of a hidden layer" << endl <<
			"	" << TG_NEURAL_CMD_TRAIN << " {number}: set the number of training examples" << endl <<
			"	" << TG_NEURAL_CMD_ESTVAL << " {1 - 100%}: set the percentage of train data to be used in "
					"estimation set. Default " << TG_NEURAL_DEF_ESTVAL << endl <<
			"	" << TG_NEURAL_CMD_ALPHA << " {alpha}: set the parameter alpha used for training the "
					"neural network. Default " << TG_NEURAL_DEF_ALPHA << endl <<
			"	" << TG_NEURAL_CMD_MAXIT << " {max-it}: set the maximum number of iterations. Default is "
					"unset" << endl <<
			"	" << TG_NEURAL_CMD_MINIT << " {min-it}: set the minimum number of iterations. Default is "
					"unset" << endl;

	return lSs.str();
}

set<QueryType> TgNeural::fSQueryTypes() {
	set<QueryType> lQTypes;
	lQTypes.insert(QTEa);
	lQTypes.insert(QTConn);

	return lQTypes;
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TgNeural::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TgNeural::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TgNeural::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(TG_NEURAL_CMD_LAYERS) {
			MOVEARG;
			cLayers = fStrToInt(lCurArg);
			MOVEARG
			cLayerSize = fStrToInt(lCurArg);
		}
		else COMP(TG_NEURAL_CMD_TRAIN) {
			MOVEARG;
			cTrainEx = fStrToInt(lCurArg);
		}
		else COMP(TG_NEURAL_CMD_ESTVAL) {
			MOVEARG;
			cEstValRatio = fStrToInt(lCurArg);
			if (cEstValRatio < 1 || cEstValRatio > 100) {
				cEstValRatio = TG_NEURAL_DEF_ESTVAL;
				UNKNOWN;
			}
		}
		else COMP(TG_NEURAL_CMD_ALPHA) {
			MOVEARG;
			cAlpha = fStrToDouble(lCurArg);
			if (cAlpha < 0) {
				cAlpha = TG_NEURAL_DEF_ALPHA;
				UNKNOWN;
			}
		}
		else COMP(TG_NEURAL_CMD_MAXIT) {
			MOVEARG;
			cMaxIt = fStrToInt(lCurArg);
			if (cMaxIt < 0) {
				cMaxIt = -1;
				UNKNOWN;
			}
		}
		else COMP(TG_NEURAL_CMD_MINIT) {
			MOVEARG;
			cMinIt = fStrToInt(lCurArg);
			if (cMinIt < 0) {
				cMinIt = -1;
				UNKNOWN;
			}
		}
		else {
			UNKNOWN;
		}
	}

	DEB(LVL_CONN, "TgNeural::fProcessArgs: Arguments processed" << endl);
}
