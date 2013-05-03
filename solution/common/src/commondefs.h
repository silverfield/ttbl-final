#ifndef COMMONDEFS_H_
#define COMMONDEFS_H_

/*****************************************************************/
/* Constants
/*****************************************************************/

//max. size of commands for ttblazer
#define COMMAND_BUFF_SIZE 1000
//max size of message output by ttblazer
#define MSG_BUFF_SIZE 5000
//port on which Commander and ttblazer's communicator communicate
#define COMMAND_PORT 6001
//port on which Printer listens for messages
#define PRINT_PORT 6002
#define PARTMSG_END "PARTMSG_END"

/*****************************************************************/
/* Data types
/*****************************************************************/

/* Return values of functions
 *
 * OK = well, ok :-)
 * NOK = function was unable to perform necessary actions, but the system should be in a stable state
 * ERR = something went wrong in the function and it may result in unstable system */
enum Ret { OK, NOK, ER };

#endif /* COMMONDEFS_H_ */
