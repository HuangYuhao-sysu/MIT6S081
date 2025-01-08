
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM 35
/* First time I use recursive function and find its outcome is right.
 So what's the meaning of using pipe?
 You can even be restricted by the number of pipes!
 But if you use recursive, Things looks better.
 Wired... See the example below
 ----------- FIRST ATTEMPT OUTPUT: NUM = 100
 $ primes
 prime 2
 prime 3
 prime 5
 prime 7
 prime 11
 prime 13
 prime 17
 prime 19
 prime 23
 prime 29
 prime 31
 prime 37
 prime 41
 prime 43
 prime 47
 prime 53
 prime 59
 prime 61
 prime 67
 prime 71
 prime 73
 prime 79
 prime 83
 prime 89
 prime 97 
 ----------- NON-FIRST-ATTEMPT OUTPUT: NUM = 100
 $ primes
 prime 2
 prime 3
 prime 5
 prime 7
 prime 11
 prime 13
 prime 17
 prime 19
 prime 23
 prime 29
 prime 31
 prime 37
 )+/5;=CGIOSYaprime 0 <- What the hell of these?
-- Things went wrong!*/

// Define the line below will use recursive function.
// #define FIRST_ATTEMPT

int pDoesntDivideN(int p, int n) {
    if(n%p == 0) {
        return 0;
    } else {
        return 1;
    }
}

#ifdef FIRST_ATTEMPT
int prime(int num, int* left) {
    if(num == 0) {
        return 0;
    } else {
        int p = left[0];
        int j = 0;
        int* right = malloc(sizeof(int)*num);
        printf("prime %d\n", p);
        for(int i = 1; i < num; ++i) {
            int n = left[i];
            if(pDoesntDivideN(p, n)) {
                right[j] = n;
                j++;
            }
        }

        if (j != 0) {
            return prime(j, right);
        } else {
            return 0;
        }

    }
}

int main(int argc, char *argv[]) {
    if(argc != 1) {
        printf("Usage: primes...\n");
        exit(1);
    }

    int* left = malloc(sizeof(int)*(NUM-1));

    for(int i = 0; i < NUM-1; ++i) {
        left[i] = i + 2;
    }
    prime(NUM-1, left);
    exit(0);
}
#else
int prime(int num, int pReadPort) {
    if(num == 0) {
        return 0;
    } else {
        int* readData  = malloc(sizeof(int)*num);
        int* writeData  = malloc(sizeof(int)*num);
        read(pReadPort, readData, num*4);
        close(pReadPort);
        int p = readData[0];
        int j = 0;
        int right[2];
        pipe(right);
        printf("prime %d\n", p);
        for(int i = 1; i < num; ++i) {
            int n = readData[i];
            if(pDoesntDivideN(p, n)) {
                writeData[j] = n;
                j++;
            }
        }
        if (j != 0) {
            int pid = fork();
            if (pid == 0) {
                return prime(j, right[0]);
            } else {
                write(right[1], writeData, j*4);
                close(right[1]);
                wait(0);
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc != 1) {
        printf("Usage: primes...\n");
        exit(1);
    }

    int* left = malloc(sizeof(int)*(NUM-1));

    for(int i = 0; i < NUM-1; ++i) {
        left[i] = i + 2;
    }
    int p[2];
    pipe(p);
    write(p[1], left, (NUM-1)*4);
    prime(NUM-1, p[0]);
    exit(0);
}

#endif