/*  memcard.c
 *
 *  Milkymist memory card driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011 Sebastien Bourdeauducq
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/diskdevs.h>
#include <rtems/blkdev.h>
#include <rtems/status-checks.h>
#include <errno.h>
#include <bsp.h>
#include "../include/system_conf.h"
#include "milkymist_memcard.h"

//#define MEMCARD_DEBUG

#define BLOCK_SIZE 512

static void memcard_start_cmd_tx(void)
{
  MM_WRITE(MM_MEMCARD_ENABLE, MEMCARD_ENABLE_CMD_TX);
}

static void memcard_start_cmd_rx(void)
{
  MM_WRITE(MM_MEMCARD_PENDING, MEMCARD_PENDING_CMD_RX);
  MM_WRITE(MM_MEMCARD_START, MEMCARD_START_CMD_RX);
  MM_WRITE(MM_MEMCARD_ENABLE, MEMCARD_ENABLE_CMD_RX);
}

static void memcard_start_cmd_dat_rx(void)
{
  MM_WRITE(MM_MEMCARD_PENDING, MEMCARD_PENDING_CMD_RX|MEMCARD_PENDING_DAT_RX);
  MM_WRITE(MM_MEMCARD_START, MEMCARD_START_CMD_RX|MEMCARD_START_DAT_RX);
  MM_WRITE(MM_MEMCARD_ENABLE, MEMCARD_ENABLE_CMD_RX|MEMCARD_ENABLE_DAT_RX);
}

static void memcard_send_command(unsigned char cmd, unsigned int arg)
{
  unsigned char packet[6];
  int a;
  int i;
  unsigned char data;
  unsigned char crc;

  packet[0] = cmd | 0x40;
  packet[1] = ((arg >> 24) & 0xff);
  packet[2] = ((arg >> 16) & 0xff);
  packet[3] = ((arg >> 8) & 0xff);
  packet[4] = (arg & 0xff);

  crc = 0;
  for(a=0;a<5;a++) {
    data = packet[a];
    for(i=0;i<8;i++) {
      crc <<= 1;
      if((data & 0x80) ^ (crc & 0x80))
        crc ^= 0x09;
      data <<= 1;
    }
  }
  crc = (crc<<1) | 1;

  packet[5] = crc;

#ifdef MEMCARD_DEBUG
  printk(">> %02x %02x %02x %02x %02x %02x\n",
    packet[0], packet[1], packet[2], packet[3], packet[4], packet[5]);
#endif

  for(i=0;i<6;i++) {
    MM_WRITE(MM_MEMCARD_CMD, packet[i]);
    while(MM_READ(MM_MEMCARD_PENDING) & MEMCARD_PENDING_CMD_TX);
  }
}

static void memcard_send_dummy(void)
{
  MM_WRITE(MM_MEMCARD_CMD, 0xff);
  while(MM_READ(MM_MEMCARD_PENDING) & MEMCARD_PENDING_CMD_TX);
}

static bool memcard_receive_command(unsigned char *buffer, int len)
{
  int i;
  int timeout;

  for(i=0;i<len;i++) {
    timeout = 2000000;
    while(!(MM_READ(MM_MEMCARD_PENDING) & MEMCARD_PENDING_CMD_RX)) {
      timeout--;
      if(timeout == 0) {
        #ifdef MEMCARD_DEBUG
        printk("Command receive timeout\n");
        #endif
        return false;
      }
    }
    buffer[i] = MM_READ(MM_MEMCARD_CMD);
    MM_WRITE(MM_MEMCARD_PENDING, MEMCARD_PENDING_CMD_RX);
  }

  while(!(MM_READ(MM_MEMCARD_PENDING) & MEMCARD_PENDING_CMD_RX));

  #ifdef MEMCARD_DEBUG
  printk("<< ");
  for(i=0;i<len;i++)
    printk("%02x ", buffer[i]);
  printk("\n");
  #endif

  return true;
}

static bool memcard_receive_command_data(unsigned char *command,
  unsigned int *data)
{
  int i, j;
  int timeout;

  i = 0;
  j = 0;
  while(j < 128) {
    timeout = 2000000;
    while(!(MM_READ(MM_MEMCARD_PENDING) &
      (MEMCARD_PENDING_CMD_RX|MEMCARD_PENDING_DAT_RX))) {
      timeout--;
      if(timeout == 0) {
        #ifdef MEMCARD_DEBUG
        printk("Command receive timeout\n");
        #endif
        return false;
      }
    }
    if(MM_READ(MM_MEMCARD_PENDING) & MEMCARD_PENDING_CMD_RX) {
      command[i++] = MM_READ(MM_MEMCARD_CMD);
      MM_WRITE(MM_MEMCARD_PENDING, MEMCARD_PENDING_CMD_RX);
      if(i == 6)
        /* disable command RX */
        MM_WRITE(MM_MEMCARD_ENABLE, MEMCARD_ENABLE_DAT_RX);
    }
    if(MM_READ(MM_MEMCARD_PENDING) & MEMCARD_PENDING_DAT_RX) {
      data[j++] = MM_READ(MM_MEMCARD_DAT);
      MM_WRITE(MM_MEMCARD_PENDING, MEMCARD_PENDING_DAT_RX);
    }
  }

  /* Get CRC (ignored) */
  for(i=0;i<2;i++) {
    while(!(MM_READ(MM_MEMCARD_PENDING) & MEMCARD_PENDING_DAT_RX));
    #ifdef MEMCARD_DEBUG
    printk("CRC: %08x\n", MM_READ(MM_MEMCARD_DAT));
    #endif
    MM_WRITE(MM_MEMCARD_PENDING, MEMCARD_PENDING_DAT_RX);
  }

  while(!(MM_READ(MM_MEMCARD_PENDING) & MEMCARD_PENDING_DAT_RX));

  #ifdef MEMCARD_DEBUG
  printk("<< %02x %02x %02x %02x %02x %02x\n",
    command[0], command[1], command[2], command[3], command[4], command[5]);
  #endif

  //for(i=0;i<128;i++)
  //  printk("%08x ", data[i]);
  //printk("\n");

  return true;
}

static unsigned int block_count;

static int memcard_disk_block_read(rtems_blkdev_request *r)
{
  unsigned char b[6];
  unsigned int i, nblocks;
  unsigned int block;

  block = RTEMS_BLKDEV_START_BLOCK(r);
  nblocks = r->bufnum;

  for(i=0;i<nblocks;i++) {
    /* CMD17 - read block */
    memcard_start_cmd_tx();
    memcard_send_command(17, (block+i)*BLOCK_SIZE);
    memcard_start_cmd_dat_rx();
    if(!memcard_receive_command_data(b, (unsigned int *)r->bufs[i].buffer))
      return -RTEMS_IO_ERROR;
  }

  r->req_done(r->done_arg, RTEMS_SUCCESSFUL);

  return 0;
}

static int memcard_disk_block_write(rtems_blkdev_request *r)
{
  return -RTEMS_IO_ERROR;
}

static rtems_status_code memcard_init(void)
{
  unsigned char b[17];
  unsigned int rca;
  unsigned int block_shift;
  unsigned int c_size;
  unsigned int c_size_mult;

  MM_WRITE(MM_MEMCARD_CLK2XDIV, 250);

  /* CMD0 */
  memcard_start_cmd_tx();
  memcard_send_command(0, 0);

  memcard_send_dummy();

  /* CMD8 */
  memcard_send_command(8, 0x1aa);
  memcard_start_cmd_rx();
  if(!memcard_receive_command(b, 6)) return RTEMS_IO_ERROR;

  /* ACMD41 - initialize */
  while(1) {
    memcard_start_cmd_tx();
    memcard_send_command(55, 0);
    memcard_start_cmd_rx();
    if(!memcard_receive_command(b, 6)) return RTEMS_IO_ERROR;
    memcard_start_cmd_tx();
    memcard_send_command(41, 0x00300000);
    memcard_start_cmd_rx();
    if(!memcard_receive_command(b, 6)) return RTEMS_IO_ERROR;
    if(b[1] & 0x80) break;
    #ifdef MEMCARD_DEBUG
    printk("Card is busy, retrying\n");
    #endif
  }

  /* CMD2 - get CID */
  memcard_start_cmd_tx();
  memcard_send_command(2, 0);
  memcard_start_cmd_rx();
  if(!memcard_receive_command(b, 17)) return RTEMS_IO_ERROR;

  /* CMD3 - get RCA */
  memcard_start_cmd_tx();
  memcard_send_command(3, 0);
  memcard_start_cmd_rx();
  if(!memcard_receive_command(b, 6)) return RTEMS_IO_ERROR;
  rca = (((unsigned int)b[1]) << 8)|((unsigned int)b[2]);
  #ifdef MEMCARD_DEBUG
  printk("RCA: %04x\n", rca);
  #endif

  /* CMD9 - get CSD */
  memcard_start_cmd_tx();
  memcard_send_command(9, rca << 16);
  memcard_start_cmd_rx();
  if(!memcard_receive_command(b, 17)) return RTEMS_IO_ERROR;

  if(((b)[0] >> 6) != 0)
    return RTEMS_IO_ERROR;

  block_shift = ((unsigned int)(b)[5] & 0xf);
  c_size = ((((unsigned int)(b)[6] & 0x3) << 10)
    + (((unsigned int)(b)[7]) << 2)
    + ((((unsigned int)(b)[8]) >> 6) & 0x3));
  c_size_mult = ((((b)[9] & 0x3) << 1) + (((b)[10] >> 7) & 0x1));
  block_count = (c_size + 1) * (1U << (c_size_mult + 2));

  /* convert to 512-byte blocks for the sake of simplicity */
  if(block_shift < 9)
    return RTEMS_IO_ERROR;
  block_count <<= block_shift - 9;

  /* CMD7 - select card */
  memcard_start_cmd_tx();
  memcard_send_command(7, rca << 16);
  memcard_start_cmd_rx();
  if(!memcard_receive_command(b, 6)) return RTEMS_IO_ERROR;

  /* ACMD6 - set bus width */
  memcard_start_cmd_tx();
  memcard_send_command(55, rca << 16);
  memcard_start_cmd_rx();
  if(!memcard_receive_command(b, 6)) return RTEMS_IO_ERROR;
  memcard_start_cmd_tx();
  memcard_send_command(6, 2);
  memcard_start_cmd_rx();
  if(!memcard_receive_command(b, 6)) return RTEMS_IO_ERROR;

  MM_WRITE(MM_MEMCARD_CLK2XDIV, 3);

  return RTEMS_SUCCESSFUL;
}

static int memcard_disk_ioctl(rtems_disk_device *dd, uint32_t req, void *arg)
{
  if (req == RTEMS_BLKIO_REQUEST) {
    rtems_blkdev_request *r = (rtems_blkdev_request *)arg;
    switch (r->req) {
      case RTEMS_BLKDEV_REQ_READ:
        return memcard_disk_block_read(r);
      case RTEMS_BLKDEV_REQ_WRITE:
        return memcard_disk_block_write(r);
      default:
        errno = EINVAL;
        return -1;
    }
  } else if (req == RTEMS_BLKIO_CAPABILITIES) {
    *(uint32_t *)arg = RTEMS_BLKDEV_CAP_MULTISECTOR_CONT;
    return 0;
  } else {
    errno = EINVAL;
    return -1;
  }
}

static rtems_status_code memcard_disk_init(
  rtems_device_major_number major, rtems_device_minor_number minor,
  void *arg)
{
  rtems_status_code sc;
  dev_t dev;

  sc = rtems_disk_io_initialize();
  RTEMS_CHECK_SC(sc, "Initialize RTEMS disk IO");

  dev = rtems_filesystem_make_dev_t(major, 0);

  sc = memcard_init();
  RTEMS_CHECK_SC(sc, "Initialize memory card");

  sc = rtems_disk_create_phys(dev, BLOCK_SIZE, block_count, memcard_disk_ioctl,
    NULL, "/dev/memcard");
  RTEMS_CHECK_SC(sc, "Create disk device");

  return RTEMS_SUCCESSFUL;
}


static const rtems_driver_address_table memcard_disk_ops = {
  .initialization_entry = memcard_disk_init,
  .open_entry = rtems_blkdev_generic_open,
  .close_entry = rtems_blkdev_generic_close,
  .read_entry = rtems_blkdev_generic_read,
  .write_entry = rtems_blkdev_generic_write,
  .control_entry = rtems_blkdev_generic_ioctl
};

rtems_status_code memcard_register(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_device_major_number major = 0;

  sc = rtems_io_register_driver(0, &memcard_disk_ops, &major);
  RTEMS_CHECK_SC(sc, "Register disk memory card driver");

  return RTEMS_SUCCESSFUL;
}
