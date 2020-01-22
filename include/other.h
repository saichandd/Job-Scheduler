#include <sys/stat.h>
#include <fcntl.h>



//whos connecting this file
int startJobber();
int startJobsFunctionAsRequested(TASK *task, char * user_input);
int isReqValid(char * req);
char * getJobString(char * user_input, int l);
void callHelp();
int getSecondWordAsInt(char * user_input, int l);
void prinJobStatus(int jobid);
void printJobs();
int enableEnginesOn();
int getWordCount(COMMAND *command);
char ** getArgumentStr(COMMAND *command);
void reapJob(int i);