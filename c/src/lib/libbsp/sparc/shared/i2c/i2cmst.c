/*
 * Driver for GRLIB port of OpenCores I2C-master
 *
 * COPYRIGHT (c) 2007 Gaisler Research
 * based on the RTEMS MPC83xx I2C driver (c) 2007 Embedded Brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * This file contains the driver and initialization code
 *
 * 2007-09-27: First version of driver (jan@gaisler.com)
 */


#include <bsp.h>
#include <i2cmst.h>
#include <ambapp.h>
#include <grlib.h>
#include <rtems/libi2c.h>

/* Enable debug printks? */
/* #define DEBUG */

/* Default to 40 MHz system clock? */
/*
  #ifndef SYS_FREQ_kHZ
  #define SYS_FREQ_kHZ 40000
  #endif
*/


/* Calculates the scaler value for 100 kHz operation */
static int gr_i2cmst_calc_scaler(int sysfreq)
{
  return sysfreq/500 - 1;
}

/* Wait for the current transfer to end */
static int gr_i2cmst_wait(gr_i2cmst_prv_t *prv_ptr, uint8_t expected_sts)
{
  uint32_t tout = 0;
  int current_sts;
#if defined(DEBUG)
  printk("(gr_i2cmst_wait called...");
#endif

  do {
    if (tout++ > 1000000) {
      return RTEMS_TIMEOUT;
    }
  } while (prv_ptr->reg_ptr->cmdsts & GRI2C_STS_TIP);

  current_sts = prv_ptr->reg_ptr->cmdsts & ~GRI2C_STS_IF & ~GRI2C_STS_BUSY;

  if (current_sts != expected_sts) {
#if defined(DEBUG)
    if (prv_ptr->reg_ptr->cmdsts & GRI2C_STS_RXACK) {
      printk("Transfer NAKed..");
    }
    if (prv_ptr->reg_ptr->cmdsts & GRI2C_STS_AL) {
      printk("arbitration lost..");
    }
    if (prv_ptr->reg_ptr->cmdsts & GRI2C_STS_TIP) {
      printk("transfer still in progress, huh?..");
    }
    printk("exited with IO error..)");
#endif
    return RTEMS_IO_ERROR;
  }

#if defined(DEBUG)
  printk("exited...)");
#endif
  return RTEMS_SUCCESSFUL;
}

/* Initialize hardware core */
static rtems_status_code gr_i2cmst_init(rtems_libi2c_bus_t *bushdl)
{
  gr_i2cmst_prv_t *prv_ptr = &(((gr_i2cmst_desc_t *)(bushdl))->prv);
#if defined(DEBUG)
  printk("gr_i2cmst_init called...");
#endif

  /* Disable core before changing prescale register */
  prv_ptr->reg_ptr->ctrl = 0;

  /* Calculate and set prescale value */
  prv_ptr->reg_ptr->prescl = gr_i2cmst_calc_scaler(prv_ptr->sysfreq);

  /* Enable core, interrupts are not enabled */
  prv_ptr->reg_ptr->ctrl = GRI2C_CTRL_EN;

  /* Clear possible START condition */
  prv_ptr->sendstart = 0;

#if defined(DEBUG)
  printk("exited\n");
#endif
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code gr_i2cmst_send_start(rtems_libi2c_bus_t *bushdl)
{
  gr_i2cmst_prv_t *prv_ptr = &(((gr_i2cmst_desc_t *)(bushdl))->prv);
#if defined(DEBUG)
  printk("gr_i2cmst_send_start called...");
#endif

  /* The OC I2C core does not work with stand alone START events,
     instead the event is buffered */
  prv_ptr->sendstart = GRI2C_CMD_STA;

#if defined(DEBUG)
  printk("exited\n");
#endif
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code gr_i2cmst_send_stop(rtems_libi2c_bus_t *bushdl)
{
  gr_i2cmst_prv_t *prv_ptr = &(((gr_i2cmst_desc_t *)(bushdl))->prv);
#if defined(DEBUG)
  printk("gr_i2cmst_send_stop called...");
#endif

  prv_ptr->reg_ptr->cmdsts = GRI2C_CMD_STO;

#if defined(DEBUG)
  printk("exited\n");
#endif
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code gr_i2cmst_send_addr(rtems_libi2c_bus_t *bushdl,
					     uint32_t addr, int rw)
{
  gr_i2cmst_prv_t *prv_ptr = &(((gr_i2cmst_desc_t *)(bushdl))->prv);
  uint8_t addr_byte;
  rtems_status_code rc;
#if defined(DEBUG)
  printk("gr_i2cmst_send_addr called, addr = 0x%x, rw = %d...",
	 addr, rw);
#endif

  /*  Check if long address is needed */
  if (addr > 0x7f) {
    addr_byte = ((addr >> 7) & 0x06) | (rw ? 1 : 0);

    prv_ptr->reg_ptr->tdrd = addr_byte;
    prv_ptr->reg_ptr->cmdsts = GRI2C_CMD_WR | prv_ptr->sendstart;
    prv_ptr->sendstart = 0;

    /* Wait for transfer to complete */
    rc = gr_i2cmst_wait(prv_ptr, GRI2C_STATUS_IDLE);
    if (rc != RTEMS_SUCCESSFUL) {
#if defined(DEBUG)
  printk("exited with error\n");
#endif
      return -rc;
    }
  }

  /* For 10-bit adresses the last byte should only be written for a
     write operation  */
  rc = RTEMS_SUCCESSFUL;
  if (addr <= 0x7f || rw == 0) {
    addr_byte = (addr << 1) | (rw ? 1 : 0);

    prv_ptr->reg_ptr->tdrd = addr_byte;
    prv_ptr->reg_ptr->cmdsts = GRI2C_CMD_WR | prv_ptr->sendstart;
    prv_ptr->sendstart = 0;

    /* Wait for transfer to complete */
    rc = gr_i2cmst_wait(prv_ptr, GRI2C_STATUS_IDLE);
    if (rc != RTEMS_SUCCESSFUL) {
#if defined(DEBUG)
  printk("exited with error\n");
#endif
      return -rc;
    }
 }

#if defined(DEBUG)
  printk("exited\n");
#endif
  return rc;
}


static int gr_i2cmst_read_bytes(rtems_libi2c_bus_t *bushdl,
				unsigned char *bytes, int nbytes)
{
  gr_i2cmst_prv_t *prv_ptr = &(((gr_i2cmst_desc_t *)(bushdl))->prv);
  unsigned char *buf = bytes;
  rtems_status_code rc;
  unsigned char expected_sts = GRI2C_STATUS_IDLE;
#if defined(DEBUG)
  printk("gr_i2cmst_read_bytes called, nbytes = %d...", nbytes);
#endif

  while (nbytes-- > 0) {
    if (nbytes == 0) {
      /* Respond with NAK to end sequential read */
      prv_ptr->reg_ptr->cmdsts = (GRI2C_CMD_RD | GRI2C_CMD_ACK |
				  prv_ptr->sendstart);
      expected_sts = GRI2C_STS_RXACK;
    } else {
      prv_ptr->reg_ptr->cmdsts = GRI2C_CMD_RD | prv_ptr->sendstart;
    }
    prv_ptr->sendstart = 0;
    /* Wait until end of transfer */
    rc = gr_i2cmst_wait(prv_ptr, expected_sts);
    if (rc != RTEMS_SUCCESSFUL) {
      return -rc;
#if defined(DEBUG)
  printk("exited with error\n");
#endif
    }
    *buf++ = prv_ptr->reg_ptr->tdrd;
  }

#if defined(DEBUG)
  printk("exited\n");
#endif
  return buf - bytes;
}

static int gr_i2cmst_write_bytes(rtems_libi2c_bus_t *bushdl,
				unsigned char *bytes, int nbytes)
{
  gr_i2cmst_prv_t *prv_ptr = &(((gr_i2cmst_desc_t *)(bushdl))->prv);
  unsigned char *buf = bytes;
  rtems_status_code rc;
#if defined(DEBUG)
  printk("gr_i2cmst_write_bytes called, nbytes = %d...", nbytes);
#endif

  while (nbytes-- > 0) {
#if defined(DEBUG)
  printk("writing byte 0x%02X...", *buf);
#endif
    prv_ptr->reg_ptr->tdrd = *buf++;
    prv_ptr->reg_ptr->cmdsts = GRI2C_CMD_WR | prv_ptr->sendstart;
    prv_ptr->sendstart = 0;

    /* Wait for transfer to complete  */
    rc = gr_i2cmst_wait(prv_ptr, GRI2C_STATUS_IDLE);

    if (rc != RTEMS_SUCCESSFUL) {
#if defined(DEBUG)
  printk("exited with error\n");
#endif
      return -rc;
    }
  }

#if defined(DEBUG)
  printk("exited\n");
#endif
  return buf - bytes;
}

static rtems_libi2c_bus_ops_t gr_i2cmst_ops = {
  init:        gr_i2cmst_init,
  send_start:  gr_i2cmst_send_start,
  send_stop:   gr_i2cmst_send_stop,
  send_addr:   gr_i2cmst_send_addr,
  read_bytes:  gr_i2cmst_read_bytes,
  write_bytes: gr_i2cmst_write_bytes,
};


static gr_i2cmst_desc_t gr_i2cmst_desc = {
  { /* rtems_libi2c_bus_t */
    ops     : &gr_i2cmst_ops,
    size    : sizeof(gr_i2cmst_ops),
  },
  { /* gr_i2cmst_prv_t, private data */
    reg_ptr : NULL,
    sysfreq : 40000,
  }

};

/* Scans for I2CMST core and initalizes i2c library */
rtems_status_code leon_register_i2c(struct ambapp_bus *abus)
{
#if defined(DEBUG)
  printk("leon_register_i2c called...");
#endif

  int rc;
  int device_found = 0;
  struct ambapp_apb_info apbi2cmst;

  /* Scan AMBA bus for I2CMST core */
  device_found = ambapp_find_apbslv(abus, VENDOR_GAISLER, GAISLER_I2CMST,
				    &apbi2cmst);

  if (device_found == 1) {

    /* Initialize i2c library */
    rc = rtems_libi2c_initialize();
    if (rc < 0) {
#if defined(DEBUG)
      printk("rtems_libi2x_initialize failed, exiting...\n");
#endif
      return rc;
    }

    gr_i2cmst_desc.prv.reg_ptr = (gr_i2cmst_regs_t *)apbi2cmst.start;

    /* Detect system frequency, same as in apbuart_initialize */
#ifndef SYS_FREQ_kHZ
#if defined(LEON3)
	/* LEON3: find timer address via AMBA Plug&Play info */
	{
	  struct ambapp_apb_info gptimer;
	  struct gptimer_regs *tregs;

	  if (ambapp_find_apbslv(abus, VENDOR_GAISLER,
				 GAISLER_GPTIMER, &gptimer) == 1 ) {
	    tregs = (struct gptimer_regs *)gptimer.start;
	    gr_i2cmst_desc.prv.sysfreq = (tregs->scaler_reload+1)*1000;
	  } else {
	    gr_i2cmst_desc.prv.sysfreq = 40000; /* Default to 40MHz */
	  }
	}
#elif defined(LEON2)
	/* LEON2: use hardcoded address to get to timer */
	{
	  LEON_Register_Map *regs = (LEON_Register_Map *)0x80000000;
	  gr_i2cmst_desc.prv.sysfreq = (regs->Scaler_Reload+1)*1000;
	}
#else
#error CPU not supported for I2CMST driver */
#endif
#else
	/* Use hardcoded frequency */
	gr_i2cmst_desc.prv.sysfreq = SYS_FREQ_kHZ;
#endif

    rc = rtems_libi2c_register_bus("/dev/i2c1", &gr_i2cmst_desc.bus_desc);
    if (rc < 0) {
#if defined(DEBUG)
      printk("rtems_libi2c_register_bus failed, exiting..\n");
#endif
      return -rc;
    }
  }

#if defined(DEBUG)
  printk("exited\n");
#endif
  return 0;
}
