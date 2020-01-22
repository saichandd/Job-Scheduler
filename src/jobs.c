    /*
 * Job manager for "jobber".
 */
#include <stdlib.h>
#include <string.h>
#include "jobber.h"
#include "task.h"

#include "other.h"


#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>

//GLOBALLLL VARIABULLLL ----------------------
int enable = 0;
int num_runners = 0;
//goes to max 4
int present_runners = 0;

//jobber table
//global struct -eveything intitlized  to 0
struct {
    int available;
    JOB_STATUS job_status;
    // int job_id;
    pid_t pid;
    TASK *task;
    char * job;
    int exit_status;
} ALL_JOBS[MAX_JOBS];


// char *job_status_names[] = {"new", "waiting", "running", "paused", "cancelled", "completed", "aborted"};

//i can go to a max of 7, then error
int i = 0;

int flag = 0;

int addJobToTable(JOB_STATUS job_status, pid_t pid, TASK * task, char * job);
void updateInitializedJob(int jobid);
int findEmptySlotInTable();
int startAJob(int jobid);
int startPipeline(PIPELINE *pipeline);
int startPipelineMaster(PIPELINE *pipeline);
int getCommandCount(PIPELINE *pipeline);
int createRunnerProcess(int jobid);
int changeStatus(int jobid,JOB_STATUS present_status,JOB_STATUS change_to_status);

void signal_handler(){
    //some job ended?
    //do something now?
    flag = 1;
}

int hook_handler(){

    pid_t pid;
    int status;

    if ((pid = (waitpid(-1, &status, WNOHANG))) != -1){
        num_runners--;
       /* Handle the death of pid p */
        // int exit_status = WIFEXITED(status);
        for(int i = 0; i < 8; i++){
            if(ALL_JOBS[i].pid == pid){

                // printf("--- %d\n", ALL_JOBS[i].pid);
                // printf("--- %d\n", ALL_JOBS[i].job_status);

                changeStatus(i, RUNNING,COMPLETED);
                int exit_status = WEXITSTATUS(status);

                // printf("%d\n",exit_status);
                sf_job_end(i, pid,exit_status);
                sf_job_status_change(i, RUNNING, COMPLETED);
            }
        }
    }
    flag = 0;
    return 0;
}

// void reapJob(int i){

//     ALL_JOBS[i].available = 1;
//     ALL_JOBS[i].job_status = 0;
//     ALL_JOBS[i].pid = -1;
//     ALL_JOBS[i].task = NULL;
//     ALL_JOBS[i].job = NULL;
//     ALL_JOBS[i].exit_status = -420;

// }

int jobs_init(void) {

    signal(SIGCHLD, signal_handler);
    //intitlizing everything to available
    for(int i = 0; i < 8; i++){
        ALL_JOBS[i].available = 1;
        ALL_JOBS[i].pid = -1;
        ALL_JOBS[i].exit_status = -420;
        ALL_JOBS[i].task = NULL;
        ALL_JOBS[i].job = NULL;
    }

    sf_set_readline_signal_hook(hook_handler);

    return 1;
}

int startJobber(){

    jobs_init();
    while(1){
        char *prompt = "jobber> ";
        char *user_input = sf_readline(prompt);

        // if(strcmp(user_input,"\n") == 0){
        //     continue;
        // }

        if( user_input == NULL){
            // EOF with empty
            debug("EOF");
            return 0;
        }
        //if it retuns empty string, just send jobber again
        else if(strcmp(user_input,"") == 0){
            debug("user_input is empty");
            continue;
        }

        char * user_input_copy = malloc(strlen(user_input) + 1);
        strcpy(user_input_copy, user_input);

        TASK *task = parse_task(&user_input_copy); //NOTE: user_input_copy gets changed by this function
        if( task == NULL){
            //UNSUCESSFULL PARSING
            debug("TASK is NULL");
            return 0;
        }
        //something should happen here
        else{
            //
            startJobsFunctionAsRequested(task, user_input);
            debug("TASK returned something");
        }
        // free(user_input);

        //CHECK JOBS TABLE AND SEE IF YOU CAN ADD A RUNNER
    }

    return 0;
    abort();
}

void jobs_fini(void) {
    // TO BE IMPLEMENTED
    abort();
}

int jobs_set_enabled(int enabled){

    int prev_state_enabled = enable;
    if(enabled > 0){
        enable = 1;
    }
    else{
        enable = 0;
    }

    return prev_state_enabled;
}

int jobs_get_enabled() {
    return enable;
}


//has more failing criteria
int job_create(char *job) {

    if(strcmp(job, "") == 0){
        printf("Error: spool\n");
        return 1;
    }

    char * job_copy = malloc(strlen(job) + 1);
    strcpy(job_copy, job);

    TASK *task = parse_task(&job_copy);

    printf("TASK: %s\n", job);
    int jobid = addJobToTable(NEW, -400, task, job);

    return jobid;
    // abort();
}


int addJobToTable(JOB_STATUS job_status, pid_t pid, TASK * task, char * job){


    int i = findEmptySlotInTable();

    if(i==-1){
        return -1;
    }
    debug("empty slot is %d", i);

    ALL_JOBS[i].available = 0;
    ALL_JOBS[i].job_status = job_status;
    ALL_JOBS[i].pid = pid;
    ALL_JOBS[i].task = task;
    ALL_JOBS[i].job = job;

    if(job_status == NEW){
        sf_job_create(i);
    }

    updateInitializedJob(i);

    return i;
    // tableUpdate();
}

int findEmptySlotInTable(){
    for(int i = 0; i < MAX_JOBS; i++){
        if(ALL_JOBS[i].available == 1){
            return i;
        }
    }
    return -1;
}

void updateInitializedJob(int jobid){

    if(enable == 0 && num_runners < 4){
        sf_job_status_change(jobid,ALL_JOBS[jobid].job_status, WAITING);
        ALL_JOBS[jobid].job_status = WAITING;
        // printf("job %d [%s]: %s\n", jobid, job_status_names[WAITING], ALL_JOBS[jobid].job);
    }
    else{
        ;
    }
}
// void tableUpdate(){
//     //checks the whole table and edits it
//     if enable = 0{
//         //enable all jobs
//     }
//     else{

//     }
// }

int job_expunge(int jobid) {

    if(ALL_JOBS[jobid].exit_status == ABORTED || ALL_JOBS[jobid].exit_status == COMPLETED){
        ALL_JOBS[i].available = 1;
        ALL_JOBS[i].job_status = 0;
        ALL_JOBS[i].pid = -1;
        ALL_JOBS[i].task = NULL;
        ALL_JOBS[i].job = NULL;
        ALL_JOBS[i].exit_status = -420;

        return 0;
    }

    return -1;
}

int job_cancel(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

int job_pause(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

int job_resume(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

int job_get_pgid(int jobid) {
    if(ALL_JOBS[i].job_status == RUNNING || ALL_JOBS[i].job_status == PAUSED || ALL_JOBS[i].job_status == CANCELED){
        return ALL_JOBS[i].pid;
    }
    return -1;
}

JOB_STATUS job_get_status(int jobid) {

    //is job exists
    if(ALL_JOBS[i].available == 0){
        return ALL_JOBS[jobid].job_status;
    }
    else{
        return -1;
    }
}

int job_get_result(int jobid) {

    if(ALL_JOBS[i].job_status == COMPLETED){
        return ALL_JOBS[i].pid;
    }
    return -1;
}

int job_was_canceled(int jobid) {
    // if(ALL_JOBS[i].job_status == CANCELED){
    //     ret
    // }
    return -1;
}

char *job_get_taskspec(int jobid) {
    if(ALL_JOBS[jobid].available == 0){
        return ALL_JOBS[jobid].job;
    }
    return NULL;
}


void prinJobStatus(int jobid){
    int job_status = ALL_JOBS[jobid].job_status;
    // printf("%s\n", );
    printf("job %d [%s]: %s\n", jobid, job_status_names[job_status], ALL_JOBS[jobid].job);
}

int getNumberOfRunningJobs(){
    int count = 0;
    for(int i = 0; i < 8; i++){
        //not empty
        if(ALL_JOBS[i].job_status == RUNNING){
            count++;
        }
    }
    return count;
}

void printJobs(){

    if(enable == 1){
        printf("Starting jobs in enabled\n");
    }else{
        printf("Starting jobs in disabled\n");
    }
    for(int i = 0; i < 8; i++){
        //not empty
        if(ALL_JOBS[i].available == 0){
            prinJobStatus(i);
        }
    }
}



int enableEnginesOn(){
    jobs_set_enabled(1);
    for(int i = 0; i < 8; i++){
        //not empty, waiting
        if(ALL_JOBS[i].available == 0 && ALL_JOBS[i].job_status == WAITING && num_runners < 4){
            num_runners++;
            ALL_JOBS[i].exit_status = createRunnerProcess(i);
        }
    }
    return 0;
}

int createRunnerProcess(int jobid){
    // int status;
    pid_t pid;
    // pid_t childpid;
    // create runner processess
    pid = fork();
    if(pid ==-1){
        //error
    }
    if(pid == 0){
        startAJob(jobid);

        //waitpid
        // waitpid(pid, &status, 0);
        // // printf("before changing status\n");
        // changeStatus(jobid, RUNNING,COMPLETED);
        // sf_job_status_change(jobid, RUNNING, COMPLETED);
        // printf("wtf aborted\n");
        exit(0);
    }
    else{
        ALL_JOBS[jobid].pid = pid;
        // printf("%d\n", pid);
        //change status in table
        changeStatus(jobid, WAITING,RUNNING);
        sf_job_start(jobid, pid);
        sf_job_status_change(jobid, WAITING, RUNNING);
    }
    // sf_job_end(int jobid, int pgid, int result)
    return 0;
}

int startAJob(int jobid){
    // create master processess for piepeline
    TASK *task = ALL_JOBS[jobid].task;

    //go through all pipelines
    PIPELINE_LIST * pl_list = task->pipelines;
    while(pl_list != NULL){
        startPipelineMaster(pl_list->first);
        pl_list = pl_list->rest;
    }
    return 0;
}

// last pipelines exit value is needed
// so return exit values
int startPipelineMaster(PIPELINE *pipeline){
    // char * input_path = pipeline->input_path;
    // char * output_path = pipeline->output_path;

    int master_status;
    // int master_child_status;
    pid_t master_pid;
    // pid_t childpid;
    // create runner processess
    master_pid = fork();
    if(master_pid ==-1){
        //error
    }
    else if(master_pid == 0){
        startPipeline(pipeline);
    }
    else{
        while((master_pid = waitpid(-1, &master_status, 0)) > 0){
            if(WIFEXITED(master_status))
                debug("%d terminated = %d\n", master_pid, WEXITSTATUS(master_status));
            else
                debug("child %d,\n", master_pid);
                // abort();
        }
    }

    return master_status;
}


int startPipeline(PIPELINE * pipeline){

    COMMAND_LIST * c_list = pipeline->commands;
    pid_t pid;
    int status;

    int old_fds[2];
    int new_fds[2];

    debug("pipeline started\n");

    int j = 0;
    // j runs in even numbers
    while(c_list != NULL){

        // printf("j = %d\n", j);
        COMMAND *command = c_list->first;


        if(c_list -> rest != NULL){
            pipe(new_fds);
        }

        pid = fork();

        if(pid == 0){
            // printf("child got run\n");

            // if first comamnd



            //if not last command
            if(j == 0 && pipeline->input_path != NULL){

                int fdin = open(pipeline->input_path, O_RDONLY);
                dup2(fdin,STDIN_FILENO);
                close(fdin);
                // redirectoin
                // printf("we got here 2\n");
            }
            else if (j != 0 ){

                dup2(old_fds[0],0);
                close(old_fds[0]);
                close(old_fds[1]);
            }


            if(c_list->rest == NULL && pipeline->output_path != NULL){

                int fdout = open(pipeline->output_path, O_WRONLY | O_CREAT);
                dup2(fdout, STDOUT_FILENO);
                close(fdout);
                // redirectoin
                // printf("we got here 2\n");
            }
            else if(c_list -> rest != NULL){
                close(new_fds[0]);
                dup2(new_fds[1], 1);
                close(new_fds[1]);
            }

            char *strings[10] = {NULL};
            char **ptr = strings;

            WORD_LIST *words  = command->words;

            int i = 0;
            while(words != NULL){

                ptr[i] = words->first;
                words = words->rest;
                i++;
            }
            //execvp takes words and
            // printf("jobber>\n");
            if(execvp(command->words->first, strings) < 0){
                debug("aborting in execvp\n");
                abort();
                // exit(EXIT_FAILURE);
            }
        }
        else if(pid < 0){
            debug("aborting in pid < 0\n");
            abort();
        }
        else{

            if(j!=0){
                close(old_fds[0]);
                close(old_fds[1]);

            }
            // wait(NULL);
            if(c_list -> rest != NULL){
                old_fds[0] = new_fds[0];
                old_fds[1] = new_fds[1];
            }
        }

        c_list = c_list->rest;
        j+=1;
    }

    while((pid = waitpid(-1, &status, 0)) > 0){
        if(WIFEXITED(status))
            debug("%d terminated, exit status = %d\n", pid, WEXITSTATUS(status) );
        else
            debug("%d aborted,\n", pid);
            // abort();
    }

    // exit();
    exit(0);
}


int getWordCount(COMMAND *command){
    int count = 0;
    WORD_LIST * w_list = command->words;
    while(w_list != NULL){
        count++;
        w_list = w_list->rest;
    }
    return count;
}



//CHANAGES JOB STATUS
int changeStatus(int jobid,JOB_STATUS present_status,JOB_STATUS change_to_status){

    // just verifying
    if(ALL_JOBS[jobid].available == 0 && ALL_JOBS[jobid].job_status == present_status){
        ALL_JOBS[jobid].job_status = change_to_status;
        return 1;
    } else{
        return -1;
    }

}