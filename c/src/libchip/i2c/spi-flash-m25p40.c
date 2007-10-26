/*===============================================================*\
| Project: SPI driver for M25P40 like spi flash device            |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
\*===============================================================*/
/*
 * FIXME: currently, this driver only supports read accesses
 * write/erase accesses are to be completed
 */


#include <rtems.h>
#include <rtems/libi2c.h>

#include <libchip/spi-flash-m25p40.h>
#include <rtems/libio.h>

#define FLASH_M25P40_CMD_WREN  0x06
#define FLASH_M25P40_CMD_WRDIS 0x04
#define FLASH_M25P40_CMD_RDID  0x9F
#define FLASH_M25P40_CMD_RDSR  0x05
#define FLASH_M25P40_CMD_WRSR  0x01
#define FLASH_M25P40_CMD_READ  0x03
#define FLASH_M25P40_CMD_PP    0x02  /* page program                       */
#define FLASH_M25P40_CMD_SE    0xD8  /* sector erase                       */
#define FLASH_M25P40_CMD_BE    0xC7  /* bulk erase                         */
#define FLASH_M25P40_CMD_DP    0xB9  /* deep power down                    */
#define FLASH_M25P40_CMD_RES   0xAB  /* release from deep power down       */

#define M25P40_PAGE_SIZE 256
#define M25P40_TOTAL_SIZE (512*1024)

const rtems_libi2c_tfr_mode_t spi_flash_m25p40_tfr_mode = {
  baudrate:      20000000, /* maximum bits per second                   */
  bits_per_char: 8,        /* how many bits per byte/word/longword?     */
  lsb_first:      FALSE,   /* FALSE: send MSB first                     */
  clock_inv:      FALSE,   /* FALSE: non-inverted clock (high active)   */
  clock_phs:      FALSE    /* FALSE: clock starts in middle of data tfr */
} ;

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code spi_flash_m25p40_wait_ms
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   wait a certain interval given in ms                                     |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int ms                                  /* time to wait in milliseconds   */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  rtems_status_code          rc = RTEMS_SUCCESSFUL;
  rtems_interval ticks_per_second;

  rc = rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND,&ticks_per_second);
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_task_wake_after(ticks_per_second * ms / 1000);
  }
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code spi_flash_m25p40_write
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   write a block of data to flash                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_device_major_number major,        /* major device number            */
 rtems_device_major_number minor,        /* minor device number            */
 void                      *arg          /* ptr to write argument struct   */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  rtems_status_code          rc = RTEMS_SUCCESSFUL;
  rtems_libio_rw_args_t *rwargs = arg;
  unsigned                  off = rwargs->offset;
  int                       cnt = rwargs->count;
  unsigned char            *buf = (unsigned char *)rwargs->buffer;
  int                bytes_sent = 0;
  int                  curr_cnt;
  unsigned char       cmdbuf[4];
  int                   ret_cnt = 0;
  /*
   * FIXME: check arguments
   */
  if (rc == RTEMS_SUCCESSFUL) {
  }
  /*
   * select device, set transfer mode, address device
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_libi2c_send_start(minor);
  }
  /*
   * set transfer mode
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = -rtems_libi2c_ioctl(minor,
			     RTEMS_LIBI2C_IOCTL_SET_TFRMODE,
			     &spi_flash_m25p40_tfr_mode);
  }
  /*
   * send write_enable command
   */
  if (rc == RTEMS_SUCCESSFUL) {
    cmdbuf[0] = FLASH_M25P40_CMD_WREN;
    ret_cnt = rtems_libi2c_write_bytes(minor,cmdbuf,1);
    if (ret_cnt < 0) {
      rc = -ret_cnt;
    }
  }
  /*
   * terminate transfer
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_libi2c_send_stop(minor);
  }
  while ((rc == RTEMS_SUCCESSFUL) && 
	 (cnt > bytes_sent)) {
    curr_cnt = cnt;
    if ((off              / M25P40_PAGE_SIZE) != 
	((off+curr_cnt+1) / M25P40_PAGE_SIZE)) {
      curr_cnt = M25P40_PAGE_SIZE - (off % M25P40_PAGE_SIZE);
    }
    /*
     * select device, set transfer mode, address device
     */
    if (rc == RTEMS_SUCCESSFUL) {
      rc = rtems_libi2c_send_start(minor);
    }
    /*
     * set transfer mode
     */
    if (rc == RTEMS_SUCCESSFUL) {
      rc = -rtems_libi2c_ioctl(minor,
			       RTEMS_LIBI2C_IOCTL_SET_TFRMODE,
			       &spi_flash_m25p40_tfr_mode);
    }
    /*
     * send "page program" command and address
     */
    if (rc == RTEMS_SUCCESSFUL) {
      cmdbuf[0] = FLASH_M25P40_CMD_PP;
      cmdbuf[1] = (off >> 16) & 0xff;
      cmdbuf[2] = (off >>  8) & 0xff;
      cmdbuf[3] = (off >>  0) & 0xff;
      ret_cnt = rtems_libi2c_write_bytes(minor,cmdbuf,4);
      if (ret_cnt < 0) {
	rc = -ret_cnt;
      }
    }
    /*
     * send write data
     */
    if (rc == RTEMS_SUCCESSFUL) {
      ret_cnt = rtems_libi2c_write_bytes(minor,buf,curr_cnt);
      if (ret_cnt < 0) {
	rc = -ret_cnt;
      }
    }
    /*
     * terminate transfer
     */
    if (rc == RTEMS_SUCCESSFUL) {
      rc = rtems_libi2c_send_stop(minor);
    }
    /*
     * wait proper time for data to store: 5ms
     */
    if (rc == RTEMS_SUCCESSFUL) {
      rc = spi_flash_m25p40_wait_ms(5);
    }
    /*
     * adjust bytecount to be sent and pointers
     */
    bytes_sent += curr_cnt;
    off        += curr_cnt;
    buf        += curr_cnt;
  }
  rwargs->bytes_moved = bytes_sent;
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code spi_flash_m25p40_read
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   read a block of data from flash                                         |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_device_major_number major,        /* major device number            */
 rtems_device_major_number minor,        /* minor device number            */
 void                      *arg          /* ptr to read argument struct    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  rtems_libio_rw_args_t *rwargs = arg;
  unsigned                  off = rwargs->offset;
  int                       cnt = rwargs->count;
  unsigned char            *buf = (unsigned char *)rwargs->buffer;
  unsigned char       cmdbuf[4];
  int                   ret_cnt = 0;
  /*
   * FIXME: check arguments
   */
  if (rc == RTEMS_SUCCESSFUL) {
  }
  /*
   * select device, set transfer mode, address device
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_libi2c_send_start(minor);
  }
  /*
   * set transfer mode
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = -rtems_libi2c_ioctl(minor,
			     RTEMS_LIBI2C_IOCTL_SET_TFRMODE,
			     &spi_flash_m25p40_tfr_mode);
  }
  /*
   * address device
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_libi2c_send_addr(minor,TRUE);
  }

  if (off >= M25P40_TOTAL_SIZE) {
    /* 
     * HACK: beyond size of Flash array? then read status register instead
     */
    /*
     * send read status register command
     */
    if (rc == RTEMS_SUCCESSFUL) {
      cmdbuf[0] = FLASH_M25P40_CMD_RDSR;
      ret_cnt = rtems_libi2c_write_bytes(minor,cmdbuf,1);
      if (ret_cnt < 0) {
	rc = -ret_cnt;
      }
    }
  }
  else {
    /*
     * send read command and address
     */
    if (rc == RTEMS_SUCCESSFUL) {
      cmdbuf[0] = FLASH_M25P40_CMD_READ;
      cmdbuf[1] = (off >> 16) & 0xff;
      cmdbuf[2] = (off >>  8) & 0xff;
      cmdbuf[3] = (off >>  0) & 0xff;
      ret_cnt = rtems_libi2c_write_bytes(minor,cmdbuf,4);
      if (ret_cnt < 0) {
	rc = -ret_cnt;
      }
    }
  }
  /*
   * fetch read data 
   */
  if (rc == RTEMS_SUCCESSFUL) {
    ret_cnt = rtems_libi2c_read_bytes (minor,buf,cnt);
    if (ret_cnt < 0) {
      rc = -ret_cnt;
    }
  }
    
  /*
   * terminate transfer
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_libi2c_send_stop(minor);
  }
  rwargs->bytes_moved = (rc == RTEMS_SUCCESSFUL) ? ret_cnt : 0;

  return rc;
}

static rtems_driver_address_table spi_flash_m25p40_rw_ops = {
  read_entry:  spi_flash_m25p40_read,
  write_entry: spi_flash_m25p40_write
};

static rtems_libi2c_drv_t spi_flash_m25p40_rw_drv_tbl = {
  ops:         &spi_flash_m25p40_rw_ops,
  size:        sizeof (spi_flash_m25p40_rw_drv_tbl),
};

rtems_libi2c_drv_t *spi_flash_m25p40_rw_driver_descriptor = 
&spi_flash_m25p40_rw_drv_tbl;

static rtems_driver_address_table spi_flash_m25p40_ro_ops = {
  read_entry:  spi_flash_m25p40_read,
};

static rtems_libi2c_drv_t spi_flash_m25p40_ro_drv_tbl = {
  ops:         &spi_flash_m25p40_ro_ops,
  size:        sizeof (spi_flash_m25p40_ro_drv_tbl),
};

rtems_libi2c_drv_t *spi_flash_m25p40_ro_driver_descriptor = 
&spi_flash_m25p40_ro_drv_tbl;
