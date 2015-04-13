/* GRSPW ROUTER APB-Register Driver.
 *
 * COPYRIGHT (c) 2010.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <stdio.h>

#include <drvmgr/drvmgr.h>
#include <drvmgr/ambapp_bus.h>
#include <bsp/grspw_router.h>

#define ROUTER_DBG(args...)

#define REG_READ(adr) (*(volatile unsigned int *)(adr))
#define REG_WRITE(adr, value) (*(volatile unsigned int *)(adr) = (value))

struct router_regs {
	unsigned int resv1;		/* 0x000 */
	unsigned int psetup[255];	/* 0x004 */
	unsigned int resv2[32];		/* 0x400 */
	unsigned int routes[224];	/* 0x480 */
	unsigned int pctrl[32];		/* 0x800 */
	unsigned int psts[32];		/* 0x880 */
	unsigned int treload[32];	/* 0x900 */
	unsigned int resv3[32];		/* 0x980 */
	unsigned int cfgsts;		/* 0xA00 */
	unsigned int timecode;		/* 0xA04 */
	unsigned int ver;		/* 0xA08 */
	unsigned int idiv;		/* 0xA0C */
	unsigned int cfgwe;		/* 0xA10 */
	unsigned int tprescaler;	/* 0xA14 */
	unsigned int resv4[123];	/* 0xA18 */
	unsigned int charo[31];		/* 0xC04 */
	unsigned int resv5;		/* 0xC80 */
	unsigned int chari[31];		/* 0xC84 */
	unsigned int resv6;		/* 0xD00 */
	unsigned int pkto[31];		/* 0xD04 */
	unsigned int resv7;		/* 0xD80 */
	unsigned int pkti[31];		/* 0xD84 */
};

struct router_priv {
	char devName[32];
	struct drvmgr_dev *dev;
	struct router_regs *regs;
	int minor;
	int open;
	struct router_hw_info hwinfo;
	int nports;
};

static rtems_device_driver router_initialize(
        rtems_device_major_number  major,
        rtems_device_minor_number  minor,
        void                    * arg
        );

static rtems_device_driver router_open(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        );

static rtems_device_driver router_close(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        );

static rtems_device_driver router_control(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        );

#define ROUTER_DRIVER_TABLE_ENTRY \
  { router_initialize, \
    router_open, \
    router_close, \
    NULL, \
    NULL, \
    router_control }

static void router_hwinfo(
	struct router_priv *priv,
	struct router_hw_info *hwinfo);

static rtems_driver_address_table router_driver = ROUTER_DRIVER_TABLE_ENTRY;
static int router_driver_io_registered = 0;
static rtems_device_major_number router_driver_io_major = 0;

/******************* Driver manager interface ***********************/

/* Driver prototypes */
int router_register_io(rtems_device_major_number *m);

int router_init2(struct drvmgr_dev *dev);

struct drvmgr_drv_ops router_ops = 
{
	.init = {NULL,  router_init2, NULL, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id router_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_SPW_ROUTER},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info router_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_SPW_ROUTER_ID,/* Driver ID */
		"ROUTER_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&router_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct router_priv),	/* Let DRVMGR allocate for us */
	},
	&router_ids[0],
};

void router_register_drv (void)
{
	drvmgr_drv_register(&router_drv_info.general);
}

int router_init2(struct drvmgr_dev *dev)
{
	struct router_priv *priv = dev->priv;
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	char prefix[32];
	rtems_status_code status;

	if ( priv == NULL )
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* Do initialization */
	if ( router_driver_io_registered == 0) {
		/* Register the I/O driver only once for all cores */
		if ( router_register_io(&router_driver_io_major) ) {
			/* Failed to register I/O driver */
			return DRVMGR_FAIL;
		}

		router_driver_io_registered = 1;
	}

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)priv->dev->businfo;
	if ( ambadev == NULL ) {
		return DRVMGR_FAIL;
	}
	pnpinfo = &ambadev->info;
	priv->regs = (struct router_regs *)pnpinfo->ahb_slv->start[0];
	priv->minor = dev->minor_drv;

	/* Register character device in registered region */
	router_hwinfo(priv, &priv->hwinfo);
	priv->open = 0;
	priv->nports = priv->hwinfo.nports_spw + priv->hwinfo.nports_amba +
			priv->hwinfo.nports_fifo;
	if ( (priv->nports < 2) || (priv->nports > 32) )
		return DRVMGR_FAIL;

	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if ( drvmgr_get_dev_prefix(dev, prefix) ) {
		/* Failed to get prefix, make sure of a unique FS name
		 * by using the driver minor.
		 */
		sprintf(priv->devName, "/dev/router%d", dev->minor_drv);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "/dev/%srouter%d", prefix, dev->minor_bus);
	}

	/* Register Device */
	status = rtems_io_register_name(priv->devName, router_driver_io_major, dev->minor_drv);
	if (status != RTEMS_SUCCESSFUL) {
		return DRVMGR_FAIL;
	}

	return DRVMGR_OK;
}

int router_register_io(rtems_device_major_number *m)
{
	rtems_status_code r;

	if ((r = rtems_io_register_driver(0, &router_driver, m)) == RTEMS_SUCCESSFUL) {
		ROUTER_DBG("ROUTER driver successfully registered, major: %d\n", *m);
	} else {
		switch(r) {
		case RTEMS_TOO_MANY:
			printk("ROUTER rtems_io_register_driver failed: RTEMS_TOO_MANY\n");
			return -1;
		case RTEMS_INVALID_NUMBER:  
			printk("ROUTER rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n");
			return -1;
		case RTEMS_RESOURCE_IN_USE:
			printk("ROUTER rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n");
			return -1;
		default:
			printk("ROUTER rtems_io_register_driver failed\n");
			return -1;
		}
	}
	return 0;
}

static rtems_device_driver router_initialize(
	rtems_device_major_number major,
	rtems_device_minor_number minor,
	void *arg
	)
{
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver router_open(
	rtems_device_major_number major,
	rtems_device_minor_number minor,
	void                    * arg
	)
{
	struct router_priv *priv;
	struct drvmgr_dev *dev;

	if ( drvmgr_get_dev(&router_drv_info.general, minor, &dev) ) {
		ROUTER_DBG("Wrong minor %d\n", minor);
		return RTEMS_INVALID_NAME;
	}
	priv = (struct router_priv *)dev->priv;

	if ( !priv || priv->open ) {
		return RTEMS_RESOURCE_IN_USE;
	}

	priv->open = 1;

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver router_close(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        )
{
	struct router_priv *priv;
	struct drvmgr_dev *dev;

	if ( drvmgr_get_dev(&router_drv_info.general, minor, &dev) ) {
		ROUTER_DBG("Wrong minor %d\n", minor);
		return RTEMS_INVALID_NAME;
	}
	priv = (struct router_priv *)dev->priv;

	priv->open = 0;

	return RTEMS_SUCCESSFUL;
}

static void router_hwinfo(
	struct router_priv *priv,
	struct router_hw_info *hwinfo)
{
	unsigned int tmp;

	tmp = REG_READ(&priv->regs->cfgsts);
	hwinfo->nports_spw   = (tmp >> 27) & 0x1f;
	hwinfo->nports_amba  = (tmp >> 22) & 0x1f;
	hwinfo->nports_fifo  = (tmp >> 17) & 0x1f;
	hwinfo->timers_avail = (tmp >>  1) & 0x1;
	hwinfo->pnp_avail    = (tmp >>  0) & 0x1;

	tmp = REG_READ(&priv->regs->ver);
	hwinfo->ver_major   = (tmp >> 24) & 0xff;
	hwinfo->ver_minor   = (tmp >> 16) & 0xff;
	hwinfo->ver_patch   = (tmp >>  8) & 0xff;
	hwinfo->iid         = (tmp >>  0) & 0xff;
}

static int router_config_set(
	struct router_priv *priv,
	struct router_config *cfg)
{
	int i;

	if ( (cfg->flags & (ROUTER_FLG_TPRES|ROUTER_FLG_TRLD)) &&
	     !priv->hwinfo.timers_avail ) {
		return RTEMS_NOT_IMPLEMENTED;
	}

	/* Write only configuration bits in Config register */
	if ( cfg->flags & ROUTER_FLG_CFG ) {
		REG_WRITE(&priv->regs->cfgsts, cfg->config & ~0x4);
	}

	/* Write Instance ID to Version Register */
	if ( cfg->flags & ROUTER_FLG_IID ) {
		REG_WRITE(&priv->regs->ver, cfg->iid);
	}

	/* Write startup-clock-divisor Register */
	if ( cfg->flags & ROUTER_FLG_IDIV ) {
		REG_WRITE(&priv->regs->idiv, cfg->idiv);
	}

	/* Write Timer Prescaler Register */
	if ( cfg->flags & ROUTER_FLG_TPRES ) {
		REG_WRITE(&priv->regs->tprescaler, cfg->timer_prescaler);
	}

	/* Write Timer Reload Register */
	if ( cfg->flags & ROUTER_FLG_TRLD ) {
		for (i=0; i<=priv->nports; i++)
			REG_WRITE(&priv->regs->treload[i], cfg->timer_reload[i]);
	}

	return 0;
}

static int router_config_read(
	struct router_priv *priv,
	struct router_config *cfg)
{
	int i;

	cfg->config = REG_READ(&priv->regs->cfgsts) & ~0xffff0007;
	cfg->iid = REG_READ(&priv->regs->ver) & 0xff;
	cfg->idiv = REG_READ(&priv->regs->idiv) & 0xff;
	cfg->timer_prescaler = REG_READ(&priv->regs->tprescaler);
	for (i=0; i<=priv->nports; i++)
		cfg->timer_reload[i] = REG_READ(&priv->regs->treload[i]);

	return 0;
}

static int router_routes_set(
	struct router_priv *priv,
	struct router_routes *routes)
{
	int i;
	for (i=0; i<224; i++)
		REG_WRITE(&priv->regs->routes[i], routes->route[i]);
	return 0;
}

static int router_routes_read(
	struct router_priv *priv,
	struct router_routes *routes)
{
	int i;
	for (i=0; i<224; i++)
		routes->route[i] = REG_READ(&priv->regs->routes[i]);
	return 0;
}

static int router_ps_set(struct router_priv *priv, struct router_ps *ps)
{
	int i;
	unsigned int *p = &ps->ps[0];
	for (i=0; i<255; i++,p++) 
		REG_WRITE(&priv->regs->psetup[i], *p);
	return 0;
}

static int router_ps_read(struct router_priv *priv, struct router_ps *ps)
{
	int i;
	unsigned int *p = &ps->ps[0];
	for (i=0; i<255; i++,p++) 
		REG_WRITE(&priv->regs->psetup[i], *p);
	return 0;
}

static int router_we_set(struct router_priv *priv, int we)
{
	REG_WRITE(&priv->regs->cfgwe, we & 0x1);
	return 0;
}

static int router_port_ctrl(struct router_priv *priv, struct router_port *port)
{
	unsigned int ctrl, sts;

	if ( port->port > priv->nports )
		return RTEMS_INVALID_NAME;

	ctrl = port->ctrl;
	if ( port->flag & ROUTER_PORTFLG_GET_CTRL ) {
		ctrl = REG_READ(&priv->regs->pctrl[port->port]);
	}
	sts = port->sts;
	if ( port->flag & ROUTER_PORTFLG_GET_STS ) {
		sts = REG_READ(&priv->regs->psts[port->port]);
	}

	if ( port->flag & ROUTER_PORTFLG_SET_CTRL ) {
		REG_WRITE(&priv->regs->pctrl[port->port], port->ctrl);
	}
	if ( port->flag & ROUTER_PORTFLG_SET_STS ) {
		REG_WRITE(&priv->regs->psts[port->port], port->sts);
	}

	port->ctrl = ctrl;
	port->sts = sts;
	return 0;
}

static int router_cfgsts_set(struct router_priv *priv, unsigned int cfgsts)
{
	REG_WRITE(&priv->regs->cfgsts, cfgsts);
	return 0;
}

static int router_cfgsts_read(struct router_priv *priv, unsigned int *cfgsts)
{
	*cfgsts = REG_READ(&priv->regs->cfgsts);
	return 0;
}

static int router_tc_read(struct router_priv *priv, unsigned int *tc)
{
	*tc = REG_READ(&priv->regs->timecode);
	return 0;
}

static rtems_device_driver router_control(
	rtems_device_major_number major,
	rtems_device_minor_number minor,
	void                    * arg
	)
{
	struct router_priv *priv;
	struct drvmgr_dev *dev;
	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *) arg;
	void *argp = (void *)ioarg->buffer;

	if ( drvmgr_get_dev(&router_drv_info.general, minor, &dev) ) {
		ROUTER_DBG("Wrong minor %d\n", minor);
		return RTEMS_INVALID_NAME;
	}
	priv = (struct router_priv *)dev->priv;

	ioarg->ioctl_return = 0;
	switch (ioarg->command) {

	/* Get Hardware support/information available */
	case GRSPWR_IOCTL_HWINFO:
	{
		struct router_hw_info *hwinfo = argp;
		router_hwinfo(priv, hwinfo);
		break;
	}

	/* Set Router Configuration */
	case GRSPWR_IOCTL_CFG_SET:
	{
		struct router_config *cfg = argp;
		return router_config_set(priv, cfg);
	}

	/* Read Router Configuration */
	case GRSPWR_IOCTL_CFG_GET:
	{
		struct router_config *cfg = argp;
		router_config_read(priv, cfg);
		break;
	}

	/* Routes */
	case GRSPWR_IOCTL_ROUTES_SET:
	{
		struct router_routes *routes = argp;
		return router_routes_set(priv, routes);
	}

	case GRSPWR_IOCTL_ROUTES_GET:
	{
		struct router_routes *routes = argp;
		router_routes_read(priv, routes);
		break;
	}

	/* Port Setup */
	case GRSPWR_IOCTL_PS_SET:
	{
		struct router_ps *ps = argp;
		return router_ps_set(priv, ps);
	}

	case GRSPWR_IOCTL_PS_GET:
	{
		struct router_ps *ps = argp;
		router_ps_read(priv, ps);
		break;
	}

	/* Set configuration write enable */
	case GRSPWR_IOCTL_WE_SET:
	{
		return router_we_set(priv, (int)argp);
	}

	/* Set/Get Port Control/Status */
	case GRSPWR_IOCTL_PORT:
	{
		struct router_port *port = argp;
		int result;
		if ( (result=router_port_ctrl(priv, port)) )
			return result;
		break;
	}

	/* Set Router Configuration/Status Register */
	case GRSPWR_IOCTL_CFGSTS_SET:
	{
		return router_cfgsts_set(priv, (int)argp);
	}

	/* Get Router Configuration/Status Register */
	case GRSPWR_IOCTL_CFGSTS_GET:
	{
		unsigned int *cfgsts = argp;
		router_cfgsts_read(priv, cfgsts);
		break;
	}

	/* Get Current Time-Code Register */
	case GRSPWR_IOCTL_TC_GET:
	{
		unsigned int *tc = argp;
		router_tc_read(priv, tc);
		break;
	}

	default: return RTEMS_NOT_IMPLEMENTED;
	}

	return 0;
}
