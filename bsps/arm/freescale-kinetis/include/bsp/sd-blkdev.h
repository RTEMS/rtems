#ifndef _SPI_SD_BLKDEV_H_
#define _SPI_SD_BLKDEV_H_

#include <rtems/blkdev.h>

int sd_card_blkdev_ioctl( rtems_disk_device *dd, uint32_t req, void *arg);
int sd_card_blkdev_bnum(uint32_t *pNum);

#endif