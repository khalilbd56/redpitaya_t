#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN 976
#define POUT 968

#define VALUE_MAX 30
#define BUFFER_MAX 3

#define MAX_PATH 64


static int pin_export(int pin)
{
	char shell[MAX_PATH];
	sprintf(shell,"echo %d > /sys/class/gpio/export", pin);
	system(shell);
	return 0;
}

static int pin_unexport(int pin)
{
        char shell[MAX_PATH];
        sprintf(shell,"echo %d > /sys/class/gpio/unexport", pin);
        system(shell);

	return 0;
}

static int pin_direction(int pin, int dir){

	char shell[MAX_PATH];
	snprintf(shell, MAX_PATH, "echo %s > /sys/class/gpio/gpio%d/direction",((dir==IN)?"in":"out"),pin);
	system(shell);

	return 0;
}

static int pin_write(int pin, int value)
{
	char path[VALUE_MAX];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	// get pin value file descrptor
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Unable to to open sysfs pins value file %s for writing\n",path);
		return -1;
	}
	if(value==LOW){
		//write low
		if (1 != write(fd, "0", 1)) {
			fprintf(stderr, "Unable to write value\n");
			return -1;
		}
	}
        else if(value==HIGH){
		//write high
		if (1 != write(fd, "1", 1)) {
                	fprintf(stderr, "Unable to write value\n");
                	return -1;
		}
	}else fprintf(stderr, "Nonvalid pin value requested\n");

	//close file
	close(fd);
	return 0;
}

int main(void)
{
	// apply required fpga bitstream mercury or classic
	// system("cat /opt/redpitaya/fpga/fpga_classic.bit > /dev/xdevcfg");
	
	int repeat = 10;
	
	// export pins
	if (-1 == pin_export(POUT)) return 1;

	// set pin direction
	if (-1 == pin_direction(POUT, OUT) ) return 2;
	
	for (int i = 1; i <= repeat; i++){
		// set pin value
		if (-1 == pin_write( POUT, i % 2)) return 3;
		printf("Setting pin %d to %d\n", POUT,i % 2);

		usleep(10000);
	}
	// unexport pins on exit
	if (-1 == pin_unexport(POUT)) return 4;
	return 0;
}
