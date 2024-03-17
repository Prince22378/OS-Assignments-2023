#ifndef QUEUE_H
#define QUEUE_H

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

// Function to create a new queue
struct Queue* createQueue();

// Function to check if the queue is empty
int isEmpty(struct Queue* queue);

// Function to enqueue an item into the queue
void enqueue(struct Queue* queue, char* command, int PID, long exec_time, long wait_time);

// Function to dequeue an item from the queue
struct QueueItem* dequeue(struct Queue* queue);

// Function to show the contents of the queue
void showQueue(struct Queue* queue);

void copyAndPrintFirstNItems(struct Queue* mainQueue, struct Queue* newQueue, int N);
void roundRobinScheduling(struct Queue* newQueue, int timeQuantum);
extern struct Queue* ready_queue;
extern struct Queue* newQueue;

#endif // QUEUE_H
