extern "C" { 
#include "wib_i2c.h"
#include <string.h>
#include <stdlib.h>

#include <errno.h>
#include <unistd.h>
#include <linux/i2c.h>


#include <linux/i2c-dev.h>
#include <i2c/smbus.h>


#include <sys/ioctl.h>
#include <fcntl.h>



#ifndef SIMULATION

int i2c_init(i2c_t *i2c, char *device) {
    //printf("i2c_init fd = %d\n",i2c->fd);
	i2c->fd = open(device,O_RDWR);
    if (i2c->fd < 1) {
		printf("i2c_init failed %s\n",strerror(errno));
		return 1;
	}
	else {
		//printf("i2c_init fd = 0x%02X\n",i2c->fd);
	}
    i2c->slave = 0;
    return 0;
}

int i2c_free(i2c_t *i2c) {
	//printf("closing i2c 0x%x\n",i2c->fd);
	
    if (i2c->fd != 0) close(i2c->fd);
	//printf("i2c_init free\n");
    return 0;
}

int i2c_reg_read(i2c_t *i2c, uint8_t slave, uint8_t reg) {
    if (slave != i2c->slave) {
        ioctl(i2c->fd, I2C_SLAVE, slave);
        i2c->slave = slave;
    }
    int res = i2c_smbus_read_byte_data(i2c->fd,reg);
    if (res < 0) {
        printf("i2c_reg_read failed %s\n",strerror(errno));
    }
    return res;
}

int i2c_reg_write(i2c_t *i2c, uint8_t slave, uint8_t reg, uint8_t data) {
    if (slave != i2c->slave) {
        ioctl(i2c->fd, I2C_SLAVE, slave);
        i2c->slave = slave;
    }
    int res = i2c_smbus_write_byte_data(i2c->fd, reg, data);
    if (res < 0) {
        printf("i2c_reg_write failed %s\n",strerror(errno));
    }
    return res;
}

int i2c_readwrite(i2c_t *i2c, uint8_t slave, uint8_t *rbuf, size_t rlen, uint8_t *wbuf, size_t wlen) {
    i2c->slave = -1; //not sure what happens to the ioctl I2C_SLAVE here
    i2c_msg msg[2] = {
                       { slave, I2C_M_RD, (uint16_t)rlen, rbuf },
                       { slave, 0, (uint16_t)wlen, wbuf }
                     };
    i2c_rdwr_ioctl_data ioctl_data = { msg, 2 };
    int res = ioctl(i2c->fd, I2C_RDWR, &ioctl_data);
    if (res == -1) {
        printf("i2c_readwrite failed %s\n",strerror(errno));
    }
    return res;
}

int i2c_writeread(i2c_t *i2c, uint8_t slave, uint8_t *wbuf, size_t wlen, uint8_t *rbuf, size_t rlen) {
    i2c->slave = -1; //not sure what happens to the ioctl I2C_SLAVE here
    i2c_msg msg[2] = {
                       { slave, 0, (uint16_t)wlen, wbuf },
                       { slave, I2C_M_RD, (uint16_t)rlen, rbuf }
                     };
    i2c_rdwr_ioctl_data ioctl_data = { msg, 2 };
    int res = ioctl(i2c->fd, I2C_RDWR, &ioctl_data);
    if (res == -1) {
        printf("i2c_writeread failed %s\n",strerror(errno));
    }
    return res;
}

int i2c_read(i2c_t *i2c, uint8_t slave, uint8_t *buf, size_t len) {
    i2c->slave = -1; //not sure what happens to the ioctl I2C_SLAVE here
    i2c_msg message = { slave, I2C_M_RD, (uint16_t)len, buf };
    i2c_rdwr_ioctl_data ioctl_data = { &message, 1 };
    int res = ioctl(i2c->fd, I2C_RDWR, &ioctl_data);
    if (res == -1) {
        printf("i2c_read failed %s, slave=%x \n",strerror(errno), slave);
    }
    return res;
}

int i2c_write(i2c_t *i2c, uint8_t slave, uint8_t *buf, size_t len) {
    i2c->slave = -1; //not sure what happens to the ioctl I2C_SLAVE here
    i2c_msg message = { slave, 0, (uint16_t)len, buf };
    i2c_rdwr_ioctl_data ioctl_data = { &message, 1 };
    int res = ioctl(i2c->fd, I2C_RDWR, &ioctl_data);
    if (res == -1) {
       printf("i2c_write failed %s, slave=%x\n",strerror(errno), slave);
    }
    return res;
}

int i2c_block_write(i2c_t *i2c, uint8_t slave, uint8_t reg, uint8_t *buf, size_t len) {
    if (slave != i2c->slave) {
        ioctl(i2c->fd, I2C_SLAVE, slave);
        i2c->slave = slave;
    }
    int res = i2c_smbus_write_i2c_block_data(i2c->fd, reg, len, buf);
    if (res < 0) {
       printf("i2c_write failed %s\n",strerror(errno));
    }
    return res;
}

int i2c_block_read(i2c_t *i2c, uint8_t slave, uint8_t reg, uint8_t *buf, size_t len) {
    if (slave != i2c->slave) {
        ioctl(i2c->fd, I2C_SLAVE, slave);
        i2c->slave = slave;
    }
    int res = i2c_smbus_read_i2c_block_data(i2c->fd, reg, len, buf);
    if (res < 0) {
       printf("i2c_block_read failed %s\n",strerror(errno));
    }
    return res;
}

#else

int i2c_init(i2c_t *i2c, char *device) {
    return 0;
}

int i2c_free(i2c_t *i2c) {
    return 0;
}

int i2c_read(i2c_t *i2c, uint8_t slave, uint8_t *buf, size_t len) {
    return 0;
}

int i2c_write(i2c_t *i2c, uint8_t slave, uint8_t *buf, size_t len) {
    return 0;
}

int i2c_readwrite(i2c_t *i2c, uint8_t slave, uint8_t *rbuf, size_t rlen, uint8_t *buf, size_t len) {
    return 0;
}

int i2c_writeread(i2c_t *i2c, uint8_t slave, uint8_t *wbuf, size_t wlen, uint8_t *buf, size_t len) {
    return 0;
}


int i2c_reg_read(i2c_t *i2c, uint8_t slave, uint8_t reg) {
    return 0;
}

int i2c_reg_write(i2c_t *i2c, uint8_t slave, uint8_t reg, uint8_t data) {
    return 0;
}

int i2c_block_write(i2c_t *i2c, uint8_t slave, uint8_t reg, uint8_t *buf, size_t len) {
    return 0;
}

int i2c_block_read(i2c_t *i2c, uint8_t slave, uint8_t reg, uint8_t *buf, size_t len) {
    return 0;
}


#endif

}
