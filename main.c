#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/types.h>

#define I2C_ADDR 0x40

pca9685_init(int fd);
pca9685_write_reg(int fd, uint8_t reg);


int main(int argc, char* argv[])
{
    int fd;
    
    fd = open("/dev/i2c-1", O_RDWR);
    if( fd < 0 ){
        printf("Error opening file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if( ioctl(fd, I2C_SLAVE, I2C_ADDR) < 0 ){
        printf("ioctl error");
        return EXIT_FAILURE;
    }
    
    return 0;
}