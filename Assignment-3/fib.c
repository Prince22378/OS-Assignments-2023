#include <stdio.h>
#include <stdlib.h>

int fib(int n) {
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }
    return fib(n - 1) + fib(n - 2);
}
int main(){
    printf("\n%d\n",fib(45));
}

// int main(int argc, char *argv[]) {
//     if (argc != 2) {
//         printf("Usage: %s <number>\n", argv[0]);
//         return 1;
//     }

//     int input = atoi(argv[1]);

//     // printf("%d\n", input);

//     if (input < 0) {
//         printf("Input must be a non-negative integer.\n");
//         return 1;
//     }

//     printf("\n%d\n", fib(input));

//     return 0;
// }
