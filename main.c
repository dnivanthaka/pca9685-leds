#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/types.h>
#include <stdint.h>
#include <time.h>

#include "pca9685.h"

#define I2C_ADDR         0x40
#define I2C_ALLCALL_ADDR 0x70

void pca9685_init(int fd);

uint8_t pca9685_write_reg(int fd, uint8_t reg, uint8_t data);
uint8_t pca9685_read_reg(int fd, uint8_t reg);
uint8_t pca9685_write_reg_pair(int fd, uint8_t reg, uint16_t data);
uint16_t pca9685_read_reg_pair(int fd, uint8_t reg);

uint8_t pca9685_read_regs(int fd, uint8_t st_reg, uint8_t* data, int count);
uint8_t pca9685_write_regs(int fd, uint8_t st_reg, uint8_t* data, int count);

//uint8_t pca9685_setpin(int fd, uint8_t pin, uint16_t low, uint16_t high);
//uint8_t[2] mode_regs = {0x00, 0x00};

uint8_t pca9685_regs[76] = {0};

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

    //seeding the random number generator
    srand(time(NULL));
    
    pca9685_init(fd);
    int tmp = 0;
    for(;;){
        pca9685_write_reg_pair(fd, LED0_ON_L, 0);
	tmp = rand() % 3072;
        pca9685_write_reg_pair(fd, LED0_OFF_L, tmp + 1023);
        //pca9685_write_reg_pair(fd, LED0_OFF_L, 4095);

        pca9685_write_reg_pair(fd, LED1_ON_L, 0);
	tmp = rand() % 3072;
        pca9685_write_reg_pair(fd, LED1_OFF_L, tmp + 1023);
        //pca9685_write_reg_pair(fd, LED1_OFF_L, 4095);

        pca9685_write_reg_pair(fd, LED2_ON_L, 0);
	tmp = rand() % 3072;
        pca9685_write_reg_pair(fd, LED2_OFF_L, tmp + 1023);

        pca9685_write_reg_pair(fd, LED3_ON_L, 0);
	tmp = rand() % 3072;
        pca9685_write_reg_pair(fd, LED3_OFF_L, tmp + 1023);

	tmp = rand() % 10000;
	usleep(tmp * 15);
    }
    
    close(fd);
    
    return 0;
}

void pca9685_init(int fd)
{
    //Check for valid device reg values
    pca9685_regs[MODE1] = pca9685_read_reg(fd, MODE1);
    pca9685_regs[MODE2] = pca9685_read_reg(fd, MODE2);
    //setting device to sleep mode
    pca9685_regs[MODE1] |= SLEEP_BIT;
    pca9685_write_reg(fd, MODE1, pca9685_regs[MODE1]);

    //Restart routine, as per datasheet
    if(pca9685_regs[MODE1] & RESTART_BIT){
        pca9685_regs[MODE1] &= ~SLEEP_BIT;
        pca9685_write_reg(fd, MODE1, pca9685_regs[MODE1]);
	usleep(1000);
        pca9685_regs[MODE1] |= RESTART_BIT;
        pca9685_write_reg(fd, MODE1, pca9685_regs[MODE1]);
    }
    
    printf("MODE1 = %x\n", pca9685_regs[MODE1]);
    printf("MODE2 = %x\n", pca9685_regs[MODE2]);
    
    if(pca9685_regs[MODE1] != 0x11 || pca9685_regs[MODE2] != 0x04){
        printf("Invalid status values read.\n");
        return;
    }

    //uint8_t tmp[2] = {0};
    //pca9685_read_regs(fd, MODE1, tmp, 1);
    //printf("tmp1 = %x\n", tmp[0]);
    //printf("tmp2 = %x\n", tmp[1]);
    
    //waking up device
    pca9685_regs[MODE1] &= ~SLEEP_BIT;
    pca9685_write_reg(fd, MODE1, pca9685_regs[MODE1]);
    //delay as per datasheet
    usleep(1000);
}

uint8_t pca9685_write_reg(int fd, uint8_t reg, uint8_t data)
{
    uint8_t dta[2] = {0};
    dta[0] = reg;
    dta[1] = data;
    
    if(write(fd, dta, 2) < 0){
        printf("Cannot write to reg %x\n", reg);
        return 0;
    }
    
    return 1;
}

uint8_t pca9685_read_reg(int fd, uint8_t reg)
{   
    uint8_t data;
    
    if(write(fd, &reg, 1) < 0){
        printf("Cannot write to reg %x\n", reg);
        return 0;
    }
    
    if(read(fd, &data, 1) < 0){
        printf("Cannot read from reg %x\n", reg);
        return 0;
    }
    return data;
}

uint8_t pca9685_write_reg_pair(int fd, uint8_t reg, uint16_t data)
{
    uint8_t byte[2];
    byte[0] = (uint8_t)(data & 0xff);
    byte[1] = (uint8_t)((data >> 8) & 0x1f);
    
    
    pca9685_write_reg(fd, reg, byte[0]);
    pca9685_write_reg(fd, reg + 1, byte[1]);
    
    return 0;
}

uint16_t pca9685_read_reg_pair(int fd, uint8_t reg)
{
    uint16_t word;
    uint8_t byte[2];
    
    byte[0] = pca9685_read_reg(fd, reg);
    byte[1] = pca9685_read_reg(fd, reg + 1);
    byte[1] &= 0x1f;
    
    word = (8 << byte[1]) | byte[0];
    return word;
}


uint8_t pca9685_read_regs(int fd, uint8_t st_reg, uint8_t* data, int count)
{
    int i;
    
    //enabling auto increment on the device
    pca9685_regs[MODE1] |= AI_BIT;
    pca9685_write_reg(fd, MODE1, pca9685_regs[MODE1]);
    
    if(write(fd, &st_reg, 1) < 0){
        printf("Cannot write to reg %x\n", st_reg);
        return -1;
    }
    if(read(fd, data, count) < 0){
        printf("Cannot read from reg %x\n", st_reg);
        return -1;
    }
    
    //disabling auto increment
    pca9685_regs[MODE1] &= ~AI_BIT;
    pca9685_write_reg(fd, MODE1, pca9685_regs[MODE1]);
    
    return count;
}

uint8_t pca9685_write_regs(int fd, uint8_t st_reg, uint8_t* data, int count)
{
    int i;
    
    //enabling auto increment on the device
    pca9685_regs[MODE1] |= AI_BIT;
    pca9685_write_reg(fd, MODE1, pca9685_regs[MODE1]);
    
    if(write(fd, &st_reg, 1) < 0){
        printf("Cannot write to reg %x\n", st_reg);
        return -1;
    }
    if(write(fd, data, count) < 0){
        printf("Cannot write to reg %x\n", st_reg);
        return -1;
    }
    
    //disabling auto increment
    pca9685_regs[MODE1] &= ~AI_BIT;
    pca9685_write_reg(fd, MODE1, pca9685_regs[MODE1]);
    
    return count;
}
