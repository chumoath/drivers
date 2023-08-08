#include <stdio.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>


static struct input_event inputevent;


int main(int argc, char *argv[])
{
    int fd;
    int err = 0;
    char *filename;

    filename = argv[1];

    if (argc != 2) {
        printf ("Error Usage\n");
        return -1;
    }

    fd = open(filename, O_RDWR);
    if (fd < 0) {
        printf ("can not open file %s\n", filename);
        return -1;
    }

    while (1) {
        err = read(fd, &inputevent, sizeof(inputevent));
        if (err > 0) {
            switch (inputevent.type) {
                case EV_KEY:
                    if (inputevent.code < BTN_MISC) {
                        printf ("key %d %s\n", inputevent.code, inputevent.value ? "press": "release");
                    } else {
                        printf ("button %d %s\n", inputevent.code, inputevent.value ? "press" : "release");
                    }
                    break;
                case EV_REL:
                    break;
                case EV_ABS:
                    break;
                case EV_MSC:
                    break;
                case EV_SW:
                    break;
            }
        } else {
            printf ("read data failed\n");
        }
    }

    return 0;
}