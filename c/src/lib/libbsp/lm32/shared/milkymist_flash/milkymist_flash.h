/*  milkymist_flash.h
 *  
 *  Copyright (C) 2010 Sebastien Bourdeauducq
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef __MILKYMIST_FLASH_H_
#define __MILKYMIST_FLASH_H_

/* Ioctls */
#define FLASH_GET_SIZE            0x4600
#define FLASH_GET_BLOCKSIZE       0x4601
#define FLASH_ERASE_BLOCK         0x4602

struct flash_partition {
	unsigned int start_address;
	unsigned int length;
};

rtems_device_driver flash_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver flash_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver flash_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver flash_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define FLASH_DRIVER_TABLE_ENTRY {flash_initialize, \
NULL, NULL, flash_read, flash_write, flash_control}

#endif /* __MILKYMIST_FLASH_H_ */
