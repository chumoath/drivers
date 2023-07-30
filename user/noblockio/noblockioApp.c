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


int main(int argc, char *argv[])
{
    int fd;
    int ret = 0;
    char *filename;
    struct pollfd fds;
    fd_set readfs;
    struct timeval timeout;
    unsigned char data;

    if (argc != 2) {
        printf ("Error Usage\n");
        return -1;
    }

    filename = argv[1];
    fd = open(filename, O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        printf ("can not open file %s\n", filename);
        return -1;
    }

    fds.fd = fd;
    fds.events = POLLIN;

    while (1) {
        // fds 是一个数组
        ret = poll(&fds, 1, 500);
        if (ret) {
            ret = read(fd, &data, sizeof(data));
            if (ret < 0) {

            } else {
                if (data)
                    printf ("key value = %d\n", data);
            }
        } else if (ret == 0) {
            // 超时
        } else if (ret < 0) {
            // 错误
        }
    }

    close(fd);
    return ret;
}