/* Processing command line arguments */
#ifndef CMDLNPROC_H_
#define CMDLNPROC_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include "central.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

#define CLN_HELP "-h"
#define CLN_SCRIPT "-script"
#define CLN_TESTS "-tests"
#define CLN_DEB "-deb"
#define CLN_DEB_ON "on"
#define CLN_DEB_OFF "off"
#define CLN_LVLON "-lvlon"
#define CLN_LVLOFF "-lvloff"
#define CLN_LOGFILE "-logfile"
#define CLN_LOGPORT "-logport"
#define CLN_PERFORM "-perf"
#define CLN_PERF_SEP "^"
#define CLN_CMD_SRC "-cs"

/*****************************************************************/
/* Macros
/*****************************************************************/

/*****************************************************************/
/* Class
/*****************************************************************/

/*****************************************************************/
/* Functions
/*****************************************************************/

Ret fProcessCmdln(int argc, char *argv[]);

#endif /* CMDLNPROC_H_ */
