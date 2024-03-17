#include <iostream>
#include <list>
#include <functional>
#include <ctime>
#include <pthread.h>

int user_main(int argc, char **argv);

void demonstration(std::function<void()> &&lambda) {
  lambda();
}

typedef struct {
    int low;
    int high;
    int size;
    std::function<void(int)> lambda;
    double execution_time; //Field to store execution time
} thread_args;

void func(thread_args *args) {
    clock_t start_time = clock();
    // for ensuring that we dont leave out any index (because of remainder), we increased the size of chunk by 1.
    // but because of that the last thread might try updating value out of the size of array since the high for 
    // this thread is more than size, this way we update the high for this thread to the size, thus solving the problem.
    if(args->high> args->size){
        args->high=args->size; 
    }

    for (int i = args->low; i < args->high; i++) {
    // printf("high %d----%d---\n",i,args->high);
        args->lambda(i);
    }
    clock_t end_time = clock();
    args->execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC*1000.0;
}

void *thread_func(void *ptr) {
    thread_args *t = ((thread_args *)ptr);
    func(t);
    return NULL;
}

void parallel_for(int low, int SIZE, std::function<void(int)> &&lambda, int NTHREADS) {
    clock_t start_time = clock();
    const int Thres = 0;
    if(NTHREADS<0){   //if floating value if passed it will automatically converted into integer. eg 1.5 = 1 so 1 thread will be created
      perror(" Negative Thread not possible");
      exit(1);
    }
    if(SIZE>=Thres && NTHREADS >0){
        pthread_t tid[NTHREADS];
        thread_args args[NTHREADS];
        int chunk = SIZE / NTHREADS;
        if(SIZE % NTHREADS!=0){
            chunk+=1;
        }

        for (int i = 0; i < NTHREADS; i++) {
            args[i].low = i * chunk;
            args[i].high = (i + 1) * chunk;
            args[i].lambda = lambda;
            args[i].size = SIZE;
            if(pthread_create(&tid[i], NULL, thread_func, (void *)&args[i])!=0){
                fprintf(stderr, "Error creating thread %d.\n", i + 1);
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < NTHREADS; i++) {
            if(pthread_join(tid[i], NULL)!=0){
                fprintf(stderr, "Error joining thread.\n");
                exit(EXIT_FAILURE); 
            }
            std::cout << "Thread " << i + 1 << " Execution Time: " << args[i].execution_time << " milliseconds\n";
        }
    }
    else{
        for (int i = low; i < SIZE; i++) {
            lambda(i);
        }
    }
    clock_t end_time = clock();
    double total_execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000.0; //Convert to milliseconds
    std::cout << "Total Execution Time: " << total_execution_time << " milliseconds\n";
}


typedef struct{
    int low1;
    int high1;
    int low2;
    int high2;
    int SIZE;
    std::function<void(int, int)> lambda;
    double execution_time;
    } thread_args1;

void func1(thread_args1 *args1) {
    clock_t start_time = clock();
        if(args1->high1> args1->SIZE){
            args1->high1=args1->SIZE; 
        }
        if(args1->high2>args1->SIZE){
            args1->high2= args1->SIZE;
        }
    
    for (int i = args1->low1; i < args1->high1; i++) {
        for(int j=args1->low2; j< args1->high2; j++){
            args1->lambda(i,j);
        }
    }
    clock_t end_time = clock();
    args1->execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC*1000.0; //Convert to milliseconds
}

void *thread_func1(void *ptr) {
    thread_args1 *t = ((thread_args1 *)ptr);
    
    func1(t);
    return NULL;
}

void parallel_for(int low1, int SIZE1, int low2, int SIZE2, std::function<void(int, int)> &&lambda, int NTHREADS) {
    clock_t start_time = clock(); // Start time
    const int Thres = 0;
    if(NTHREADS<0){
    perror(" Negative Thread not possible");
    exit(1);
    }

    if(SIZE1>=Thres && NTHREADS>0){
        pthread_t tid1[NTHREADS][NTHREADS];
        thread_args1 args1[NTHREADS][NTHREADS];

        int chunk = SIZE1 / NTHREADS;
        // int chunk2 = SIZE2 / NTHREADS;
        if(SIZE1 % NTHREADS!=0){
                chunk+=1;
        }
        for (int i = 0; i < NTHREADS; i++) {
            for (int j = 0; j < NTHREADS; j++) {
                args1[i][j].low1 = i * chunk;
                args1[i][j].high1 = (i + 1) * chunk;
                args1[i][j].low2 = j * chunk;
                args1[i][j].high2 = (j + 1) * chunk;
    // kya hua? upar wale tarike se, sare threads ko high low 0-2 or 2-4 milraha tha, okay, 
    // but 4th index kabhi update honeke liye kaha nahi gaya, so if last wale ko include karneke liye,
    // last wale ka high badhadiya to accomodate that extra index left out due to remainder
                // args1[i][j].low1 = i * chunk;
                // args1[i][j].high1 = (i == NTHREADS - 1) ? SIZE1 : (i + 1) * chunk; 
                // args1[i][j].low2 = j * chunk;
                // args1[i][j].high2 = (j == NTHREADS - 1) ? SIZE2 : (j + 1) * chunk;
                args1[i][j].lambda = lambda;
                args1[i][j].SIZE = SIZE1;
                if(pthread_create(&tid1[i][j], NULL, thread_func1, (void *)&args1[i][j])!=0){
                    fprintf(stderr, "Error creating thread %d%d.\n", i + 1, j + 1);
                    exit(EXIT_FAILURE);
                }
                
            }
        }

        for (int i = 0; i < NTHREADS; i++) {
            for (int j = 0; j < NTHREADS; j++) {
                if(pthread_join(tid1[i][j], NULL)!=0){
                    fprintf(stderr, "Error joining thread.\n");
                    exit(EXIT_FAILURE); 
                }
                std::cout << "Thread " << i + 1 << j + 1 << " Execution Time: " << args1[i][j].execution_time << " milliseconds\n";
            }
        }
    }
    else{
        for (int i = low1; i < SIZE1; i++) {
            for(int j=low2;j<SIZE1;j++){
               lambda(i,j);
            }
        }
    }
    clock_t end_time = clock(); // End time
    double total_execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000.0; // Convert to milliseconds
    std::cout << "Total Execution Time: " << total_execution_time << " milliseconds\n";
}


int main(int argc, char **argv) {
    int x = 5, y = 1;
    auto lambda1 = [x, &y]() {
        y = 5;
        std::cout << "====== Welcome to Assignment-" << y << " of the CSE231(A) ======\n";
    };
    demonstration(lambda1);
    int rc = user_main(argc, argv);
    auto lambda2 = [y]() {
        std::cout<< "====== Hope you enjoyed CSE231(A) ======\n";
    };
    demonstration(lambda2);
    return rc;
}
#define main user_main
