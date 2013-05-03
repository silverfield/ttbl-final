/* common strong connectivity stuff */
#ifndef STRCONN_H_
#define STRCONN_H_

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

class StrConnResult : public AnalyseResult {
private:
	//----- data -----
	bool cConn;
	Partition cPartition;

	bool cConnDefined;
	bool cPartitionDefined;
	bool cPartitionDetDefined;
public:
	//----- constructors -----
	StrConnResult() {
		cConnDefined = false;
		cPartitionDefined = false;
		cPartitionDetDefined = false;

		cConn = false;
	}

	//----- required interface -----
	virtual std::string fGetResultNameString() {
		return "*** Strong connectivity analysis results ***";
	}

	virtual std::string fGetResultString() {
		std::stringstream lResult (std::stringstream::out);

		//lResult << fGetResultNameString() << std::endl;
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

		lResult << "STR. CONNECTED: " << gBoolStr[cConn] << std::endl;

		return lResult.str();
	}

	std::string fGetNumberOfCompString() {
		std::stringstream lResult (std::stringstream::out);

		lResult << "# OF STR. CONNECTED COMPs: " << cPartition.fGetSetCount() << std::endl;
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
		lResult << "CHECK SUM: " << lCheckSum << std::endl;

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

#endif /* STRCONN_H_ */
