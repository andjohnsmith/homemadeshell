#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* For fork, exec, sleep */
#include <sys/types.h>
#include <unistd.h>
/* For waitpid */
#include <sys/wait.h>

//Check
#include <fcntl.h>

/******************************
 * Defines
 ******************************/
#define TRUE  1
#define FALSE 0

#define MAX_COMMAND_LINE 1024

#define PROMPT ("mysh$ ")


/******************************
 * Structures
 ******************************/
/*
 * A job struct.  Feel free to change as needed.
 */


struct job_t {
    char * full_command;
    int argc;
    char **argv;
    int is_background;
    char * binary;
    int pid;
    int position;
    int running;
};
typedef struct job_t job_t;

/*
 A Node Struct
 */
struct Node {
    struct job_t *job;
    struct Node *next;
};


/*
A Node List
 */
struct NodeList {
    int size;
    int total;
    struct Node *head;
    struct Node *tail;
};



/******************************
 * Global Variables
 ******************************/

/*
 * Interactive or batch mode
 */
int is_batch = FALSE;

 /*
  *Created Past template
  */
int is_debug = FALSE;
int num_batch_files = 0;
char **batch_files;
/*
 * Counts
 */
int total_jobs_display_ctr = 0;
int total_jobs    = 0;
int total_jobs_bg = 0;
int total_history = 0;

/******************************
 * Function declarations
 ******************************/

/*
 Job Creations/Methods
 */
struct job_t *jobCreate(char *full_command, int argc, char **argv, int is_background, char *binary);
char *jobFullCommand(struct job_t *job);
int jobArgc(struct job_t *job);
char **jobArgv(struct job_t *job);
int jobIsBackground(struct job_t *job);
char *jobBinary(struct job_t *job);

void setPosition(struct job_t *job, int position);
void setPID(struct job_t *job, int PID);
void setRunning(struct job_t *job);
void setDone(struct job_t *job);

/*
 Temp Job Creations/Methods
 */

/*
 NodeList Creation
 */
struct NodeList *listCreate();
void listAdd(struct NodeList *list, struct job_t *job);
void listHistory(struct NodeList *list);
void listRemove(struct NodeList *list, int pid);
void listJobs(struct NodeList *list);

/*
 NodeList deletion
 */
void clearList(struct NodeList *list);
void jobDelete(struct job_t *job);
void clearJobs(struct NodeList *list);
void endAll(struct NodeList *list, struct NodeList *jobs);

/*
 * Parse command line arguments passed to myshell upon startup.
 *
 * Parameters:
 *  argc : Number of command line arguments
 *  argv : Array of pointers to strings
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int parse_args_main(int argc, char **argv);

/*
 * Main routine for batch mode
 *
 * Parameters:
 *  None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int batch_mode(void);

/*
 * Main routine for interactive mode
 *
 * Parameters:
 *  None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int interactive_mode(void);

/*
 * Launch a job
 *
 * Parameters:
 *   loc_job : job to execute
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int launch_job(job_t * loc_job, struct NodeList *jobs, int fileDescriptor);

//Returns int of old stdin or stdout
int fileRedirectionInt(job_t *job);
/*
 * Built-in 'exit' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_exit(void);

/*
 * Built-in 'jobs' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_jobs(void);

/*
 * Built-in 'history' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_history(void);

/*
 * Built-in 'wait' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_wait(struct NodeList *);

/*
 * Built-in 'fg' command
 *
 * Parameters:
 *   None, or job id
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_fg(int, struct NodeList *);

int parseLine(char *, char *, struct NodeList *, struct NodeList *);

#endif /* MYSHELL_H */
