#include <bsp.h>

#define ram_addr_low  (* (volatile char *) IO_TO_LOCAL(0x0074))
#define ram_addr_high (* (volatile char *) IO_TO_LOCAL(0x0075))
#define ram_data      (* (volatile char *) IO_TO_LOCAL(0x0077))
#define ram_end_addr  0x1ff8

int read_nvram(char *buff, int offset, int len) {
    int i;

    if (offset < 0 || offset >= ram_end_addr) {
        return 0;
    }
    if (offset + len > ram_end_addr) {
        len = ram_end_addr - offset;
    }
    for (i = 0; i < len; i++) {
        ram_addr_high = offset >> 8;
        ram_addr_low  = offset;
        *buff++ = ram_data;
        offset++;
    }
    return len;
}

