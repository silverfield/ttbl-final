/* Neural network - multilayer perceptron. Following can be parametrized:
 * - number of input neurons
 * - number of output neurons
 * - number of hidden layers
 * - number of neurons in a hidden layer
 * - hidden layer activation function
 * - output layer activation function
 *
 * The network is a multi-layer perceptron implementing back-propagation.
 *
 * Training has two phases:
 * - first is ended with early stopping criterion
 * - second is a smooth-out phase with decreased alpha. Lasts at least some constant number of iterations
 * and ends also on early stopping
 *
 * Minimum number of iterations can be set to override the early stopping criterion in training
 * Maximum number of iterations can be set as well
 **/
#ifndef NEURAL_H_
#define NEURAL_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

//ratio (new validation error/old validation error) used in early stopping
#define STOP_RATIO 1
#define DOUBLE_INACCURACY 0.00000001
//how many iterations we look back when determining early stopping
#define ITERATION_OFFSET 10
//fraction of alpha used in second phase of training
#define ALPHA_DIV 10
//minimum number of iterations in second phase
#define PHASE_TWO_IT 20

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Types
/*****************************************************************/

enum AlgVariant {ALG_STD, ALG_MOMENT, ALG_WEIGHTS};
enum ActFunc {FUNC_SIGM, FUNC_TANH, FUNC_ID};

/*****************************************************************/
/* Class
/*****************************************************************/

class NeuralNet {
private:
	//----- data -----
	int cN; //number of input neurons
	int cM; //number of output neurons
	int cL; //number of hidden layers
	int cK; //number of neurons per hidden layer

	AlgVariant cAlgVariant; //algorithm for adjusting weights
	ActFunc cHiddenActFunc;
	ActFunc cOutputActFunc;

	std::vector<std::vector<std::vector<double> > > cWeights; //cWeights[layer_pair][to_neur][from_neur]

	//----- train specific data -----
	double cAlpha;
	std::vector<std::pair<std::vector<double>, std::vector<double> > > *cEstSet;

	//----- implementation -----
	//error of output (iY) against desired output (iD)
	double fComputeInstantError(std::vector<double> &iY, std::vector<double> &iD);
	//total error of the dataset
	double fGetDataSetError(
			std::vector<std::pair<std::vector<double>, std::vector<double> > > &iDataSet);
	//returns string with current weights of the perceptron
	std::string fGetWeightsString();
	//performs one training iteration. returns estimation error
	double fTrainIteration();
	//computes given layer from its input
	std::vector<double> fComputeLayer(int iLayer, std::vector<double> &iInput);
	//randomizes weights (to numbers from <-1, 1>)
	void fRandomizeWeights();
	//returns number of neurons in given layer
	int fGetNeuronCount(int iLayer);
	//gets the number of times the network activates wrong neurons (rounding is used on output layer)
	int fGetWrongAnswers(std::vector<std::pair<std::vector<double>, std::vector<double> > > &iDataSet);
	//adjusts weights of perceptron
	void fAdjustWeights(std::vector<double> &iD, std::vector<double> &iY,
			std::vector<double> &iInput);
	//standard way of adjusting weights in back-propagation
	void fStdAdjustWeights(std::vector<double> &iD, std::vector<double> &iY,
			std::vector<double> &iInput);
	//computes the net values (sum of weighted inputs) for each neuron of specified layer
	std::vector<double> fComputeNets(int iLayer, std::vector<double> &iInput);
	Ret fGetEstValSets(
			std::vector<std::pair<std::vector<double>, std::vector<double> > > &iTrainSet,
			std::vector<std::pair<std::vector<double>, std::vector<double> > > *oEstSet,
			std::vector<std::pair<std::vector<double>, std::vector<double> > > *oValSet,
			int iEstValRatio);

	//----- activation functions -----
	double fSigmoid(double iX);
	double fTanh(double iX);
	double fActivationFunction(ActFunc iFunction, double iX);
	double fDerSigmoid(double iX);
	double fDerTanh(double iX);
	double fDerActivationFunction(ActFunc iFunction, double iX);
public:
	//----- constructors -----
	NeuralNet(int iN, int iM, int iL, int iK);

	//----- interface -----
	//computes the outcome of the perceptron on a given input
	std::vector<double> fCompute(std::vector<double> &iInput);

	//the same, but rounds off the values at the output
	std::vector<int> fComputeRound(std::vector<double> &iInput);

	//training with a given validation and estimation sets
	double fTrain(
			std::vector<std::pair<std::vector<double>, std::vector<double> > > &iTrainSet,
			int iEstValRatio,
			double iAlpha, int iMaxIt, int iMinIt,
			double *oValWa = NULL, double *oEstWa = NULL, double *oEstErr = NULL, int *oIter = NULL);

	//test trained model on the given data
	void fTest(
			std::vector<std::pair<std::vector<double>, std::vector<double> > > &iTestSet,
			double *oAvgError,
			double *oStdDev,
			double *oWrongOutputs);

	//space in bytes occupied by the network
	int fGetSpace();

	inline void fSetHiddenAct(ActFunc iActFunc) {cHiddenActFunc = iActFunc;};
	inline void fSetOutputAct(ActFunc iActFunc) {cOutputActFunc = iActFunc;};
	inline void fSetAlgVariant(AlgVariant iAlgVariant) {cAlgVariant = iAlgVariant;};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* NEURAL_H_ */
