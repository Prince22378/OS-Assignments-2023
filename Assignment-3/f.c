#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
// #include "header.h"
#include "queue.h"
#define MAX_HISTORY_SIZE 50
#define MAX_INPUT_LENGTH 1024

// Function to create a new queue
struct Queue* createQueue();
// Function to check if the queue is empty
int isEmpty(struct Queue* queue);
// Function to enqueue an item into the queue
void enqueue(struct Queue* queue, char* command, int PID, long exec_time, long wait_time);
// Function to dequeue an item from the queue
struct QueueItem* dequeue(struct Queue* queue);
// Function to show the contents of the queue
// void showQueue(struct Queue* queue);
// Declaration of functions in sourcefile2.c
void printReadyQueueContents();

void copyAndPrintFirstNItems(struct Queue* mainQueue, struct Queue* newQueue, int N);
void roundRobinScheduling(struct Queue* newQueue, int timeQuantum);
// struct Queue* ready_queue;

int NCPU,TSLICE;
// Struct to store command history with execution details
struct CommandHistory {
    char* command;
    pid_t pid;
    struct timeval start_time;
    struct timeval end_time;
};
struct CommandHistory history[MAX_HISTORY_SIZE];
int history_count = 0; // Number of commands in history

// Function to add commands to history
void add_to_history(char* command, pid_t pid, struct timeval start_time, struct timeval end_time) {
    if (pid != -1) {  // Entries with a valid process ID
        if (history_count < MAX_HISTORY_SIZE) {
            history[history_count].command = strdup(command);
            history[history_count].pid = pid;
            history[history_count].start_time = start_time;
            history[history_count].end_time = end_time;
            history_count++;
        } else {
            free(history[0].command); // Removing the oldest command from history
            for (int i = 0; i < history_count - 1; i++) {
                history[i] = history[i + 1];
            }
            history[history_count - 1].command = strdup(command);
            history[history_count - 1].pid = pid;
            history[history_count - 1].start_time = start_time;
            history[history_count - 1].end_time = end_time;
        }
    }
}

// Function to display history
void display_history() {
    for (int i = 0; i < history_count; i++) {
        printf(" %d. %s\n",i+1, history[i].command);
    }
}

// Function to get the current time
void get_current_time(struct timeval* tv) {
    gettimeofday(tv, NULL);
}

// Function to create a process and run a command
int create_process_and_run(char* command) {
    struct timeval start_time, end_time;
    int status;
    get_current_time(&start_time); // Record starting time
    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        // This code runs in the child process
        // Execute the command using exec
        status = execlp("/bin/sh", "/bin/sh", "-c", command, (char*)NULL);

        if (status == -1) {
            perror("Failed to execute the command");
            exit(EXIT_FAILURE);
        }
    } else {
        // This code runs in the parent process
        // Wait for the child process to complete
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status)) {
            // Child process exited normally, get its exit status
            status = WEXITSTATUS(status);
        } else {
            // Child process did not exit normally
            status = -1;
        }
        get_current_time(&end_time); // Record ending time
        // Add the command to history with execution details
        add_to_history(command, child_pid, start_time, end_time);
    }
    return status;
}

// Function to launch a command
int launch(char* command) {
    int status;
    status = create_process_and_run(command);
    return status;
}

// Function to read user input
char* read_user_input() {
    char* input = malloc(MAX_INPUT_LENGTH); // Allocate memory for input
    if (input == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    // Read user input
    if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
        free(input);
        return NULL; // End of input or error
    }
    // Remove the trailing newline character if present
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }
    return input;
}

// void handle_alarm(int sig, pid_t pid){
//     printf("\nstop\n");
//     kill(pid,SIGSTOP);
//     exit(0);
// }


// void executescheduler(struct Queue* ready_queue, int NCPU, int TSILCE){
//     for(int i = 0; i<NCPU; i++){
//         if(!isEmpty(ready_queue)){
//             struct QueueItem* p = dequeue(ready_queue);
//             kill(p->data[0], SIGCONT);
//         }
//     }
// }
int arr[10];
volatile int done = 0; // Flag to check if the process is done

// void handle_alarm(int sig) {
//     // Stop the current process
//     // for(int i = 0; i<NCPU; i++){
//     //     printf("%d\n",arr[i]);
//     //     if(kill((pid_t)arr[i], SIGSTOP)==0){
//     //         printf("Stopping\n");
//     //     }else{
//     //         perror("Failed\n");
//     //     }
//     // }
//     done = 1;
// }

// void executescheduler(struct Queue* ready_queue, int NCPU, int TSILCE){
//     for(int i = 0; i<NCPU; i++){
//         if(!isEmpty(ready_queue)){
//             struct QueueItem* p = dequeue(ready_queue);
//             arr[i] = p->data[0];
//             // kill(p->data[0], SIGCONT);
//             // signal(SIGALRM, handle_alarm);
//             // Set up the signal handler using sigaction
//             struct sigaction sa;
//             sa.sa_handler = handle_alarm;
//             sigemptyset(&sa.sa_mask);
//             sa.sa_flags = 0;
//             sigaction(SIGALRM, &sa, NULL);
//             alarm(TSILCE);
//             int result;
//             int start_time = time(NULL);
//             while (!done) {
//                 result = kill(p->data[0], SIGCONT);;
//                 int current_time = time(NULL);
//                 if (current_time - start_time >= TSILCE) {
//                     kill(p->data[0],SIGSTOP);
//                     break;
//                 }
//             }
//             // pause();
//         }
//     }
// }


void handle_alarm(int sig){
    // printf("\nstop\n");
    // kill(pid,SIGSTOP);
    for(int i = 0; i<NCPU; i++){
        printf("Stopping process %d",arr[i]);
        kill(arr[i],SIGSTOP);
    }
    exit(0);
}


void executescheduler(struct Queue* ready_queue, int NCPU, int TSILCE){
    for(int i = 0; i<NCPU; i++){
        if(!isEmpty(ready_queue)){
            struct QueueItem* p = dequeue(ready_queue);
            kill(p->data[0], SIGCONT);
            signal(SIGALRM, handle_alarm);
            alarm(TSILCE);
            enqueue(ready_queue,p,p->data[0],p->data[1],p->data[2]);
        }
    }
}


// Main shell loop
void shell_loop() {
    ready_queue = createQueue();
    // newQueue = createQueue();
    int status;
    char* last_command = NULL; // Store the last entered command
    do {
        printf("$ ");
        char* command = read_user_input();
        if (command != NULL) {
                        // Check if the entered command is not the same as the previous one
            if (last_command == NULL || strcmp(command, last_command) != 0) {
                add_to_history(command, -1, (struct timeval){0}, (struct timeval){0});  // Cmd, PID, start time, end time
                free(last_command); // Free the previous command
                last_command = strdup(command); // Update the last_command
            }
            if (strcmp(command, "exit") == 0) {
                // Display command history with execution details before exiting
                for (int i = 0; i < history_count; i++) {
                    printf("Command: %s\n", history[i].command);
                    printf("Process ID: %d\n", history[i].pid);
                    printf("Start Time: %ld.%06ld seconds\n", history[i].start_time.tv_sec, history[i].start_time.tv_usec);
                    printf("End Time: %ld.%06ld seconds\n", history[i].end_time.tv_sec, history[i].end_time.tv_usec);
                    printf("Duration: %ld.%06ld seconds\n",
                        history[i].end_time.tv_sec - history[i].start_time.tv_sec,
                        history[i].end_time.tv_usec - history[i].start_time.tv_usec);
                    printf("\n");
                }
                free(command);
                free(last_command);
                break;
            } else if (strcmp(command, "history") == 0) {
                display_history();
            }else if (strncmp(command, "submit", 6) == 0) {
                // Extract the executable path from the "submit" command
                char* executable_path = command + 7; // Skip "submit "
                // Create a new process
                pid_t child = fork();
                if (child == 0) {
                    // This is the child process
                    // Start the execution of the program directly
                    // char* const argv[] = {executable_path, NULL};
                    // execvp(executable_path, argv);
                    execlp("/bin/sh", "/bin/sh", "-c", executable_path, (char*)NULL);
                    // If execvp returns, there was an error
                    perror("execvp");
                    exit(EXIT_FAILURE);
                } else if (child > 0) {
                    if(kill(child, SIGSTOP)==0){

                    }else{
                        perror("Suspend failed");
                    }
                    // This is the parent process
                    // SIGSTOP;
                    enqueue(ready_queue, executable_path,child,0,0);
                    // printf("Enqueued: %s\n", executable_path);
                    // Store the child's process ID in history
                    add_to_history(command, child, (struct timeval){0}, (struct timeval){0});
                    int child_status;
                    // waitpid(child, &child_status, 0);
                } else {
                    fprintf(stderr, "Failed to create a new process for submission.\n");
                }
            }else if (strncmp(command, "show", 4) == 0) {
                showQueue(ready_queue);
            }
            else if (strncmp(command, "execute", 7) == 0) {
                executescheduler(ready_queue,NCPU,TSLICE);
            }else {
                status = launch(command);
                if (status != 0) {
                    fprintf(stderr, "Command execution failed: %s\n", command);
                }
            }
        }
        free(command);
    } while (1);
}

// Main function
int main(int argc, char* argv[]) {
    ready_queue = createQueue();
    showQueue(ready_queue);
    if (argc != 3) {
        fprintf(stderr, "Usage: %s NCPU TSLICE\n", argv[0]);
        return 1;
    }
    NCPU = atoi(argv[1]);
    TSLICE = atoi(argv[2]);

    if (NCPU <= 0 || TSLICE <= 0) {
        fprintf(stderr, "NCPU and TSLICE must be positive integers.\n");
        return 1;
    }

    // Call the shell loop
    shell_loop();
}
