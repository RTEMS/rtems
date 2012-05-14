/*  video.c
 *
 *  Milkymist video input driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011 Sebastien Bourdeauducq
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <rtems.h>
#include <bsp.h>
#include <bsp/irq-generic.h>
#include <rtems/libio.h>
#include <rtems/status-checks.h>
#include "../include/system_conf.h"
#include "milkymist_video.h"

#define DEVICE_NAME "/dev/video"
#define N_BUFFERS 3
#define FRAME_W 720
#define FRAME_H 288

static bool buffers_locked[N_BUFFERS];
static void *buffers[N_BUFFERS];
static int last_buffer;
static int current_buffer;

static rtems_isr frame_handler(rtems_vector_number n)
{
  int remaining_attempts;

  lm32_interrupt_ack(1 << MM_IRQ_VIDEOIN);
  
  last_buffer = current_buffer;
  
  /* get a new buffer */
  remaining_attempts = N_BUFFERS;
  do {
    current_buffer++;
    if(current_buffer == N_BUFFERS)
      current_buffer = 0;
    remaining_attempts--;
  } while(buffers_locked[current_buffer] && (remaining_attempts > 0));

  MM_WRITE(MM_BT656_BASE, (unsigned int)buffers[current_buffer]);
  
  if(buffers_locked[current_buffer])
    printk("Failed to find unlocked buffer\n");
}

static void i2c_delay(void)
{
  unsigned int i;

  for(i=0;i<1000;i++) __asm__("nop");
}

/* I2C bit-banging functions from http://en.wikipedia.org/wiki/I2c */
static unsigned int i2c_read_bit(void)
{
  unsigned int bit;

  /* Let the slave drive data */
  MM_WRITE(MM_BT656_I2C, 0);
  i2c_delay();
  MM_WRITE(MM_BT656_I2C, BT656_I2C_SDC);
  i2c_delay();
  bit = MM_READ(MM_BT656_I2C) & BT656_I2C_SDAIN;
  i2c_delay();
  MM_WRITE(MM_BT656_I2C, 0);
  return bit;
}

static void i2c_write_bit(unsigned int bit)
{
  if(bit) {
    MM_WRITE(MM_BT656_I2C, BT656_I2C_SDAOE|BT656_I2C_SDAOUT);
  } else {
    MM_WRITE(MM_BT656_I2C, BT656_I2C_SDAOE);
  }
  i2c_delay();
  MM_WRITE(MM_BT656_I2C, MM_READ(MM_BT656_I2C) | BT656_I2C_SDC);
  i2c_delay();
  MM_WRITE(MM_BT656_I2C, MM_READ(MM_BT656_I2C) & ~BT656_I2C_SDC);
}

static int i2c_started;

static void i2c_start_cond(void)
{
  if(i2c_started) {
    /* set SDA to 1 */
    MM_WRITE(MM_BT656_I2C, BT656_I2C_SDAOE|BT656_I2C_SDAOUT);
    i2c_delay();
    MM_WRITE(MM_BT656_I2C, MM_READ(MM_BT656_I2C) | BT656_I2C_SDC);
  }
  /* SCL is high, set SDA from 1 to 0 */
  MM_WRITE(MM_BT656_I2C, BT656_I2C_SDAOE|BT656_I2C_SDC);
  i2c_delay();
  MM_WRITE(MM_BT656_I2C, BT656_I2C_SDAOE);
  i2c_started = 1;
}

static void i2c_stop_cond(void)
{
  /* set SDA to 0 */
  MM_WRITE(MM_BT656_I2C, BT656_I2C_SDAOE);
  i2c_delay();
  /* Clock stretching */
  MM_WRITE(MM_BT656_I2C, BT656_I2C_SDAOE|BT656_I2C_SDC);
  /* SCL is high, set SDA from 0 to 1 */
  MM_WRITE(MM_BT656_I2C, BT656_I2C_SDC);
  i2c_delay();
  i2c_started = 0;
}

static unsigned int i2c_write(unsigned char byte)
{
  unsigned int bit;
  unsigned int ack;

  for(bit = 0; bit < 8; bit++) {
    i2c_write_bit(byte & 0x80);
    byte <<= 1;
  }
  ack = !i2c_read_bit();
  return ack;
}

static unsigned char i2c_read(int ack)
{
  unsigned char byte = 0;
  unsigned int bit;

  for(bit = 0; bit < 8; bit++) {
    byte <<= 1;
    byte |= i2c_read_bit();
  }
  i2c_write_bit(!ack);
  return byte;
}

static unsigned char read_reg(unsigned char addr)
{
  unsigned char r;

  i2c_start_cond();
  i2c_write(0x40);
  i2c_write(addr);
  i2c_start_cond();
  i2c_write(0x41);
  r = i2c_read(0);
  i2c_stop_cond();

  return r;
}

static void write_reg(unsigned char addr, unsigned char val)
{
  i2c_start_cond();
  i2c_write(0x40);
  i2c_write(addr);
  i2c_write(val);
  i2c_stop_cond();
}

static const char vreg_addr[] = {
  0x1d, 0xc3, 0xc4
};

static const char vreg_dat[] = {
  0x40, 0x05, 0x80
};

rtems_device_driver video_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  rtems_isr_entry dummy;
  int i;
  
  MM_WRITE(MM_BT656_I2C, BT656_I2C_SDC);

  sc = rtems_io_register_name(DEVICE_NAME, major, 0);
  RTEMS_CHECK_SC(sc, "create video input device");

  rtems_interrupt_catch(frame_handler, MM_IRQ_VIDEOIN, &dummy);
  bsp_interrupt_vector_enable(MM_IRQ_VIDEOIN);
  
  for(i=0;i<sizeof(vreg_addr);i++)
    write_reg(vreg_addr[i], vreg_dat[i]);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver video_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  int i;
  int status;

  for(i=0;i<N_BUFFERS;i++) {
    status = posix_memalign(&buffers[i], 32, 2*FRAME_W*FRAME_H);
    if(status != 0) {
      i--;
      while(i > 0) {
        free(buffers[i]);
        i--;
      }
      return RTEMS_UNSATISFIED;
    }
  }
  
  last_buffer = -1;
  current_buffer = 0;
  
  MM_WRITE(MM_BT656_BASE, (unsigned int)buffers[current_buffer]);
  MM_WRITE(MM_BT656_FILTERSTATUS, BT656_FILTER_FIELD1);
  
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver video_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  int i;
  
  MM_WRITE(MM_BT656_FILTERSTATUS, 0);
  while(MM_READ(MM_BT656_FILTERSTATUS) & BT656_FILTER_INFRAME);
  for(i=0;i<N_BUFFERS;i++)
    free(buffers[i]);
  return RTEMS_SUCCESSFUL;
}

static void invalidate_caches(void)
{
  volatile char *flushbase = (char *)FMLBRG_FLUSH_BASE;
  int i, offset;

  offset = 0;
  for (i=0;i<FMLBRG_LINE_COUNT;i++) {
    flushbase[offset] = 0;
    offset += FMLBRG_LINE_LENGTH;
  }
  __asm__ volatile( /* Invalidate Level-1 data cache */
    "wcsr DCC, r0\n"
    "nop\n"
  );
}

static void set_format(int format)
{
  switch(format) {
    case VIDEO_FORMAT_CVBS6:
      write_reg(0x00, 0x00);
      write_reg(0xc3, 0x05);
      write_reg(0xc4, 0x80);
      break;
    case VIDEO_FORMAT_CVBS5:
      write_reg(0x00, 0x00);
      write_reg(0xc3, 0x0d);
      write_reg(0xc4, 0x80);
      break;
    case VIDEO_FORMAT_CVBS4:
      write_reg(0x00, 0x00);
      write_reg(0xc3, 0x04);
      write_reg(0xc4, 0x80);
      break;
    case VIDEO_FORMAT_SVIDEO:
      write_reg(0x00, 0x06);
      write_reg(0xc3, 0xd5);
      write_reg(0xc4, 0x80);
      break;
    case VIDEO_FORMAT_COMPONENT:
      write_reg(0x00, 0x09);
      write_reg(0xc3, 0x45);
      write_reg(0xc4, 0x8d);
      break;
  }
}

rtems_device_driver video_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;
  unsigned int *a = (unsigned int *)args->buffer;
  rtems_status_code sc;

  switch (args->command) {
    case VIDEO_BUFFER_LOCK:
      if (last_buffer == -1) {
        *a = 0;
      } else {
        bsp_interrupt_vector_disable(MM_IRQ_VIDEOIN);
        if(*a) invalidate_caches();
        *a = (unsigned int)buffers[last_buffer];
        buffers_locked[last_buffer] = true;
        bsp_interrupt_vector_enable(MM_IRQ_VIDEOIN);
      }
      sc = RTEMS_SUCCESSFUL;
      break;
    case VIDEO_BUFFER_UNLOCK: {
      int i;
      for(i=0;i<N_BUFFERS;i++) {
        if ((unsigned int)buffers[i] == (unsigned int)a) {
          buffers_locked[i] = false;
          break;
        }
      }
      sc = RTEMS_SUCCESSFUL;
      break;
    }
    
    case VIDEO_SET_BRIGHTNESS:
      write_reg(0x0a, (unsigned int)a);
      sc = RTEMS_SUCCESSFUL;
      break;
    case VIDEO_GET_BRIGHTNESS:
      *a = read_reg(0x0a);
      sc = RTEMS_SUCCESSFUL;
      break;
    case VIDEO_SET_CONTRAST:
      write_reg(0x08, (unsigned int)a);
      sc = RTEMS_SUCCESSFUL;
      break;
    case VIDEO_GET_CONTRAST:
      *a = read_reg(0x08);
      sc = RTEMS_SUCCESSFUL;
      break;
    case VIDEO_SET_HUE:
      write_reg(0x0b, (unsigned int)a);
      sc = RTEMS_SUCCESSFUL;
      break;
    case VIDEO_GET_HUE:
      *a = read_reg(0x0b);
      sc = RTEMS_SUCCESSFUL;
      break;
    
    case VIDEO_GET_SIGNAL:
      *a = read_reg(0x10);
      sc = RTEMS_SUCCESSFUL;
      break;
    
    case VIDEO_SET_REGISTER:
      write_reg(((unsigned int)a & 0xffff0000) >> 16,
        (unsigned int)a & 0x0000ffff);
      sc = RTEMS_SUCCESSFUL;
      break;
    case VIDEO_GET_REGISTER:
      *a = read_reg(*a);
      sc = RTEMS_SUCCESSFUL;
      break;
    
    case VIDEO_SET_FORMAT:
      set_format((int)a);
      sc = RTEMS_SUCCESSFUL;
      break;
    
    default:
      sc = RTEMS_UNSATISFIED;
      break;
  }

  if (sc == RTEMS_SUCCESSFUL)
    args->ioctl_return = 0;
  else
    args->ioctl_return = -1;

  return sc;
}
