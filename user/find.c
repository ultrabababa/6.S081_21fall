#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

const char DOT[DIRSIZ] = ".";
const char DOTDOT[DIRSIZ] = "..";

int is_dot_pattern(struct dirent de) {
    int is_dot = memcmp(de.name, DOT, DIRSIZ) == 0;
    int is_dot_dot = memcmp(de.name, DOTDOT, DIRSIZ) == 0;
    return is_dot || is_dot_dot;
}

void find(char *path, char *target) {
    // printf("current path: %s\n", path);
    int fd;
    struct stat st; // stat of one dirent

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_FILE:
        // get the name of the file
        char *p;
        for (p = path + strlen(path); p >= path && *p != '/'; p--)
            ;
        p++;

        if (strcmp(p, target) == 0) {
            printf("%s\n", path);
        }
        break;
    
    case T_DIR:
        struct dirent de; // to get all the dirents in the directory
        char buf[512], *pp; // use buf to form the path of each dirent

        while (read(fd, &de, sizeof(de)) == sizeof(de)) { // read each dirent in the directory
            if (de.inum == 0 || is_dot_pattern(de) == 1) { // skip invalid dirent or . or ..
                continue;
            }

            // form the path of the current dirent
            strcpy(buf, path);
            pp = buf + strlen(buf);
            *pp++ = '/';
            memmove(pp, de.name, DIRSIZ);
            pp[DIRSIZ] = 0;

            find(buf, target); // recurse the dirent
        }
    
    default:
        break;
    }

    close(fd);
    return;
}

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], ".") != 0) {
        fprintf(2, "Usage: find . <file>\n");
        exit(1);
    }

    char *path = argv[1];
    char *target = argv[2];

    // printf("target: %s\n", target);
    find(path, target);

    exit(0);
}
