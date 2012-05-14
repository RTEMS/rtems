/*  flash.c
 *
 *  Milkymist flash driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011 Sebastien Bourdeauducq
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems.h>
#include <stdio.h>
#include <bsp.h>
#include <string.h>
#include <rtems/libio.h>
#include <rtems/status-checks.h>
#include "../include/system_conf.h"
#include "milkymist_flash.h"

static struct flash_partition partitions[FLASH_PARTITION_COUNT]
  = FLASH_PARTITIONS;

static rtems_id flash_lock;

rtems_device_driver flash_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  int i;
  char devname[16];

  for (i=0;i<FLASH_PARTITION_COUNT;i++) {
    sprintf(devname, "/dev/flash%d", i+1);
    sc = rtems_io_register_name(devname, major, i);
    RTEMS_CHECK_SC(sc, "Create flash device");
  }

  sc = rtems_semaphore_create(
    rtems_build_name('F', 'L', 'S', 'H'),
    1,
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &flash_lock
  );
  RTEMS_CHECK_SC(sc, "create semaphore");

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver flash_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  void *startaddr;
  int len;

  if (minor >= FLASH_PARTITION_COUNT)
    return RTEMS_UNSATISFIED;

  startaddr = (void *)(partitions[minor].start_address
    + (unsigned int)rw_args->offset);
  len = partitions[minor].length - rw_args->offset;
  if (len > rw_args->count)
    len = rw_args->count;
  if (len <= 0) {
    rw_args->bytes_moved = 0;
    return RTEMS_SUCCESSFUL;
  }
  
  rtems_semaphore_obtain(flash_lock, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  memcpy(rw_args->buffer, startaddr, len);
  rtems_semaphore_release(flash_lock);

  rw_args->bytes_moved = len;
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver flash_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  volatile unsigned short *startaddr;
  unsigned short *srcdata;
  int len;
  int this_time;
  int remaining;
  int i;

  if (minor >= FLASH_PARTITION_COUNT)
    return RTEMS_UNSATISFIED;

  startaddr = (unsigned short *)(partitions[minor].start_address
    + (unsigned int)rw_args->offset);
  len = partitions[minor].length - rw_args->offset;
  if (len > rw_args->count)
    len = rw_args->count;
  if (len <= 2) {
    rw_args->bytes_moved = 0;
    return RTEMS_SUCCESSFUL;
  }
  len /= 2;
  srcdata = (unsigned short *)rw_args->buffer;
  remaining = len;

  rtems_semaphore_obtain(flash_lock, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  while (remaining > 0) {
    this_time = remaining;
    if (this_time > 256)
      this_time = 256;
    /* Issue "Buffered Programming Setup" command
     * and wait for buffer available.
     */
    do {
      *startaddr = 0x00e8;
    } while (!(*startaddr & 0x0080));
    /* Load word count */
    *startaddr = this_time-1;
    /* Fill buffer */
    for(i=0;i<this_time;i++)
      startaddr[i] = srcdata[i];
    /* Issue "Buffer Programming Confirm" command */
    *startaddr = 0x00d0;
    while (!(*startaddr & 0x0080)); /* read status register, wait for ready */
    *startaddr = 0x0050; /* clear status register */
    /* update state */
    startaddr += this_time;
    srcdata += this_time;
    remaining -= this_time;
  }
  *startaddr = 0x00ff; /* back to read array mode */
  rtems_semaphore_release(flash_lock);

  rw_args->bytes_moved = 2*len;
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver flash_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;
  unsigned int eraseaddr_i;
  volatile unsigned short *eraseaddr;

  if (minor >= FLASH_PARTITION_COUNT) {
    args->ioctl_return = -1;
    return RTEMS_UNSATISFIED;
  }

  switch (args->command) {
    case FLASH_GET_SIZE:
      *((unsigned int *)args->buffer) = partitions[minor].length;
      break;
    case FLASH_GET_BLOCKSIZE:
      *((unsigned int *)args->buffer) = 128*1024;
      break;
    case FLASH_ERASE_BLOCK:
      eraseaddr_i = (unsigned int)args->buffer;
      if (eraseaddr_i >= partitions[minor].length) {
        args->ioctl_return = -1;
        return RTEMS_UNSATISFIED;
      }
      eraseaddr_i = eraseaddr_i + partitions[minor].start_address;
      eraseaddr = (unsigned short *)eraseaddr_i;
      rtems_semaphore_obtain(flash_lock, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      *eraseaddr = 0x0020; /* erase */
      *eraseaddr = 0x00d0;
      while(!(*eraseaddr & 0x0080)); /* read status register, wait for ready */
      *eraseaddr = 0x0050; /* clear status register */
      *eraseaddr = 0x00ff; /* back to read array mode */
      rtems_semaphore_release(flash_lock);
      break;
    default:
      args->ioctl_return = -1;
      return RTEMS_UNSATISFIED;
  }
  args->ioctl_return = 0;
  return RTEMS_SUCCESSFUL;
}
