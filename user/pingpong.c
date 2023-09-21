#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    int p[2];

    char buf[4];
    int pid;

    pipe(p);

    if ((pid = fork()) < 0) {
        fprintf(2, "fork failed\n");
        close(p[0]);
        close(p[1]);
    }

    if (pid == 0) {
        // child

        if (read(p[0], buf, sizeof(buf)) > 0) {
            printf("%d: got %s\n", getpid(), buf);
        }

        write(p[1], "pong", 4);
        close(p[0]);
		close(p[1]);
    } else {
        // parent
    
        write(p[1], "ping", 4);

        if (read(p[0], buf, sizeof(buf)) > 0) {
            printf("%d: got %s\n", getpid(), buf);
        }

        close(p[0]);
		close(p[1]);
    }
    exit(0);
}