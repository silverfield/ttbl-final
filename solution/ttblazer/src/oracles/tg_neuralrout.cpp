/*****************************************************************/
/* Includes
/*****************************************************************/
#include <string>
#include <vector>
#include <set>

#include "../central.h"
#include "../algdijkstra.h"

#include "tg_neuralrout.h"

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

TgNeuralRout::TgNeuralRout(vector<string> iArguments) {
	cPreprocessed = false;

	cNeuralNet = NULL;

	cEventCount = -1;
	cLayers = -1;
	cLayerSize = -1;
	cTrainEx = -1;
	cEstValRatio = TG_NEURAL_DEF_ESTVAL;
	cAlpha = TG_NEURAL_DEF_ALPHA;
	cMinIt = -1;
	cMaxIt = -1;

	cArgs = iArguments;

	fProcessArgs();
}

TgNeuralRout::~TgNeuralRout() {
	delete cNeuralNet;
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

Ret TgNeuralRout::fPreprocess(Object *iObject) {
	cGraph = dynamic_cast<TimeGraphObject*>(iObject);

	//----- prerequisites -----
	if (cLayers == -1) {
		ERR("TgNeuralRout::fPreprocess: Number of hidden layers not set" << endl);
		return NOK;
	}
	if (cLayerSize == -1) {
		ERR("TgNeuralRout::fPreprocess: Hidden layer size not set" << endl);
		return NOK;
	}
	if (cTrainEx == -1) {
		ERR("TgNeuralRout::fPreprocess: Number of training examples not set" << endl);
		return NOK;
	}
	if (cEstValRatio == -1) {
		ERR("TgNeuralRout::fPreprocess: Estimation ratio is not set" << endl);
		return NOK;
	}
	if (cAlpha == -1) {
		ERR("TgNeuralRout::fPreprocess: Alpha is not set" << endl);
		return NOK;
	}

	//----- preprocessing -----
	INFO("Starting preprocessing of Neural network oracle (for TG graphs)" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	//build Id->Index map
	INFO("Building ID->Index map" << endl);
	cGraph->fComputeIdIndexMap();

	//get all cities
	INFO("Getting cities" << endl);
	cIndexToCity = cGraph->fGetCities();
	int c = cIndexToCity.size();
	for (size_t i = 0; i < c; i++) {
		cCitiesToIndex[cIndexToCity[i]] = i;
	}

	int n = cGraph->fGetN();

	//get events
	INFO("Getting events" << endl);
	vector<Event> lEvents = cGraph->fGetEvents(unordered_set<string>(), ANY_RANGE);
	int e = lEvents.size();
	cEventCount = e;
	for (size_t i = 0; i < e; i++) {
		if (cEventToIndex.count(lEvents[i].cCity) == 0) {
			cEventToIndex[lEvents[i].cCity] = map<int, int>();
		}

		cEventToIndex[lEvents[i].cCity][lEvents[i].cTime] = i;
	}

	//create neural network
	INFO("Create neural network " << e + c << " -> " << c << endl);
	cNeuralNet = new NeuralNet(e + c, c, cLayers, cLayerSize);
	cNeuralNet->fSetOutputAct(FUNC_SIGM);

	//get the train set
	INFO("Going to get the train set" << endl);
	vector<pair<vector<double>, vector<double> > > lTrainSet;
	for (int i = 0; i < cTrainEx; i++) {
		int lDepartEvent = rand() % cEventCount;
		int lArriveCity = rand() % c;

		Connection lConn = fDijkWrapConn(cGraph, lEvents[lDepartEvent].cCity, lEvents[lDepartEvent].cTime,
				cIndexToCity[lArriveCity]);
		if (lConn.fGetSize() == 0) {
			continue;
		}

		lTrainSet.push_back(pair<vector<double>, vector<double> >(
				vector<double>(cEventCount + c, 0),
				vector<double>(c, 0))
		);

		int lTrainExample = lTrainSet.size() - 1;

		//input
		vector<double> *lInput = &(lTrainSet[lTrainExample].first);
		(*lInput)[lDepartEvent] = 1;
		(*lInput)[cEventCount + lArriveCity] = 1;
		INFO("	" << lTrainExample << ". From " << lEvents[lDepartEvent].cCity << ". To "
				<< cIndexToCity[lArriveCity] << endl);

		//output
		vector<double> *lOutput = &(lTrainSet[lTrainExample].second);

		//get first elementary connection
		int lConnIndex = 1;
		while (lConnIndex < lConn.fGetSize() && lConn.fGetCity(lConnIndex) == lConn.fGetFirstCity()) {
			lConnIndex++;
		}
		if (lConnIndex >= lConn.fGetSize()) {
			continue;
		}
		Event lFirstRoute(lConn.fGetCity(lConnIndex), lConn.fGetTime(lConnIndex));

		DEB(LVL_OR_TGNEURAL, "TgNeuralRout::fPreprocess: Connection " << lConn.fGetString(", ") << endl);
		DEB(LVL_OR_TGNEURAL, "TgNeuralRout::fPreprocess: First routing "
				<< lFirstRoute.fGetString() << endl);

		(*lOutput)[cCitiesToIndex[lFirstRoute.cCity]] = 1;

		DEB(LVL_OR_TGNEURAL, "TgNeuralRout::fPreprocess: Desired output "
				<< fGetVectorString(*lOutput) << endl);
	}
	DEB(LVL_OR_TGNEURAL, "TgNeuralRout::fPreprocess: TRAIN set: " << endl);
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
	INFO("Ending preprocessing of Neural network oracle (for TG graphs)" << endl);

	cPreprocessed = true;

	return OK;
}

int TgNeuralRout::fGetSpace() {
	int lBytes = 0;

	lBytes += fSizeOf(cArgs);
	for (vector<string>::iterator i = cArgs.begin(); i != cArgs.end(); i++) {
		lBytes += i->length();
	}

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

	lBytes += sizeof(*this);

	return lBytes;
}

Connection TgNeuralRout::fQueryConn(string iFrom, int iTime, string iTo) {
	if (cPreprocessed == false) {
		ERR("TgNeuralRout::fQueryConn: Preprocessing not carried out" << endl);
		return Connection();
	}

	//----- compute -----
	INFO("Starting querying Neural network oracle (for Time graphs) for the connections" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	int lToCity = cCitiesToIndex[iTo];

	string lCurrentCity = iFrom;
	int lCurrentTime = iTime;
	Connection lConn;
	lConn.fAdd(iFrom, iTime);

	while (lCurrentCity != iTo) {
		//get indices
		map<string, map<int, int> >::iterator lFromCityIt = cEventToIndex.find(lCurrentCity);
		if (lFromCityIt == cEventToIndex.end()) {
			ERR("City " << lCurrentCity << " not found" << endl);
			return Connection();
		}
		map<int, int>::iterator lFromEventIt = lFromCityIt->second.find(lCurrentTime);
		if (lFromEventIt == lFromCityIt->second.end()) {
			ERR("Event " << lCurrentCity << "/" << fTimeFormatToString(lCurrentTime)
					<< " not found" << endl);
			return Connection();
		}
		int lFromEvent = lFromEventIt->second;

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
			ERR("Unable to compute the connection. Neural network error" << endl);
			return Connection();
		}
		DEB(LVL_OR_TGNEURAL, "Computed output: " << fGetVectorString(lOutput) << endl);

		//get the best route
		int lMaxCityIndex = lOutput[0];
		for (int i = 1; i < cCitiesToIndex.size(); i++) {
			if (lOutput[i] > lOutput[lMaxCityIndex]) {
				lMaxCityIndex = i;
			}
		}

		lCurrentTime = cGraph->fGetReachingTime(lCurrentCity, lCurrentTime, cIndexToCity[lMaxCityIndex]);
		if (lCurrentTime == -1) {
			ERR("Unable to compute the connection. NN outputs wrong sequence" << endl);
			return Connection();
		}
		lCurrentCity = cIndexToCity[lMaxCityIndex];

		lConn.fAdd(lCurrentCity, lCurrentTime);
	}

	return lConn;
}

int TgNeuralRout::fQueryEa(string iFrom, int iTime, string iTo) {
	if (cPreprocessed == false) {
		ERR("TgNeuralRout::fQueryEa: Preprocessing not carried out" << endl);
		return -1;
	}

	//----- compute -----
	INFO("Starting querying Neural network oracle (for TG graphs) for the earliest arrival" << endl);
	Ret lRet = gAlgTimer->fStartNewTimer();

	Connection lConn = fQueryConn(iFrom, iTime, iTo);
	if (lConn.fGetSize() == 0) {
		return -1;
	}

	if (lRet == OK) {gAlgTimer->fStopTimer();}
	INFO("Ending querying Neural network oracle (for TG graphs) for the earliest arrival" << endl);

	return lConn.fGetLength();
}

string TgNeuralRout::fSGetHelp() {
	stringstream lSs (stringstream::out);

	lSs << "HELP Neural network oracle (for TG graphs, routing based)" << endl <<
			"	" << TG_NEURALROUT_CMD_LAYERS << " {layer count, layer size}: set the number of hidden layers"
					" and the size of a hidden layer" << endl <<
			"	" << TG_NEURALROUT_CMD_TRAIN << " {number}: set the number of training examples" << endl <<
			"	" << TG_NEURALROUT_CMD_ESTVAL << " {1 - 100%}: set the percentage of train data to be used in "
					"estimation set. Default " << TG_NEURAL_DEF_ESTVAL << endl <<
			"	" << TG_NEURALROUT_CMD_ALPHA << " {alpha}: set the parameter alpha used for training the "
					"neural network. Default " << TG_NEURAL_DEF_ALPHA << endl <<
			"	" << TG_NEURALROUT_CMD_MAXIT << " {max-it}: set the maximum number of iterations. Default is "
					"unset" << endl <<
			"	" << TG_NEURALROUT_CMD_MINIT << " {min-it}: set the minimum number of iterations. Default is "
					"unset" << endl;

	return lSs.str();
}

set<QueryType> TgNeuralRout::fSQueryTypes() {
	set<QueryType> lQTypes;
	lQTypes.insert(QTEa);
	lQTypes.insert(QTConn);

	return lQTypes;
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

#define COMP(x) if (cArgs[lPos] == x)
#define UNKNOWN ERR("TgNeuralRout::fProcessArgs: Unknown command." << endl << fGetHelp()); \
	return;
#define MOVEARG lPos++; \
	if (lPos > cArgs.size() - 1) { \
		ERR("TgNeuralRout::fProcessArgs: Insufficient number of arguments when trying to process arg. number " \
				<< lPos + 1 << endl << fGetHelp()); \
		return; \
	} \
	lCurArg = cArgs[lPos];
void TgNeuralRout::fProcessArgs() {
	int lPos = -1;
	string lCurArg = "";

	int lItemsSize = (int)cArgs.size();
	while (lPos < lItemsSize - 1) {
		MOVEARG;
		COMP(TG_NEURALROUT_CMD_LAYERS) {
			MOVEARG;
			cLayers = fStrToInt(lCurArg);
			MOVEARG
			cLayerSize = fStrToInt(lCurArg);
		}
		else COMP(TG_NEURALROUT_CMD_TRAIN) {
			MOVEARG;
			cTrainEx = fStrToInt(lCurArg);
		}
		else COMP(TG_NEURALROUT_CMD_ESTVAL) {
			MOVEARG;
			cEstValRatio = fStrToInt(lCurArg);
			if (cEstValRatio < 1 || cEstValRatio > 100) {
				cEstValRatio = TG_NEURAL_DEF_ESTVAL;
				UNKNOWN;
			}
		}
		else COMP(TG_NEURALROUT_CMD_ALPHA) {
			MOVEARG;
			cAlpha = fStrToDouble(lCurArg);
			if (cAlpha < 0) {
				cAlpha = TG_NEURAL_DEF_ALPHA;
				UNKNOWN;
			}
		}
		else COMP(TG_NEURALROUT_CMD_MAXIT) {
			MOVEARG;
			cMaxIt = fStrToInt(lCurArg);
			if (cMaxIt < 0) {
				cMaxIt = -1;
				UNKNOWN;
			}
		}
		else COMP(TG_NEURALROUT_CMD_MINIT) {
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

	DEB(LVL_CONN, "TgNeuralRout::fProcessArgs: Arguments processed" << endl);
}
