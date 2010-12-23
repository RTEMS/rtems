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
| this file contains the MPC83xx I2C driver                       |
\*===============================================================*/
#include <stdlib.h>
#include <bsp.h>
#include <bsp/irq.h>
#if defined(__GEN83xx_BSP_h)
  #include <mpc83xx/mpc83xx_i2cdrv.h>
#elif defined(LIBBSP_POWERPC_MPC55XXEVB_BSP_H)
  #include <bsp/mpc83xx_i2cdrv.h>
#endif
#include <rtems/error.h>
#include <rtems/bspIo.h>
#include <errno.h>
#include <rtems/libi2c.h>

#undef DEBUG

#if defined(__GEN83xx_BSP_h)
  #define I2CCR_MEN  (1 << 7)   /* module enable */
#elif defined(LIBBSP_POWERPC_MPC55XXEVB_BSP_H)
  #define I2CCR_MDIS (1 << 7)   /* module disable */
#endif
#define I2CCR_MIEN (1 << 6)     /* module interrupt enable */
#define I2CCR_MSTA (1 << 5)     /* 0->1 generates a start condiiton, 1->0 a stop */
#define I2CCR_MTX  (1 << 4)     /* 0 = receive mode, 1 = transmit mode           */
#define I2CCR_TXAK (1 << 3)     /* 0 = send ack 1 = send nak during receive      */
#define I2CCR_RSTA (1 << 2)     /* 1 = send repeated start condition             */
#define I2CCR_BCST (1 << 0)     /* 0 = disable 1 = enable broadcast accept       */

#define I2CSR_MCF  (1 << 7)     /* data transfer (0=transfer in progres) */
#define I2CSR_MAAS (1 << 6)     /* addessed as slave   */
#define I2CSR_MBB  (1 << 5)     /* bus busy            */
#define I2CSR_MAL  (1 << 4)     /* arbitration lost    */
#define I2CSR_BCSTM (1 << 3)    /* broadcast match     */
#define I2CSR_SRW  (1 << 2)     /* slave read/write    */
#define I2CSR_MIF  (1 << 1)     /* module interrupt    */
#define I2CSR_RXAK (1 << 0)     /* receive acknowledge */

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code mpc83xx_i2c_find_clock_divider
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   determine proper divider value                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 uint8_t *result,                        /* result value                   */
 int divider                             /* requested divider              */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  int i;
  int fdr_val;
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  struct {
    int divider;
    int fdr_val;
  } dividers[] ={
#if defined(__GEN83xx_BSP_h)
    {  256,0x20 }, {  288,0x21 }, {  320,0x22 }, {  352,0x23 },
    {  384,0x00 }, {  416,0x01 }, {  448,0x25 }, {  480,0x02 },
    {  512,0x26 }, {  576,0x03 }, {  640,0x04 }, {  704,0x05 },
    {  768,0x29 }, {  832,0x06 }, {  896,0x2a }, { 1024,0x07 },
    { 1152,0x08 }, { 1280,0x09 }, { 1536,0x0A }, { 1792,0x2E },
    { 1920,0x0B }, { 2048,0x2F }, { 2304,0x0C }, { 2560,0x0D },
    { 3072,0x0E }, { 3584,0x32 }, { 3840,0x0F }, { 4096,0x33 },
    { 4608,0x10 }, { 5120,0x11 }, { 6144,0x12 }, { 7168,0x36 },
    { 7680,0x13 }, { 8192,0x37 }, { 9216,0x14 }, {10240,0x15 },
    {12288,0x16 }, {14336,0x3A }, {15360,0x17 }, {16384,0x3B },
    {18432,0x18 }, {20480,0x19 }, {24576,0x1A }, {28672,0x3E },
    {30720,0x1B }, {32768,0x3F }, {36864,0x1C }, {40960,0x1D },
    {49152,0x1E }, {61440,0x1F }
#elif defined(LIBBSP_POWERPC_MPC55XXEVB_BSP_H)
    { 768, 0x31 }
#endif
  };

  if (divider <= 0) {
    sc = RTEMS_INVALID_NUMBER;
  }

  if (sc == RTEMS_SUCCESSFUL) {
    sc = RTEMS_INVALID_NUMBER;
    for (i = 0, fdr_val = -1; i < sizeof(dividers)/sizeof(dividers[0]); i++) {
      fdr_val = dividers[i].fdr_val;
      if (dividers[i].divider >= divider)
	{
	  sc = RTEMS_SUCCESSFUL;
	  *result = fdr_val;
	  break;
	}
    }
  }
  return sc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int mpc83xx_i2c_wait
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   wait for i2c to become idle                                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 mpc83xx_i2c_softc_t *softc_ptr,          /* handle              */
 uint8_t              desired_status,     /* desired status word */
 uint8_t              status_mask         /* status word mask    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
  uint8_t act_status;
  rtems_status_code rc;
  uint32_t tout;

#if defined(DEBUG)
  printk("mpc83xx_i2c_wait called... ");
#endif

  if (softc_ptr->initialized) {
    /*
     * enable interrupt mask
     */
    softc_ptr->reg_ptr->i2ccr |= I2CCR_MIEN;
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
    } while (!(softc_ptr->reg_ptr->i2csr & I2CSR_MIF));
  }
  softc_ptr->reg_ptr->i2ccr &= ~I2CCR_MIEN;

  act_status = softc_ptr->reg_ptr->i2csr;
  if ((act_status  & status_mask) != desired_status) {
#if defined(DEBUG)
    printk("... exit with RTEMS_IO_ERROR\r\n");
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
static void mpc83xx_i2c_irq_handler
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
  mpc83xx_i2c_softc_t *softc_ptr = (mpc83xx_i2c_softc_t *)handle;

  /*
   * clear IRQ flag
   */
  #if defined(__GEN83xx_BSP_h)
    softc_ptr->reg_ptr->i2csr &= ~I2CSR_MIF;
  #elif defined(LIBBSP_POWERPC_MPC55XXEVB_BSP_H)
    softc_ptr->reg_ptr->i2csr = I2CSR_MIF;
  #endif

  /*
   * disable interrupt mask
   */
  softc_ptr->reg_ptr->i2ccr &= ~I2CCR_MIEN;
  if (softc_ptr->initialized) {
    rtems_semaphore_release(softc_ptr->irq_sema_id);
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void mpc83xx_i2c_irq_on_off
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
static int mpc83xx_i2c_irq_isOn
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
static void mpc83xx_i2c_install_irq_handler
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   (un-)install the interrupt handler                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 mpc83xx_i2c_softc_t *softc_ptr,        /* ptr to control structure        */
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
    mpc83xx_i2c_irq_handler,           /* rtems_irq_hdl           */
    (rtems_irq_hdl_param)softc_ptr,    /* (rtems_irq_hdl_param)   */
    mpc83xx_i2c_irq_on_off,            /* (rtems_irq_enable)      */
    mpc83xx_i2c_irq_on_off,            /* (rtems_irq_disable)     */
    mpc83xx_i2c_irq_isOn               /* (rtems_irq_is_enabled)  */
  };

  /*
   * (un-)install handler for I2C device
   */
  if (install) {
    /*
     * create semaphore for IRQ synchronization
     */
    rc = rtems_semaphore_create(rtems_build_name('i','2','c','s'),
				0,
				RTEMS_FIFO
				| RTEMS_SIMPLE_BINARY_SEMAPHORE,
				0,
				&softc_ptr->irq_sema_id);
    if (rc != RTEMS_SUCCESSFUL) {
      rtems_panic("I2C: cannot create semaphore");
    }
    if (!BSP_install_rtems_irq_handler (&irq_conn_data)) {
      rtems_panic("I2C: cannot install IRQ handler");
    }
  }
  else {
    if (!BSP_remove_rtems_irq_handler (&irq_conn_data)) {
      rtems_panic("I2C: cannot uninstall IRQ handler");
    }
    /*
     * delete sync semaphore
     */
    if (softc_ptr->irq_sema_id != 0) {
      rc = rtems_semaphore_delete(softc_ptr->irq_sema_id);
      if (rc != RTEMS_SUCCESSFUL) {
	rtems_panic("I2C: cannot delete semaphore");
      }
    }
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code mpc83xx_i2c_init
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
  mpc83xx_i2c_softc_t *softc_ptr = &(((mpc83xx_i2c_desc_t *)(bh))->softc);
  uint8_t fdr_val;
  int errval;
#if defined(DEBUG)
  printk("mpc83xx_i2c_init called... ");
#endif
  /*
   * init HW registers
   */
  /*
   * init frequency divider to 100kHz
   */
  errval = mpc83xx_i2c_find_clock_divider(&fdr_val,
					  softc_ptr->base_frq/100000);
  if (errval != 0) {
    return errval;
  }
  softc_ptr->reg_ptr->i2cfdr = fdr_val;
  /*
   * init digital filter sampling rate
   */
  softc_ptr->reg_ptr->i2cdfsrr = 0x10 ; /* no special filtering needed */
  /*
   * set own slave address to broadcast (0x00)
   */
  softc_ptr->reg_ptr->i2cadr = 0x00 ;

  /*
   * set control register to module enable
   */
  #if defined(__GEN83xx_BSP_h)
    softc_ptr->reg_ptr->i2ccr = I2CCR_MEN;
  #elif defined(LIBBSP_POWERPC_MPC55XXEVB_BSP_H)
    softc_ptr->reg_ptr->i2ccr = 0;
  #endif

  /*
   * init interrupt stuff
   */
  mpc83xx_i2c_install_irq_handler(softc_ptr,TRUE);

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
static rtems_status_code mpc83xx_i2c_send_start
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   send a start condition to bus                                           |
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
  mpc83xx_i2c_softc_t *softc_ptr = &(((mpc83xx_i2c_desc_t *)(bh))->softc);

#if defined(DEBUG)
  printk("mpc83xx_i2c_send_start called... ");
#endif
  if (0 != (softc_ptr->reg_ptr->i2ccr & I2CCR_MSTA)) {
    /*
     * already started, so send a "repeated start"
     */
    softc_ptr->reg_ptr->i2ccr |= I2CCR_RSTA;
  }
  else {
    softc_ptr->reg_ptr->i2ccr |= I2CCR_MSTA;
  }

#if defined(DEBUG)
  printk("... exit OK\r\n");
#endif
  return 0;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code mpc83xx_i2c_send_stop
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   send a stop condition to bus                                            |
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
  mpc83xx_i2c_softc_t *softc_ptr = &(((mpc83xx_i2c_desc_t *)(bh))->softc);

#if defined(DEBUG)
  printk("mpc83xx_i2c_send_stop called... ");
#endif
  softc_ptr->reg_ptr->i2ccr &= ~I2CCR_MSTA;
  /*
   * wait, 'til stop has been executed
   */
  while (0 != (softc_ptr->reg_ptr->i2csr & I2CSR_MBB)) {
    rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
  }
#if defined(DEBUG)
  printk("... exit OK\r\n");
#endif
  return 0;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code mpc83xx_i2c_send_addr
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
  mpc83xx_i2c_softc_t *softc_ptr = &(((mpc83xx_i2c_desc_t *)(bh))->softc);
  bool long_addr = false;
  uint8_t addr_byte;
  rtems_status_code rc;

#if defined(DEBUG)
  printk("mpc83xx_i2c_send_addr called... ");
#endif
  softc_ptr->reg_ptr->i2ccr |= I2CCR_MTX;
  /*
   * determine, whether short or long address is needed, determine rd/wr
   */
  if (addr > 0x7f) {
    long_addr = true;
    addr_byte = (0xf0
		 | ((addr >> 7) & 0x06)
		 | ((rw) ? 1 : 0));
    /*
     * send first byte
     */
    softc_ptr->reg_ptr->i2cdr = addr_byte;
    /*
     * wait for successful transfer
     */
    rc = mpc83xx_i2c_wait(softc_ptr, I2CSR_MCF, I2CSR_MCF | I2CSR_RXAK);
    if (rc != RTEMS_SUCCESSFUL) {
#if defined(DEBUG)
      printk("... exit rc=%d\r\n",rc);
#endif
      return rc;
    }
  }
  /*
   * send (final) byte
   */
  addr_byte = ((addr << 1)
	       | ((rw) ? 1 : 0));

  softc_ptr->reg_ptr->i2cdr = addr_byte;
  /*
   * wait for successful transfer
   */
  rc = mpc83xx_i2c_wait(softc_ptr, I2CSR_MCF, I2CSR_MCF | I2CSR_RXAK);

#if defined(DEBUG)
  printk("... exit rc=%d\r\n",rc);
#endif
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int mpc83xx_i2c_read_bytes
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   receive some bytes from I2C device                                      |
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
  mpc83xx_i2c_softc_t *softc_ptr = &(((mpc83xx_i2c_desc_t *)(bh))->softc);
  rtems_status_code rc;
  unsigned char *p = buf;
  unsigned char dummy;

#if defined(DEBUG)
  printk("mpc83xx_i2c_read_bytes called... ");
#endif
  softc_ptr->reg_ptr->i2ccr &= ~I2CCR_MTX;
  softc_ptr->reg_ptr->i2ccr &= ~I2CCR_TXAK;
  /*
   * FIXME: do we need to deactivate TXAK from the start,
   * when only one byte is to be received?
   */
  /*
   * we need a dummy transfer here to start the first read
   */
  dummy = softc_ptr->reg_ptr->i2cdr;

  while (len-- > 0) {
    if (len == 0) {
      /*
       * last byte is not acknowledged
       */
      softc_ptr->reg_ptr->i2ccr |= I2CCR_TXAK;
    }
    /*
     * wait 'til end of transfer
     */
    rc = mpc83xx_i2c_wait(softc_ptr, I2CSR_MCF, I2CSR_MCF);
    if (rc != RTEMS_SUCCESSFUL) {
#if defined(DEBUG)
      printk("... exit rc=%d\r\n",-rc);
#endif
      return -rc;
    }
    *p++ = softc_ptr->reg_ptr->i2cdr;

  }

 /*
  * wait 'til end of last transfer
  */
  rc = mpc83xx_i2c_wait(softc_ptr, I2CSR_MCF, I2CSR_MCF);

#if defined(DEBUG)
  printk("... exit OK, rc=%d\r\n",p-buf);
#endif
  return p - buf;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int mpc83xx_i2c_write_bytes
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   send some bytes to I2C device                                           |
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
  mpc83xx_i2c_softc_t *softc_ptr = &(((mpc83xx_i2c_desc_t *)(bh))->softc);
  rtems_status_code rc;
  unsigned char *p = buf;

#if defined(DEBUG)
  printk("mpc83xx_i2c_write_bytes called... ");
#endif
  softc_ptr->reg_ptr->i2ccr =
    (softc_ptr->reg_ptr->i2ccr & ~I2CCR_TXAK) | I2CCR_MTX;
  while (len-- > 0) {
    int rxack = len != 0 ? I2CSR_RXAK : 0;

    softc_ptr->reg_ptr->i2cdr = *p++;
    /*
     * wait 'til end of transfer
     */
    rc = mpc83xx_i2c_wait(softc_ptr, I2CSR_MCF, I2CSR_MCF | rxack);
    if (rc != RTEMS_SUCCESSFUL) {
#if defined(DEBUG)
      printk("... exit rc=%d\r\n",-rc);
#endif
      return -rc;
    }
  }
#if defined(DEBUG)
  printk("... exit OK, rc=%d\r\n",p-buf);
#endif
  return p - buf;
}

rtems_libi2c_bus_ops_t mpc83xx_i2c_ops = {
  .init = mpc83xx_i2c_init,
  .send_start = mpc83xx_i2c_send_start,
  .send_stop = mpc83xx_i2c_send_stop,
  .send_addr = mpc83xx_i2c_send_addr,
  .read_bytes = mpc83xx_i2c_read_bytes,
  .write_bytes = mpc83xx_i2c_write_bytes,
};

