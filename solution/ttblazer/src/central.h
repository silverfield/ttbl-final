/* This header is included in almost all others, it contains:
 * - the most common includes that would be included almost everywhere
 * - global objects used at more places  */

#ifndef CENTRAL_H_
#define CENTRAL_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <stddef.h>

#include "../../common/src/commondefs.h"
#include "../../common/src/common.h"
#include "../../common/src/logger.h"
#include "../../common/src/algtimer.h"

#include "communicator.h"
#include "options.h"
#include "ttblazerlvls.h"

/*****************************************************************/
/* Definitions
/*****************************************************************/

#define NULL_STR "null"
#define ANY_RANGE std::pair<int, int>(-1, INT_MAX)

/*****************************************************************/
/* Types
/*****************************************************************/

enum ObjType {OTUg, OTTe, OTTd, OTTt, OTErr};
extern std::string gObjTypeCmdStr [];
extern std::string gObjTypeCmdStrs [];
extern std::string gObjTypeStr [];
extern std::string gObjTypeStrings []; //something more solid then str
extern std::string gObjTypeStringS []; //with capital first letter

enum QueryType {QTSp, QTDist, QTConn, QTEa};
extern std::string gQueryTypeStr [];
extern std::string gQueryTypeStrSmall [];
std::string fGetQueryTypeSetString(std::set<QueryType> iSet);

enum Conn {CONNStrong, CONNWeak, CONNNone};
extern std::string gConnStr [];
extern std::string gBoolStr [];

/*****************************************************************/
/* Global object declarations
/*****************************************************************/

extern Logger *gLogger;
extern Communicator *gCommunicator;
extern Options* gOptions;
extern AlgTimer* gAlgTimer;

/*****************************************************************/
/* Functions
/*****************************************************************/

std::string fTimeFormatToString(int iMinutes);
int fStringToTimeFormat(std::string iString);

#endif /* CENTRAL_H_ */
