/*===============================================================*\
| Project: SPI driver for spi memory devices                      |
+-----------------------------------------------------------------+
|                    Copyright (c) 2008                           |
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
 * FIXME: currently, this driver only supports read/write accesses
 * erase accesses are to be completed
 */


#include <rtems.h>
#include <rtems/libi2c.h>

#include <libchip/spi-memdrv.h>
#include <rtems/libio.h>

#define SPI_MEM_CMD_WREN  0x06
#define SPI_MEM_CMD_WRDIS 0x04
#define SPI_MEM_CMD_RDID  0x9F
#define SPI_MEM_CMD_RDSR  0x05
#define SPI_MEM_CMD_WRSR  0x01
#define SPI_MEM_CMD_READ  0x03
#define SPI_MEM_CMD_PP    0x02  /* page program                       */
#define SPI_MEM_CMD_SE    0xD8  /* sector erase                       */
#define SPI_MEM_CMD_BE    0xC7  /* bulk erase                         */
#define SPI_MEM_CMD_DP    0xB9  /* deep power down                    */
#define SPI_MEM_CMD_RES   0xAB  /* release from deep power down       */

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code spi_memdrv_minor2param_ptr
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   translate given minor device number to param pointer                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_device_minor_number minor,                /* minor number of device */
 spi_memdrv_param_t **param_ptr                  /* ptr to param ptr       */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  rtems_status_code   rc = RTEMS_SUCCESSFUL;
  spi_memdrv_t *drv_ptr;

  if (rc == RTEMS_SUCCESSFUL) {
    rc = -rtems_libi2c_ioctl(minor,
			     RTEMS_LIBI2C_IOCTL_GET_DRV_T,
			     &drv_ptr);
  }
  if ((rc == RTEMS_SUCCESSFUL) &&
      (drv_ptr->libi2c_drv_entry.size != sizeof(spi_memdrv_t))) {
    rc = RTEMS_INVALID_SIZE;
  }
  if (rc == RTEMS_SUCCESSFUL) {
    *param_ptr = &(drv_ptr->spi_memdrv_param);
  }
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code spi_memdrv_wait_ms
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
rtems_status_code spi_memdrv_write
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   write a block of data to flash                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_device_major_number major,        /* major device number            */
 rtems_device_minor_number minor,        /* minor device number            */
 void                      *arg          /* ptr to write argument struct   */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  rtems_status_code          rc = RTEMS_SUCCESSFUL;
  rtems_libio_rw_args_t *rwargs = arg;
  off_t                     off = rwargs->offset;
  int                       cnt = rwargs->count;
  unsigned char            *buf = (unsigned char *)rwargs->buffer;
  int                bytes_sent = 0;
  int                  curr_cnt;
  unsigned char       cmdbuf[4];
  int                   ret_cnt = 0;
  int                  cmd_size;
  spi_memdrv_param_t  *mem_param_ptr;
  rtems_libi2c_tfr_mode_t tfr_mode = {
    .baudrate =      20000000, /* maximum bits per second                   */
    .bits_per_char = 8,        /* how many bits per byte/word/longword?     */
    .lsb_first =     FALSE,    /* FALSE: send MSB first                     */
    .clock_inv =     FALSE,    /* FALSE: non-inverted clock (high active)   */
    .clock_phs =     FALSE     /* FALSE: clock starts in middle of data tfr */
  } ;

  /*
   * get mem parameters
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = spi_memdrv_minor2param_ptr(minor,&mem_param_ptr);
  }
  /*
   * check arguments
   */
  if (rc == RTEMS_SUCCESSFUL) {
    if ((cnt <= 0)                      ||
	(cnt > mem_param_ptr->mem_size) ||
	(off > (mem_param_ptr->mem_size-cnt))) {
      rc = RTEMS_INVALID_SIZE;
    }
    else if (buf == NULL) {
      rc = RTEMS_INVALID_ADDRESS;
    }
  }
  while ((rc == RTEMS_SUCCESSFUL) &&
	 (cnt > bytes_sent)) {
    curr_cnt = cnt - bytes_sent;
    if ((mem_param_ptr->page_size > 0) &&
	(off              / mem_param_ptr->page_size) !=
	((off+curr_cnt+1) / mem_param_ptr->page_size)) {
      curr_cnt = mem_param_ptr->page_size - (off % mem_param_ptr->page_size);
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
      tfr_mode.baudrate = mem_param_ptr->baudrate;
      rc = -rtems_libi2c_ioctl(minor,
			       RTEMS_LIBI2C_IOCTL_SET_TFRMODE,
			       &tfr_mode);
    }

    /*
     * address device
     */
    if (rc == RTEMS_SUCCESSFUL) {
      rc = rtems_libi2c_send_addr(minor,TRUE);
    }

    /*
     * send write_enable command
     */
    if (rc == RTEMS_SUCCESSFUL) {
      cmdbuf[0] = SPI_MEM_CMD_WREN;
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
    /*
     * select device, set transfer mode
     */
    if (rc == RTEMS_SUCCESSFUL) {
      rc = rtems_libi2c_send_start(minor);
    }

    /*
     * address device
     */
    if (rc == RTEMS_SUCCESSFUL) {
      rc = rtems_libi2c_send_addr(minor,TRUE);
    }

    /*
     * set transfer mode
     */
    if (rc == RTEMS_SUCCESSFUL) {
      rc = -rtems_libi2c_ioctl(minor,
			       RTEMS_LIBI2C_IOCTL_SET_TFRMODE,
			       &tfr_mode);
    }
    /*
     * send "page program" command and address
     */
    if (rc == RTEMS_SUCCESSFUL) {
      cmdbuf[0] = SPI_MEM_CMD_PP;
      if (mem_param_ptr->mem_size > 0x10000 /* 256*256 */) {
	cmdbuf[1] = (off >> 16) & 0xff;
	cmdbuf[2] = (off >>  8) & 0xff;
	cmdbuf[3] = (off >>  0) & 0xff;
	cmd_size  = 4;
      }
      else if (mem_param_ptr->mem_size > 256) {
	cmdbuf[1] = (off >>  8) & 0xff;
	cmdbuf[2] = (off >>  0) & 0xff;
	cmd_size  = 3;
      }
      else {
	cmdbuf[1] = (off >>  0) & 0xff;
	cmd_size  = 1;
      }

      ret_cnt = rtems_libi2c_write_bytes(minor,cmdbuf,cmd_size);
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
     * FIXME: select proper interval or poll, until device is finished
     */
    if (rc == RTEMS_SUCCESSFUL) {
      rc = spi_memdrv_wait_ms(5);
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
rtems_status_code spi_memdrv_read
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   read a block of data from flash                                         |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_device_major_number major,        /* major device number            */
 rtems_device_minor_number minor,        /* minor device number            */
 void                      *arg          /* ptr to read argument struct    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  rtems_libio_rw_args_t *rwargs = arg;
  off_t                     off = rwargs->offset;
  int                       cnt = rwargs->count;
  unsigned char            *buf = (unsigned char *)rwargs->buffer;
  unsigned char         cmdbuf[4];
  int                   ret_cnt = 0;
  int                  cmd_size;
  spi_memdrv_param_t  *mem_param_ptr;
  rtems_libi2c_tfr_mode_t tfr_mode = {
    .baudrate =      20000000, /* maximum bits per second                   */
    .bits_per_char = 8,        /* how many bits per byte/word/longword?     */
    .lsb_first =     FALSE,    /* FALSE: send MSB first                     */
    .clock_inv =     FALSE,    /* FALSE: non-inverted clock (high active)   */
    .clock_phs =     FALSE     /* FALSE: clock starts in middle of data tfr */
  };

  /*
   * get mem parameters
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = spi_memdrv_minor2param_ptr(minor,&mem_param_ptr);
  }
  /*
   * check arguments
   */
  if (rc == RTEMS_SUCCESSFUL) {
    if ((cnt <= 0)                      ||
	(cnt > mem_param_ptr->mem_size) ||
	(off > (mem_param_ptr->mem_size-cnt))) {
      rc = RTEMS_INVALID_SIZE;
    }
    else if (buf == NULL) {
      rc = RTEMS_INVALID_ADDRESS;
    }
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
    tfr_mode.baudrate = mem_param_ptr->baudrate;
    rc = -rtems_libi2c_ioctl(minor,
			     RTEMS_LIBI2C_IOCTL_SET_TFRMODE,
			     &tfr_mode);
  }
  /*
   * address device
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_libi2c_send_addr(minor,TRUE);
  }

  if (off >= mem_param_ptr->mem_size) {
    /*
     * HACK: beyond size of memory array? then read status register instead
     */
    /*
     * send read status register command
     */
    if (rc == RTEMS_SUCCESSFUL) {
      cmdbuf[0] = SPI_MEM_CMD_RDSR;
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
      cmdbuf[0] = SPI_MEM_CMD_READ;
      if (mem_param_ptr->mem_size > 0x10000 /* 256*256 */) {
	cmdbuf[1] = (off >> 16) & 0xff;
	cmdbuf[2] = (off >>  8) & 0xff;
	cmdbuf[3] = (off >>  0) & 0xff;
	cmd_size  = 4;
      }
      else if (mem_param_ptr->mem_size > 256) {
	cmdbuf[1] = (off >>  8) & 0xff;
	cmdbuf[2] = (off >>  0) & 0xff;
	cmd_size  = 3;
      }
      else {
	cmdbuf[1] = (off >>  0) & 0xff;
	cmd_size  = 1;
      }
      ret_cnt = rtems_libi2c_write_bytes(minor,cmdbuf,cmd_size);
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

/*
 * driver operation tables
 */
rtems_driver_address_table spi_memdrv_rw_ops = {
  .read_entry =  spi_memdrv_read,
  .write_entry = spi_memdrv_write
};

rtems_driver_address_table spi_memdrv_ro_ops = {
  .read_entry =  spi_memdrv_read,
};

