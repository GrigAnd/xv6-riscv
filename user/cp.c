#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

char buf[512];

void strcat(char *dest, const char *src) {
    while (*dest != '\0') {
        dest++;
    }
    while ((*dest++ = *src++) != '\0')
        ;
}


void cpfile(char *srcdir, char *destdir) {
    int srcfd = open(srcdir, O_RDONLY);
    int destfd = open(destdir, O_CREATE | O_RDWR);
    int n;

    while ((n = read(srcfd, buf, sizeof(buf))) > 0) {
        if (write(destfd, buf, n) != n) {
            fprintf(2, "cp: write error\n");
            exit(1);
        }
    }
    if (n < 0) {
        fprintf(2, "cp: read error\n");
        exit(1);
    }
    close(srcfd);
    close(destfd);
}

void copydir(char *srcdir, char *destdir) {
    int srcdird = open(srcdir, O_RDONLY);
    if (srcdird < 0) {
        fprintf(2, "cp: cannot open directory %s\n", srcdir);
        exit(1);
    }

    if (mkdir(destdir) < 0) {
        fprintf(2, "cp: cannot create directory %s\n", destdir);
        exit(1);
    }

    struct dirent de;
    struct stat st;
    while (read(srcdird, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0 || (strcmp(de.name, ".") == 0) ||
            (strcmp(de.name, "..") == 0)) {
            continue;
        }
        char srcfile[256], destfile[256];
        strcpy(srcfile, srcdir);
        strcpy(destfile, destdir);
        strcat(srcfile, "/");
        strcat(destfile, "/");
        strcat(srcfile, de.name);
        strcat(destfile, de.name);

        if (stat(srcfile, &st) == 0 && st.type == T_DIR) {
            copydir(srcfile, destfile);
        } else {
            cpfile(srcfile, destfile);
        }
    }
    close(srcdird);
}

int main(int argc, char *argv[]) {
    // Validate command line arguments
    if (argc != 3) {
        fprintf(2, "Usage: cp source_directory destination_directory\n");
        exit(1);
    }

    char *srcdir = argv[1];
    char *destdir = argv[2];

    // check is file of directory
    struct stat st;
    if (stat(srcdir, &st) < 0) {
        fprintf(2, "cp: cannot stat %s\n", srcdir);
        exit(1);
    }

    switch (st.type) {
        case T_FILE:
            cpfile(srcdir, destdir);
            break;
        case T_DIR:
            copydir(srcdir, destdir);
            break;
        default:
            fprintf(2, "cp: %s is not a file or directory\n", srcdir);
            exit(1);
            break;
    }

    exit(0);
}
