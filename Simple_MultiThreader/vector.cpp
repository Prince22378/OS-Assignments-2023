// #include "simple-multithreader.h"
#include"simple.h"
#include <assert.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  // intialize problem size
  int numThread = argc>1 ? atoi(argv[1]) : 2;
  int size = argc>2 ? atoi(argv[2]) : 48000000;  
  // allocate vectors
  int* A = new int[size];
  int* B = new int[size];
  int* C = new int[size];
  // initialize the vectors
  std::fill(A, A+size, 1);
  std::fill(B, B+size, 1);
  std::fill(C, C+size, 0);

  // start the parallel addition of two vectors
  parallel_for(0, size, [&](int i){
    C[i]=A[i] + B[i];
  }, numThread);

  // verify the result vector
  for(int i=0; i<size; i++){ 
    // printf("Value__%d   ",C[i]);
    assert(C[i] == 2);
    // printf("HELOO ASSERTING DONE %d\n",i);
  }

  //printing number of threads and size to check whether argc is working or not
  // cout<<numThread;
  // cout<<size;   //--remember doing error handling

  printf("Test Success\n");
  // cleanup memory
  delete[] A;
  delete[] B;
  delete[] C;
  return 0;
}
