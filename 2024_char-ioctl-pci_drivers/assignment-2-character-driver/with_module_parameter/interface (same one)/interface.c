#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main() {
    int read_val;
    char buffer[256];
    ssize_t bytes_read;
    int fd = open("/dev/mydev", O_RDWR);

    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    bytes_read = read(fd, &read_val, sizeof(read_val));
    
    
    if (bytes_read < 0) {
        perror("Failed reading from device\n");
    } else {
        printf("Read from device:\"%d\"\n", read_val);
    }
    
    printf("Type number:\n");
    scanf("%s", buffer);
    ssize_t bytes_written = write(fd, &buffer, sizeof(buffer));

    if (bytes_written < 0) {
        perror("Failed to write to device");
    } else {
        printf("Successfully wrote \"%s\" to the device\n", buffer);
    }

    close(fd);
    return 0;
}
