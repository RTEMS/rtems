/*===============================================================*\
| Project: RTEMS support for PGH360                               |
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
| this file contains the M360 SPI driver                          |
\*===============================================================*/
#include <stdlib.h>
#include <bsp.h>
#include <rtems/m68k/m68360.h>
#include <rtems/m68k/m360_spi.h>
#include <rtems/error.h>
#include <rtems/bspIo.h>
#include <errno.h>
#include <rtems/libi2c.h>

#undef DEBUG
static m360_spi_softc_t *m360_spi_softc_ptr;
/*
 * this is a dummy receive buffer for sequences,
 * where only send data are available
 */
uint8_t m360_spi_dummy_rxbuf[2];
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code m360_spi_baud_to_mode
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   determine proper divider value                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 uint32_t baudrate,                      /* desired baudrate               */
 uint32_t *spimode                       /* result value                   */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  uint32_t divider;
  uint16_t tmpmode = 0;
  /*
   * determine clock divider and DIV16 bit
   */
  divider = m360_clock_rate/baudrate;
  if (divider > 64) {
    tmpmode = M360_SPMODE_DIV16;
    divider /= 16;
  }
  if ((divider <  1) ||
      (divider > 64)) {
    return RTEMS_INVALID_NUMBER;
  }
  else {
    tmpmode |= M360_SPMODE_PM(divider/4-1);
  }
  *spimode = tmpmode;
  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code m360_spi_char_mode
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   determine proper value for character size                               |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 m360_spi_softc_t *softc_ptr,            /* handle                         */
 uint32_t bits_per_char,                 /* bits per character             */
 bool     lsb_first,                     /* TRUE: send LSB first           */
 uint16_t *spimode                       /* result value                   */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  uint16_t tmpmode;

  /*
   * calculate data format
   */
  if ((bits_per_char >= 4) &&
      (bits_per_char <= 16)) {
    tmpmode = M360_SPMODE_CLEN( bits_per_char-1);
  }
  else {
    return RTEMS_INVALID_NUMBER;
  }

  *spimode = tmpmode;
  return 0;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int m360_spi_wait
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   wait for spi to become idle                                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 m360_spi_softc_t    *softc_ptr           /* handle              */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  uint16_t act_status;
  rtems_status_code rc;
  uint32_t tout;

#if defined(DEBUG)
  printk("m360_spi_wait called... ");
#endif
  if (softc_ptr->initialized) {
    /*
     * allow interrupts, when receiver is not empty
     */
    m360.spim = (M360_SPIE_TXE | M360_SPIE_TXB |
		 M360_SPIE_BSY | M360_SPIE_MME);

    rc = rtems_semaphore_obtain(softc_ptr->irq_sema_id,
				RTEMS_WAIT,
				RTEMS_NO_TIMEOUT);
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
    } while (!(m360.spie & M360_SPIE_TXB));
  }

  act_status = m360.spie;
  if ((act_status  & (M360_SPIE_TXE | M360_SPIE_TXB |
		      M360_SPIE_BSY | M360_SPIE_MME))!= M360_SPIE_TXB) {
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
static rtems_isr m360_spi_irq_handler
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle interrupts                                                       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_vector_number v                          /* vector number           */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  m360_spi_softc_t *softc_ptr = m360_spi_softc_ptr;

  /*
   * disable interrupt mask
   */
  m360.spim = 0;
  if (softc_ptr->initialized) {
    rtems_semaphore_release(softc_ptr->irq_sema_id);
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void m360_spi_install_irq_handler
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   (un-)install the interrupt handler                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 m360_spi_softc_t *softc_ptr,           /* ptr to control structure        */
 int install                            /* TRUE: install, FALSE: remove    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;

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
    if (rc == RTEMS_SUCCESSFUL) {
      rc = rtems_interrupt_catch (m360_spi_irq_handler,
				  (m360.cicr & 0xE0) | 0x05,
				  &softc_ptr->old_handler);
      if (rc != RTEMS_SUCCESSFUL) {
	rtems_panic("SPI: cannot install IRQ handler");
      }
    }
    /*
     * enable IRQ in CPIC
     */
    if (rc == RTEMS_SUCCESSFUL) {
      m360.cimr |= (1 << 5);
    }
  }
  else {
    rtems_isr_entry old_handler;
    /*
     * disable IRQ in CPIC
     */
    if (rc == RTEMS_SUCCESSFUL) {
      m360.cimr &= ~(1 << 5);
    }
    rc = rtems_interrupt_catch (softc_ptr->old_handler,
				(m360.cicr & 0xE0) | 0x05,
				&old_handler);
    if (rc != RTEMS_SUCCESSFUL) {
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
rtems_status_code m360_spi_init
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
  m360_spi_softc_t *softc_ptr = &(((m360_spi_desc_t *)(bh))->softc);
  rtems_status_code rc = RTEMS_SUCCESSFUL;

#if defined(DEBUG)
  printk("m360_spi_init called... ");
#endif
  /*
   * init HW registers:
   */
  /*
   * FIXME: set default mode in SPMODE
   */

  /*
   * allocate BDs (1x RX, 1x TX)
   */
  if (rc == RTEMS_SUCCESSFUL) {
    softc_ptr->rx_bd = M360AllocateBufferDescriptors (1);
    softc_ptr->tx_bd = M360AllocateBufferDescriptors (1);
    if ((softc_ptr->rx_bd == NULL) ||
	(softc_ptr->tx_bd == NULL)) {
      rc = RTEMS_NO_MEMORY;
    }
  }
  /*
   * set parameter RAM
   */
  m360.spip.rbase = (char *)softc_ptr->rx_bd - (char *)&m360;
  m360.spip.tbase = (char *)softc_ptr->tx_bd - (char *)&m360;
  m360.spip.rfcr  = M360_RFCR_MOT | M360_RFCR_DMA_SPACE;
  m360.spip.tfcr  = M360_RFCR_MOT | M360_RFCR_DMA_SPACE;
  m360.spip.mrblr = 2;

  /*
   * issue "InitRxTx" Command to CP
   */
  M360ExecuteRISC (M360_CR_OP_INIT_RX_TX | M360_CR_CHAN_SPI);

  /*
   * init interrupt stuff
   */
  if (rc == RTEMS_SUCCESSFUL) {
    m360_spi_install_irq_handler(softc_ptr,TRUE);
  }
  if (rc == RTEMS_SUCCESSFUL) {
    /*
     * set up ports
     * LINE      PAR  DIR  DAT
     * -----------------------
     * MOSI       1    1    x
     * MISO       1    1    x
     * CLK        1    1    x
     */

    /* set Port B Pin Assignment Register... */
    m360.pbpar =
      m360.pbpar
      | M360_PB_SPI_MISO_MSK
      | M360_PB_SPI_MOSI_MSK
      | M360_PB_SPI_CLK_MSK;

    /* set Port B Data Direction Register... */
    m360.pbdir =
      m360.pbdir
      | M360_PB_SPI_MISO_MSK
      | M360_PB_SPI_MOSI_MSK
      | M360_PB_SPI_CLK_MSK;
  }
  /*
   * mark, that we have initialized
   */
  if (rc == RTEMS_SUCCESSFUL) {
    softc_ptr->initialized = TRUE;
  }
#if defined(DEBUG)
  printk("... exit OK\r\n");
#endif
  return rc;
}


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int m360_spi_read_write_bytes
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
  m360_spi_softc_t *softc_ptr = &(((m360_spi_desc_t *)(bh))->softc);
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  int bc = 0;

#if defined(DEBUG)
  printk("m360_spi_read_write_bytes called... ");
#endif

  /*
   * prepare RxBD
   */
  if (rc == RTEMS_SUCCESSFUL) {
    if (rbuf == NULL) {
      /*
       * no Tx buffer: receive to dummy buffer
       */
      m360.spip.mrblr          = sizeof(m360_spi_dummy_rxbuf);
      softc_ptr->rx_bd->buffer = m360_spi_dummy_rxbuf;
      softc_ptr->rx_bd->length = 0;
      softc_ptr->rx_bd->status = (M360_BD_EMPTY | M360_BD_WRAP |
				  M360_BD_CONTINUOUS);
    }
    else {
      m360.spip.mrblr          = len;
      softc_ptr->rx_bd->buffer = rbuf;
      softc_ptr->rx_bd->length = 0;
      softc_ptr->rx_bd->status = (M360_BD_EMPTY | M360_BD_WRAP);
    }
  }
  /*
   * prepare TxBD
   */
  if (rc == RTEMS_SUCCESSFUL) {
    if (tbuf == NULL) {
      /*
       * FIXME: no Tx buffer: transmit from dummy buffer
       */
      softc_ptr->tx_bd->buffer = m360_spi_dummy_rxbuf;
      softc_ptr->tx_bd->length = len;
      softc_ptr->tx_bd->status = (M360_BD_READY | M360_BD_WRAP |
				  M360_BD_CONTINUOUS);
    }
    else {
      softc_ptr->tx_bd->buffer = tbuf;
      softc_ptr->tx_bd->length = len;
      softc_ptr->tx_bd->status = (M360_BD_READY | M360_BD_WRAP);
    }
  }

  if (rc == RTEMS_SUCCESSFUL) {
    /*
     * set START command
     */
    m360.spcom = M360_SPCOM_STR;
    /*
     * wait for SPI to finish
     */
    rc = m360_spi_wait(softc_ptr);
  }
#if defined(DEBUG)
  printk("... exit OK, rc=%d\r\n",bc);
#endif
  return (rc == RTEMS_SUCCESSFUL) ? bc : -rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int m360_spi_read_bytes
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
  return m360_spi_read_write_bytes(bh,buf,NULL,len);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int m360_spi_write_bytes
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
  return m360_spi_read_write_bytes(bh,NULL,buf,len);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code m360_spi_set_tfr_mode
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
  m360_spi_softc_t *softc_ptr = &(((m360_spi_desc_t *)(bh))->softc);
  uint32_t spimode_baud,spimode;
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  /*
   * FIXME: set proper mode
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = m360_spi_baud_to_mode(tfr_mode->baudrate,&spimode_baud);
  }
  if (rc == RTEMS_SUCCESSFUL) {
    rc = m360_spi_char_mode(softc_ptr,
			       tfr_mode->bits_per_char,
			       tfr_mode->lsb_first,
			       &spimode);
  }
  if (rc == RTEMS_SUCCESSFUL) {
    spimode |= spimode_baud;
    spimode |= M360_SPMODE_MASTER; /* set master mode */
    if (!tfr_mode->lsb_first) {
      spimode |= M360_SPMODE_REV;
    }
    if (tfr_mode->clock_inv) {
      spimode |= M360_SPMODE_CI;
    }
    if (tfr_mode->clock_phs) {
      spimode |= M360_SPMODE_CP;
    }
  }

  if (rc == RTEMS_SUCCESSFUL) {
    /*
     * disable SPI
     */
    m360.spmode &= ~M360_SPMODE_EN;
    /*
     * set new mode and reenable SPI
     */
    m360.spmode = spimode | M360_SPMODE_EN;
  }
  return rc;
}


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int m360_spi_ioctl
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
      -m360_spi_set_tfr_mode(bh,
				(const rtems_libi2c_tfr_mode_t *)arg);
    break;
  case RTEMS_LIBI2C_IOCTL_READ_WRITE:
    ret_val =
      m360_spi_read_write_bytes(bh,
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

/*=========================================================================*\
| Board-specific adaptation functions                                       |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code bsp_spi_sel_addr
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   address a slave device on the bus                                       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 uint32_t addr,                          /* address to send on bus         */
 int rw                                  /* 0=write,1=read                 */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
#if defined(PGH360)
  /*
   * select given device
   */
  /*
   * GPIO1[24] is SPI_A0
   * GPIO1[25] is SPI_A1
   * GPIO1[26] is SPI_A2
   * set pins to address
   */
  switch(addr) {
  case PGH360_SPI_ADDR_EEPROM:
    m360.pbdat &= ~PGH360_PB_SPI_EEP_CE_MSK;
    break;
  case PGH360_SPI_ADDR_DISP4_DATA:
    m360.pbdat = (m360.pbdat
		  & ~(PGH360_PB_SPI_DISP4_CE_MSK |
		      PGH360_PB_SPI_DISP4_RS_MSK));
    break;
  case PGH360_SPI_ADDR_DISP4_CTRL:
    m360.pbdat = (m360.pbdat
		  & ~(PGH360_PB_SPI_DISP4_CE_MSK)
		  |   PGH360_PB_SPI_DISP4_RS_MSK);
    break;
  default:
    return RTEMS_INVALID_NUMBER;
  }
#endif /* PGH360 */
  return  RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code bsp_spi_send_start_dummy
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   dummy function, SPI has no start condition                              |
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
  return 0;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code bsp_spi_send_stop
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   deselect SPI                                                            |
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
#if defined(DEBUG)
  printk("bsp_spi_send_stop called... ");
#endif
#if defined(PGH360)
    m360.pbdat = (m360.pbdat
		  | PGH360_PB_SPI_DISP4_CE_MSK
		  | PGH360_PB_SPI_EEP_CE_MSK);
#endif
#if defined(DEBUG)
  printk("... exit OK\r\n");
#endif
  return 0;
}

/*=========================================================================*\
| list of handlers                                                          |
\*=========================================================================*/

rtems_libi2c_bus_ops_t bsp_spi_ops = {
  init:             m360_spi_init,
  send_start:       bsp_spi_send_start_dummy,
  send_stop:        bsp_spi_send_stop,
  send_addr:        bsp_spi_sel_addr,
  read_bytes:       m360_spi_read_bytes,
  write_bytes:      m360_spi_write_bytes,
  ioctl:            m360_spi_ioctl
};

static m360_spi_desc_t bsp_spi_bus_desc = {
  {/* public fields */
    ops:	&bsp_spi_ops,
    size:	sizeof(bsp_spi_bus_desc)
  },
  { /* our private fields */
    initialized: FALSE
  }
};

/*=========================================================================*\
| initialization                                                            |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code bsp_register_spi
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   register SPI bus and devices                                            |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void                                    /* <none>                         */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0 or error code                                                        |
\*=========================================================================*/
{
  int ret_code;
  int spi_busno;

  /*
   * init I2C library (if not already done)
   */
  rtems_libi2c_initialize ();

  /*
   * init port pins used to address/select SPI devices
   */

#if defined(PGH360)

  /*
   * set up ports
   * LINE      PAR  DIR  DAT
   * -----------------------
   * EEP_CE     0    1 act-high
   * DISP4_CS   0    1 act-high
   * DISP4_RS   0    1 active
   */

  /* set Port B Pin Assignment Register... */
  m360.pbpar =
    (m360.pbpar
     & ~(PGH360_PB_SPI_EEP_CE_MSK
	 | PGH360_PB_SPI_DISP4_CE_MSK
	 | PGH360_PB_SPI_DISP4_RS_MSK));

    /* set Port B Data Direction Register... */
  m360.pbdir =
    m360.pbdir
    | PGH360_PB_SPI_EEP_CE_MSK
    | PGH360_PB_SPI_DISP4_CE_MSK
    | PGH360_PB_SPI_DISP4_RS_MSK;

  /* set Port B Data Register to inactive CE state */
  m360.pbdat =
    m360.pbdat
    | PGH360_PB_SPI_DISP4_CE_MSK
    | PGH360_PB_SPI_DISP4_RS_MSK;
#endif

  /*
   * register SPI bus
   */
  ret_code = rtems_libi2c_register_bus("/dev/spi",
				       &(bsp_spi_bus_desc.bus_desc));
  if (ret_code < 0) {
    return -ret_code;
  }
  spi_busno = ret_code;
#if defined(PGH360)
  /*
   * register devices
   */
#if 0
  ret_code = rtems_libi2c_register_drv(RTEMS_BSP_SPI_FLASH_DEVICE_NAME,
				       spi_flash_m25p40_rw_driver_descriptor,
				       spi_busno,0x00);
  if (ret_code < 0) {
    return -ret_code;
  }
#endif
#endif /* defined(PGH360) */
  /*
   * FIXME: further drivers, when available
   */
  return 0;
}


