#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "xargs: missing command\n");
        exit(1);
    }
    
    // get command name and initial args
    char *arguments[MAXARG];
    arguments[0] = argv[1]; // exec() requires first arg to be cmd
    int i = 2;
    while (i < argc) {
        arguments[i - 1] = argv[i];
        i++;
    }
    i -= 1;

    // get next optional arg
    char arg[512], *p = arg;
    char c;

    while (read(0, &c, 1) == 1) { // read an argument
        if (p >= arg + sizeof(arg) - 1) {
            fprintf(2, "xargs: arg size too long\n");
            exit(1);
        }

        *p++ = c;
        if (c == '\n') {
            *(p - 1) = '\0';
            arguments[i] = arg;
            arguments[i + 1] = 0; // arg array should end with NULL
            // printf("append arg[%d]: %s", i, arguments[i]);
            int pid = fork();
            if (pid == 0) { // child process execute the cmd
                // for (int j = 0; j < i + 1; j++) {
                //     printf("exec: arguments[%d]: %s\n", j, arguments[j]);
                // }
                if (exec(arguments[0], arguments) < 0) {
                    fprintf(2, "exec error: %s %s\n", arguments[0], arguments[i]);
                    exit(1);
                }
            } else {
                wait((int *) 0);
                p = arg;
            }
        }
    }
    
    exit(0);
}