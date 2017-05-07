#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/types.h>
#include <stdint.h>
#include <time.h>

#include "pca9685.h"
#include "i2c_master.h"

#define I2C_ADDR         0x40
#define I2C_ALLCALL_ADDR 0x70

//uint8_t pca9685_setpin(int fd, uint8_t pin, uint16_t low, uint16_t high);
//uint8_t[2] mode_regs = {0x00, 0x00};


int main(int argc, char* argv[])
{
    int fd;
    
    fd = i2c_mopen(1);
    i2c_mset_addr(fd, I2C_ADDR);
    //seeding the random number generator
    srand(time(NULL));
    
    pca9685_init(fd);
    int tmp = 0;
    for(;;){
        i2c_mwrite_reg_pair(fd, LED0_ON_L, 0);
	tmp = rand() % 3072;
        i2c_mwrite_reg_pair(fd, LED0_OFF_L, tmp + 1023);
        //pca9685_write_reg_pair(fd, LED0_OFF_L, 4095);

        i2c_mwrite_reg_pair(fd, LED1_ON_L, 0);
	tmp = rand() % 3072;
        i2c_mwrite_reg_pair(fd, LED1_OFF_L, tmp + 1023);
        //pca9685_write_reg_pair(fd, LED1_OFF_L, 4095);

        i2c_mwrite_reg_pair(fd, LED2_ON_L, 0);
	tmp = rand() % 3072;
        i2c_mwrite_reg_pair(fd, LED2_OFF_L, tmp + 1023);

        i2c_mwrite_reg_pair(fd, LED3_ON_L, 0);
	tmp = rand() % 3072;
        i2c_mwrite_reg_pair(fd, LED3_OFF_L, tmp + 1023);

	tmp = rand() % 10000;
	usleep(tmp * 25);
    }
    
    i2c_mclose(fd);
    
    return 0;
}


