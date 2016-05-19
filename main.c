#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/types.h>

#include "pca9685.h"

#define I2C_ADDR         0x40
#define I2C_ALLCALL_ADDR 0x70

void pca9685_init(int fd);
uint8_t pca9685_write_byte(int fd, uint8_t data);
uint8_t pca9685_read_byte(int fd);
uint8_t pca9685_read_regs(int fd, uint8_t* regs, uint8_t* data, int count);
uint8_t pca9685_write_regs(int fd, uint8_t* regs, uint8_t* data, int count);
//uint8_t[2] mode_regs = {0x00, 0x00};

uint8_t[76] pca9685_regs = {0};

int main(int argc, char* argv[])
{
    int fd;
    
    fd = open("/dev/i2c-1", O_RDWR|O_SYNC);
    if(fd < 0){
        printf("Error opening file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if(ioctl(fd, I2C_SLAVE, I2C_ADDR) < 0){
        printf("ioctl error");
        return EXIT_FAILURE;
    }
    
    pca9685_init(fd);
    
    close(fd);
    
    return 0;
}

void pca9685_init(int fd)
{
    //Check for valid device reg values
    pca9685_write_byte(fd, MODE1);
    pca9685_regs[MODE1] = pca9685_read_byte(fd);
    pca9685_write_byte(fd, MODE2);
    pca9685_regs[MODE2] = pca9685_read_byte(fd);
    
    prinf("MODE1 = %x\n", pca9685_regs[MODE1]);
    prinf("MODE2 = %x\n", pca9685_regs[MODE2]);
    
    if(pca9685_regs[MODE1] != 0x11 || pca9685_regs[MODE2] != 0x04){
        printf("Invalid status values read.\n");
        return;
    }
}

uint8_t pca9685_write_byte(int fd, uint8_t data)
{
    return write(fd, data, 1);
}

uint8_t pca9685_read_byte(int fd)
{
    uint8_t data;
    if(read(fd, &data, 1) < 0){
        return -1;
    }
    return data;
}

uint8_t pca9685_read_regs(int fd, uint8_t* regs, uint8_t* data, int count)
{
    int i;
    for(i=0;i<count;i++){
        if(write(fd, (regs + i), 1) < 0){
            printf("Cannot write to device\n");
            return -1;
        }
        if(read(fd, (data + i), 1) < 0){
            printf("Cannot read from device\n");
            return -1;
        }
    }
    
    return count;
}

uint8_t pca9685_write_regs(int fd, uint8_t* regs, uint8_t* data, int count)
{
    return count;
}
