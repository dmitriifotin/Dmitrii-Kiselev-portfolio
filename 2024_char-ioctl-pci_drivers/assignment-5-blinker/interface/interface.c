#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#define WR_BLINK_RATE _IOW('a','a',u_int32_t *)
#define RD_BLINK_RATE _IOR('a','b',u_int32_t *)

int main() {
    u_int32_t value;
    int fd = open("/dev/mydev4", O_RDWR);
	
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }
    while (1) {
	    printf("Type number:\n");
	    scanf("%x", &value);
	    printf("Setting new blink rate %x\n", value);
	    ioctl(fd, WR_BLINK_RATE, &value);
	    
	    if (value < 1) {
	    	printf("Illegal input\n");
	    	break;
	    }
	    
	    printf("Reading current blink rate from driver\n");
	    ioctl(fd, RD_BLINK_RATE, &value);
	    printf("Blink rate: %x\n", value);
    }
    close(fd);
    return 0;
}
