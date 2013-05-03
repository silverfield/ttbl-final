/* Neural network approach for time-expanded graphs - NN answers the routing information - the next
 * city to visit
 *
 * - user specifies: the number of hidden layers (space complexity) and amount of training examples (
 * time complexity of pre-processing)
 * - input of the neural network are neurons - one for each node of the time-expanded graph (that is -
 * for each combination of city/time) and one for each city. The couple of neurons (departing city/time,
 * destination city) specifies the EAP query and we mark the two input neurons with "1".
 * - output is a single neuron - it outputs the earliest arrival for the given input
 * */
#ifndef TGNEURALROUT_H_
#define TGNEURALROUT_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string>

#include "oracle.h"
#include "../central.h"
#include "../neuralnet.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define TG_NEURALROUT_NAME "neuralrout"

#define TG_NEURALROUT_CMD_LAYERS "-l"
#define TG_NEURALROUT_CMD_TRAIN "-t"
#define TG_NEURALROUT_CMD_ESTVAL "-er"
#define TG_NEURALROUT_CMD_ALPHA "-a"
#define TG_NEURALROUT_CMD_MAXIT "-max"
#define TG_NEURALROUT_CMD_MINIT "-min"

#define TG_NEURAL_DEF_ESTVAL 80
#define TG_NEURAL_DEF_ALPHA 0.1

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class - NeuralRout
/*****************************************************************/

class TgNeuralRout: public TimetableOracle {
private:
	//----- data -----
	TimeGraphObject *cGraph;
	std::map<std::string, int> cCitiesToIndex;
	std::vector<std::string> cIndexToCity;
	std::map<std::string, std::map<int, int> > cEventToIndex;
	int cEventCount;
	NeuralNet *cNeuralNet;
	double cTimeRange;
	bool cPreprocessed;

	int cLayers;
	int cLayerSize;
	int cTrainEx;
	int cEstValRatio;
	int cMaxIt;
	int cMinIt;
	double cAlpha;

	std::vector<std::string> cArgs;

	//----- implementation -----
	void fProcessArgs();
public:
	//----- Constructors -----
	TgNeuralRout(std::vector<std::string> iArguments);
	~TgNeuralRout();

	//----- required interface - Oracle -----
	virtual Ret fPreprocess(Object *iObject);

	virtual inline std::string fGetHelp() {return fSGetHelp();};
	virtual inline std::string fGetName() {return fSGetName();};
	virtual inline bool fAnswers(QueryType iQt) {return fSAnswers(iQt);};
	virtual inline std::set<QueryType> fQueryTypes() {return fSQueryTypes();};
	virtual inline std::vector<std::string> fGetArgs() {return cArgs;};
	virtual int fGetSpace();
	virtual inline std::set<ObjType> fMeantFor() {return fSMeantFor();};

	//----- required interface - TimetableOracle -----
	virtual Connection fQueryConn(std::string iFrom, int iTime, std::string iTo);
	virtual int fQueryEa(std::string iFrom, int iTime, std::string iTo);

	//----- Static interface -----
	static std::string fSGetHelp();
	static inline std::string fSGetName() {return TG_NEURALROUT_NAME;};
	static inline bool fSAnswers(QueryType iQt) {return (iQt == QTConn || iQt == QTEa);};
	static std::set<QueryType> fSQueryTypes();
	static inline std::set<ObjType> fSMeantFor() {
		std::set<ObjType> lSet;
		lSet.insert(OTTe);
		lSet.insert(OTTd);
		return lSet;
	};
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* TGNEURALROUT_H_ */
