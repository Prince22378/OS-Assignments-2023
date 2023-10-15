#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>     // Include for fork, execlp
#include <sys/wait.h>   // Include for waitpid
struct QueueItem {
    char* command;
    int data[3]; // Index 0: executable_path, Index 1: PID, Index 2: execution time, Index 3: waiting time
    int state;  //0 = running , else ready
    struct QueueItem* next;
};

// Structure for a queue
struct Queue {
    struct QueueItem* front;
    struct QueueItem* rear;
};
struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

int isEmpty(struct Queue* queue) {
    return (queue->front == NULL);
}
void enqueue(struct Queue* queue, char* command, int PID, long exec_time, long wait_time) {
    struct QueueItem* newItem = (struct QueueItem*)malloc(sizeof(struct QueueItem));
    if (!newItem) {
        fprintf(stderr, "Memory allocation error for a new queue item.\n");
        exit(EXIT_FAILURE);
    }
    // newItem->command = strdup(command);
    newItem->command = strdup(command);
    newItem->data[0] = PID;
    newItem->data[1] = exec_time;
    newItem->data[2] = wait_time;
    newItem->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = newItem;
    } else {
        queue->rear->next = newItem;
        queue->rear = newItem;
    }
}

struct QueueItem* dequeue(struct Queue* queue) {
    if (queue->front == NULL) {
        return NULL; // Queue is empty
    }
    struct QueueItem* item = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    return item;
}
void showQueue(struct Queue* queue) {
    struct QueueItem* current = queue->front;
    while (current != NULL) {
        printf("Enqueued: %s\n", current->command);
        printf("Data: ");
        for (int i = 0; i < 3; i++) {
            printf("%d ", current->data[i]);
        }
        printf("\n");
        current = current->next;
    }
}

struct Queue* ready_queue;
// struct Node {
//     char data[100]; // Assuming each string has a maximum length of 100 characters
//     struct Node* next;
// };

// struct Queue {
//     struct Node* front;
//     struct Node* rear;
// };
// Structure for a queue item
struct Queue* newQueue;
void copyAndPrintFirstNItems(struct Queue* mainQueue, struct Queue* newQueue, int N) {
    for (int i = 0; i < N; i++) {
        struct QueueItem* process = dequeue(mainQueue);
        if (process != NULL) {
            enqueue(newQueue, process->command, process->data[0], process->data[1], process->data[2]);
            printf("Enqueued: %s\n", process->command);
            printf("Data: ");
            for (int j = 0; j < 3; j++) {
                printf("%d ", process->data[j]);
            }
            printf("\n");
        }
    }
}
// void roundRobinScheduling(struct Queue* newQueue, int timeQuantum) {
//     struct QueueItem* currentProcess = NULL;

//     while (!isEmpty(newQueue)) {
//         currentProcess = dequeue(newQueue);
//         int pid = fork();
        
//         if (pid == -1) {
//             perror("Fork failed");
//             exit(EXIT_FAILURE);
//         }

//         if (pid == 0) {
//             // Child process: Execute the command
//             execlp(currentProcess->command, currentProcess->command, (char*)NULL);
//             perror("Exec failed");
//             exit(EXIT_FAILURE);
//         } else {
//             // Parent process: Wait for the child process to complete
//             int status;
//             waitpid(pid, &status, 0);
//             if (WIFEXITED(status)) {
//                 printf("Process with PID %d completed\n", pid);
//             }
//         }
        
//         free(currentProcess->command);
//         free(currentProcess);
//     }
// }
// void enqueue(struct Queue* queue, const char* data) {
//     struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
//     strncpy(newNode->data, data, sizeof(newNode->data) - 1);
//     newNode->data[sizeof(newNode->data) - 1] = '\0'; // Ensure null-terminated string
//     newNode->next = NULL;
//     if (isEmpty(queue)) {
//         queue->front = queue->rear = newNode;
//     } else {
//         queue->rear->next = newNode;
//         queue->rear = newNode;
//     }
// }
// void enqueue(struct Queue* queue, char* command, int data[4]) {
//     struct QueueItem* newItem = (struct QueueItem*)malloc(sizeof(struct QueueItem));
//     if (!newItem) {
//         fprintf(stderr, "Memory allocation error for a new queue item.\n");
//         exit(EXIT_FAILURE);
//     }
//     newItem->command = strdup(command);
//     for (int i = 0; i < 4; i++) {
//         newItem->data[i] = data[i];
//     }
//     newItem->next = NULL;
//     if (queue->rear == NULL) {
//         queue->front = queue->rear = newItem;
//         return;
//     }
//     queue->rear->next = newItem;
//     queue->rear = newItem;
// }

// void showQueue(struct Queue* queue) {
//     if (isEmpty(queue)) {
//         printf("The ready queue is empty.\n");
//     } else {
//         struct Node* current = queue->front;
//         printf("Ready Queue contents:\n");
//         while (current != NULL) {
//             printf("%s\n", current->data);
//             current = current->next;
//         }
//     }
// }
