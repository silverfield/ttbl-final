/* Debug levels for files in "ttblazer/src" */
#ifndef TTBLAZERLVLS_H_
#define TTBLAZERLVLS_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

//common levels 0 - 19
#include "../../common/src/commonlvls.h"

/*****************************************************************/
/* Constants
/*****************************************************************/

//CAT_HERE_FROM

//ttblazer logic (20 - 39)
#define LVL_MAIN 20
#define LVL_CMDPROC 21
#define LVL_COMMUNICATOR 22
#define LVL_MANIPULATOR 23
#define LVL_CMDLNPROC 24
#define LVL_TESTS 25
#define LVL_OPTIONS 26
#define LVL_CENTRAL 27
#define LVL_ALGS 28
#define LVL_STRUCTS 29
#define LVL_NEURAL 30
#define LVL_NEURALDET 31
#define LVL_ALGDET 32
#define LVL_ALGDETT 33

//objects (40 - 44)
#define LVL_UNDERGR 40
#define LVL_TIMETABLE 41
#define LVL_TIMEEXP 42
#define LVL_TIMEDEP 43
#define LVL_GRAPH 44

//oracles (50 - 59)
#define LVL_ORACLEINFOS 50
#define LVL_DIJKSTRA 51
#define LVL_OR_TGNEURAL 52

//modifiers (60 - 69)

//analysers (70 - 79)
#define LVL_CONN 70
#define LVL_PATHS 71
#define LVL_CONNS 72

//generators (80 - 89)

//SPECIAL levels 90 - 99
#define LVL_ALREADY 90

//CAT_HERE_TO

//initially turned on debug levels
#define LVL_INIT {\
	LVL_COMMON, \
	/*LVL_RECEIVER,*/ \
	LVL_LOGGER, \
	LVL_MAIN, \
	LVL_COMMUNICATOR, \
	LVL_CMDPROC, \
	LVL_GRAPH, \
	LVL_UNDERGR, \
	LVL_TIMETABLE, \
	LVL_TIMEEXP, \
	/*LVL_TIMEDEP,*/ \
	LVL_MANIPULATOR, \
	LVL_TESTS, \
	LVL_DIJKSTRA, \
	LVL_ALGS, \
	LVL_STRUCTS, \
	LVL_TIMER, \
	LVL_ALREADY, \
	LVL_PATHS, \
	LVL_NEURAL, \
	LVL_OR_TGNEURAL, \
	}

#endif /* TTBLAZERLVLS_H_ */
