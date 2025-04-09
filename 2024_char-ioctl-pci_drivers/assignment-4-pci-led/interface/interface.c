#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#define WR_LEDCTL _IOW('a','a',u_int32_t *)
#define RD_LEDCTL _IOR('a','b',u_int32_t *)

int main() {
    u_int32_t value;
    int fd = open("/dev/mydev4", O_RDWR);

    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }
    
    printf("Type number:\n");
    scanf("%x", &value);
    printf("Writing input to driver\n");
    ioctl(fd, WR_LEDCTL, (u_int32_t*) &value);
    
    printf("Reading input from driver\n");
    ioctl(fd, RD_LEDCTL, (u_int32_t*) &value);
    printf("Value read:%d\n", value);

    close(fd);
    return 0;
}
