/*****************************************************************/
/* Includes
/*****************************************************************/

#include <sstream>
#include <math.h>
#include <list>

#include "../../common/src/structsizer.h"

#include "central.h"

#include "neuralnet.h"

using namespace std;

/*****************************************************************/
/* Class - constructors
/*****************************************************************/

NeuralNet::NeuralNet(int iN, int iM, int iL, int iK) {
	cN = iN;
	cM = iM;
	cL = iL;
	cK = iK;

	cEstSet = NULL;

	cAlgVariant = ALG_STD;
	cHiddenActFunc = FUNC_SIGM;
	cOutputActFunc = FUNC_SIGM;

	cAlpha = -1;
}

/*****************************************************************/
/* Class - interface
/*****************************************************************/

vector<double> NeuralNet::fCompute(vector<double> &iInput) {
	if (iInput.size() != cN) {
		ERR("NeuralNet::fCompute: Input not of the required size" << endl);
		return vector<double>();
	}

	DEB(LVL_NEURAL, "NeuralNet::fCompute: Input is: " << fGetVectorString(iInput) << endl);

	//compute the output (layer by layer)
	vector<double> lCurrentInput = iInput;
	for (int i = 1; i <= cL + 1; i++) {
		//computing values i-th layer
		vector<double> lCurrentOutput = fComputeLayer(i, lCurrentInput);
		lCurrentInput = lCurrentOutput;

		DEB(LVL_NEURALDET, "NeuralNet::fCompute: Layer " << i << " output: " <<
				fGetVectorString(lCurrentOutput) << endl);
	}

	DEB(LVL_NEURAL, "NeuralNet::fCompute: Output is: " << fGetVectorString(lCurrentInput) << endl);

	return lCurrentInput;
}

vector<int> NeuralNet::fComputeRound(vector<double> &iInput) {
	//compute the normal output
	vector<double> lOutput = fCompute(iInput);

	//round up the output
	vector<int> lRounded;
	for (vector<double>::size_type i = 0; i < lOutput.size(); i++) {
		lRounded.push_back(fRound(lOutput[i]));
	}

	DEB(LVL_NEURAL, "NeuralNet::fComputeRound: Output is: " << fGetVectorString(lRounded) << endl);

	return lRounded;
}

double NeuralNet::fTrain(
		vector<pair<vector<double>, vector<double> > > &iTrainSet,
		int iEstValRatio,
		double iAlpha, int iMaxIt, int iMinIt,
		double *oValWa, double *oEstWa, double *oEstErr, int *oIter) {
	//randomize weights
	fRandomizeWeights();

	//get estimation/validation sets
	vector<pair<vector<double>, vector<double> > > lEstSet;
	vector<pair<vector<double>, vector<double> > > lValSet;
	Ret lRetSets = fGetEstValSets(iTrainSet, &lEstSet, &lValSet, iEstValRatio);
	if (lRetSets != OK) {
		ERR("NeuralNet::fTrain: Problem retrieving estimation and validation sets");
		return -1;
	}

	DEB(LVL_NEURAL, "NeuralNet::fTrain: Training begins, perceptron weights are " << endl <<
			fGetWeightsString() << endl);

	//set train-specific parameters
	cEstSet = &lEstSet;
	cAlpha = iAlpha;

	//get initial wrong answers
	double lInitEstError = fGetDataSetError(lEstSet);
	double lInitValError = fGetDataSetError(lValSet);
	int lInitWrongEst = fGetWrongAnswers(lEstSet);
	int lInitWrongVal = fGetWrongAnswers(lValSet);
	INFO("Initial estimation error = " << lInitEstError << endl);
	INFO("Initial validation error = " << lInitValError << endl);
	INFO("Initial wrong estimation answers = " << lInitWrongEst << "/" <<
			lEstSet.size() << endl);
	INFO("Initial wrong validation answers = " << lInitWrongVal << "/" <<
			lValSet.size() << endl);

	bool lPhaseTwo = false;
	int lPhaseTwoStartIt = -1;
	list<double> lValErrors;
	int lIteration = 0;
	double lEstError = -1;
	int lWrongEst = -1;
	int lWrongVal = -1;

	//iterate training iterations till the early stopping-criterion is not met
	while (true) {
		DEB(LVL_NEURAL, "NeuralNet::fTrain:	ITERATION " << lIteration << "---------------------" << endl);

		//training iteration
		lEstError = fTrainIteration();

		//compute errors and wrong answers
		double lNewValError = fGetDataSetError(lValSet);
		lWrongEst = fGetWrongAnswers(lEstSet);
		lWrongVal = fGetWrongAnswers(lValSet);
		INFO("NeuralNet::fTrain: Estimation error = " << lEstError << endl);
		INFO("NeuralNet::fTrain: Validation error = " << lNewValError << endl);
		INFO("Wrong estimation answers = " << lWrongEst << "/" <<
				lEstSet.size() << endl);
		INFO("Wrong validation answers = " << lWrongVal << "/" <<
				lValSet.size() << endl);

		lValErrors.push_back(lNewValError);
		INFO("Training iteration " << lIteration << ". Last validation errors " <<
				fGetListString(lValErrors) << endl);

		//max iteration stopping criterion
		if (lIteration > iMaxIt && iMaxIt != -1) {
			break;
		}

		if (lIteration > ITERATION_OFFSET) {
			if (iMinIt == -1 || lIteration > iMinIt) {
				if (lPhaseTwo == false) {
					//early stopping criterion
					if (lNewValError/lValErrors.front() > STOP_RATIO) {
						INFO("Starting phase two" << endl);
						lPhaseTwo = true;
						lPhaseTwoStartIt = lIteration;
						cAlpha = cAlpha / ALPHA_DIV;
					}
				}
				else if (lIteration > lPhaseTwoStartIt + PHASE_TWO_IT &&
						lNewValError/lValErrors.front() > STOP_RATIO) {
					break;
				}
			}
			lValErrors.pop_front();
		}

		lIteration++;
	}

	if (oEstWa != NULL) {
		*oEstWa = (double)lWrongEst/(double)lEstSet.size();
	}
	if (oValWa != NULL) {
		*oValWa = (double)lWrongVal/(double)lValSet.size();
	}
	if (oEstErr != NULL) {
		*oEstErr = lEstError;
	}

	DEB(LVL_NEURAL, "NeuralNet::fTrain: Weights of trained perceptron look like this:" << endl <<
			fGetWeightsString() << endl);
	DEB(LVL_NEURAL, "NeuralNet::fTrain: Number of iterations: " << lIteration << endl);

	if (oIter != NULL) {
		*oIter = lIteration;
	}
	return lValErrors.back();
}

void NeuralNet::fTest(
		vector<pair<vector<double>, vector<double> > > &iTestSet,
		double *oAvgError,
		double *oStdDev,
		double *oWrongOutputs) {
	//compute errors
	vector<double> lErrors;
	for (vector<pair<vector<double>, vector<double> > >::size_type i = 0; i < iTestSet.size(); i++) {
		//compute output and error
		vector<double> lY = fCompute(iTestSet[i].first);
		double lError = fComputeInstantError(lY, iTestSet[i].second);
		lErrors.push_back(lError);
	}

	//avg. error
	double lAvgError = 0;
	for (vector<double>::iterator i = lErrors.begin(); i != lErrors.end(); i++) {
		lAvgError += *i;
	}
	lAvgError /= lErrors.size();
	*oAvgError = lAvgError;

	//std. dev
	double lStdDev = 0;
	for (vector<double>::iterator i = lErrors.begin(); i != lErrors.end(); i++) {
		lStdDev += pow(*i - lAvgError, 2);
	}
	lStdDev /= lErrors.size();
	lStdDev = sqrt(lStdDev);
	*oStdDev = lStdDev;

	//wrong answers
	*oWrongOutputs = (double)fGetWrongAnswers(iTestSet) / (double)iTestSet.size();
	*oWrongOutputs *= 100;
}

int NeuralNet::fGetSpace() {
	int lBytes = 0;

	lBytes += fSizeOf<vector<vector<double> > >(cWeights);
	for (vector<vector<vector<double> > >::iterator i = cWeights.begin();
			i != cWeights.end(); i++) {
		lBytes += fSizeOf<vector<double> >(*i);
		for (vector<vector<double> >::iterator j = i->begin(); j != i->end(); j++) {
			lBytes += fSizeOf<double>(*j);
		}
	}

	return lBytes;
}

/*****************************************************************/
/* Class - implementation
/*****************************************************************/

double NeuralNet::fComputeInstantError(vector<double> &iY, vector<double> &iD) {
	double lResult = 0;
	for (vector<double>::size_type i = 0; i < iY.size(); i++) {
		lResult += pow(iD[i] - iY[i], 2) / (double)2;
	}

	return lResult;
}

double NeuralNet::fGetDataSetError(vector<pair<vector<double>, vector<double> > > &iDataSet) {
	double lTotalError = 0;
	for (vector<pair<vector<double>, vector<double> > >::size_type i = 0; i < iDataSet.size(); i++) {
		//compute output and error
		vector<double> lY = fCompute(iDataSet[i].first);
		double lError = fComputeInstantError(lY, iDataSet[i].second);
		lTotalError += lError;

		DEB(LVL_NEURAL, "NeuralNet::fGetDataSetError: Computed output: " << fGetVectorString(lY) << endl);
		DEB(LVL_NEURAL, "NeuralNet::fGetDataSetError: Instantaneous error: " << lError << endl);
		DEB(LVL_NEURAL, "NeuralNet::fGetDataSetError: Total error: " << lTotalError << endl);
	}

	return lTotalError;
}

string NeuralNet::fGetWeightsString() {
	ostringstream lSs;

	for (int i = 0; i < cL + 1; i++) {
		lSs << "Layer " << i << " and " << i + 1 << endl;
		for (int j = 0; j < fGetNeuronCount(i + 1); j++) {
			lSs << "	To " << j << endl;
			for (int k = 0; k < fGetNeuronCount(i) + 1; k++) {
				if (k == fGetNeuronCount(i)) {
					lSs << "		From bias: " << cWeights[i][j][k] << endl;
				}
				else {
					lSs << "		From " << k << ": " << cWeights[i][j][k] << endl;
				}
			}
		}
	}

	return lSs.str();
}

vector<double> NeuralNet::fComputeLayer(int iLayer, vector<double> &iInput) {
	if (iLayer < 1 || iLayer > cL + 1) {
		ERR("NeuralNet::fComputeLayer: Incorrect layer number" << endl);
		return vector<double>();
	}
	if (iInput.size() != fGetNeuronCount(iLayer - 1)) {
		ERR("NeuralNet::fComputeLayer: Input not of the required size" << endl);
		return vector<double>();
	}

	//first compute net values for each neuron
	vector<double> lNets = fComputeNets(iLayer, iInput);

	int lToDimension = fGetNeuronCount(iLayer);
	vector<double> lResult;
	lResult.reserve(lToDimension);

	//compute new layer by applying activation function to given net value
	for (int i = 0; i < lToDimension; i++) {
		//hidden layer
		if (iLayer < cL + 1) {
			lResult.push_back(fActivationFunction(cHiddenActFunc, lNets[i]));
		}
		//output layer
		else {
			lResult.push_back(fActivationFunction(cOutputActFunc, lNets[i]));
		}
	}

	return lResult;
}

double NeuralNet::fTrainIteration() {
	//choose permutation of inputs
	vector<int> lPermutation = fRandomPermutation(cEstSet->size());
	DEB(LVL_NEURAL, "NeuralNet::fTrainIteration: Permutation is: " << fGetVectorString(lPermutation)
			<< endl);

	double lTotalError = 0;
	for (vector<pair<vector<int>, vector<double> > >::size_type i = 0; i < cEstSet->size(); i++) {
		DEB(LVL_NEURAL, "NeuralNet::fTrainIteration: Training example " << i << "/" << cEstSet->size()
				<< endl);

		//choose input
		int j = lPermutation[i];

		DEB(LVL_NEURAL, "NeuralNet::fTrainIteration: In/Out pair: " << endl <<
				"	" << fGetVectorString((*cEstSet)[j].first) << endl <<
				"	" << fGetVectorString((*cEstSet)[j].second) << endl);

		//compute output
		vector<double> lY = fCompute((*cEstSet)[j].first);

		//evaluate error
		double lError = fComputeInstantError(lY, (*cEstSet)[j].second);
		lTotalError += lError;

		DEB(LVL_NEURAL, "NeuralNet::fTrainIteration: Computed output: " << fGetVectorString(lY) << endl);
		DEB(LVL_NEURAL, "NeuralNet::fTrainIteration: Desired output: " << fGetVectorString(
				(*cEstSet)[j].second) << endl);
		DEB(LVL_NEURAL, "NeuralNet::fTrainIteration: Instantaneous error: " << lError << endl);
		DEB(LVL_NEURAL, "NeuralNet::fTrainIteration: Total error: " << lTotalError << endl);

		//adjust weights
		if (lError > 0 + DOUBLE_INACCURACY) {
			fAdjustWeights((*cEstSet)[j].second, lY, (*cEstSet)[j].first);
		}
	}

	return lTotalError;
}

void NeuralNet::fRandomizeWeights() {
	cWeights.clear();

	cWeights.reserve(cL + 2);
	//for each couple of adjacent layers
	for (int i = 0; i < cL + 1; i++) {
		cWeights.push_back(vector<vector<double> >());
		cWeights[i].reserve(fGetNeuronCount(i + 1));
		//for each vertex in the layer
		for (int j = 0; j < fGetNeuronCount(i + 1); j++) {
			cWeights[i].push_back(vector<double>());
			cWeights[i][j].reserve(fGetNeuronCount(i) + 1);
			//for each vertex in the previous layer, keep in mind bias
			for (int k = 0; k < fGetNeuronCount(i) + 1; k++) {
				double lRandom = 1 - 2 * ((double)rand()/(double)RAND_MAX);
				cWeights[i][j].push_back(lRandom);
			}
		}
	}
}

int NeuralNet::fGetNeuronCount(int iLayer) {
	if (iLayer == 0) {
		return cN;
	}
	if (iLayer == cL + 1) {
		return cM;
	}

	return cK;
}

int NeuralNet::fGetWrongAnswers(vector<pair<vector<double>, vector<double> > > &iDataSet) {
	int lResult = 0;

	for (vector<pair<vector<int>, vector<double> > >::size_type i = 0; i < iDataSet.size(); i++) {
		//compute the outcome for given input
		vector<double> lY = fCompute(iDataSet[i].first);

		//round it up
		vector<int> lRoundedY;
		for (vector<double>::size_type j = 0; j < lY.size(); j++) {
			lRoundedY.push_back(fRound(lY[j]));
		}

		//round up the wanna-be output
		vector<int> lRoundedD;
		for (vector<double>::size_type j = 0; j < iDataSet[i].second.size(); j++) {
			lRoundedD.push_back(fRound(iDataSet[i].second[j]));
		}

		DEB(LVL_NEURAL, "NeuralNet::fGetWrongAnswers: Input: " << fGetVectorString(iDataSet[i].first)
				<< endl);
		DEB(LVL_NEURAL, "NeuralNet::fGetWrongAnswers: Wanna be: " << fGetVectorString(iDataSet[i].second)
				<< endl);
		DEB(LVL_NEURAL, "NeuralNet::fGetWrongAnswers: Output: " << fGetVectorString(lY)
				<< endl);
		DEB(LVL_NEURAL, "NeuralNet::fGetWrongAnswers: Rounded output: " << fGetVectorString(lRoundedY)
				<< endl);
		DEB(LVL_NEURAL, "NeuralNet::fGetWrongAnswers: Rounded wanna be: " << fGetVectorString(lRoundedD)
				<< endl);

		//check if they disagree
		for (vector<int>::size_type j = 0; j < lRoundedD.size(); j++) {
			if (lRoundedD[j] != lRoundedY[j]) {
				DEB(LVL_NEURAL, "NeuralNet::fGetWrongAnswers: MATCH NOK" << endl);
				lResult++;
				break;
			}
			if (j == lRoundedD.size() - 1) {
				DEB(LVL_NEURAL, "NeuralNet::fGetWrongAnswers: MATCH OK" << endl);
			}
		}
	}

	return lResult;
}

void NeuralNet::fAdjustWeights(vector<double> &iD, vector<double> &iY, vector<double> &iInput) {
	if (cAlgVariant == ALG_STD) {
		fStdAdjustWeights(iD, iY, iInput);
		return;
	}
	//others are not implemented yet

	//default value
	fStdAdjustWeights(iD, iY, iInput);
}

void NeuralNet::fStdAdjustWeights(vector<double> &iD, vector<double> &iY, vector<double> &iInput) {
	//----- find out nets and values for each layer -----

	DEB(LVL_NEURALDET, "Weights before: " << endl << fGetWeightsString() << endl);

	//initialize data structures
	vector<vector<double> > lValues; //range 0..cL + 1. begins with layer 0
	lValues.push_back(vector<double>());
	lValues[0] = iInput;
	vector<vector<double> > lNets; //range 0..cL + 1. begins with layer 1
	lNets.push_back(vector<double>());

	//compute nets and values
	vector<double> lCurrentInput;
	lCurrentInput.reserve(iInput.size());
	for (vector<int>::size_type i = 0; i < iInput.size(); i++) {
		lCurrentInput.push_back(iInput[i]);
	}
	for (int i = 1; i <= cL + 1; i++) {
		//computing values i-th layer
		vector<double> lLayerNets = fComputeNets(i, lCurrentInput);
		vector<double> lCurrentOutput = fComputeLayer(i, lCurrentInput);

		lNets.push_back(lLayerNets);
		lValues.push_back(lCurrentOutput);

		lCurrentInput = lCurrentOutput;
	}

	//print debugging information
	DEB(LVL_NEURALDET, "NeuralNet::fStdAdjustWeights: Values: " << endl);
	for (vector<vector<double> >::size_type i = 0; i < lValues.size(); i++) {
		DEB(LVL_NEURALDET, "	" << i << ": " << fGetVectorString(lValues[i]) << endl);
	}
	DEB(LVL_NEURALDET, "NeuralNet::fStdAdjustWeights: Nets: " << endl);
	for (vector<vector<double> >::size_type i = 0; i < lValues.size(); i++) {
		DEB(LVL_NEURALDET, "	" << i << ": " << fGetVectorString(lNets[i]) << endl);
	}

	//----- compute the mistakes (for each layer-pair) -----

	//initialize mistakes data structure
	vector<vector<double> > lMistakes(cL + 2, vector<double>()); //range 0..cL + 1. begins with layer 1

	//output layer
	vector<double> *lLastMistakes = &(lMistakes[cL + 1]);
	lLastMistakes->reserve(cM);
	for (int i = 0; i < cM; i++) {
		double lOutputDerivative = fDerActivationFunction(cOutputActFunc, lNets[cL + 1][i]);
		double lMistake = (iD[i] - lValues[cL + 1][i]) * lOutputDerivative;
		lLastMistakes->push_back(lMistake);
	}

	//hidden layers
	for (int i = cL; i >= 1; i--) {
		//mistakes for i-th layer
		vector<double> *lLayerMistakes = &(lMistakes[i]);
		for (int j = 0; j < fGetNeuronCount(i); j++) {
			double lDerivative = fDerActivationFunction(cHiddenActFunc, lNets[i][j]);

			double lSum = 0;
			int lSumIndexBound = fGetNeuronCount(i + 1);
			for (int k = 0; k < lSumIndexBound; k++) {
				lSum += cWeights[i][k][j] * lMistakes[i + 1][k];
			}

			double lMistake = lSum * lDerivative;
			lLayerMistakes->push_back(lMistake);
		}
	}

	//print debugging information
	DEB(LVL_NEURALDET, "NeuralNet::fStdAdjustWeights: Mistakes: " << endl);
	for (vector<vector<double> >::size_type i = 0; i < lMistakes.size(); i++) {
		DEB(LVL_NEURALDET, "	" << i << ": " << fGetVectorString(lMistakes[i]) << endl);
	}

	//----- adjust the weights -----

	//last layer pair
	for (int i = 0; i < fGetNeuronCount(cL) + 1; i++) {
		for (int j = 0; j < fGetNeuronCount(cL + 1); j++) {
			//non-bias weight
			if (i != fGetNeuronCount(cL)) {
				double lDelta = cAlpha * lMistakes[cL + 1][j] * lValues[cL][i];
				//cPrevDeltas[cL][j][i] = lDelta;
				cWeights[cL][j][i] += lDelta;
			}
			//bias weight
			else {
				double lDelta = -cAlpha * lMistakes[cL + 1][j];
				//cPrevDeltas[cL][j][i] = lDelta;
				cWeights[cL][j][i] += lDelta;
			}
		}
	}

	//hidden layer pairs and first layer pair
	for (int i = cL - 1; i >= 0; i--) {
		for (int j = 0; j < fGetNeuronCount(i) + 1; j++) {
			for (int k = 0; k < fGetNeuronCount(i + 1); k++) {
				//non-bias weight
				if (j != fGetNeuronCount(i)) {
					double lDelta = cAlpha * lMistakes[i + 1][k] * lValues[i][j];
					//cPrevDeltas[i][k][j] = lDelta;
					cWeights[i][k][j] += lDelta;
				}
				//bias weight
				else {
					double lDelta = -cAlpha * lMistakes[i + 1][k];
					//cPrevDeltas[i][k][j] = lDelta;
					cWeights[i][k][j] += lDelta;
				}
			}
		}
	}

	DEB(LVL_NEURALDET, "Weights after: " << endl << fGetWeightsString() << endl);
}

vector<double> NeuralNet::fComputeNets(int iLayer, vector<double> &iInput) {
	if (iLayer < 1 || iLayer > cL + 1) {
		ERR("NeuralNet::fComputeNets: Incorrect layer number" << endl);
		return vector<double>();
	}
	if (iInput.size() != fGetNeuronCount(iLayer - 1)) {
		ERR("NeuralNet::fComputeNets: Input not of the required size" << endl);
		return vector<double>();
	}

	vector<double> lNets;

	int lFromDimension = fGetNeuronCount(iLayer - 1);
	int lToDimension = fGetNeuronCount(iLayer);

	//compute nets for given layer
	for (int i = 0; i < lToDimension; i++) {
		double lNet = 0;
		for (int j = 0; j < lFromDimension; j++) {
			lNet += cWeights[iLayer - 1][i][j] * iInput[j];
		}
		lNet -= cWeights[iLayer - 1][i][lFromDimension];
		lNets.push_back(lNet);
	}

	return lNets;
}

Ret NeuralNet::fGetEstValSets(
		vector<pair<vector<double>, vector<double> > > &iTrainSet,
		vector<pair<vector<double>, vector<double> > > *oEstSet,
		vector<pair<vector<double>, vector<double> > > *oValSet,
		int iEstValRatio) {
	if (oEstSet == NULL || oValSet == NULL) {
		ERR("NeuralNet::fGetEstValSets: Est or Val set cannot be null" << endl);
		return NOK;
	}
	if (iEstValRatio < 1 || iEstValRatio > 100) {
		ERR("NeuralNet::fGetEstValSets: Est/Val ratio out of range" << endl);
		return NOK;
	}

	int lEstSize = (int)((double)(iTrainSet.size() * iEstValRatio) / (double)100);
	if (iEstValRatio == 100) {
		lEstSize = iTrainSet.size();
	}
	vector<int> lRandPerm = fRandomPermutation(iTrainSet.size());
	for (vector<int>::size_type i = 0; i < lRandPerm.size(); i++) {
		//estimation data
		if (i < lEstSize) {
			oEstSet->push_back(iTrainSet[lRandPerm[i]]);
		}
		else {
			oValSet->push_back(iTrainSet[lRandPerm[i]]);
		}
	}
	DEB(LVL_NEURAL, "TgNeuralEa::fPreprocess: ESTIMATION set: " << endl);
	for (size_t i = 0; i < oEstSet->size(); i++) {
		DEB(LVL_NEURAL, "	" << i << endl);
		DEB(LVL_NEURAL, "		" << fGetVectorString((*oEstSet)[i].first) << endl);
		DEB(LVL_NEURAL, "		" << fGetVectorString((*oEstSet)[i].second) << endl);
	}
	DEB(LVL_NEURAL, "TgNeuralEa::fPreprocess: VALIDATION set: " << endl);
	for (size_t i = 0; i < oValSet->size(); i++) {
		DEB(LVL_NEURAL, "	" << i << endl);
		DEB(LVL_NEURAL, "		" << fGetVectorString((*oValSet)[i].first) << endl);
		DEB(LVL_NEURAL, "		" << fGetVectorString((*oValSet)[i].second) << endl);
	}

	return OK;
}

/*****************************************************************/
/* Implementation - activation functions
/*****************************************************************/

double NeuralNet::fSigmoid(double iX) {
	return (double)1 / (1 + exp(-iX));
}

double NeuralNet::fTanh(double iX) {
	return (exp(2 * iX) - 1) / (exp(2 * iX) + 1);
}

double NeuralNet::fActivationFunction(ActFunc iFunction, double iX) {
	if (iFunction == FUNC_SIGM) {
		return fSigmoid(iX);
	}
	if (iFunction == FUNC_TANH) {
		return fTanh(iX);
	}
	if (iFunction == FUNC_ID) {
		return iX;
	}

	return -1;
}

double NeuralNet::fDerSigmoid(double iX) {
	return fSigmoid(iX) * (1 - fSigmoid(iX));
}

double NeuralNet::fDerTanh(double iX) {
	return 1 / pow((exp(2 * iX) + 1) / (2 * exp(iX)), 2);
}

double NeuralNet::fDerActivationFunction(ActFunc iFunction, double iX) {
	if (iFunction == FUNC_SIGM) {
		return fDerSigmoid(iX);
	}
	if (iFunction == FUNC_TANH) {
		return fDerTanh(iX);
	}
	if (iFunction == FUNC_ID) {
		return 1;
	}

	return -1;
}
