#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int p2c[2]; // parent to child
    int c2p[2];
    char byte = 'l'; // byte to send
    
    if (pipe(p2c) < 0 || pipe(c2p) < 0) {
        fprintf(2, "create pipe failed!\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed!\n");
        exit(1);
    }

    if (pid == 0) { // child process
        close(p2c[1]); // close wirte end of parent to child pipe
        close(c2p[0]); // close read end of child to parent pipe

        if (read(p2c[0], &byte, 1) != 1) {
            fprintf(2, "child read failed!\n");
            exit(1);
        } else {
            printf("%d: received ping\n", getpid());
        }

        if (write(c2p[1], &byte, 1) != 1) {
            fprintf(2, "child write failed!\n");
            exit(1);
        }

        close(p2c[0]);
        close(c2p[1]);
        exit(0);
    } else { // parent process
        close(p2c[0]);
        close(c2p[1]);

        if (write(p2c[1], &byte, 1) != 1) {
            fprintf(2, "parent write failed!\n");
            exit(1);
        }

        if (read(c2p[0], &byte, 1) != 1) {
            fprintf(2, "parent read failed!\n");
            exit(1);
        } else {
            printf("%d: received pong\n", getpid());
        }
    }

    close(p2c[1]);
    close(c2p[0]);

    wait((int *) 0);

    exit(0);
}
