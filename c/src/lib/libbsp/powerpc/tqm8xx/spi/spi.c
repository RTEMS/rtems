/*===============================================================*\
| Project: RTEMS support for tqm8xx                               |
+-----------------------------------------------------------------+
|                    Copyright (c) 2009                           |
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
| this file contains the MPC8xx SPI driver                        |
\*===============================================================*/
#include <stdlib.h>
#include <bsp.h>
#include <mpc8xx.h>
#include <bsp/spi.h>
#include <libchip/disp_hcms29xx.h>
#include <rtems/error.h>
#include <rtems/bspIo.h>
#include <errno.h>
#include <rtems/libi2c.h>
#include <bsp/irq.h>

#define M8xx_PB_SPI_MISO_MSK       (1<<(31-28))
#define M8xx_PB_SPI_MOSI_MSK       (1<<(31-29))
#define M8xx_PB_SPI_CLK_MSK        (1<<(31-30))

#undef DEBUG

/*
 * this is a dummy receive buffer for sequences,
 * where only send data are available
 */
uint8_t m8xx_spi_dummy_rxbuf[2];
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code m8xx_spi_baud_to_mode
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
  divider = BSP_bus_frequency/baudrate;
  if (divider > 64) {
    tmpmode = M8xx_SPMODE_DIV16;
    divider /= 16;
  }
  if ((divider <  1) ||
      (divider > 64)) {
    return RTEMS_INVALID_NUMBER;
  }
  else {
    tmpmode |= M8xx_SPMODE_PM(divider/4-1);
  }
  *spimode = tmpmode;
  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code m8xx_spi_char_mode
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   determine proper value for character size                               |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 m8xx_spi_softc_t *softc_ptr,            /* handle                         */
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
    tmpmode = M8xx_SPMODE_CLEN( bits_per_char-1);
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
static int m8xx_spi_wait
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   wait for spi to become idle                                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 m8xx_spi_softc_t    *softc_ptr           /* handle              */
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
  printk("m8xx_spi_wait called... ");
#endif
  if (softc_ptr->initialized) {
    /*
     * allow interrupts, when receiver is not empty
     */
    m8xx.spim = (M8xx_SPIE_TXE | M8xx_SPIE_TXB |
		 M8xx_SPIE_BSY | M8xx_SPIE_MME);

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
    } while (!(m8xx.spie & M8xx_SPIE_TXB));
  }

  act_status = m8xx.spie;
  if ((act_status  & (M8xx_SPIE_TXE | M8xx_SPIE_TXB |
		      M8xx_SPIE_BSY | M8xx_SPIE_MME))!= M8xx_SPIE_TXB) {
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
static rtems_isr m8xx_spi_irq_handler
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle interrupts                                                       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void *arg
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  m8xx_spi_softc_t *softc_ptr = arg;

  /*
   * disable interrupt mask
   */
  m8xx.spim = 0;
  if (softc_ptr->initialized) {
    rtems_semaphore_release(softc_ptr->irq_sema_id);
  }
}

static void
mpc8xx_spi_irq_on(const rtems_irq_connect_data *irq)
{
}

static void
mpc8xx_spi_irq_off(const rtems_irq_connect_data *irq)
{
}

static int
mpc8xx_spi_irq_isOn(const rtems_irq_connect_data *irq)
{
  return 1;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void m8xx_spi_install_irq_handler
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   install the interrupt handler                                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 m8xx_spi_softc_t *softc_ptr,           /* ptr to control structure        */
 int install                            /* TRUE: install, FALSE: remove    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;

  /*
   * install handler for SPI device
   */
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
    rtems_irq_connect_data irq_conn_data = {
      BSP_CPM_IRQ_SPI,
      m8xx_spi_irq_handler,            /* rtems_irq_hdl           */
      (rtems_irq_hdl_param)softc_ptr,  /* (rtems_irq_hdl_param)   */
      mpc8xx_spi_irq_on,               /* (rtems_irq_enable)      */
      mpc8xx_spi_irq_off,              /* (rtems_irq_disable)     */
      mpc8xx_spi_irq_isOn              /* (rtems_irq_is_enabled)  */
    };
    if (!BSP_install_rtems_irq_handler (&irq_conn_data)) {
      rtems_panic("SPI: cannot install IRQ handler");
    }
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code m8xx_spi_init
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
  m8xx_spi_softc_t *softc_ptr = &(((m8xx_spi_desc_t *)(bh))->softc);
  rtems_status_code rc = RTEMS_SUCCESSFUL;

#if defined(DEBUG)
  printk("m8xx_spi_init called... ");
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
    softc_ptr->rx_bd = m8xx_bd_allocate (1);
    softc_ptr->tx_bd = m8xx_bd_allocate (1);
    if ((softc_ptr->rx_bd == NULL) ||
	(softc_ptr->tx_bd == NULL)) {
      rc = RTEMS_NO_MEMORY;
    }
  }
  /*
   * set parameter RAM
   */
  m8xx.spip.rbase = (char *)softc_ptr->rx_bd - (char *)&m8xx;
  m8xx.spip.tbase = (char *)softc_ptr->tx_bd - (char *)&m8xx;
  m8xx.spip.rfcr  = M8xx_RFCR_MOT | M8xx_RFCR_DMA_SPACE(0);
  m8xx.spip.tfcr  = M8xx_RFCR_MOT | M8xx_RFCR_DMA_SPACE(0);
  m8xx.spip.mrblr = 2;

  /*
   * issue "InitRxTx" Command to CP
   */
  m8xx_cp_execute_cmd (M8xx_CR_OP_INIT_RX_TX | M8xx_CR_CHAN_SPI);

  /*
   * init interrupt stuff
   */
  if (rc == RTEMS_SUCCESSFUL) {
    m8xx_spi_install_irq_handler(softc_ptr,TRUE);
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
    m8xx.pbpar =
      m8xx.pbpar
      | M8xx_PB_SPI_MISO_MSK
      | M8xx_PB_SPI_MOSI_MSK
      | M8xx_PB_SPI_CLK_MSK;

    /* set Port B Data Direction Register... */
    m8xx.pbdir =
      m8xx.pbdir
      | M8xx_PB_SPI_MISO_MSK
      | M8xx_PB_SPI_MOSI_MSK
      | M8xx_PB_SPI_CLK_MSK;
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
static int m8xx_spi_read_write_bytes
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
  m8xx_spi_softc_t *softc_ptr = &(((m8xx_spi_desc_t *)(bh))->softc);
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  int bc = 0;

#if defined(DEBUG)
  printk("m8xx_spi_read_write_bytes called... ");
#endif

  /*
   * prepare RxBD
   */
  if (rc == RTEMS_SUCCESSFUL) {
    if (rbuf == NULL) {
      /*
       * no Tx buffer: receive to dummy buffer
       */
      m8xx.spip.mrblr          = sizeof(m8xx_spi_dummy_rxbuf);
      softc_ptr->rx_bd->buffer = m8xx_spi_dummy_rxbuf;
      softc_ptr->rx_bd->length = 0;
      softc_ptr->rx_bd->status = (M8xx_BD_EMPTY | M8xx_BD_WRAP |
				  M8xx_BD_CONTINUOUS);
    }
    else {
      m8xx.spip.mrblr          = len;
      softc_ptr->rx_bd->buffer = rbuf;
      softc_ptr->rx_bd->length = 0;
      softc_ptr->rx_bd->status = (M8xx_BD_EMPTY | M8xx_BD_WRAP);
    }
  }
  /*
   * prepare TxBD
   */
  if (rc == RTEMS_SUCCESSFUL) {
    if (tbuf == NULL) {
      /*
       * no Tx buffer: transmit from dummy buffer
       */
      softc_ptr->tx_bd->buffer = m8xx_spi_dummy_rxbuf;
      softc_ptr->tx_bd->length = len;
      softc_ptr->tx_bd->status = (M8xx_BD_READY | M8xx_BD_WRAP |
				  M8xx_BD_CONTINUOUS);
    }
    else {
      softc_ptr->tx_bd->buffer = (char *)tbuf;
      softc_ptr->tx_bd->length = len;
      softc_ptr->tx_bd->status = (M8xx_BD_READY | M8xx_BD_WRAP);
    }
  }

  if (rc == RTEMS_SUCCESSFUL) {
    /*
     * set START command
     */
    m8xx.spcom = M8xx_SPCOM_STR;
    /*
     * wait for SPI to finish
     */
    rc = m8xx_spi_wait(softc_ptr);
  }
#if defined(DEBUG)
  printk("... exit OK, rc=%d\r\n",bc);
#endif
  return (rc == RTEMS_SUCCESSFUL) ? bc : -rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int m8xx_spi_read_bytes
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
  return m8xx_spi_read_write_bytes(bh,buf,NULL,len);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int m8xx_spi_write_bytes
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
  return m8xx_spi_read_write_bytes(bh,NULL,buf,len);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code m8xx_spi_set_tfr_mode
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
  m8xx_spi_softc_t *softc_ptr = &(((m8xx_spi_desc_t *)(bh))->softc);
  uint32_t spimode_baud;
  uint16_t spimode;
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  /*
   * FIXME: set proper mode
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = m8xx_spi_baud_to_mode(tfr_mode->baudrate,&spimode_baud);
  }
  if (rc == RTEMS_SUCCESSFUL) {
    rc = m8xx_spi_char_mode(softc_ptr,
			       tfr_mode->bits_per_char,
			       tfr_mode->lsb_first,
			       &spimode);
  }
  if (rc == RTEMS_SUCCESSFUL) {
    spimode |= spimode_baud;
    spimode |= M8xx_SPMODE_MASTER; /* set master mode */
    if (!tfr_mode->lsb_first) {
      spimode |= M8xx_SPMODE_REV;
    }
    if (tfr_mode->clock_inv) {
      spimode |= M8xx_SPMODE_CI;
    }
    if (tfr_mode->clock_phs) {
      spimode |= M8xx_SPMODE_CP;
    }
  }

  if (rc == RTEMS_SUCCESSFUL) {
    /*
     * disable SPI
     */
    m8xx.spmode &= ~M8xx_SPMODE_EN;
    /*
     * set new mode and reenable SPI
     */
    m8xx.spmode = spimode | M8xx_SPMODE_EN;
  }
  return rc;
}


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int m8xx_spi_ioctl
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
      -m8xx_spi_set_tfr_mode(bh,
				(const rtems_libi2c_tfr_mode_t *)arg);
    break;
  case RTEMS_LIBI2C_IOCTL_READ_WRITE:
    ret_val =
      m8xx_spi_read_write_bytes(bh,
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
static rtems_status_code bsp_dummy_spi_sel_addr
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
|    rtems_status_code                                                      |
\*=========================================================================*/
{			    
  return  RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code bsp_dummy_spi_send_start
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
  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code bsp_dummy_spi_send_stop
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
  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code bsp_pghplus_spi_sel_addr
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
|    rtems_status_code                                                      |
\*=========================================================================*/
{			    
#if defined(PGHPLUS)
  pbdat_val = m8xx.pbdat | (PGHPLUS_SPI_PB_DISP4_RS_MSK |
			    PGHPLUS_SPI_PB_DISP4_CE_MSK |
			    PGHPLUS_SPI_PB_EEP_CE_MSK);
  /*
   * select given device
   */
  switch(addr) {
  case PGHPLUS_SPI_ADDR_EEPROM:
    pbdat_val &= ~PGHPLUS_SPI_PB_EEP_CE_MSK;
    break;
  case PGHPLUS_SPI_ADDR_DISP4_DATA:
    pbdat_val = (m8xx.pbdat
		  & ~(PGHPLUS_PB_SPI_DISP4_CE_MSK |
		      PGHPLUS_PB_SPI_DISP4_RS_MSK));
    break;
  case PGHPLUS_SPI_ADDR_DISP4_CTRL:
    pbdat_val = (m8xx.pbdat
		  & ~(PGHPLUS_PB_SPI_DISP4_CE_MSK)
		  |   PGHPLUS_PB_SPI_DISP4_RS_MSK);
    break;
  default:
    return RTEMS_INVALID_NUMBER;
  }
  m8xx_pbdat = pbdat_val
#endif /* PGHPLUS */
  return  RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code bsp_pghplus_spi_send_stop
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
  printk("bsp_pghplus_spi_send_stop called... ");
#endif
    m8xx.pbdat = (m8xx.pbdat
		  | PGHPLUS_PB_SPI_DISP4_CE_MSK
		  | PGHPLUS_PB_SPI_EEP_CE_MSK);
#if defined(DEBUG)
  printk("... exit OK\r\n");
#endif
  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code bsp_pghplus_spi_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   do board specific init:                                                 |
|   - initialize pins for addressing                                        |
|   - register further drivers                                              |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int spi_busno
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  int ret_code;

#if defined(DEBUG)
  printk("bsp_pghplus_spi_init called... ");
#endif

  /*
   * init port pins used to address/select SPI devices
   */

  /*
   * set up ports
   * LINE      PAR  DIR  DAT
   * -----------------------
   * EEP_CE     0    1 act-high
   * DISP4_CS   0    1 act-high
   * DISP4_RS   0    1 active
   */

  /* set Port B Pin Assignment Register... */
  m8xx.pbpar =
    (m8xx.pbpar
     & ~(PGHPLUS_PB_SPI_EEP_CE_MSK
	 | PGHPLUS_PB_SPI_DISP4_CE_MSK
	 | PGHPLUS_PB_SPI_DISP4_RS_MSK));

    /* set Port B Data Direction Register... */
  m8xx.pbdir =
    m8xx.pbdir
    | PGHPLUS_PB_SPI_EEP_CE_MSK
    | PGHPLUS_PB_SPI_DISP4_CE_MSK
    | PGHPLUS_PB_SPI_DISP4_RS_MSK;

  /* set Port B Data Register to inactive CE state */
  m8xx.pbdat =
    m8xx.pbdat
    | PGHPLUS_PB_SPI_DISP4_CE_MSK
    | PGHPLUS_PB_SPI_DISP4_RS_MSK;

  /*
   * register devices
   */
  ret_code = rtems_libi2c_register_drv("disp",
				       disp_hcms29xx_driver_descriptor,
				       spi_busno,PGHPLUS_SPI_ADDR_DISP4);
  if (ret_code < 0) {
    return -ret_code;
  }

#if defined(DEBUG)
  printk("... exit OK\r\n");
#endif
  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code bsp_dummy_spi_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   do board specific init:                                                 |
|   - initialize pins for addressing                                        |
|   - register further drivers                                              |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int spi_busno
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
#if defined(DEBUG)
  printk("bsp_dummy_spi_init called... ");
#endif

#if defined(DEBUG)
  printk("... exit OK\r\n");
#endif
  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| list of handlers                                                          |
\*=========================================================================*/

rtems_libi2c_bus_ops_t bsp_spi_ops = {
  init:             m8xx_spi_init,
  send_start:       bsp_dummy_spi_send_start,
  send_stop:        SPI_SEND_STOP_FNC,
  send_addr:        SPI_SEND_ADDR_FNC,
  read_bytes:       m8xx_spi_read_bytes,
  write_bytes:      m8xx_spi_write_bytes,
  ioctl:            m8xx_spi_ioctl
};

static m8xx_spi_desc_t bsp_spi_bus_desc = {
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
   * register SPI bus
   */
  ret_code = rtems_libi2c_register_bus("/dev/spi",
				       &(bsp_spi_bus_desc.bus_desc));
  if (ret_code < 0) {
    return -ret_code;
  }
  spi_busno = ret_code;

  SPI_BOARD_INIT_FNC(spi_busno);
  /*
   * FIXME: further drivers, when available
   */
  return RTEMS_SUCCESSFUL;
}


