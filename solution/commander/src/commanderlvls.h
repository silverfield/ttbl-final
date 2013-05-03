/* Debug levels for files in "commander" */

#ifndef COMMANDERLVLS_H_
#define COMMANDERLVLS_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

//common levels 0 - 19
#include "../../common/src/commonlvls.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

//commander levels 60 - 64
#define LVL_COMMANDER 60

//SPECIAL levels 90 - 99

//initially turned on debug levels
#define LVL_INIT {\
	LVL_COMMON, \
	LVL_RECEIVER, \
	LVL_LOGGER, \
	LVL_COMMANDER, \
	}

#endif /* COMMANDERLVLS_H_ */
