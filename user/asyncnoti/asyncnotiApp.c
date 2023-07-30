#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>

static int fd = 0;

static void sigio_signal_func(int signum)
{
    int err = 0;
    unsigned  int keyvalue = 0;

    err = read(fd, &keyvalue, sizeof(keyvalue));
    if (err < 0) {

    } else {
        printf ("sigio signal! key value = %d\n", keyvalue);
    }
}

int main(int argc, char *argv[])
{
    int flags = 0;
    char *filename;
    if (argc != 2) {
        printf ("Error Usage\n");
        return -1;
    }

    filename = argv[1];
    fd = open(filename, O_RDWR);
    if (fd < 0) {
        printf ("can not open file %s\n", filename);
        return -1;
    }

    signal(SIGIO, sigio_signal_func);

    // 将当前进程进程号告诉内核
    fcntl(fd, F_SETOWN, getpid());

    // 获取当前进程状态
    flags = fcntl(fd, F_GETFD);

    // 当前进程启动异步通知
    fcntl(fd, F_SETFL, flags | FASYNC);

    while (1) {
        sleep(2);
    }

    close(fd);
    return 0;
}