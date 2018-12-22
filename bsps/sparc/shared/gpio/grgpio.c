/*  GRGPIO GPIO Driver interface.
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <rtems/bspIo.h>
#include <string.h>
#include <stdio.h>

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/grgpio.h>
#include <grlib/gpiolib.h>
#include <grlib/ambapp.h>
#include <grlib/grlib.h>
#include <grlib/grlib_impl.h>

/*#define DEBUG 1*/

#ifdef DEBUG
#define DBG(x...) printk(x)
#define STATIC
#else
#define DBG(x...) 
#define STATIC static
#endif

struct grgpio_isr {
	drvmgr_isr isr;
	void *arg;
};

struct grgpio_priv {
	struct drvmgr_dev	*dev;
	struct grgpio_regs		*regs;
	int				irq;
	int				minor;

	/* Driver implementation */
	int				port_cnt;
	unsigned char			port_handles[32];
	struct grgpio_isr		isrs[32];
	struct gpiolib_drv		gpiolib_desc;
	unsigned int			bypass;
	unsigned int			imask;
};

/******************* Driver Manager Part ***********************/

int grgpio_device_init(struct grgpio_priv *priv);

int grgpio_init1(struct drvmgr_dev *dev);
int grgpio_init2(struct drvmgr_dev *dev);

struct drvmgr_drv_ops grgpio_ops = 
{
	.init = {grgpio_init1, NULL, NULL, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id grgpio_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_GPIO},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info grgpio_drv_info =
{
	{
		DRVMGR_OBJ_DRV,				/* Driver */
		NULL,					/* Next driver */
		NULL,					/* Device list */
		DRIVER_AMBAPP_GAISLER_GRGPIO_ID,	/* Driver ID */
		"GRGPIO_DRV",				/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,			/* Bus Type */
		&grgpio_ops,
		NULL,					/* Funcs */
		0,					/* No devices yet */
		0,
	},
	&grgpio_ids[0]
};

void grgpio_register_drv (void)
{
	DBG("Registering GRGPIO driver\n");
	drvmgr_drv_register(&grgpio_drv_info.general);
}

/* Register GRGPIO pins as quick as possible to the GPIO library,
 * other drivers may depend upon them in INIT LEVEL 2.
 * Note that since IRQ may not be available in init1, it is assumed
 * that the GPIOLibrary does not request IRQ routines until LEVEL 2.
 */
int grgpio_init1(struct drvmgr_dev *dev)
{
	struct grgpio_priv *priv;
	int status, port;

	DBG("GRGPIO[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	/* This core will not find other cores, but other driver may depend upon 
	 * the GPIO library to function. So, we set up GPIO right away.
	 */

	/* Initialize library if not already done */
	status = gpiolib_initialize();
	if ( status < 0 )
		return DRVMGR_FAIL;

	priv = dev->priv = grlib_calloc(1, sizeof(*priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	priv->dev = dev;

	if ( grgpio_device_init(priv) ) {
		free(dev->priv);
		dev->priv = NULL;
		return DRVMGR_FAIL;
	}

	/* Register all ports available on this core as GPIO port to 
	 * upper layer
	 */
	for(port=0; port<priv->port_cnt; port++) {
		priv->port_handles[port] = port;
		gpiolib_drv_register(&priv->gpiolib_desc,
					&priv->port_handles[port]);
	}

	return DRVMGR_OK;
}

/******************* Driver Implementation ***********************/

/* Find port from handle, returns -1 if not found */
static int grgpio_find_port(void *handle, struct grgpio_priv **priv)
{
	unsigned char portnr;
	
	portnr = *(unsigned char *)handle;
	if ( portnr > 31 )
		return -1;
	*priv = (struct grgpio_priv *)
		(((unsigned int)handle - portnr*sizeof(unsigned char)) - 
		offsetof(struct grgpio_priv, port_handles));
	return portnr;
}

static int grgpio_gpiolib_open(void *handle)
{
	struct grgpio_priv *priv;
	int portnr;

	portnr = grgpio_find_port(handle, &priv);
	if ( portnr < 0 ) {
		DBG("GRGPIO: FAILED OPENING HANDLE 0x%08x\n", handle);
		return -1;
	}
	DBG("GRGPIO[0x%08x][%d]: OPENING\n", priv->regs, portnr);

	/* Open the device, nothing to be done... */

	return 0;
}

static int grgpio_grpiolib_config(void *handle, struct gpiolib_config *cfg)
{
	struct grgpio_priv *priv;
	int portnr;
	unsigned int mask;

	portnr = grgpio_find_port(handle, &priv);
	if ( portnr < 0 ) {
		return -1;
	}
	DBG("GRGPIO[0x%08x][%d]: CONFIG\n", priv->regs, portnr);

	/* Configure the device. And check that operation is supported,
	 * not all I/O Pins have IRQ support.
	 */
	mask = (1<<portnr);

	/* Return error when IRQ not supported by this I/O Line and it
	 * is beeing enabled by user.
	 */
	if ( ((mask & priv->imask) == 0) && cfg->mask )
		return -1;

	priv->regs->imask &= ~mask; /* Disable interrupt temporarily */

	/* Configure settings before enabling interrupt */
	priv->regs->ipol = (priv->regs->ipol & ~mask) | (cfg->irq_polarity ? mask : 0);
	priv->regs->iedge = (priv->regs->iedge & ~mask) | (cfg->irq_level ? 0 : mask);
	priv->regs->imask |= cfg->mask ? mask : 0;

	return 0;
}

static int grgpio_grpiolib_get(void *handle, int *inval)
{
	struct grgpio_priv *priv;
	int portnr;

	portnr = grgpio_find_port(handle, &priv);
	if ( portnr < 0 ) {
		return -1;
	}
	DBG("GRGPIO[0x%08x][%d]: GET\n", priv->regs, portnr);

	/* Get current status of the port */
	if ( inval )
		*inval = (priv->regs->data >> portnr) & 0x1;

	return 0;
}

static int grgpio_grpiolib_irq_opts(void *handle, unsigned int options)
{
	struct grgpio_priv *priv;
	int portnr;
	drvmgr_isr isr;
	void *arg;

	portnr = grgpio_find_port(handle, &priv);
	if ( portnr < 0 ) {
		return -1;
	}
	DBG("GRGPIO[0x%08x][%d]: IRQ OPTS 0x%x\n", priv->regs, portnr, options);

	if ( options & GPIOLIB_IRQ_FORCE )
		return -1;

	isr = priv->isrs[portnr].isr;
	arg = priv->isrs[portnr].arg;

	if ( options & GPIOLIB_IRQ_DISABLE ) {
		/* Disable interrupt at interrupt controller */
		if ( drvmgr_interrupt_unregister(priv->dev, portnr, isr, arg) ) {
			return -1;
		}
	}
	if ( options & GPIOLIB_IRQ_CLEAR ) {
		/* Clear interrupt at interrupt controller */
		if ( drvmgr_interrupt_clear(priv->dev, portnr) ) {
			return -1;
		}
	}
	if ( options & GPIOLIB_IRQ_ENABLE ) {
		/* Enable interrupt at interrupt controller */
		if ( drvmgr_interrupt_register(priv->dev, portnr, "grgpio", isr, arg) ) {
			return -1;
		}
	}
	if ( options & GPIOLIB_IRQ_MASK ) {
		/* Mask (disable) interrupt at interrupt controller */
		if ( drvmgr_interrupt_mask(priv->dev, portnr) ) {
			return -1;
		}
	}
	if ( options & GPIOLIB_IRQ_UNMASK ) {
		/* Unmask (enable) interrupt at interrupt controller */
		if ( drvmgr_interrupt_unmask(priv->dev, portnr) ) {
			return -1;
		}
	}

	return 0;
}

static int grgpio_grpiolib_irq_register(void *handle, void *func, void *arg)
{
	struct grgpio_priv *priv;
	int portnr;

	portnr = grgpio_find_port(handle, &priv);
	if ( portnr < 0 ) {
		DBG("GRGPIO: FAILED OPENING HANDLE 0x%08x\n", handle);
		return -1;
	}
	DBG("GRGPIO: OPENING %d at [0x%08x]\n", portnr, priv->regs);

	/* Since the user doesn't provide the ISR and argument, we must... */
	priv->isrs[portnr].isr = func;
	priv->isrs[portnr].arg = arg;

	return 0;
}

static int grgpio_grpiolib_set(void *handle, int dir, int outval)
{
	struct grgpio_priv *priv;
	int portnr;
	unsigned int mask;

	portnr = grgpio_find_port(handle, &priv);
	if ( portnr < 0 ) {
		DBG("GRGPIO: FAILED OPENING HANDLE 0x%08x\n", handle);
		return -1;
	}
	DBG("GRGPIO: OPENING %d at [0x%08x]\n", portnr, priv->regs);

	/* Set Direction and Output */
	mask = 1<<portnr;
	priv->regs->dir = (priv->regs->dir & ~mask) | (dir ? mask : 0);
	priv->regs->output = (priv->regs->output & ~mask) | (outval ? mask : 0);

	return 0;
}

static int grgpio_gpiolib_show(void *handle)
{
	struct grgpio_priv *priv;
	int portnr, i, regs[7];
	volatile unsigned int *reg;

	portnr = grgpio_find_port(handle, &priv);
	if ( portnr < 0 ) {
		DBG("GRGPIO: FAILED SHOWING HANDLE 0x%08x\n", handle);
		return -1;
	}
	for (i=0, reg=&priv->regs->data; i<7; i++, reg++) {
		regs[i] = ( *reg >> portnr) & 1;
	}
	printf("GRGPIO[%p] PORT[%d]: IN/OUT/DIR: [%d,%d,%d], MASK/POL/EDGE: [%d,%d,%d], BYPASS: %d\n",
		priv->regs, portnr, regs[0], regs[1], regs[2], regs[3], regs[4], regs[5], regs[6]);
	return 0;
}

static int grgpio_gpiolib_get_info(void *handle, struct gpiolib_info *pinfo)
{
	struct grgpio_priv *priv;
	int portnr;
	char prefix[48];
	struct drvmgr_dev *dev;

	if ( !pinfo )
		return -1;

	portnr = grgpio_find_port(handle, &priv);
	if ( portnr < 0 ) {
		DBG("GRGPIO: FAILED GET_INFO HANDLE 0x%08x\n", handle);
		return -1;
	}

	/* Get Filesystem name prefix */
	dev = priv->dev;
	prefix[0] = '\0';
	if ( drvmgr_get_dev_prefix(dev, prefix) ) {
		/* Failed to get prefix, make sure of a unique FS name
		 * by using the driver minor.
		 */
		snprintf(pinfo->devName, 64, "/dev/grgpio%d/%d", dev->minor_drv, portnr);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		snprintf(pinfo->devName, 64, "/dev/%sgrgpio%d/%d", prefix, dev->minor_bus, portnr);
	}

	return 0;
}

static struct gpiolib_drv_ops grgpio_gpiolib_ops = 
{
	.config		= grgpio_grpiolib_config,
	.get		= grgpio_grpiolib_get,
	.irq_opts	= grgpio_grpiolib_irq_opts,
	.irq_register	= grgpio_grpiolib_irq_register,
	.open		= grgpio_gpiolib_open,
	.set		= grgpio_grpiolib_set,
	.show		= grgpio_gpiolib_show,
	.get_info	= grgpio_gpiolib_get_info,
};

int grgpio_device_init(struct grgpio_priv *priv)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	union drvmgr_key_value *value;
	unsigned int mask;
	int port_cnt;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)priv->dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	priv->irq = pnpinfo->irq;
	priv->regs = (struct grgpio_regs *)pnpinfo->apb_slv->start;

	DBG("GRGPIO: 0x%08x irq %d\n", (unsigned int)priv->regs, priv->irq);

	/* Mask all Interrupts */
	priv->regs->imask = 0;

	/* Make IRQ Rising edge triggered default */
	priv->regs->ipol = 0xfffffffe;
	priv->regs->iedge = 0xfffffffe;

	/* Read what I/O lines have IRQ support */
	priv->imask = priv->regs->ipol;

	/* Let the user configure the port count, this might be needed
	 * when the GPIO lines must not be changed (assigned during bootup)
	 */
	value = drvmgr_dev_key_get(priv->dev, "nBits", DRVMGR_KT_INT);
	if ( value ) {
		priv->port_cnt = value->i;
	} else {
		/* Auto detect number of GPIO ports */
		priv->regs->dir = 0;
		priv->regs->output = 0xffffffff;
		mask = priv->regs->output;
		priv->regs->output = 0;

		for(port_cnt=0; port_cnt<32; port_cnt++)
			if ( (mask & (1<<port_cnt)) == 0 )
				break;
		priv->port_cnt = port_cnt;
	}

	/* Let the user configure the BYPASS register, this might be needed
	 * to select which cores can do I/O on a pin.
	 */
	value = drvmgr_dev_key_get(priv->dev, "bypass", DRVMGR_KT_INT);
	if ( value ) {
		priv->bypass = value->i;
	} else {
		priv->bypass = 0;
	}
	priv->regs->bypass = priv->bypass;

	/* Prepare GPIOLIB layer */
	priv->gpiolib_desc.ops = &grgpio_gpiolib_ops;

	return 0;
}
