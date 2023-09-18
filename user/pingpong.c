#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    int p2c[2];
    int c2p[2];

    char buf[4];

    int pid;

    pipe(p2c);
    pipe(c2p);

    if ((pid = fork()) < 0) {
        fprintf(2, "fork failed\n");
        close(c2p[0]);
        close(c2p[1]);
        close(p2c[0]);
        close(p2c[1]);
    }

    if (pid == 0) {
        // child
        close(c2p[0]); // close read
        close(p2c[1]); // close write

        if (read(p2c[0], buf, sizeof(buf)) > 0) {
            printf("%d: got %s\n", getpid(), buf);
        }

        write(c2p[1], "pong", 4);
        close(p2c[0]);
		close(c2p[1]);
    } else {
        // parent
        close(c2p[1]); // close write
        close(p2c[0]); // close read
		
        write(p2c[1], "ping", 4);

        if (read(c2p[0], buf, sizeof(buf)) > 0) {
            printf("%d: got %s\n", getpid(), buf);
        }

        close(c2p[0]);
		close(p2c[1]);
    }
    exit(0);
}