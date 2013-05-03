/* common connectivity stuff */
#ifndef CONNECTIVITY_H_
#define CONNECTIVITY_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "../central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

class ConnResult : public AnalyseResult {
private:
	//----- data -----
	bool cConn;
	Partition cPartition;

	bool cConnDefined;
	bool cPartitionDefined;
	bool cPartitionDetDefined;
public:
	//----- constructors -----
	ConnResult() {
		cConnDefined = false;
		cPartitionDefined = false;
		cPartitionDetDefined = false;

		cConn = false;
	}

	//----- required interface -----
	virtual std::string fGetResultNameString() {
		return "*** Connectivity analysis results ***";
	}

	virtual std::string fGetResultString() {
		std::stringstream lResult (std::stringstream::out);

		if (cConnDefined) {
			lResult << fIsConnString();
		}
		if (cPartitionDefined) {
			lResult << fGetNumberOfCompString();
		}
		if (cPartitionDetDefined) {
			lResult << fGetPartitionDetString();
		}

		return lResult.str();
	}

	//----- interface -----
	void fSetConn(bool iConn) {
		cConn = iConn;
		cConnDefined = true;
	}

	void fSetPartition(Partition iPartition) {
		cPartition = iPartition;
		cPartition.fSortBySize();
		cPartitionDefined = true;
	}

	void fSetPartitionDet(bool iPartDet) {
		cPartitionDetDefined = true;
	}

	std::string fIsConnString() {
		std::stringstream lResult (std::stringstream::out);

		lResult << "CONNECTED: " << gBoolStr[cConn] << std::endl;

		return lResult.str();
	}

	std::string fGetNumberOfCompString() {
		std::stringstream lResult (std::stringstream::out);

		lResult << "# OF CONNECTED COMPs: " << cPartition.fGetSetCount() << std::endl;
		lResult << "COMPONENT SIZES: ";
		int lCheckSum = 0;
		for (int i = 0; i < cPartition.fGetSetCount(); i++) {
			lCheckSum += cPartition.fGetSet(i).size();
			lResult << cPartition.fGetSet(i).size();
			if (i != cPartition.fGetSetCount() - 1) {
				lResult << ", ";
			}
		}
		lResult << std::endl;

		return lResult.str();
	}

	std::string fGetPartitionDetString() {
		std::stringstream lResult (std::stringstream::out);

		lResult << "COMPONENTS: " << std::endl;
		for (int i = 0; i < cPartition.fGetSetCount(); i++) {
			lResult << "	" << fGetVectorString(cPartition.fGetSet(i)) << std::endl;
		}

		return lResult.str();
	}
};

/*****************************************************************/
/* Functions
/*****************************************************************/

#endif /* CONNECTIVITY_H_ */
