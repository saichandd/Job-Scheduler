#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jobber.h"
#include "task.h"
#include "other.h"

//calls the right jobs function depending on the init
int startJobsFunctionAsRequested(TASK *task, char * user_input){
    // debug("startJobs called");

    //request by user
    char * req = task->pipelines->first->commands->first->words->first;
    // debug("%s %s", req, user_input);
    int req_num = isReqValid(req);

    //ABORT
    if(req_num == 0){
        printf("Unrecognized Task: %s\n", req);
        return 1;
    }

    //help
    if(req_num == 1){
        callHelp();
        return 1;
    }

    if(req_num == 2){
        // cancelAllJobs();
        exit(EXIT_SUCCESS);
    }
    else if(req_num == 3){
        debug("status");
        int job_num = getSecondWordAsInt(user_input, strlen(req));
        prinJobStatus(job_num);
        return 1;
    }
    else if(req_num == 4){
        printJobs();
        debug("jobs");
        // printJobReport()

    }
    else if(req_num == 5){
        // enabled = 1;
        jobs_set_enabled(1);
        enableEnginesOn();
        debug("enable");
    }
    else if(req_num == 6){
        jobs_set_enabled(0);
        debug("disable");
    }
    //CHANGE this so that spool echo works too ---------------------------------------
    else if(req_num == 7){
        char * job = getJobString(user_input, strlen(req));
        job_create(job);
        // create the freaking jobs here
        //where the fun begins
    }
    else if(req_num == 8){
        int job_num = getSecondWordAsInt(user_input, strlen(req));
        job_pause(job_num);
    }
    else if(req_num == 9){
        int job_num = getSecondWordAsInt(user_input, strlen(req));
        job_resume(job_num);
    }
    else if(req_num == 10){
        int job_num = getSecondWordAsInt(user_input, strlen(req));
        job_cancel(job_num);
    }
    else if(req_num == 11){
        int job_num = getSecondWordAsInt(user_input, strlen(req));
        job_expunge(job_num);
    }
    else{
        debug("SOS SOS SOS");
        abort();
    }
    return 0;
}

int getSecondWordAsInt(char * user_input, int l){
    user_input += l+1;
    return (int)(*user_input) - 48;
}

char * getJobString(char * user_input, int l){
    user_input += l+2;
    //replace last character with \0
    user_input[strlen(user_input)-1] = 0;

    debug("Job String after remove quotes is %s and length %ld", user_input, strlen(user_input));
    return user_input;
}


int isReqValid(char * req){

    if(strcmp(req, "help") == 0){
        return 1;
    }
    else if(strcmp(req, "quit") == 0){
        return 2;
    }
    else if(strcmp(req, "status") == 0){
        return 3;
    }
    else if(strcmp(req, "jobs") == 0){
        return 4;
    }
    else if(strcmp(req, "enable") == 0){
        return 5;
    }
    else if(strcmp(req, "disable") == 0){
        return 6;
    }
    else if(strcmp(req, "spool") == 0){
        return 7;
    }
    else if(strcmp(req, "pause") == 0){
        return 8;
    }
    else if(strcmp(req, "resume") == 0){
        return 9;
    }
    else if(strcmp(req, "cancel") == 0){
        return 10;
    }
    else if(strcmp(req, "expunge") == 0){
        return 11;
    }
    else{
        return 0;
    }
}



void callHelp(){

    printf("Available commands:\n\
help (0 args) Print this help message\n\
quit (0 args) Quit the program\n\
enable (0 args) Allow jobs to start\n\
disable (0 args) Prevent jobs from starting\n\
spool (1 args) Spool a new job\n\
pause (1 args) Pause a running job\n\
resume (1 args) Resume a paused job\n\
cancel (1 args) Cancel an unfinished job\n\
expunge (1 args) Expunge a finished job\n\
status (1 args) Print the status of a job\n\
jobs (0 args) Print the status of all jobs\n");

}
