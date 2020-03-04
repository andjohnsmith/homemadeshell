#include "mysh.h"

#define LINELEN 256

int main(int argc, char * argv[]) {
    /*
     * Parse Command line arguments to check if this is an interactive or batch
     * mode run.
     */
    if (0 != (parse_args_main(argc, argv))) {
        fprintf(stderr, "Error: Invalid command line!\n");
        return -1;
    }

    /*
     * If in batch mode then process all batch files
     */
    if (TRUE == is_batch) {
        if (TRUE == is_debug) {
            printf("Batch Mode!\n");
        }

        //        ( 0 != (ret = batch_mode()) )
        if (0 != (batch_mode())) {
            fprintf(stderr, "Error: Batch mode returned a failure!\n");
        }
    }
    /*
     * Otherwise proceed in interactive mode
     */
    else if (FALSE == is_batch) {
        if (TRUE == is_debug) {
            printf("Interactive Mode!\n");
        }

        //(ret = interactive_mode())
        if (0 != (interactive_mode())) {
            fprintf(stderr, "Error: Interactive mode returned a failure!\n");
        }
    }
    /*
     * This should never happen, but otherwise unknown mode
     */
    else {
        fprintf(stderr, "Error: Unknown execution mode!\n");
        return -1;
    }


    /*
     * Display counts
     */
    
    

    batch_files = NULL;
    return 0;
}

void printOut(){
    printf("-------------------------------\n");
    printf("Total number of jobs               = %d\n", total_jobs);
    printf("Total number of jobs in history    = %d\n", total_history);
    printf("Total number of jobs in background = %d\n", total_jobs_bg);
}

int parse_args_main(int argc, char **argv) {

    //INTERACTIVE MODE
    if (argc < 2) {

        //BATCH MODE
    } else {
        is_batch = TRUE;

        //FILL argv into batch_files
        batch_files = &argv[1];
        num_batch_files = argc - 1;
    }

    /*
     * If no command line arguments were passed then this is an interactive
     * mode run.
     */

    /*
     * If command line arguments were supplied then this is batch mode.
     */

    return 0;
}

int batch_mode(void) {
    struct NodeList *list = listCreate();
    struct NodeList *jobs = listCreate();
    int result;
    int i;

    for (i = 0; i < num_batch_files; i++) {
        printf("Looking in file: %s\n", batch_files[i]);
        /*
         NOTE:  size_t is an unsigned int
         ssize_t is a signed int, signed so that it can return a -1
         */
        char *line = NULL;
        FILE *fptr = NULL;
        size_t len = 0;
        ssize_t read;

        /*
         * Read stdin, break out of loop if Ctrl-D
         */

        if ((fptr = fopen(batch_files[i], "r")) == NULL) {
            printf("Error! opening file");

            //BREAK FROM EVERYTHING
            return(-1);
        }

        //READ until EOF
        while ((read = getline(&line, &len, fptr)) != -1) {
            char *token = NULL;

            if ('\n' == line[strlen(line) - 1]) {
                line[strlen(line) - 1] = '\0';
            }

            token = strtok(line, " ");
            result = parseLine(token, line, list, jobs);
            if (result == 0) {
                endAll(list,jobs);
                return result;
            }
            //PARSE AND EXECUTE
        }

        //NOTE: FREE on our own because getline allocates the space
        free(line);
        line = NULL;
        fptr = NULL;
    }

    endAll(list,jobs);
    return 0;
}

int interactive_mode(void) {
    struct NodeList *list = listCreate();
    struct NodeList *jobs = listCreate();
    char *line = NULL;
    char *token = NULL;
    size_t len = 0;
    ssize_t read;
    int result;

    do {
      printf(PROMPT);
        read = getline(&line, &len, stdin);

        //CHECK FOR EOF
        if (read == -1) {
            endAll(list,jobs);
            return 0;
        }

        //Stripping newline
        if ('\n' == line[strlen(line) - 1]) {
            line[strlen(line) - 1] = '\0';
        }

        //TOKENIZE
        token = strtok(line," ");

        result = parseLine(token, line, list, jobs);
        if (result == 0) {
            endAll(list,jobs);
            return result;
        }
        //EXECUTE COMMAND
    } while (1 == 1);

    endAll(list,jobs);
    return 0;
}

int parseLine(char *token, char *line, struct NodeList *list, struct NodeList *jobs) {
    int background = 0;
    int redirect = 0;

    while (token != NULL) {
        redirect = 0;
        if(strcmp(token, ";") == 0){
            //SKip everyting
             token = strtok(NULL, " ");
        }else if (strcmp(token, "exit") == 0) {
            //wait for background processes
            builtin_wait(jobs);
            total_history++;
            return 0;
        } else if (strcmp(token, "jobs") == 0) {
            char **tempArgs = (char **) malloc(sizeof(char *) * 2);
            tempArgs[0] = (char *) malloc(sizeof(char) * (strlen("jobs") + 1));

            tempArgs[1] = NULL;
            strcpy(tempArgs[0], "jobs");

            struct job_t *job = jobCreate(line, 2, tempArgs, background, tempArgs[0]);
            listAdd(list, job);
            total_history++;

            listJobs(jobs);
            token = strtok(NULL, " ");
        } else if (strcmp(token, "history") == 0) {
            //HISTORY
            //ALLOCATE FOR HISTORY - Recent
            char **tempArgs = (char **) malloc(sizeof(char *) * 2);
            tempArgs[0] = (char *) malloc(sizeof(char) * (strlen("history") + 1));

            tempArgs[1] = NULL;
            strcpy(tempArgs[0], "history");

            struct job_t *job = jobCreate(line, 2, tempArgs, background, tempArgs[0]);
            listAdd(list, job);
            total_history++;
            //
            listHistory(list);

            //READ NEXT
            token = strtok(NULL, " ");
        } else if (strcmp(token, "fg") == 0) {
            /* need to take in number for id */
            //Alloc for history

            token = strtok(NULL, " ");
            if (token == NULL || strcmp(token, ";")==0) {
                //ALLOCATE FOR HISTORY - Recent
                char **tempArgs = (char **) malloc(sizeof(char *) * 2);
                tempArgs[0] = (char *) malloc(sizeof(char) * (strlen("fg") + 1));

                tempArgs[1] = NULL;
                strcpy(tempArgs[0], "fg");

                struct job_t *job = jobCreate(line, 2, tempArgs, background, tempArgs[0]);
                listAdd(list, job);
                total_history++;
                //

                builtin_fg(-1, jobs);
            } else if (atoi(token) <= 0) {
                printf("Invalid ID given\n");
            } else {
                //ALLOCATE FOR HISTORY - Recent
                char **tempArgs = (char **) malloc(sizeof(char *) * 3);
                tempArgs[0] = (char *) malloc(sizeof(char) * (strlen("fg") + 1));
                tempArgs[1] = (char *) malloc(sizeof(char) * (strlen(token) + 1));
                tempArgs[2] = NULL;
                strcpy(tempArgs[0], "fg");
                strcpy(tempArgs[1], token);

                struct job_t *job = jobCreate(line, 2, tempArgs, background, tempArgs[0]);
                listAdd(list, job);
                total_history++;

                builtin_fg(atoi(token), jobs);
                token = strtok(NULL, " ");
            }
        } else if (strcmp(token, "wait") == 0) {
            total_history++;
            builtin_wait(jobs);
            token = strtok(NULL, " ");
        } else {
            int i = 0;
            char **tempArgs = (char **) malloc(sizeof(char *) * 1);
            while (token != NULL) {
                //REALLOC
                if (strcmp(token, ";") == 0) {
                    //SPLIT
                    token = strtok(NULL, " ");
                    break;
                } else if (strcmp(token, "&") == 0) {
                    tempArgs = realloc(tempArgs, (sizeof(char *) * (i + 2)));

                    tempArgs[i] = (char *) malloc(sizeof(char) * (strlen(token) + 1));
                    strcpy(tempArgs[i], token);
                    token = strtok(NULL, " ");
                    background = 1;
                    break;
                } else if (strcmp(token, ">") == 0 || strcmp(token, "<") == 0 ) {
                    //IF THIS TOKEN isnt file null
                    tempArgs = realloc(tempArgs, (sizeof(char *) * (i + 3)));
                    tempArgs[1] = (char *) malloc(sizeof(char) * (strlen(token) + 1));
                    strcpy(tempArgs[1], token);
                    token = strtok(NULL, " ");
                    if (token == NULL) {
                        //DIDNT PASS A FILE
                        printf("ERROR - NO FILE");
                        return -1;
                    }

                    //{Command} '<' {file} - Space for 3
                    tempArgs[2] = (char *) malloc(sizeof(char) * (strlen(token) + 1));
                    strcpy(tempArgs[2], token);
                    token = strtok(NULL, " ");
                    redirect = 1;
                } else {
                    tempArgs = realloc(tempArgs, (sizeof(char *) * (i + 2)));
                    tempArgs[i] = (char *) malloc(sizeof(char) * (strlen(token) + 1));
                    strcpy(tempArgs[i], token);
                    token = strtok(NULL, " ");
                    i++;
                }
            }
            if (!redirect) {
                tempArgs[i] = NULL;
            } else {
                tempArgs[3] = NULL;
            }

            struct job_t *job = jobCreate(line, i + 1, tempArgs, background, tempArgs[0]);
            listAdd(list, job);
            total_history++;
            if (jobIsBackground(job) == 1) {
                setPosition(job, jobs->total + 1);
                listAdd(jobs, job);
            }

            if (redirect) {
                if (strcmp(tempArgs[1], ">") == 0 || strcmp(tempArgs[1], "<") == 0) {
                    //CALL redirect
                    launch_job(job, jobs, fileRedirectionInt(job));
                } else {
                    launch_job(job, jobs,-1);
                }
            } else {
                 launch_job(job, jobs,-1);
            }

            //RESET BACKGROUND
            redirect = 0;
            background = 0;
        }
        //vvvvv LINE != NULL vvvvvv
    }
    
    return 1;
}

/*
 * You will want one or more helper functions for parsing the commands
 * and then call the following functions to execute them
 */

int launch_job(job_t *loc_job, struct NodeList *jobs, int file_descriptor) {
    total_jobs++;
    pid_t c_pid = 0;
    int status = 0;
    char *binary = jobBinary(loc_job);
    char **args = jobArgv(loc_job);

    //If we are in file redirection
    if (file_descriptor != -1) {
        if (strcmp(loc_job->argv[1], ">") == 0) {
            int newfile_descriptor = open(loc_job->argv[2], O_WRONLY);
            dup2(newfile_descriptor, STDOUT_FILENO);
        } else {
            int newfile_descriptor = open(loc_job->argv[2], O_RDONLY);
            dup2(newfile_descriptor, STDIN_FILENO);
        }

    }

    /* fork a child process */
    c_pid = fork();

    if (c_pid < 0) {
        fprintf(stderr, "Error: Fork failed!\n");
        return -1;
    } else if (c_pid == 0) {
        execvp(binary, args);
        /* exec does not return on success.
         * If we are here then error out */
        fprintf(stderr, "Error: Exec failed!\n");
        exit(-1);
    } else {
        //FOREGROUND
        if (jobIsBackground(loc_job) == 0) {
            waitpid(c_pid, &status, 0);
        } else {
            //BACKGROUND
            total_jobs_bg++;
            setPID(loc_job,c_pid);
            waitpid(c_pid, &status, WNOHANG);
            setDone(loc_job);
        }

        if (file_descriptor != -1) {
            if (strcmp(loc_job->argv[1], ">") == 0) {
                dup2(file_descriptor, STDOUT_FILENO);
            } else {
                dup2(file_descriptor, STDIN_FILENO);
            }
            close(file_descriptor);
        }
    }

    return 0;
}

int builtin_exit(void) {

    return 0;
}

int builtin_jobs(void) {

    return 0;
}

int builtin_history(void) {

    return 0;
}

int builtin_wait(struct NodeList *jobs) {
    struct Node *current_node = jobs->head;
    struct job_t *job = NULL;

    current_node = jobs->head;
    while (current_node != NULL) {
        job = current_node->job;
        waitpid(job->pid, NULL, 0);
        current_node = current_node->next;
    }

    job = NULL;
    current_node = NULL;
    return 0;
}

int builtin_fg(int id, struct NodeList *jobs) {
    if (jobs->head == NULL) {
        printf("No jobs in background\n");
        return 0;
    } else {
        struct job_t *job = NULL;

        if (id == -1) {
            /* no argument means select most recent job */
            job = jobs->tail->job;
        } else {
            int i = 0;
            struct Node *current_node = jobs->head;

            while (i < id - 1) {
                if (current_node->next == NULL) {
                    printf("Invalid ID given\n");
                    return 0;
                }
                current_node = current_node->next;
                i++;
            }
            job = current_node->job;
            current_node = NULL;
        }
        waitpid(job->pid, NULL, 0);

        job = NULL;
        return 1;
    }
}

int fileRedirectionInt(job_t *job) {
    //redirects output to specified file which is in tempArgs[2]
    if (strcmp(job->argv[1], ">") == 0) {
        int default_stdout;
        //Save int to the regulat stdOut
        default_stdout = dup(STDOUT_FILENO);
        return default_stdout;
    //redirects output to specified file which is in tempArgs[2]
    } else {
        int default_stdout;
        //Save int to the regulat stdOut
        default_stdout = dup(STDIN_FILENO);
        return default_stdout;
    }
}



/*
 Job Functions
 */

struct job_t *jobCreate(char *full_command, int argc, char **argv, int is_background, char *binary) {
    struct job_t *job = NULL;
    job = (struct job_t *) malloc(sizeof(struct job_t));
    job->full_command = (char *) malloc(sizeof(char)
        * (strlen(full_command) + 1));
    strcpy(job->full_command, full_command);
    job->argc = argc;

    job->argv = argv;
    job->is_background = is_background;
    job->binary = (char *) malloc(sizeof(char) * (strlen(binary) + 1));
    strcpy(job->binary, binary);
    job->pid = 0;
    job->position = 0;
    job->running = 1;
    return job;
}

char *jobFullCommand(struct job_t *job) {
    return (job->full_command);
}

int jobArgc(struct job_t *job) {
    return job->argc;
}

char **jobArgv(struct job_t *job) {
    return job->argv;
}

int jobIsBackground(struct job_t *job) {
    return job->is_background;
}

char *jobBinary(struct job_t *job) {
    return job->binary;
}

void setPosition(struct job_t *job, int position) {
    job->position = position;
}
void setPID(struct job_t *job, int PID) {
    job->pid = PID;
}
void setRunning(struct job_t *job) {
    job->running = 1;
}

void setDone(struct job_t *job) {
    job->running = 0;
}

/*
 Node list functions
 */

struct NodeList *listCreate() {
    struct NodeList *list = malloc(sizeof(struct NodeList));
    list->size = 0;
    list->total = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void listAdd(struct NodeList *list, struct job_t *job) {

    if (list->head == NULL) {
        struct Node *onlyElement = malloc(sizeof(struct Node));
        onlyElement->job = job;
        onlyElement->next = NULL;
        list->head = onlyElement;
        list->tail = onlyElement;
        //NON-EMPTY-LIST
    } else {
        //ADD NODE TO END OF THE LIST
        struct Node *newTail = malloc(sizeof(struct Node));
        struct Node *prevTail = list->tail;
        newTail->job = job;
        newTail->next = NULL;
        prevTail->next = newTail;
        list->tail = newTail;
    }
    list->size++;
    list->total++;
}

void listHistory(struct NodeList *list) {
    struct Node *curr = list->head;
    if (list->head == NULL) {
        printf("LIST IS NULL\n");
        return;
    }
    int count = 1;
    while (curr != NULL) {
        int i;
        printf("%d ", count);
        for (i = 0; i < curr->job->argc - 1; i++) {
            printf("%s ", curr->job->argv[i]);
        }

        if (curr->job->is_background) {
            printf("%c", '&');
        }
        printf("\n");
        curr = curr->next;
        count++;
    }
}

void listJobs(struct NodeList *list) {
    int status = 0;
    struct Node *curr = list->head;
    struct Node *temp = NULL;
    struct Node *previous = NULL;
    if (list->head == NULL) {
        return;
    }
    while (curr != NULL) {
        if (waitpid(curr->job->pid, &status, WNOHANG) != 0) {

            printf("[%d]\tDone\t", curr->job->position);
            if (previous == NULL) {
                list->head = curr->next;
            } else {
                previous->next = curr->next;
            }
            //Print out
            int i;
            for (i = 0; i < curr->job->argc - 1; i++) {
                printf(" %s ", curr->job->argv[i]);
            }
            //FREE THE NODE
            temp = curr;
            curr = curr->next;
            free(temp);
            //FREE NODE
            list->size = list->size - 1;
            printf("\n");
        } else {
            printf("[%d]\tRunning\t", curr->job->position);
            int i;
            for (i = 0; i < curr->job->argc - 1; i++) {
                printf(" %s ", curr->job->argv[i]);
            }
            printf("\n");
            previous = curr;
            curr = curr->next;
        }
    }

    if (list->size == 0) {
        list->total = 0;
    }
}

void listRemove(struct NodeList *list, int pid) {
    struct Node *curr = list->head;
    struct Node *previous = NULL;
    int i;
    for (i = 0; i < list->size; i++) {
        if (curr->job->pid == pid) {
            if (previous == NULL) {
                list->head = curr->next;
            } else {
                previous->next = curr->next;
            }
            list->size = list->size - 1;
            //Free job
            jobDelete(curr->job);
            //Free node
            free(curr);
            break;
        }
        previous = curr;
        curr = curr->next;
    }
}

void clearList(struct NodeList *list){
    struct Node *tempNode;
    struct Node *delNode;
    
    tempNode = list->head;
    while(tempNode != NULL){
        delNode = tempNode;
        tempNode = tempNode->next;
        //Free Job
        jobDelete(delNode->job);
        //Free Node
        free(delNode);
    }
    //Free NodeList
    free(list);
}


void jobDelete(struct job_t *job){
    //Free every pointer in argv
    int i;
    for(i=0;i<job->argc;i++){
        free(job->argv[i]);
    }
    //Free pointer to argv
    free(job->argv);
    free(job->full_command);
    free(job->binary);
    free(job);
}

void clearJobs(struct NodeList *list){
    //Job has already been deleted but the pointer has not
    struct Node *tempNode;
    struct Node *delNode;
    
    tempNode = list->head;
    while(tempNode != NULL){
        delNode = tempNode;
        tempNode = tempNode->next;
        free(delNode);
    }
    //Free NodeList
    free(list);
}

void endAll(struct NodeList *list, struct NodeList *jobs){
    printOut();
    clearList(list);
    clearJobs(jobs);
}






