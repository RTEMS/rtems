/*  General part of a AMBA Plug & Play bus driver.
 *
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  This is the general part of the different AMBA Plug & Play
 *  drivers. The drivers are wrappers around this driver, making
 *  the code size smaller for systems with multiple AMBA Plug & 
 *  Play buses.
 *
 *  The BSP define APBUART_INFO_AVAIL in order to add the info routine
 *  used for debugging.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drvmgr/drvmgr.h>
#include <drvmgr/ambapp_bus.h>

#include <bsp.h>
#include <ambapp.h>

/*#define DEBUG 1*/
#define DBG(args...)
/*#define DBG(args...) printk(args)*/

struct grlib_gptimer_regs {
	volatile unsigned int scaler_value;   /* common timer registers */
	volatile unsigned int scaler_reload;
	volatile unsigned int status;
	volatile unsigned int notused;
};

/* AMBA IMPLEMENTATION */

int ambapp_bus_init1(struct drvmgr_bus *bus);
int ambapp_bus_remove(struct drvmgr_bus *bus);
int ambapp_unite(struct drvmgr_drv *drv, struct drvmgr_dev *dev);
int ambapp_int_register(struct drvmgr_dev *dev, int index, const char *info, drvmgr_isr isr, void *arg);
int ambapp_int_unregister(struct drvmgr_dev *dev, int index, drvmgr_isr isr, void *arg);
int ambapp_int_clear(struct drvmgr_dev *dev, int index);
int ambapp_int_mask(struct drvmgr_dev *dev, int index);
int ambapp_int_unmask(struct drvmgr_dev *dev, int index);
int ambapp_get_params(struct drvmgr_dev *dev, struct drvmgr_bus_params *params);
int ambapp_bus_freq_get(
	struct drvmgr_dev *dev,
	int options,
	unsigned int *freq_hz);
void ambapp_dev_info(struct drvmgr_dev *, void (*print)(void *p, char *str), void *p);

struct drvmgr_bus_ops ambapp_bus_ops =
{
	.init		= 
	{
		/* init1 */ ambapp_bus_init1,
		/* init2 */ NULL,
		/* init3 */ NULL,
		/* init4 */ NULL
	},
	.remove		= ambapp_bus_remove,
	.unite		= ambapp_unite,
	.int_register	= ambapp_int_register,
	.int_unregister	= ambapp_int_unregister,
	.int_clear	= ambapp_int_clear,
	.int_mask	= ambapp_int_mask,
	.int_unmask	= ambapp_int_unmask,
	.get_params	= ambapp_get_params,
	.get_freq	= ambapp_bus_freq_get,
#ifdef AMBAPPBUS_INFO_AVAIL
	.get_info_dev	= ambapp_dev_info,
#endif
};

struct ambapp_priv {
	struct ambapp_config		*config;
};

int ambapp_unite(struct drvmgr_drv *drv, struct drvmgr_dev *dev)
{
	struct amba_drv_info *adrv;
	struct amba_dev_id *id;
	struct amba_dev_info *amba;

	if ( !drv || !dev || !dev->parent )
		return 0;

	if ( ! (((drv->bus_type == DRVMGR_BUS_TYPE_AMBAPP) && (dev->parent->bus_type == DRVMGR_BUS_TYPE_AMBAPP)) ||
	       ((drv->bus_type == DRVMGR_BUS_TYPE_AMBAPP_RMAP) && (dev->parent->bus_type == DRVMGR_BUS_TYPE_AMBAPP_RMAP)) ||
	       ((drv->bus_type == DRVMGR_BUS_TYPE_AMBAPP_DIST) && (dev->parent->bus_type == DRVMGR_BUS_TYPE_AMBAPP_DIST)))
	   ) {
		return 0;
	}

	amba = (struct amba_dev_info *)dev->businfo;
	if ( !amba )
		return 0;

	adrv = (struct amba_drv_info *)drv;
	id = adrv->ids;
	if ( !id )
		return 0;
	while( id->vendor != 0 ) {
		if ( (id->vendor == amba->id.vendor) &&
		      (id->device == amba->id.device) ) {
			/* Unite device and driver */
			DBG("DRV 0x%x and DEV 0x%x united\n", (unsigned int)drv, (unsigned int)dev);
			return 1;
		}
		id++;
	}

	return 0;
}

static int ambapp_int_get(struct drvmgr_dev *dev, int index)
{
	int irq;

	/* Relative (positive) or absolute (negative) IRQ number */
	if ( index >= 0 ) {
		/* IRQ Index relative to Cores base IRQ */

		/* Get Base IRQ */
		irq = ((struct amba_dev_info *)dev->businfo)->info.irq;
		if ( irq < 0 )
			return -1;
		irq += index;
	} else {
		/* Absolute IRQ number */
		irq = -index;
	}
	return irq;
}

int ambapp_int_register(
	struct drvmgr_dev *dev,
	int index,
	const char *info,
	drvmgr_isr isr,
	void *arg)
{
	struct ambapp_priv *priv;
	int irq;

	priv = dev->parent->priv;

	/* Get IRQ number from index and device information */
	irq = ambapp_int_get(dev, index);
	if ( irq < 0 ) 
		return DRVMGR_EINVAL;

	DBG("Register interrupt on 0x%x for dev 0x%x (IRQ: %d)\n",
		(unsigned int)dev->parent->dev, (unsigned int)dev, irq);

	if ( priv->config->ops->int_register ) {
		/* Let device override driver default */
		return priv->config->ops->int_register(dev, irq, info, isr, arg);
	} else {
		return DRVMGR_ENOSYS;
	}
}

int ambapp_int_unregister(
	struct drvmgr_dev *dev,
	int index,
	drvmgr_isr isr,
	void *arg)
{
	struct ambapp_priv *priv;
	int irq;

	priv = dev->parent->priv;

	/* Get IRQ number from index and device information */
	irq = ambapp_int_get(dev, index);
	if ( irq < 0 ) 
		return DRVMGR_EINVAL;

	DBG("Unregister interrupt on 0x%x for dev 0x%x (IRQ: %d)\n",
		(unsigned int)dev->parent->dev, (unsigned int)dev, irq);

	if ( priv->config->ops->int_unregister ) {
		/* Let device override driver default */
		return priv->config->ops->int_unregister(dev, irq, isr, arg);
	} else {
		return DRVMGR_ENOSYS;
	}
}

int ambapp_int_clear(
	struct drvmgr_dev *dev,
	int index)
{
	struct ambapp_priv *priv;
	int irq;

	priv = dev->parent->priv;

	/* Get IRQ number from index and device information */
	irq = ambapp_int_get(dev, index);
	if ( irq < 0 ) 
		return -1;

	DBG("Clear interrupt on 0x%x for dev 0x%x (IRQ: %d)\n",
		(unsigned int)dev->parent->dev, (unsigned int)dev, irq);

	if ( priv->config->ops->int_clear ) {
		/* Let device override driver default */
		return priv->config->ops->int_clear(dev, irq);
	} else {
		return DRVMGR_ENOSYS;
	}
}

int ambapp_int_mask(
	struct drvmgr_dev *dev,
	int index)
{
	struct ambapp_priv *priv;
	int irq;

	priv = dev->parent->priv;

	/* Get IRQ number from index and device information */
	irq = ambapp_int_get(dev, index);
	if ( irq < 0 ) 
		return -1;

	DBG("MASK interrupt on 0x%x for dev 0x%x (IRQ: %d)\n",
		(unsigned int)dev->parent->dev, (unsigned int)dev, irq);

	if ( priv->config->ops->int_mask ) {
		/* Let device override driver default */
		return priv->config->ops->int_mask(dev, irq);
	} else {
		return DRVMGR_ENOSYS;
	}
}

int ambapp_int_unmask(
	struct drvmgr_dev *dev,
	int index)
{
	struct ambapp_priv *priv;
	int irq;

	priv = dev->parent->priv;

	/* Get IRQ number from index and device information */
	irq = ambapp_int_get(dev, index);
	if ( irq < 0 ) 
		return DRVMGR_EINVAL;

	DBG("UNMASK interrupt on 0x%x for dev 0x%x (IRQ: %d)\n",
		(unsigned int)dev->parent->dev, (unsigned int)dev, irq);

	if ( priv->config->ops->int_unmask ) {
		/* Let device override driver default */
		return priv->config->ops->int_unmask(dev, irq);
	} else {
		return DRVMGR_ENOSYS;
	}
}

/* Assign frequency to an AMBA Bus */
void ambapp_bus_freq_register(
	struct drvmgr_dev *dev,
	int amba_interface,
	unsigned int freq_hz
	)
{
	struct ambapp_priv *priv = (struct ambapp_priv *)dev->parent->priv;
	struct ambapp_dev *adev;
	struct amba_dev_info *pnp = dev->businfo;

	if ( freq_hz == 0 )
		return;

	if ( amba_interface == DEV_AHB_MST ) {
		adev = (struct ambapp_dev *)
			((unsigned int)pnp->info.ahb_mst -
				sizeof(struct ambapp_dev));
	} else if ( amba_interface == DEV_AHB_SLV ) {
		adev = (struct ambapp_dev *)
			((unsigned int)pnp->info.ahb_slv -
				sizeof(struct ambapp_dev));
	} else if ( amba_interface == DEV_APB_SLV ) {
		adev = (struct ambapp_dev *)
			((unsigned int)pnp->info.apb_slv -
				sizeof(struct ambapp_dev));
	} else {
		return;
	}

	/* Calculate Top bus frequency from lower part. The frequency comes
	 * from some kind of hardware able to report local bus frequency.
	 */
	ambapp_freq_init(priv->config->abus, adev, freq_hz);
}

int ambapp_bus_freq_get(
	struct drvmgr_dev *dev,
	int options,
	unsigned int *freq_hz)
{
	struct ambapp_priv *priv = (struct ambapp_priv *)dev->parent->priv;
	struct ambapp_dev *adev;
	struct amba_dev_info *pnp = dev->businfo;

	if ( options == DEV_AHB_MST ) {
		adev = (struct ambapp_dev *)
			((unsigned int)pnp->info.ahb_mst -
				sizeof(struct ambapp_dev));
	} else if ( options == DEV_AHB_SLV ) {
		adev = (struct ambapp_dev *)
			((unsigned int)pnp->info.ahb_slv -
				sizeof(struct ambapp_dev));
	} else if ( options == DEV_APB_SLV ) {
		adev = (struct ambapp_dev *)
			((unsigned int)pnp->info.apb_slv -
				sizeof(struct ambapp_dev));
	} else {
		*freq_hz = 0;
		return -1;
	}

	/* Calculate core/bus frequency from top most bus frequency. */
	*freq_hz = ambapp_freq_get(priv->config->abus, adev);
	if ( *freq_hz == 0 )
		return -1;
	return 0;
}

int ambapp_get_params(struct drvmgr_dev *dev, struct drvmgr_bus_params *params)
{
	struct ambapp_priv *priv = dev->parent->priv;

	if ( priv->config->ops->get_params ) {
		/* Let device override driver default */
		return priv->config->ops->get_params(dev, params);
	} else {
		return -1;
	}
}

#ifdef AMBAPPBUS_INFO_AVAIL
void ambapp_dev_info(
	struct drvmgr_dev *dev,
	void (*print_line)(void *p, char *str),
	void *p)
{
	struct amba_dev_info *devinfo;
	struct ambapp_core *core;
	char buf[64];
	int ver, i;
	char *str1, *str2, *str3;
	unsigned int ahbmst_freq, ahbslv_freq, apbslv_freq;

	if (!dev)
		return;

	devinfo = (struct amba_dev_info *)dev->businfo;
	if (!devinfo)
		return;
	core = &devinfo->info;

	print_line(p, "AMBA PnP DEVICE");

	str1 = ambapp_vendor_id2str(devinfo->id.vendor);
	if (str1 == NULL)
		str1 = "unknown";
	sprintf(buf, "VENDOR ID:   0x%04x  (%s)", devinfo->id.vendor, str1);
	print_line(p, buf);

	str1 = ambapp_device_id2str(devinfo->id.vendor, devinfo->id.device);
	if (str1 == NULL)
		str1 = "unknown";
	sprintf(buf, "DEVICE ID:   0x%04x  (%s)", devinfo->id.device, str1);
	print_line(p, buf);

	ahbmst_freq = ahbslv_freq = apbslv_freq = 0;
	ver = 0;
	str1 = str2 = str3 = "";
	if (core->ahb_mst) {
		str1 = "AHBMST ";
		ver = core->ahb_mst->ver;
		ambapp_bus_freq_get(dev, DEV_AHB_MST, &ahbmst_freq);
	}
	if (core->ahb_slv) {
		str2 = "AHBSLV ";
		ver = core->ahb_slv->ver;
		ambapp_bus_freq_get(dev, DEV_AHB_SLV, &ahbslv_freq);
	}
	if (core->apb_slv) {
		str3 = "APBSLV";
		ver = core->apb_slv->ver;
		ambapp_bus_freq_get(dev, DEV_APB_SLV, &apbslv_freq);
	}

	sprintf(buf, "IRQ:         %d", ambapp_int_get(dev, 0));
	print_line(p, buf);

	sprintf(buf, "VERSION:     0x%x", ver);
	print_line(p, buf);

	sprintf(buf, "ambapp_core: %p", core);
	print_line(p, buf);

	sprintf(buf, "interfaces:  %s%s%s", str1, str2, str3);
	print_line(p, buf);

	if (ahbmst_freq != 0) {
		sprintf(buf, "AHBMST FREQ: %dkHz", ahbmst_freq/1000);
		print_line(p, buf);
	}

	if (ahbslv_freq != 0) {
		sprintf(buf, "AHBSLV FREQ: %dkHz", ahbslv_freq/1000);
		print_line(p, buf);
	}

	if (apbslv_freq != 0) {
		sprintf(buf, "APBSLV FREQ: %dkHz", apbslv_freq/1000);
		print_line(p, buf);
	}

	if (core->ahb_slv) {
		for(i=0; i<4; i++) {
			if (core->ahb_slv->type[i] == AMBA_TYPE_AHBIO)
				str1 = " ahbio";
			else if (core->ahb_slv->type[i] == AMBA_TYPE_MEM)
				str1 = "ahbmem";
			else
				continue;
			sprintf(buf, " %s[%d]:  0x%08x-0x%08x", str1, i,
				core->ahb_slv->start[i],
				core->ahb_slv->start[i]+core->ahb_slv->mask[i]-1);
			print_line(p, buf);
		}
	}
	if (core->apb_slv) {
		sprintf(buf, "       apb:  0x%08x-0x%08x",
			core->apb_slv->start,
			core->apb_slv->start + core->apb_slv->mask - 1);
		print_line(p, buf);
	}
}
#endif

/* Fix device in last stage and/or register additional devices.
 * Function returns:
 *  0  Register device as normal
 *  1  Fixup function handles registration
 */
static int ambapp_dev_fixup(struct drvmgr_dev *dev, struct amba_dev_info *pnp)
{
	/* OCCAN speciality:
	 *  Mulitple cores are supported through the same amba AHB interface.
	 *  The number of "sub cores" can be detected by decoding the AMBA
	 *  Plug&Play version information. verion = ncores. A maximum of 8
	 *  sub cores are supported, each separeated with 0x100 inbetween.
	 *
	 *  Now, lets detect sub cores.
	 */
	if ( (pnp->info.device == GAISLER_CANAHB) &&
	     (pnp->info.vendor == VENDOR_GAISLER) ) {
		struct drvmgr_dev *newdev, *devs_to_register[8];
		struct amba_dev_info *pnpinfo;
		int subcores;
		int core;

		devs_to_register[0] = dev;
		subcores = (pnp->info.ahb_slv->ver & 0x7) + 1;
		for(core = 1; core < subcores; core++) {
			drvmgr_alloc_dev(&newdev, sizeof(*pnpinfo));
			memcpy(newdev, dev, sizeof(*newdev));
			pnpinfo = (struct amba_dev_info *)(newdev+1);
			memcpy(pnpinfo, pnp, sizeof(*pnp));
			pnpinfo->info.index = core;
			pnpinfo->info.irq += core;
			newdev->businfo = (void *)pnpinfo;

			devs_to_register[core] = newdev;
		}
		/* Register all CAN devices */
		for(core = 0; core < subcores; core++)
			drvmgr_dev_register(devs_to_register[core]);
		return 1;
	} else if ( (pnp->info.device == GAISLER_GPIO) &&
		    (pnp->info.vendor == VENDOR_GAISLER) ) {
		/* PIO[N] is connected to IRQ[N]. */
		pnp->info.irq = 0;
	}
	return 0;
}

struct ambapp_dev_reg_struct {
	struct ambapp_bus	*abus;
	struct drvmgr_bus	*bus;
	struct ambapp_dev	*ahb_mst;
	struct ambapp_dev	*ahb_slv;
	struct ambapp_dev	*apb_slv;
};

static void ambapp_core_register(
	struct ambapp_dev	*ahb_mst,
	struct ambapp_dev	*ahb_slv,
	struct ambapp_dev	*apb_slv,
	struct ambapp_dev_reg_struct *arg
	)
{
	struct drvmgr_dev *newdev;
	struct amba_dev_info *pnpinfo;
	unsigned short device;
	unsigned char vendor;
	int namelen;
	char buf[64];

	if ( ahb_mst ) {
		device = ahb_mst->device;
		vendor = ahb_mst->vendor;
	}else if ( ahb_slv ) {
		device = ahb_slv->device;
		vendor = ahb_slv->vendor;
	}else if( apb_slv ) {
		device = apb_slv->device;
		vendor = apb_slv->vendor;
	} else {
		DBG("NO DEV!\n");
		return;
	}

	DBG("CORE REGISTER DEV [%x:%x] MST: 0x%x, SLV: 0x%x, APB: 0x%x\n", vendor, device, (unsigned int)ahb_mst, (unsigned int)ahb_slv, (unsigned int)apb_slv);

	/* Get unique device name from AMBA data base by combining VENDOR and
	 * DEVICE short names
	 */
	namelen = ambapp_vendev_id2str(vendor, device, buf);

	/* Allocate a device */		
	drvmgr_alloc_dev(&newdev, sizeof(struct amba_dev_info) + namelen);
	pnpinfo = (struct amba_dev_info *)(newdev + 1);
	newdev->parent = arg->bus; /* Ourselfs */
	newdev->minor_drv = 0;
	newdev->minor_bus = 0;
	newdev->priv = NULL;
	newdev->drv = NULL;
	if (namelen > 0) {
		newdev->name = (char *)(pnpinfo + 1);
		strcpy(newdev->name, buf);
	} else {
		newdev->name = NULL;
	}
	newdev->next_in_drv = NULL;
	newdev->bus = NULL;

	/* Init PnP information, Assign Core interfaces with this device */
	pnpinfo->id.vendor = vendor;
	pnpinfo->id.device = device;
	pnpinfo->info.vendor = vendor;
	pnpinfo->info.device = device;
	pnpinfo->info.index = 0;
	if ( ahb_mst ) {
		pnpinfo->info.ahb_mst = (struct ambapp_ahb_info *)
						ahb_mst->devinfo;
		ambapp_alloc_dev(ahb_mst, (void *)newdev);
		if ( pnpinfo->info.ahb_mst->irq )
			pnpinfo->info.irq = pnpinfo->info.ahb_mst->irq;
	}
	if ( ahb_slv ) {
		pnpinfo->info.ahb_slv = (struct ambapp_ahb_info *)
					ahb_slv->devinfo;
		ambapp_alloc_dev(ahb_slv, (void *)newdev);
		if ( pnpinfo->info.ahb_slv->irq )
			pnpinfo->info.irq = pnpinfo->info.ahb_slv->irq;
	}
	if ( apb_slv ) {
		pnpinfo->info.apb_slv = (struct ambapp_apb_info *)
					apb_slv->devinfo;
		ambapp_alloc_dev(apb_slv, (void *)newdev);
		if ( pnpinfo->info.apb_slv->irq )
			pnpinfo->info.irq = pnpinfo->info.apb_slv->irq;
	}
	if ( pnpinfo->info.irq == 0 )
		pnpinfo->info.irq = -1; /* indicate no IRQ */

	/* Connect device with PnP information */
	newdev->businfo = (void *)pnpinfo;

	if ( ambapp_dev_fixup(newdev, pnpinfo) == 0 )
		drvmgr_dev_register(newdev); /* Register New Device */
}

/* Fix device registration.
 * Function returns:
 *  0  Register device as normal
 *  1  Fixup function handles registration
 */
static int ambapp_dev_register_fixup(struct ambapp_dev *dev, struct ambapp_dev_reg_struct *p)
{
	/* GR740 GRPCI2 speciality:
	 * - In the GR740 the APB_SLV is detected before the AHB_SLV
	 *   which makes the registration incorrect. We deal with it in 
	 *   this function. */
	if (    (dev->dev_type == DEV_APB_SLV) &&
		    (dev->device == GAISLER_GRPCI2) &&
		    (dev->vendor == VENDOR_GAISLER) &&
		    (p->ahb_slv == NULL) ) {
		DBG("GRPCI2 APB_SLV detected before AHB_SLV. Skipping APB_SLV registration.\n");
		return 1;
	}
	return 0;
}

/* Register one AMBA device */
static int ambapp_dev_register(struct ambapp_dev *dev, int index, void *arg)
{
	struct ambapp_dev_reg_struct *p = arg;

#ifdef DEBUG
	char *type;

	if ( dev->dev_type == DEV_AHB_MST )
		type = "AHB MST";
	else if ( dev->dev_type == DEV_AHB_SLV )
		type = "AHB SLV";
	else if ( dev->dev_type == DEV_APB_SLV )
		type = "APB SLV";
	
	DBG("Found [%d:%x:%x], %s\n", index, dev->vendor, dev->device, type);
#endif

	/* Fixup for device registration */
	if (ambapp_dev_register_fixup(dev, p)){
		return 0;
	}

	if ( dev->dev_type == DEV_AHB_MST ) {
		if ( p->ahb_mst ) {
			/* This should not happen */
			printk("ambapp_dev_register: ahb_mst not NULL!\n");
			exit(1);
		}

		/* Remember AHB Master */
		p->ahb_mst = dev;

		/* Find AHB Slave and APB slave for this Core */
		ambapp_for_each(p->abus, (OPTIONS_AHB_SLVS|OPTIONS_APB_SLVS|OPTIONS_FREE), dev->vendor, dev->device, ambapp_dev_register, p);

		ambapp_core_register(p->ahb_mst, p->ahb_slv, p->apb_slv, p);
		p->ahb_mst = p->ahb_slv = p->apb_slv = NULL;
		return 0;

	} else if ( dev->dev_type == DEV_AHB_SLV ) {
		if ( p->ahb_slv ) {
			/* Already got our AHB Slave interface */
			return 0;
		}

		/* Remember AHB Slave */
		p->ahb_slv = dev;

		if ( p->ahb_mst ) {
			/* Continue searching for APB Slave */
			return 0;
		} else {
			/* Find APB Slave interface for this Core */
			ambapp_for_each(p->abus, (OPTIONS_APB_SLVS|OPTIONS_FREE), dev->vendor, dev->device, ambapp_dev_register, p);

			ambapp_core_register(p->ahb_mst, p->ahb_slv, p->apb_slv, p);
			p->ahb_mst = p->ahb_slv = p->apb_slv = NULL;
			return 0;
		}
	} else if ( dev->dev_type == DEV_APB_SLV ) {
		if ( p->apb_slv ) {
			/* This should not happen */
			printk("ambapp_dev_register: apb_slv not NULL!\n");
			exit(1);
		}
		/* Remember APB Slave */
		p->apb_slv = dev;

		if ( p->ahb_mst || p->ahb_slv ) {
			/* Stop scanning */
			return 1;
		} else {
			ambapp_core_register(p->ahb_mst, p->ahb_slv, p->apb_slv, p);
			p->ahb_mst = p->ahb_slv = p->apb_slv = NULL;
			return 0;
		}
	}

	return 0;
}

/* Register all AMBA devices available on the AMBAPP bus */
static int ambapp_ids_register(struct drvmgr_bus *bus)
{
	struct ambapp_priv *priv = bus->priv;
	struct ambapp_bus *abus;
	struct ambapp_dev_reg_struct arg;

	DBG("ambapp_ids_register:\n");

	memset(&arg, 0, sizeof(arg));

	abus = priv->config->abus;
	arg.abus = abus;
	arg.bus = bus;

	/* Combine the AHB MST, AHB SLV and APB SLV interfaces of a core. A core has often more than
	 * one interface. A core can not have more than one interface of the same type.
	 */
	ambapp_for_each(abus, (OPTIONS_ALL_DEVS|OPTIONS_FREE), -1, -1, ambapp_dev_register, &arg);

#ifdef DEBUG
	ambapp_print(abus->root, 1);
#endif

	return DRVMGR_OK;
}

/*** DEVICE FUNCTIONS ***/

int ambapp_bus_register(struct drvmgr_dev *dev, struct ambapp_config *config)
{
	struct ambapp_priv *priv;

	if ( !config || !config->ops )
		return DRVMGR_OK;

	DBG("AMBAPP BUS: initializing\n");

	/* Register BUS */
	drvmgr_alloc_bus(&dev->bus, sizeof(struct ambapp_priv));
	priv = (struct ambapp_priv *)(dev->bus + 1);
	priv->config = config;
	if ( priv->config->bus_type == DRVMGR_BUS_TYPE_AMBAPP_DIST )
		dev->bus->bus_type = DRVMGR_BUS_TYPE_AMBAPP_DIST;
	else if ( priv->config->bus_type == DRVMGR_BUS_TYPE_AMBAPP_RMAP )
		dev->bus->bus_type = DRVMGR_BUS_TYPE_AMBAPP_RMAP;
	else
		dev->bus->bus_type = DRVMGR_BUS_TYPE_AMBAPP;
	dev->bus->next = NULL;
	dev->bus->dev = dev;
	dev->bus->priv = priv;
	dev->bus->children = NULL;
	dev->bus->ops = &ambapp_bus_ops;
	dev->bus->funcs = config->funcs;
	dev->bus->dev_cnt = 0;
	dev->bus->reslist = NULL;
	dev->bus->maps_up = config->maps_up;
	dev->bus->maps_down = config->maps_down;

	/* Add resource configuration */
	if ( priv->config->resources )
		drvmgr_bus_res_add(dev->bus, priv->config->resources);

	drvmgr_bus_register(dev->bus);

	return DRVMGR_OK;
}

/*** BUS INITIALIZE FUNCTIONS ***/

/* Initialize the bus, register devices on this bus */
int ambapp_bus_init1(struct drvmgr_bus *bus)
{
	/* Initialize the bus, register devices on this bus */
	return ambapp_ids_register(bus);
}

int ambapp_bus_remove(struct drvmgr_bus *bus)
{
	return DRVMGR_OK;
}
