#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//#define DEBUG

int
main(int argc, char *argv[]) {
    char a;
    char b;
    char c;
    char d;
    a = 'h';
    b = 'y';
    int p0[2];
    int p1[2];

    pipe(p0);
    pipe(p1);

    int pid = fork();

    if (pid == 0) {
        read(p0[0], &d, 1);
#ifdef DEBUG
        printf("%d: received ping: %c\n", getpid(), d);
#else
        printf("%d: received ping\n", getpid());
#endif // DEBUG
        write(p1[1], &b, 1);
        close(p1[1]);
    } else {
        write(p0[1], &a, 1);
        close(p0[1]);
        wait(0);
        read(p1[0], &c, 1);
#ifdef DEBUG
        printf("%d: received pong: %c\n", getpid(), c);
#else
        printf("%d: received pong\n", getpid());
#endif // DEBUG
    }
    exit(0);
}