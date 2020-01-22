#include <stdlib.h>
#include <errno.h>

#include "jobber.h"
#include "task.h"
#include "other.h"
/*
 * "Jobber" job spooler.
 */

int main(int argc, char *argv[]){

    int ret = startJobber();
    return ret;
    exit(EXIT_FAILURE);
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */

