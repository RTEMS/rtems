/*===============================================================*\
| Project: RTEMS support for MPC83xx                              |
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
| this file contains the MPC83xx SPI driver                       |
| NOTE: it uses the same API as the I2C driver                    |
\*===============================================================*/
#include <stdlib.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <mpc83xx/mpc83xx.h>
#include <mpc83xx/mpc83xx_spidrv.h>
#include <rtems/error.h>
#include <rtems/bspIo.h>
#include <errno.h>
#include <rtems/libi2c.h>

#undef DEBUG

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code mpc83xx_spi_baud_to_mode
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   determine proper divider value                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 uint32_t baudrate,                      /* desired baudrate               */
 uint32_t base_frq,                      /* input frequency                */
 uint32_t *spimode                       /* result value                   */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  uint32_t divider;
  uint32_t tmpmode = 0;
  /*
   * determine clock divider and DIV16 bit
   */
  divider = (base_frq+baudrate-1)/baudrate;
  if (divider > 64) {
    tmpmode = MPC83XX_SPIMODE_DIV16;
    divider /= 16;
  }
  if ((divider <  1) ||
      (divider > 64)) {
    return RTEMS_INVALID_NUMBER;
  }
  else {
    tmpmode |= MPC83XX_SPIMODE_PM(divider/4-1);
  }
  *spimode = tmpmode;
  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code mpc83xx_spi_char_mode
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   determine proper value for character size                               |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 mpc83xx_spi_softc_t *softc_ptr,         /* handle                         */
 uint32_t bits_per_char,                 /* bits per character             */
 bool     lsb_first,                     /* TRUE: send LSB first           */
 uint32_t *spimode                       /* result value                   */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  uint32_t tmpmode;

  if (bits_per_char == 32) {
    tmpmode = 0;
    softc_ptr->bytes_per_char = 4;
    softc_ptr->bit_shift      = 0;
  }
  else {
    if (lsb_first) {
      /*
       * non-reversed data (LSB first): 4..16 bits valid
       * always aligned to bit 16 of data register
       */
      if ((bits_per_char >= 4) &&
	  (bits_per_char <= 16)) {
	tmpmode = MPC83XX_SPIMODE_LEN( bits_per_char-1);
	softc_ptr->bytes_per_char = (bits_per_char > 8) ? 2 : 1;
	softc_ptr->bit_shift      = 16-bits_per_char;
      }
      else {
	return RTEMS_INVALID_NUMBER;
      }
    }
    else {
      /*
       * reversed data (MSB first): only 8/16/32 bits valid,
       * always in lowest bits of data register
       */
      if (bits_per_char == 8) {
	tmpmode = MPC83XX_SPIMODE_LEN(8-1);
	softc_ptr->bytes_per_char = 1;
	softc_ptr->bit_shift      = 0;
      }
      else if (bits_per_char == 16) {
	tmpmode = MPC83XX_SPIMODE_LEN(16-1);
	softc_ptr->bytes_per_char = 2;
	softc_ptr->bit_shift      = 0;
      }
      else {
	return RTEMS_INVALID_NUMBER;
      }
    }
  }

  *spimode = tmpmode;
  return 0;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int mpc83xx_spi_wait
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   wait for spi to become idle                                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 mpc83xx_spi_softc_t *softc_ptr,          /* handle              */
 uint32_t             irq_mask,           /* irq mask to use     */
 uint32_t             desired_status,     /* desired status word */
 uint32_t             status_mask         /* status word mask    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  uint32_t act_status;
  rtems_status_code rc;
  uint32_t tout;

#if defined(DEBUG)
  printk("mpc83xx_spi_wait called... ");
#endif
  if (softc_ptr->initialized) {
    /*
     * allow interrupts, when receiver is not empty
     */
    softc_ptr->reg_ptr->spim = irq_mask;
    rc = rtems_semaphore_obtain(softc_ptr->irq_sema_id,RTEMS_WAIT,100);
    if (rc != RTEMS_SUCCESSFUL) {
      return rc;
    }
  }
  else {
    tout = 0;
    do {
      if (tout++ > 1000000) {
#if defined(DEBUG)
	printk("... exit with RTEMS_TIMEOUT\r\n");
#endif
	return RTEMS_TIMEOUT;
      }
      /*
       * wait for SPI to terminate
       */
    } while (!(softc_ptr->reg_ptr->spie & MPC83XX_SPIE_NE));
  }

  act_status = softc_ptr->reg_ptr->spie;
  if ((act_status  & status_mask)!= desired_status) {
#if defined(DEBUG)
    printk("... exit with RTEMS_IO_ERROR,"
	   "act_status=0x%04x,mask=0x%04x,desired_status=0x%04x\r\n",
	   act_status,status_mask,desired_status);
#endif
    return RTEMS_IO_ERROR;
  }
#if defined(DEBUG)
	printk("... exit OK\r\n");
#endif
  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void mpc83xx_spi_irq_handler
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle interrupts                                                       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_irq_hdl_param handle     /* handle, is softc_ptr structure          */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  mpc83xx_spi_softc_t *softc_ptr = (mpc83xx_spi_softc_t *)handle;

  /*
   * disable interrupt mask
   */
  softc_ptr->reg_ptr->spim = 0;
  if (softc_ptr->initialized) {
    rtems_semaphore_release(softc_ptr->irq_sema_id);
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void mpc83xx_spi_irq_on_off
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   enable/disable interrupts (void, handled at different position)         |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const
 rtems_irq_connect_data *irq_conn_data   /* irq connect data                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
}


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int mpc83xx_spi_irq_isOn
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   check state of interrupts, void, done differently                       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const
 rtems_irq_connect_data *irq_conn_data  /* irq connect data                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    TRUE, if enabled                                                       |
\*=========================================================================*/
{
  return (TRUE);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void mpc83xx_spi_install_irq_handler
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   (un-)install the interrupt handler                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 mpc83xx_spi_softc_t *softc_ptr,        /* ptr to control structure        */
 int install                            /* TRUE: install, FALSE: remove    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;

  rtems_irq_connect_data irq_conn_data = {
    softc_ptr->irq_number,
    mpc83xx_spi_irq_handler,        /* rtems_irq_hdl           */
    (rtems_irq_hdl_param)softc_ptr, /* (rtems_irq_hdl_param)   */
    mpc83xx_spi_irq_on_off,         /* (rtems_irq_enable)      */
    mpc83xx_spi_irq_on_off,         /* (rtems_irq_disable)     */
    mpc83xx_spi_irq_isOn            /* (rtems_irq_is_enabled)  */
  };

  /*
   * (un-)install handler for SPI device
   */
  if (install) {
    /*
     * create semaphore for IRQ synchronization
     */
    rc = rtems_semaphore_create(rtems_build_name('s','p','i','s'),
				0,
				RTEMS_FIFO
				| RTEMS_SIMPLE_BINARY_SEMAPHORE,
				0,
				&softc_ptr->irq_sema_id);
    if (rc != RTEMS_SUCCESSFUL) {
      rtems_panic("SPI: cannot create semaphore");
    }
    if (!BSP_install_rtems_irq_handler (&irq_conn_data)) {
      rtems_panic("SPI: cannot install IRQ handler");
    }
  }
  else {
    if (!BSP_remove_rtems_irq_handler (&irq_conn_data)) {
      rtems_panic("SPI: cannot uninstall IRQ handler");
    }
    /*
     * delete sync semaphore
     */
    if (softc_ptr->irq_sema_id != 0) {
      rc = rtems_semaphore_delete(softc_ptr->irq_sema_id);
      if (rc != RTEMS_SUCCESSFUL) {
	rtems_panic("SPI: cannot delete semaphore");
      }
    }
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code mpc83xx_spi_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the driver                                                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh                  /* bus specifier structure        */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  mpc83xx_spi_softc_t *softc_ptr = &(((mpc83xx_spi_desc_t *)(bh))->softc);
#if defined(DEBUG)
  printk("mpc83xx_spi_init called... ");
#endif
  /*
   * init HW registers:
   */
  /*
   * FIXME: set default mode in SPIM
   */

  /*
   * init interrupt stuff
   */
  mpc83xx_spi_install_irq_handler(softc_ptr,TRUE);

  /*
   * mark, that we have initialized
   */
  softc_ptr->initialized = TRUE;
#if defined(DEBUG)
  printk("... exit OK\r\n");
#endif
  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int mpc83xx_spi_read_write_bytes
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   transmit/receive some bytes from SPI device                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 unsigned char *rbuf,                    /* buffer to store bytes          */
 const unsigned char *tbuf,              /* buffer to send  bytes          */
 int len                                 /* number of bytes to transceive  */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    number of bytes received or (negative) error code                      |
\*=========================================================================*/
{
  mpc83xx_spi_softc_t *softc_ptr = &(((mpc83xx_spi_desc_t *)(bh))->softc);
  rtems_status_code rc;
  int bc = 0;
  int bytes_per_char = softc_ptr->bytes_per_char;
  int bit_shift      = softc_ptr->bit_shift;
  uint32_t spird_val;

#if defined(DEBUG)
  printk("mpc83xx_spi_read_write_bytes called... ");
#endif

  while (len > bytes_per_char-1) {
    len -= bytes_per_char;
    /*
     * mark last byte in SPCOM
     */
#if defined(USE_LAST_BIT)
    softc_ptr->reg_ptr->spcom = (len < bytes_per_char) ? MPC83XX_SPCOM_LST : 0;
#else
    softc_ptr->reg_ptr->spcom = 0;
#endif
    if (tbuf == NULL) {
      /*
       * perform idle char write to read byte
       */
      softc_ptr->reg_ptr->spitd = softc_ptr->idle_char << bit_shift;
    }
    else {
      switch(bytes_per_char) {
      case 1:
	softc_ptr->reg_ptr->spitd = (*(uint8_t *)tbuf) << bit_shift;
	break;
      case 2:
	softc_ptr->reg_ptr->spitd = (*(uint16_t *)tbuf) << bit_shift;
	break;
      case 4:
	softc_ptr->reg_ptr->spitd = (*(uint32_t *)tbuf) << bit_shift;
	break;
      }
      tbuf += softc_ptr->bytes_per_char;
    }
    /*
     * wait 'til end of transfer
     */
#if defined(USE_LAST_BIT)
    rc = mpc83xx_spi_wait(softc_ptr,
			  ((len == 0)
			   ? MPC83XX_SPIE_LT
			   : MPC83XX_SPIE_NE),
			  ((len == 0)
			   ? MPC83XX_SPIE_LT
			   : MPC83XX_SPIE_NF)
			  | MPC83XX_SPIE_NE,
			  MPC83XX_SPIE_LT
			    | MPC83XX_SPIE_OV
			  | MPC83XX_SPIE_UN
			  | MPC83XX_SPIE_NE
			  | MPC83XX_SPIE_NF);
    if (len == 0) {
      /*
       * clear the "last transfer complete" event
       */
      softc_ptr->reg_ptr->spie = MPC83XX_SPIE_LT;
    }
#else
    rc = mpc83xx_spi_wait(softc_ptr,
			  MPC83XX_SPIE_NE,
			  MPC83XX_SPIE_NF
			  | MPC83XX_SPIE_NE,
			  MPC83XX_SPIE_OV
			  | MPC83XX_SPIE_UN
			  | MPC83XX_SPIE_NE
			  | MPC83XX_SPIE_NF);
#endif
    if (rc != RTEMS_SUCCESSFUL) {
#if defined(DEBUG)
      printk("... exit rc=%d\r\n",-rc);
#endif
      return -rc;
    }
    spird_val = softc_ptr->reg_ptr->spird;
    if (rbuf != NULL) {
      switch(bytes_per_char) {
      case 1:
	 (*(uint8_t  *)rbuf) = spird_val >> bit_shift;
	break;
      case 2:
	 (*(uint16_t *)rbuf) = spird_val >> bit_shift;
	break;
      case 4:
	 (*(uint32_t *)rbuf) = spird_val >> bit_shift;
	break;
      }
      rbuf += bytes_per_char;
    }
    bc += bytes_per_char;
  }
#if defined(DEBUG)
  printk("... exit OK, rc=%d\r\n",bc);
#endif
  return bc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int mpc83xx_spi_read_bytes
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   receive some bytes from SPI device                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 unsigned char *buf,                     /* buffer to store bytes          */
 int len                                 /* number of bytes to receive     */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    number of bytes received or (negative) error code                      |
\*=========================================================================*/
{
  return mpc83xx_spi_read_write_bytes(bh,buf,NULL,len);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int mpc83xx_spi_write_bytes
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   send some bytes to SPI device                                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 unsigned char *buf,                     /* buffer to send                 */
 int len                                 /* number of bytes to send        */

)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    number of bytes sent or (negative) error code                          |
\*=========================================================================*/
{
  return mpc83xx_spi_read_write_bytes(bh,NULL,buf,len);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code mpc83xx_spi_set_tfr_mode
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   set SPI to desired baudrate/clock mode/character mode                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 const rtems_libi2c_tfr_mode_t *tfr_mode /* transfer mode info             */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  mpc83xx_spi_softc_t *softc_ptr = &(((mpc83xx_spi_desc_t *)(bh))->softc);
  uint32_t spimode_baud,spimode;
  rtems_status_code rc = RTEMS_SUCCESSFUL;

  /* Set idle character */
  softc_ptr->idle_char = tfr_mode->idle_char;

  /*
   * FIXME: set proper mode
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = mpc83xx_spi_baud_to_mode(tfr_mode->baudrate,
				  softc_ptr->base_frq,
				  &spimode_baud);
  }
  if (rc == RTEMS_SUCCESSFUL) {
    rc = mpc83xx_spi_char_mode(softc_ptr,
			       tfr_mode->bits_per_char,
			       tfr_mode->lsb_first,
			       &spimode);
  }
  if (rc == RTEMS_SUCCESSFUL) {
    spimode |= spimode_baud;
    spimode |= MPC83XX_SPIMODE_M_S; /* set master mode */
    if (!tfr_mode->lsb_first) {
      spimode |= MPC83XX_SPIMODE_REV;
    }
    if (tfr_mode->clock_inv) {
      spimode |= MPC83XX_SPIMODE_CI;
    }
    if (tfr_mode->clock_phs) {
      spimode |= MPC83XX_SPIMODE_CP;
    }
  }

  if (rc == RTEMS_SUCCESSFUL) {
    /*
     * disable SPI
     */
    softc_ptr->reg_ptr->spmode &= ~MPC83XX_SPIMODE_EN;
    /*
     * set new mode and reenable SPI
     */
    softc_ptr->reg_ptr->spmode = spimode | MPC83XX_SPIMODE_EN;
  }
  return rc;
}


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int mpc83xx_spi_ioctl
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   perform selected ioctl function for SPI                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 int                 cmd,                /* ioctl command code             */
 void               *arg                 /* additional argument array      */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  int ret_val = -1;

  switch(cmd) {
  case RTEMS_LIBI2C_IOCTL_SET_TFRMODE:
    ret_val =
      -mpc83xx_spi_set_tfr_mode(bh,
				(const rtems_libi2c_tfr_mode_t *)arg);
    break;
  case RTEMS_LIBI2C_IOCTL_READ_WRITE:
    ret_val =
      mpc83xx_spi_read_write_bytes(bh,
				   ((rtems_libi2c_read_write_t *)arg)->rd_buf,
				   ((rtems_libi2c_read_write_t *)arg)->wr_buf,
				   ((rtems_libi2c_read_write_t *)arg)->byte_cnt);
    break;
  default:
    ret_val = -RTEMS_NOT_DEFINED;
    break;
  }
  return ret_val;
}



