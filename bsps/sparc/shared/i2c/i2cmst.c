/*
 * Driver for GRLIB port of OpenCores I2C-master
 *
 * COPYRIGHT (c) 2007 Cobham Gaisler AB
 * based on the RTEMS MPC83xx I2C driver (c) 2007 Embedded Brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
 * This file contains the driver and initialization code
 */

#include <bsp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <grlib/ambapp.h>
#include <rtems/libi2c.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>

#include <grlib/i2cmst.h>

#include <grlib/grlib_impl.h>

/* Enable debug printks? */
/*#define DEBUG*/

#ifdef DEBUG
 #define DBG(args...) printk(args)
#else
 #define DBG(args...)
#endif

/* The OC I2C core will perform a write after a start unless the RD bit
   in the command register has been set. Since the rtems framework has
   a send_start function we buffer that command and use it when the first
   data is written. The START is buffered in the sendstart member below */
typedef struct gr_i2cmst_prv {
  rtems_libi2c_bus_t              i2clib_desc;
  struct drvmgr_dev    *dev;
  gr_i2cmst_regs_t                *reg_ptr;
  unsigned int                    sysfreq;     /* System clock frequency in kHz */
  int                             minor;
  unsigned char                   sendstart;   /* START events are buffered here */ 
  /* rtems_irq_number             irq_number; */
  /* rtems_id                     irq_sema_id; */
} gr_i2cmst_prv_t;

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

  DBG("(gr_i2cmst_wait called...");

  do {
    if (tout++ > 1000000) {
      DBG("gr_i2cmst_wait: TIMEOUT\n");
      return RTEMS_TIMEOUT;
    }
  } while (prv_ptr->reg_ptr->cmdsts & GRI2C_STS_TIP);

  current_sts = prv_ptr->reg_ptr->cmdsts & ~GRI2C_STS_IF & ~GRI2C_STS_BUSY;

  if (current_sts != expected_sts) {
#if defined(DEBUG)
    if (prv_ptr->reg_ptr->cmdsts & GRI2C_STS_RXACK) {
      DBG("Transfer NAKed..");
    }
    if (prv_ptr->reg_ptr->cmdsts & GRI2C_STS_AL) {
      DBG("arbitration lost..");
    }
    if (prv_ptr->reg_ptr->cmdsts & GRI2C_STS_TIP) {
      DBG("transfer still in progress, huh?..");
    }
    DBG("exited with IO error..)");
#endif
    DBG("gr_i2cmst_wait: IO-ERROR\n");
    return RTEMS_IO_ERROR;
  }

  DBG("exited...)");

  return RTEMS_SUCCESSFUL;
}

/* Initialize hardware core */
static rtems_status_code gr_i2cmst_init(rtems_libi2c_bus_t *bushdl)
{
  gr_i2cmst_prv_t *prv_ptr = (gr_i2cmst_prv_t *)bushdl;

  DBG("gr_i2cmst_init called...");

  /* Disable core before changing prescale register */
  prv_ptr->reg_ptr->ctrl = 0;

  /* Calculate and set prescale value */
  prv_ptr->reg_ptr->prescl = gr_i2cmst_calc_scaler(prv_ptr->sysfreq);

  /* Enable core, interrupts are not enabled */
  prv_ptr->reg_ptr->ctrl = GRI2C_CTRL_EN;

  /* Clear possible START condition */
  prv_ptr->sendstart = 0;

  DBG("exited\n");

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code gr_i2cmst_send_start(rtems_libi2c_bus_t *bushdl)
{
  gr_i2cmst_prv_t *prv_ptr = (gr_i2cmst_prv_t *)bushdl;

  DBG("gr_i2cmst_send_start called...");

  /* The OC I2C core does not work with stand alone START events,
     instead the event is buffered */
  prv_ptr->sendstart = GRI2C_CMD_STA;

  DBG("exited\n");

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code gr_i2cmst_send_stop(rtems_libi2c_bus_t *bushdl)
{
  gr_i2cmst_prv_t *prv_ptr = (gr_i2cmst_prv_t *)bushdl;

  DBG("gr_i2cmst_send_stop called...");

  prv_ptr->reg_ptr->cmdsts = GRI2C_CMD_STO;

  DBG("exited\n");

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code gr_i2cmst_send_addr(rtems_libi2c_bus_t *bushdl,
					     uint32_t addr, int rw)
{
  gr_i2cmst_prv_t *prv_ptr = (gr_i2cmst_prv_t *)bushdl;
  uint8_t addr_byte;
  rtems_status_code rc;

  DBG("gr_i2cmst_send_addr called, addr = 0x%x, rw = %d...", 
	 addr, rw);

  /*  Check if long address is needed */
  if (addr > 0x7f) {
    addr_byte = ((addr >> 7) & 0x06) | (rw ? 1 : 0);

    prv_ptr->reg_ptr->tdrd = addr_byte;
    prv_ptr->reg_ptr->cmdsts = GRI2C_CMD_WR | prv_ptr->sendstart;
    prv_ptr->sendstart = 0;

    /* Wait for transfer to complete */
    rc = gr_i2cmst_wait(prv_ptr, GRI2C_STATUS_IDLE);
    if (rc != RTEMS_SUCCESSFUL) {

      DBG("exited with error\n");

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
      DBG("exited with error\n");
      return -rc;
    }
  }

  DBG("exited\n");
  return rc;
}


static int gr_i2cmst_read_bytes(rtems_libi2c_bus_t *bushdl,
				unsigned char *bytes, int nbytes)
{
  gr_i2cmst_prv_t *prv_ptr = (gr_i2cmst_prv_t *)bushdl;
  unsigned char *buf = bytes;
  rtems_status_code rc;
  unsigned char expected_sts = GRI2C_STATUS_IDLE;

  DBG("gr_i2cmst_read_bytes called, nbytes = %d...", nbytes);

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
      DBG("exited with error\n");
      return -rc;
    }
    *buf++ = prv_ptr->reg_ptr->tdrd;
  }

  DBG("exited\n");

  return buf - bytes;
}

static int gr_i2cmst_write_bytes(rtems_libi2c_bus_t *bushdl,
				unsigned char *bytes, int nbytes)
{
  gr_i2cmst_prv_t *prv_ptr = (gr_i2cmst_prv_t *)bushdl;
  unsigned char *buf = bytes;
  rtems_status_code rc;

  DBG("gr_i2cmst_write_bytes called, nbytes = %d...", nbytes);

  while (nbytes-- > 0) {

    DBG("writing byte 0x%02X...", *buf);

    prv_ptr->reg_ptr->tdrd = *buf++;
    prv_ptr->reg_ptr->cmdsts = GRI2C_CMD_WR | prv_ptr->sendstart;
    prv_ptr->sendstart = 0;

    /* Wait for transfer to complete  */
    rc = gr_i2cmst_wait(prv_ptr, GRI2C_STATUS_IDLE);

    if (rc != RTEMS_SUCCESSFUL) {
      DBG("exited with error\n");
      return -rc;
    }
  }

  DBG("exited\n");

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

/* Get Hardware and disable it */
static int i2cmst_device_init(gr_i2cmst_prv_t *priv)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)priv->dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	priv->reg_ptr = (gr_i2cmst_regs_t *)pnpinfo->apb_slv->start;

	/* Disable core */
	priv->reg_ptr->ctrl = 0;

	priv->i2clib_desc.ops = &gr_i2cmst_ops;
	priv->i2clib_desc.size = sizeof(gr_i2cmst_ops);
	return 0;
}


/******************* Driver Manager Part ***********************/

int i2cmst_init2(struct drvmgr_dev *dev);
int i2cmst_init3(struct drvmgr_dev *dev);

struct drvmgr_drv_ops i2cmst_ops = 
{
	.init = {NULL, i2cmst_init2, i2cmst_init3, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id i2cmst_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_I2CMST},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info i2cmst_drv_info =
{
	{
		DRVMGR_OBJ_DRV,				/* Driver */
		NULL,					/* Next driver */
		NULL,					/* Device list */
		DRIVER_AMBAPP_GAISLER_I2CMST_ID,	/* Driver ID */
		"I2CMST_DRV",				/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,			/* Bus Type */
		&i2cmst_ops,
		NULL,					/* Funcs */
		0,					/* No devices yet */
		0,
	},
	&i2cmst_ids[0]
};

void i2cmst_register_drv (void)
{
	DBG("Registering I2CMST driver\n");
	drvmgr_drv_register(&i2cmst_drv_info.general);
}

/* The I2CMST Driver is informed about a new hardware device */
int i2cmst_init2(struct drvmgr_dev *dev)
{
	gr_i2cmst_prv_t *priv;

	DBG("I2CMST[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	priv = dev->priv = grlib_calloc(1, sizeof(*priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* This core will not find other cores, so we wait for init2() */

	return DRVMGR_OK;
}

/* Init stage 2 */
int i2cmst_init3(struct drvmgr_dev *dev)
{
	gr_i2cmst_prv_t *priv;
	char prefix[32];
	char devName[32];
	int rc;

	priv = (gr_i2cmst_prv_t *)dev->priv;

	/* Do initialization */

	/* Initialize i2c library */
	rc = rtems_libi2c_initialize();
	if (rc != 0) {
		DBG("I2CMST: rtems_libi2c_initialize failed, exiting...\n");
		free(dev->priv);
		dev->priv = NULL;
		return DRVMGR_FAIL;
	}

	/* I/O system registered and initialized 
	 * Now we take care of device initialization.
	 */

	/* Get frequency */
	if ( drvmgr_freq_get(dev, DEV_APB_SLV, &priv->sysfreq) ) {
		return DRVMGR_FAIL;
	}
	priv->sysfreq = priv->sysfreq / 1000; /* Convert to kHz */

	if ( i2cmst_device_init(priv) ) {
		free(dev->priv);
		dev->priv = NULL;
		return DRVMGR_FAIL;
	}

	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if ( drvmgr_get_dev_prefix(dev, prefix) ) {
		/* Failed to get prefix, make sure of a unique FS name
		 * by using the driver minor.
		 */
		sprintf(devName, "/dev/i2c%d", dev->minor_drv+1);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(devName, "/dev/%si2c%d", prefix, dev->minor_bus+1);
	}

	/* Register Bus for this Device */
	rc = rtems_libi2c_register_bus(devName, &priv->i2clib_desc);
	if (rc < 0) {
		DBG("I2CMST: rtems_libi2c_register_bus(%s) failed, exiting..\n", devName);
		free(dev->priv);
		dev->priv = NULL;
		return DRVMGR_FAIL;
	}
	priv->minor = rc;

	return DRVMGR_OK;
}
