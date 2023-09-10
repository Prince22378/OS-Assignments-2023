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
