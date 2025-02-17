#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define CMDLINE_MAX 512

// max for each command and argument
#define CMD_MAX 62
#define ARG_MAX 450

typedef struct
{
    char *command;
    char *arguments;

    // these two can be unused, not every job involves redirecting or a file
    bool direction;
    char *fileName;
} Job;

void initializeJobs(Job *jobs, int size)
{

    for (int i = 0; i < size; i++)
    {

        jobs[i].command = (char *)malloc(CMD_MAX * sizeof(char));

        jobs[i].arguments = (char *)malloc(ARG_MAX * sizeof(char));

        /*
        // for execv function
        // first arg is the command
        jobs[i].arguments[0] = (char *)malloc(CMD_MAX * sizeof(char));

        // second arg is argument
        jobs[i].arguments[1] = (char *)malloc(ARG_SIZE_MAX * sizeof(char));

        // third arg is always null for execv
        jobs[i].arguments[2] = NULL;*/
    }
}

void freeJobs(Job *jobs, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(jobs[i].command);
        free(jobs[i].arguments);
        jobs[i].command = NULL;
        jobs[i].arguments = NULL;
    }
}

int countNumPipes(char *commandLine)
{
    // count number of pipes
    int numPipe = 0;
    char *pipeChar;
    pipeChar = strchr(commandLine, '|');
    while (pipeChar != NULL)
    {
        numPipe++;
        pipeChar = strchr(pipeChar + 1, '|');
    }

    return numPipe;
}

int main(int argc, char *argv[])
{
    do
    {
        // store the command line
        char *commandLine = NULL;
        commandLine = (char *)malloc(CMDLINE_MAX * sizeof(char));

        if (commandLine == NULL)
        {
            exit(1);
        }

        bool isPiping = false;
        int jobIndex = 0;
        int argIndex = 0;
        Job *jobs = NULL;

        printf("sshell$: ");
        // get command line
        fgets(commandLine, CMDLINE_MAX, stdin);

        int numJobs = countNumPipes(commandLine) + 1;

        // allocate space for enough jobs
        if (jobs == NULL)
        {
            jobs = (Job *)malloc(numJobs * sizeof(Job));
        }

        if (jobs == NULL)
        {
            exit(1);
        }


        

        // initialize all variables in each job
        initializeJobs(jobs, numJobs);

        // read command line character by character
        for (int i = 0; i < strlen(commandLine) && commandLine[i] != '\n'; i++)
        {

            if (commandLine[i] == ' ')
            {
                continue;
            }

            if (i == 0 || isPiping)
            {
                // add /bin/ directory to command variable
                strcat(jobs[jobIndex].command, "/bin/");
                int commandIndex = strlen(jobs[jobIndex].command);
                // read the command using the same i index
                for (; i < strlen(commandLine) && commandLine[i] != ' ' && commandLine[i] != '|' && commandLine[i] != '\n'; i++)
                {
                    jobs[jobIndex].command[commandIndex] = commandLine[i];
                    commandIndex++;
                }
                // read argument
                if (commandLine[i] == ' ')
                {
                    i++;
                    for (; i < strlen(commandLine) && commandLine[i] != '|' && commandLine[i] != '\n'; i++)
                    {
                        jobs[jobIndex].arguments[argIndex] = commandLine[i];
                        argIndex++;
                    }
                }

                // reset piping
                if (isPiping)
                {
                    isPiping = false;
                }

            }

            // piping exist so set it to true and continue to true
            if (commandLine[i] == '|')
            {
                isPiping = true;
                jobIndex++;
                argIndex = 0;
                continue;
            }
        }

        for (int i = 0; i < numJobs; i++)
        {
            pid_t pid;

            if (strlen(jobs[i].arguments) == 0) {
                jobs[i].arguments = NULL;
            }
            
            char *cmd = jobs[i].command;
            char *args[] = {cmd, jobs[i].arguments, NULL};

            pid = fork();
            if (pid == 0)
            {
                // child process
                execv(cmd, args);
                perror("execv");
                exit(1);
            }
            else if (pid > 0)
            {
                // parent process
                int retval;
                wait(&retval);

                if (args[1] == NULL) {
                    fprintf(stdout, "+ completed '%s' [%d]\n", cmd + 5, retval);
                } else {
                    fprintf(stdout, "+ completed '%s %s' [%d]\n", cmd + 5, args[1], retval);
                }
            }
            else
            {
                perror("fork");
                exit(1);
            }
        }

        freeJobs(jobs, numJobs);
        free(commandLine);
        free(jobs);
    } while (1);
    return EXIT_SUCCESS;
}

	// char *cmd = "/bin/date -u";
	// int retval;

	// //retval = system(cmd);
	// fprintf(stdout, "Return status value for '%s': %d\n", cmd, retval);
    
    // pid_t pid;
    // char *cmd = "/bin/date";
    // char **args = { cmd, "-u", NULL };
    
    // pid = fork();
    // if (pid == 0) {
    //     // child process
    //     execv(cmd, args);
    //     perror("execv");
    //     exit(1);
    // } else if (pid > 0) {
    //     // parent process
    //     int retval;
    //     wait(&retval);
    //     fprintf(stderr, "+ completed '%s %s' [%d]\n", cmd, args[1], retval);
    // } else {
    //     perror("fork");
    //     exit(1);
    // }
     
    
    // char *command;
    // size_t command_size = 512;
    
    // // allocate space to store the command line
    // command = (char *) malloc(command_size * sizeof(char));
    
    // if (command == NULL) {
    //     exit(1);
    // }
    
    // while(1) {
       
    //     printf("sshell$: ");
        
    //     // get the command after sshell$:
    //     getline(&command, &command_size, stdin);
        
    //     // check if the command has a trailing new line
    //     char *new_line = strchr(command, '\n');
        
    //     // if ptr is not NULL, then there's a trailing new line
    //     // set ptr to terminating character
    //     if (new_line != NULL) {
    //         *new_line = '\0';
    //     }
        
    //     char *first_space = strchr(command, ' ');
        
    //     int first_space_num = first_space - command;
        
    //     printf("%d\n", first_space_num);
        
    //     char* cmd = (char *) malloc(command_size * sizeof(char));
        
    //     readNextCommand(cmd, command);
        
    //     printf("%s\n", cmd);
        
    //     readNextCommand(cmd, command);
        
    //     printf("%s\n", cmd);
        
        
    //     char *date = "date";
        
    //     // check if the entered command matches one of the built-in command
    //     if (strncmp(command, date, 512) == 0) {
    //         pid_t pid;
    //         char *cmd = "/bin/date";
    //         char *args[] = { cmd, "-u", NULL };
            
    //         pid = fork();
    //         if (pid == 0) {
    //             // child process
    //             execv(cmd, args);
    //             perror("execv");
    //             exit(1);
    //         } else if (pid > 0) {
    //             // parent process
    //             int retval;
    //             wait(&retval);
    //             fprintf(stderr, "+ completed '%s' [%d]\n", command, retval);
    //         } else {
    //             perror("fork");
    //             exit(1);
    //         }
    //     }
    // }
    
