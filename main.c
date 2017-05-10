#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/types.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

#include "pca9685.h"
#include "i2c_master.h"

#define I2C_ADDR         0x40
#define I2C_ALLCALL_ADDR 0x70

//uint8_t pca9685_setpin(int fd, uint8_t pin, uint16_t low, uint16_t high);
//uint8_t[2] mode_regs = {0x00, 0x00};

/* Used for signal handling */
static volatile sig_atomic_t doneFlag = 1;
int fd;


static void catchsignal(int signo, siginfo_t *siginfo, void *context)
{
    if ((signo == SIGINT) || (signo == SIGQUIT) || (signo == SIGABRT) || (signo == SIGTERM)) {
        doneFlag = 0;
    }
}

void cleanup(int fd)
{
    printf("Cleaning up!!!\n");
    /*uint8_t data[2];

    data[0] = ALL_LED_ON_L;
    data[0] = 0x00;
    data[1] = 0x00;
    //write(fd, &data, 3);
    i2c_mwrite_reg_pair(fd, ALL_LED_ON_L, 0);
    i2c_mwrite_reg_pair(fd, ALL_LED_OFF_L, 0);

    //Turning all LEDs OFF
    write(fd, &data, 3);
    data[0] = ALL_LED_OFF_L;
    data[1] = 0x00;
    data[2] = 0x00;

    //Turning all LEDs OFF
    //write(fd, &data, 3);
    */
    //pca9685_sleep(fd);
    //pca9685_reset(fd);
    pca9685_shutdown(fd);
    //pca9685_sleep(fd);
}

void *display_pattern(void *ptr)
{
    int tmp = 0;

    while(doneFlag){
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
}

void *display_onoff(void *ptr)
{
    while(doneFlag){
        i2c_mwrite_reg_pair(fd, LED4_ON_L, 0);
        i2c_mwrite_reg_pair(fd, LED4_OFF_L,0);
        i2c_mwrite_reg_pair(fd, LED5_ON_L, 0);
        i2c_mwrite_reg_pair(fd, LED5_OFF_L,4095);

	sleep(1);

        i2c_mwrite_reg_pair(fd, LED4_ON_L, 0);
        i2c_mwrite_reg_pair(fd, LED4_OFF_L,4095);
        i2c_mwrite_reg_pair(fd, LED5_ON_L, 0);
        i2c_mwrite_reg_pair(fd, LED5_OFF_L,0);

	sleep(1);
    }
}

int main(int argc, char* argv[])
{
    pthread_t th1, th2;
    int iret1, iret2;

    // Signal handling
    struct sigaction act;
    
    fd = i2c_mopen(1);
    i2c_mset_addr(fd, I2C_ADDR);
    //seeding the random number generator
    srand(time(NULL));


    memset (&act, 0, sizeof(act));
    act.sa_sigaction = catchsignal;/* set up signal handler */
    act.sa_flags = SA_SIGINFO;
        
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    
    pca9685_init(fd);
    int tmp = 0;
    /*while(doneFlag){
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
    }*/
    //display_pattern(NULL);
    iret1 = pthread_create(&th1, NULL, display_pattern, NULL);
    iret2 = pthread_create(&th2, NULL, display_onoff, NULL);

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
   
    cleanup(fd); 
    i2c_mclose(fd);
    
    return 0;
}


