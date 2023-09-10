struct CommandHistory {
    char* command;
    pid_t pid;
    struct timeval start_time;
    struct timeval end_time;
};
//array to store command history
struct CommandHistory history[MAX_HISTORY_SIZE]; 
int history_count = 0; //number of commands in history

// function that add commands to history
void add_to_history(char* command, pid_t pid, struct timeval start_time, struct timeval end_time) {
    if (pid != -1) {  //entries with a valid process ID
        if (history_count < MAX_HISTORY_SIZE) {
            history[history_count].command = strdup(command);
            history[history_count].pid = pid;
            history[history_count].start_time = start_time;
            history[history_count].end_time = end_time;
            history_count++;
        } else {
            free(history[0].command); //removing oldest command from history
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
// function to display history
void display_history() {
    char* last_command = NULL;
    for (int i = 0; i < history_count; i++) {
        if (last_command == NULL || strcmp(history[i].command, last_command) != 0) {
            printf("%d. %s\n", i + 1, history[i].command);
            free(last_command);
            last_command = strdup(history[i].command);
        }
    }
    free(last_command);
}
int create_process_and_run(char* command) {
    struct timeval start_time, end_time;
    int status;
    char *v[3];  //array to store the tokens
    int count = -1;        
    char *tok = strtok(command, "|");
    int i = 0;
    while (tok != NULL) {
        v[i] = tok;
        count++;
        i++;
        tok = strtok(NULL, "|");
    }
    get_current_time(&start_time); //records starting time
    if(count==0){
        pid_t f_0;
        f_0= fork();
        if (f_0 < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (f_0 == 0) {
            // Child process
            // Parse the command and execute it
            char* cmd_0[MAX_INPUT_LENGTH];
            int idc = 0;
            char* tok_1 = strtok(command, " "); //spliting the command into tokens to pass in execvp
            while (tok_1 != NULL) {
                cmd_0[idc] = tok_1;
                tok_1 = strtok(NULL, " ");
                idc++;
            }
            cmd_0[idc] = NULL; //terminating the argument list using NULL
            //executing the command using execvp system call
            if (execvp(cmd_0[0], cmd_0) == -1) {
                perror("Failed to execute the command");
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent process
            int child_status;
            wait(&child_status); //waiting for the child to finish
            get_current_time(&end_time); //recording ending time
            add_to_history(command, f_0, start_time, end_time); //adding command to history with exe. details
        }
    }else if(count==1){
        //for 1 pipe i.e for instructions like cat months.txt | head -6
        char *v1[2];
        v1[0] = "sh", v1[1] = "-c";
        //creating pipe_1
        int pipe_1[2];
        if (pipe(pipe_1) == -1) {
            perror("Pipe failed");
            exit(EXIT_FAILURE);
        }
        pid_t f_1 = fork();
        if (f_1 < 0) {
            perror("Fork 1 failed");
            exit(EXIT_FAILURE);
        } else if (f_1 == 0) {
            // Child process 1: Execute first instruction (i.e. cat months.txt) and send output to the pipe
            close(pipe_1[0]);  //closing fd[0] i.e read end of pipe
            dup2(pipe_1[1], STDOUT_FILENO);   //redirect stdout to the pipe
            close(pipe_1[1]);   //closing fd[1] i.e write end of the pipe
            //calling the system commands using execlp for the first command
            execlp(v1[0], v1[0], v1[1], v[0], NULL);
            perror("Execlp failed");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            close(pipe_1[1]);  //closing the write end of the pipe in the parent so we can read the output and use it further
            int pipe_2[2];  //creating pipe_2
            if (pipe(pipe_2) == -1) {
                perror("Pipe 2 failed");
                exit(EXIT_FAILURE);
            }
            waitpid(f_1, &status, 0);  //wait for child to execute
            struct timeval start_time2, end_time2;
            pid_t f_10 = fork();            
            if (f_10 < 0) {
                perror("Fork 2 failed");
                exit(EXIT_FAILURE);
            } else if (f_10 == 0) {
                // Child process 2: Execute (head i.e. 2nd command) with input from the pipe
                close(pipe_1[1]); //closing the write end of the pipe_1
                dup2(pipe_1[0], STDIN_FILENO);  //redirect stdin to the pipe_1
                close(pipe_1[0]);  //closing the read end of the pipe_1
                dup2(pipe_2[1], STDOUT_FILENO);   //redirect stdout to the pipe_2
                close(pipe_2[1]);   //closing fd[1] i.e the write end of the pipe_2
                //calling the system commands using execlp for the second command
                execlp(v1[0], v1[0], v1[1], v[1], NULL);
                perror("Execlp head failed");
                exit(EXIT_FAILURE);
            } else {
                // Parent process
                close(pipe_1[0]);  //closing fd[0] i.e the read end of the pipe_1
                close(pipe_2[1]);  //closing fd[1] i.e the write end of the pipe_2
                //waiting for both child processes to complete or finish
                waitpid(f_1, &status, 0);
                waitpid(f_10, &status, 0);
                char *cmdd = NULL;
                cmdd = (char *)malloc(strlen(v[0]) + strlen("|") + strlen(v[1]) + 1);
                if (cmdd == NULL) {
                    perror("Memory allocation failed");
                    exit(EXIT_FAILURE);
                }
                cmdd[0] = '\0';
                strcat(cmdd, v[0]);
                strcat(cmdd, "|");
                strcat(cmdd, v[1]);
                get_current_time(&end_time2);  //getting the end_time
                add_to_history(cmdd, f_10, start_time, end_time2); //adding command to history

                char buffer[1024]; //for storing the output
                ssize_t bytes_read;
                //read the output of the second child process (i.e final output) from the pipe
                while ((bytes_read = read(pipe_2[0], buffer, sizeof(buffer))) > 0) {
                    write(STDOUT_FILENO, buffer, bytes_read);   //printing data to the terminal
                }
                close(pipe_2[0]);
                // free(command);
            }
        }     
    }else if(count==2){
        //for 2 pipe i.e for instructions like cat months.txt | head -6 | wc -l
        char *v1[2];
        v1[0] = "sh", v1[1] = "-c";
        int pipe_1[2]; //creating first pipe            
        if (pipe(pipe_1) == -1) {
            perror("Pipe 1 failed");
            exit(EXIT_FAILURE);
        }      
        pid_t child_pid1 = fork();
        if (child_pid1 < 0) {
            perror("Fork 1 failed");
            exit(EXIT_FAILURE);
        } else if (child_pid1 == 0) {
            // Child process 1: Execute 'cat months.txt' and send output to pipe 1               
            close(pipe_1[0]);   //closing fd[0] i.e the read end of pipe_1                
            dup2(pipe_1[1], STDOUT_FILENO); //redirecting stdout to pipe_1                
            close(pipe_1[1]);  //closing fd[1] i.e. the write end of pipe_1 
            //calling the system commands using execlp for the first command 
            execlp(v1[0], v1[0],v1[1],v[0], NULL);
            perror("Execlp cat failed");
            exit(EXIT_FAILURE);
        }else {
            // Parent process                
            close(pipe_1[1]);   //closing fd[0] i.e. the write end of pipe_1 in the parent
            int pipe_2[2]; //creating the pipe_2
            if (pipe(pipe_2) == -1) {
                perror("Pipe 2 failed");
                exit(EXIT_FAILURE);
            }
            pid_t child_pid2; // Fork the second child process (head -6)                
            child_pid2 = fork();
            if (child_pid2 < 0) {
                perror("Fork 2 failed");
                exit(EXIT_FAILURE);
            } else if (child_pid2 == 0) {
                // Child process 2: Execute 'head-6' with input from pipe 1 and send the output to pipe 2                    
                close(pipe_1[1]); //closing fd[1] the write end of pipe_1                    
                dup2(pipe_1[0], STDIN_FILENO); //reedirect stdin to pipe_1                    
                close(pipe_1[0]); //closing fd[0] the read end of pipe_1                    
                close(pipe_2[0]);  //closing fd[0] the read end of pipe_2                    
                dup2(pipe_2[1], STDOUT_FILENO); //redirect stdout to pipe_2                    
                close(pipe_2[1]); //cllosing fd[1] the write end of pipe_2
                //calling the system commands using execlp for the second command(head -6)
                execlp(v1[0], v1[0],v1[1],v[1], NULL);
                perror("Execlp grep failed");
                exit(EXIT_FAILURE);
            } else {
                // Parent process                    
                close(pipe_1[0]); //closing fd[0] the read end of pipe_1                    
                close(pipe_2[1]); //closing fd[1] the write end of pipe_2
                int pipe_3[2]; //creating pipe_3
                
                if (pipe(pipe_3) == -1) {
                    perror("Pipe 3 failed");
                    exit(EXIT_FAILURE);
                }
                pid_t child_pid3 = fork();       // Fork the third child process (sort) with input from pipe 2
                struct timeval start_time2, end_time2;
                if (child_pid3 < 0) {
                    perror("Fork 3 failed");
                    exit(EXIT_FAILURE);
                }
                if (child_pid3 == 0) {
                    // Child process 3: Execute 'sort' with input from pipe_2
                    close(pipe_2[1]);   //close fd[1] the write end of pipe_2
                    dup2(pipe_2[0], STDIN_FILENO); //redirect stdin to pipe_2                        
                    close(pipe_2[0]); //Close fd[0] the read end of pipe_2                        
                    dup2(pipe_3[1], STDOUT_FILENO);  //Redirect stdout to pipe_3                       
                    close(pipe_3[1]);  //close fd[1] the write end of pipe_3
                    //calling the system commands using execlp for the third (sort) command 
                    execlp(v1[0], v1[0],v1[1],v[2], NULL); 
                    perror("Execlp failed");
                    exit(EXIT_FAILURE);
                } 
                else {
                    // Parent process
                    close(pipe_2[0]);
                    close(pipe_3[1]);
                    // Read the output of the third child process (sort) from pipe_3
                    char buffer[1024]; //storing the data
                    ssize_t bytes_read;
                    while ((bytes_read = read(pipe_3[0], buffer, sizeof(buffer))) > 0) {                        
                        write(STDOUT_FILENO, buffer, bytes_read);  //print the output to console
                    }
                    char *cmdd_1 = NULL;
                    cmdd_1 = (char *)malloc(strlen(v[0]) + strlen("|") + strlen(v[1]) + strlen("|") + strlen(v[2])+1);
                    if (cmdd_1 == NULL) {
                        perror("Memory allocation failed");
                        exit(EXIT_FAILURE);
                    }
                    cmdd_1[0] = '\0';
                    strcat(cmdd_1, v[0]);
                    strcat(cmdd_1, "|");
                    strcat(cmdd_1, v[1]);
                    strcat(cmdd_1, "|");
                    strcat(cmdd_1, v[2]);
                    get_current_time(&end_time2); //get the end time
                    add_to_history(cmdd_1, child_pid2, start_time, end_time2); //adding command to history
                    close(pipe_3[0]);
                    //wait for all child processes to complete
                    wait(NULL);
                    wait(NULL);
                    wait(NULL);
                }
            }
        } 
    }
    return 0;
}
int launch(char* command) {
    int status;
    status = create_process_and_run(command);
    return status;
}
int launch_script(char* bashFile) {
    FILE* bash_file= fopen(bashFile, "r");
    if (bash_file == NULL) {
        perror("Failed to open script file");
        return 1;
    }
    char line[MAX_INPUT_LENGTH];
    while (fgets(line, sizeof(line), bash_file) != NULL) {
        // Remove trailing newline character
        char* newline = strchr(line, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }
        // Process each line of the script file here
        int status = launch(line);
        
}
