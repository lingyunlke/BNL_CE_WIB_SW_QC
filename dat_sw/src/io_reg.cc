extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "io_reg.h"



int io_reg_init(io_reg_t *reg, size_t base_addr, size_t n_reg) {

	reg->base_addr = base_addr;
	reg->fd=open("/dev/mem",O_RDWR|O_SYNC);
	if (reg->fd < 1) return 1;

	size_t ptr = (size_t) mmap(NULL,n_reg*4,PROT_READ|PROT_WRITE,MAP_SHARED,reg->fd,base_addr);
	if (ptr == -1) return 2;

    reg->ptr = (uint32_t*)(ptr);
    reg->n_reg = n_reg;
    return 0;
}

int io_reg_free(io_reg_t *reg) {
    munmap(reg->ptr,reg->n_reg*4);
    close(reg->fd);
    return 0;
}

uint32_t io_reg_read(io_reg_t *reg, size_t idx) {
    return reg->ptr[idx];
}

void io_reg_write(io_reg_t *reg, size_t idx, uint32_t data, uint32_t mask) {
    if (mask == 0xFFFFFFFF) {
        reg->ptr[idx] = data;
    } else {
        uint32_t prev = io_reg_read(reg,idx);
        data = (data & mask) | ((~mask) & prev);
        reg->ptr[idx] = data;
    }
}
}



