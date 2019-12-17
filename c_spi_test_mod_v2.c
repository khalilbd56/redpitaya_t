/* This is a simple application for testing SPI communication on a RedPitaya
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>

/* Inline functions definition */
static int init_spi();
static int release_spi();
static int write_spi(int* write_data, int size);

/* Constants definition */
int spi_fd = -1;

int main(void){

    /* Sample data */
    //char *data = "REDPITAYA SPI TEST";	
	int dataA [6][1] = {{0x002c8018}, {0x08008029}, {0x00004e42}, {0x000004b3}, {0x00ec803c}, {0x00580005}};
	
	/*******  User added code block     **********/ 
	/* Write R0~R5 data */
	for(int i=0; i<6; ++i){
		if(write_spi(dataA[i], sizeof(dataA[i])) < 0){
			printf("Write dataA[%i] to SPI failed. Error: %s\n", i, strerror(errno));
			return -1;
		}
	}
	/****** End of user code block      *********/

    return 0;
}

static int init_spi(){

    /* MODES: mode |= SPI_LOOP;
    *        mode |= SPI_CPHA;
    *        mode |= SPI_CPOL;
    *                 mode |= SPI_LSB_FIRST;
    *        mode |= SPI_CS_HIGH;
    *        mode |= SPI_3WIRE;
    *        mode |= SPI_NO_CS;
    *        mode |= SPI_READY;
    *
    * multiple possibilities possible using | */
    int mode = 0;

    /* Opening file stream */
    spi_fd = open("/dev/spidev1.0", O_RDWR | O_NOCTTY);

    if(spi_fd < 0){
        printf("Error opening spidev0.1. Error: %s\n", strerror(errno));
        return -1;
    }

    /* Setting mode (CPHA, CPOL) */
    if(ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0){
        printf("Error setting SPI_IOC_RD_MODE. Error: %s\n", strerror(errno));
        return -1;
    }

    /* Setting SPI bus speed */
    int spi_speed = 1000000;

    if(ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0){
        printf("Error setting SPI_IOC_WR_MAX_SPEED_HZ. Error: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

static int release_spi(){

    /* Release the spi resources */
    close(spi_fd);

    return 0;
}

/* Write data to the SPI bus */
static int write_spi(int* write_buffer, int size){

    if(init_spi() < 0){
        printf("Initialization of SPI failed. Error: %s\n", strerror(errno));
        return -1;
    }
    //usleep(100);

    int write_spi = write(spi_fd, write_buffer, size);

    if(write_spi < 0){
        printf("Failed to write to SPI. Error: %s\n", strerror(errno));
        return -1;
    }

    /* Release resources */
    if(release_spi() < 0){
        printf("Relase of SPI resources failed, Error: %s\n", strerror(errno));
        return -1;
    }
    usleep(200);

    return 0;
}

