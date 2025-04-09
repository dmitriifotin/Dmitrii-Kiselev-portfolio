#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int main() {
    int32_t value, number;
    int fd = open("/dev/mydev3", O_RDWR);

    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }
    
    printf("Type number:\n");
    scanf("%d", &number);
    printf("Writing input to driver\n");
    ioctl(fd, WR_VALUE, (int32_t*) &number);
    
    printf("Reading input from driver\n");
    ioctl(fd, RD_VALUE, (int32_t*) &value);
    printf("Value read:%d\n", value);

    close(fd);
    return 0;
}
