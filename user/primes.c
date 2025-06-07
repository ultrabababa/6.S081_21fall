#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int init_p[]) {
    close(init_p[1]);
    int prime;
    
    if (read(init_p[0], &prime, sizeof(int)) <= 0) { // EOF
        close(init_p[0]);
        exit(0);
    }
    printf("prime %d\n", prime);

    int next_p[2];
    pipe(next_p);

    if (fork() == 0) {
        sieve(next_p);
    } else {
        close(next_p[0]);

        int num;
        while (read(init_p[0], &num, sizeof(int)) > 0) {
            if (num % prime != 0) {
                write(next_p[1], &num, sizeof(int));
            }
        }

        close(init_p[0]);
        close(next_p[1]);

        wait((int *) 0);
    }

    exit(0);

}

int main() {
    int init_p[2];
    pipe(init_p);

    // Fork copies the parent’s file descriptor table
    int pid = fork();

    if (pid == 0) { // child process
        sieve(init_p);
    } else {
        close(init_p[0]);

        for (int i = 2; i <= 35; i++) {
            write(init_p[1], &i, sizeof(int));
        }

        close(init_p[1]);
        wait((int *) 0);
    }

    exit(0);
}