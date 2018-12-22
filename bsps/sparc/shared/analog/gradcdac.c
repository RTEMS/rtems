/*  ADC / DAC (GRADCDAC) interface implementation
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/gradcdac.h>

/****************** DEBUG Definitions ********************/
#define DBG_IOCTRL 1
#define DBG_TX 2
#define DBG_RX 4

#define DEBUG_FLAGS (DBG_IOCTRL | DBG_RX | DBG_TX )
/* Uncomment for debug output */
/*
#define DEBUG
#define DEBUGFUNCS
*/
#include <grlib/debug_defs.h>

#include <grlib/grlib_impl.h>

struct gradcdac_priv {
	struct gradcdac_regs *regs;	/* Must be first */
	struct drvmgr_dev *dev;
	char devName[48];

	unsigned int freq;
	int irqno;
	int minor;

	void (*isr_adc)(void *cookie, void *arg);
	void (*isr_dac)(void *cookie, void *arg);
	void *isr_adc_arg;
	void *isr_dac_arg;

	int open;
};

/* Global variables */

/* Print Info routines */
void gradcdac_print(void *cookie);

int gradcdac_init2(struct drvmgr_dev *dev);
int gradcdac_init3(struct drvmgr_dev *dev);
int gradcadc_device_init(struct gradcdac_priv *pDev);
void gradcdac_adc_interrupt(void *arg);
void gradcdac_dac_interrupt(void *arg);

struct drvmgr_drv_ops gradcdac_ops = 
{
	.init = {NULL, gradcdac_init2, gradcdac_init3, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id gradcdac_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_GRADCDAC},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info gradcdac_drv_info =
{
	{
		DRVMGR_OBJ_DRV,				/* Driver */
		NULL,					/* Next driver */
		NULL,					/* Device list */
		DRIVER_AMBAPP_GAISLER_GRADCDAC_ID,	/* Driver ID */
		"GRADCDAC_DRV",				/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,			/* Bus Type */
		&gradcdac_ops,
		NULL,					/* Funcs */
		0,					/* No devices yet */
		0,
	},
	&gradcdac_ids[0]
};

void gradcdac_register_drv (void)
{
	DBG("Registering GRADCDAC driver\n");
	drvmgr_drv_register(&gradcdac_drv_info.general);
}

int gradcdac_init2(struct drvmgr_dev *dev)
{
	struct gradcdac_priv *priv;

	DBG("GRADCDAC[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	priv = dev->priv = grlib_calloc(1, sizeof(*priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* This core will not find other cores, so we wait for init2() */

	return DRVMGR_OK;
}


int gradcdac_init3(struct drvmgr_dev *dev)
{
	struct gradcdac_priv *priv = dev->priv;
	char prefix[32];

	if ( !priv )
		return DRVMGR_FAIL;

	if ( gradcadc_device_init(priv) ) {
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
		sprintf(priv->devName, "/dev/gradcdac%d", dev->minor_drv);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "/dev/%sgradcdac%d", prefix, dev->minor_bus);
	}

	return DRVMGR_OK;
}

static void gradcdac_print_dev(struct gradcdac_priv *pDev)
{
	printf("======= GRADCDAC %p =======\n", pDev->regs);
	printf(" Minor:          %d\n", pDev->minor);
	printf(" Dev Name:       %s\n", pDev->devName);
	printf(" RegBase:        %p\n", pDev->regs);
	printf(" IRQ:            %d and %d\n", pDev->irqno, pDev->irqno+1);
	printf(" Core Freq:      %d kHz\n", pDev->freq / 1000);
	printf(" Opened:         %s\n", pDev->open ? "YES" : "NO");

	printf(" CONFIG:         0x%x\n", pDev->regs->config);
	printf(" STATUS:         0x%x\n", pDev->regs->status);
}

void gradcdac_print(void *cookie)
{
	struct drvmgr_dev *dev;
	struct gradcdac_priv *pDev;

	if ( cookie ) {
		gradcdac_print_dev(cookie);
		return;
	}

	/* Show all */
	dev = gradcdac_drv_info.general.dev;
	while (dev) {
		pDev = (struct gradcdac_priv *)dev->priv;
		gradcdac_print_dev(pDev);
		dev = dev->next_in_drv;
	}
}

static void gradcdac_hw_reset(struct gradcdac_regs *regs)
{
	/* Reset core */
	regs->config = 0;
	regs->adrdir = 0;
	regs->adrout = 0;
	regs->data_dir = 0;
	regs->data_out = 0;
}
	
/* Device initialization called once on startup */
int gradcadc_device_init(struct gradcdac_priv *pDev)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)pDev->dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	pDev->irqno = pnpinfo->irq;
	pDev->regs = (struct gradcdac_regs *)pnpinfo->apb_slv->start;
	pDev->minor = pDev->dev->minor_drv;

	/* Reset Hardware before attaching IRQ handler */
	gradcdac_hw_reset(pDev->regs);

	pDev->open = 0;
	
	/* Get frequency in Hz */
	if ( drvmgr_freq_get(pDev->dev, DEV_APB_SLV, &pDev->freq) ) {
		return -1;
	}

	DBG("GRADCDAC frequency: %d Hz\n", pDev->freq);

	return 0;
}

void gradcdac_dac_interrupt(void *arg)
{
	struct gradcdac_priv *pDev = arg;
	if ( pDev->isr_dac ) 
		pDev->isr_dac(pDev, pDev->isr_dac_arg);
}

void gradcdac_adc_interrupt(void *arg)
{
	struct gradcdac_priv *pDev = arg;
	if ( pDev->isr_adc ) 
		pDev->isr_adc(pDev, pDev->isr_adc_arg);
}

void *gradcdac_open(char *devname)
{
	struct gradcdac_priv *pDev;
	struct drvmgr_dev *dev;

	/* Find device by name */
	dev = gradcdac_drv_info.general.dev;
	while ( dev ) {
		pDev = (struct gradcdac_priv *)dev->priv;
		if ( pDev ) {
			if ( strncmp(pDev->devName, devname, sizeof(pDev->devName)) == 0 ) {
				/* Found matching device name */
				break;
			}
		}
		dev = dev->next_in_drv;
	}

	if ( !dev )
		return NULL;

	/* is device busy/taken? */
	if  ( pDev->open )
		return NULL;

	/* Mark device taken */
	pDev->open = 1;

	return pDev;
}

void gradcdac_set_config(void *cookie, struct gradcdac_config *cfg)
{
	struct gradcdac_priv *pDev = cookie;
	unsigned int config=0;

	config = (cfg->dac_ws<<GRADCDAC_CFG_DACWS_BIT)&GRADCDAC_CFG_DACWS;

	if ( cfg->wr_pol )
		config |= GRADCDAC_CFG_WRPOL;

	config |= (cfg->dac_dw<<GRADCDAC_CFG_DACDW_BIT)&GRADCDAC_CFG_DACDW;

	config |= (cfg->adc_ws<<GRADCDAC_CFG_ADCWS_BIT)&GRADCDAC_CFG_ADCWS;

	if ( cfg->rc_pol )
		config |= GRADCDAC_CFG_RCPOL;

	config |= (cfg->cs_mode<<GRADCDAC_CFG_CSMODE_BIT)&GRADCDAC_CFG_CSMODE;

	if ( cfg->cs_pol )
		config |= GRADCDAC_CFG_CSPOL;

	if ( cfg->ready_mode )
		config |= GRADCDAC_CFG_RDYMODE;

	if ( cfg->ready_pol )
		config |= GRADCDAC_CFG_RDYPOL;

	if ( cfg->trigg_pol )
		config |= GRADCDAC_CFG_TRIGPOL;

	config |= (cfg->trigg_mode<<GRADCDAC_CFG_TRIGMODE_BIT)&GRADCDAC_CFG_TRIGMODE;

	config |= (cfg->adc_dw<<GRADCDAC_CFG_ADCDW_BIT)&GRADCDAC_CFG_ADCDW;

	/* Write config */
	pDev->regs->config = config;
}

void gradcdac_get_config(void *cookie, struct gradcdac_config *cfg)
{
	struct gradcdac_priv *pDev = cookie;
	unsigned int config;

	if ( !cfg )
		return;

	/* Get config */
	config = pDev->regs->config;
	
	cfg->dac_ws =  (config&GRADCDAC_CFG_DACWS)>>GRADCDAC_CFG_DACWS_BIT;

	cfg->wr_pol = (config&GRADCDAC_CFG_WRPOL)>>GRADCDAC_CFG_WRPOL_BIT;

	cfg->dac_dw = (config&GRADCDAC_CFG_DACDW)>>GRADCDAC_CFG_DACDW_BIT;

	cfg->adc_ws = (config&GRADCDAC_CFG_ADCWS)>>GRADCDAC_CFG_ADCWS_BIT;

	cfg->rc_pol = (config&GRADCDAC_CFG_RCPOL)>>GRADCDAC_CFG_RCPOL_BIT;
		
	cfg->cs_mode = (config&GRADCDAC_CFG_CSMODE)>>GRADCDAC_CFG_CSMODE_BIT;

	cfg->cs_pol = (config&GRADCDAC_CFG_CSPOL)>>GRADCDAC_CFG_CSPOL_BIT;
		
	cfg->ready_mode = (config&GRADCDAC_CFG_RDYMODE)>>GRADCDAC_CFG_RDYMODE_BIT;
	
	cfg->ready_pol = (config&GRADCDAC_CFG_RDYPOL)>>GRADCDAC_CFG_RDYPOL_BIT;
	
	cfg->trigg_pol = (config&GRADCDAC_CFG_TRIGPOL)>>GRADCDAC_CFG_TRIGPOL_BIT;

	cfg->trigg_mode = (config&GRADCDAC_CFG_TRIGMODE)>>GRADCDAC_CFG_TRIGMODE_BIT;
	
	cfg->adc_dw = (config&GRADCDAC_CFG_ADCDW)>>GRADCDAC_CFG_ADCDW_BIT;
}

void gradcdac_set_cfg(void *cookie, unsigned int config)
{
	struct gradcdac_priv *pDev = cookie;
	pDev->regs->config = config;
}

unsigned int gradcdac_get_cfg(void *cookie)
{
	struct gradcdac_priv *pDev = cookie;
	return pDev->regs->config;
}

unsigned int gradcdac_get_status(void *cookie)
{
	struct gradcdac_priv *pDev = cookie;
	return pDev->regs->status;
}

/* Install IRQ handler for ADC and/or DAC interrupt.
 * The installed IRQ handler(ISR) must read the status 
 * register to clear the pending interrupt avoiding multiple
 * entries to the ISR caused by the same IRQ.
 *
 * \param adc  1=ADC interrupt, 2=ADC interrupt, 3=ADC and DAC interrupt
 * \param isr  Interrupt service routine called when IRQ is fired
 * \param arg  custom argument passed to ISR when called.
 */
int gradcdac_install_irq_handler(void *cookie, int adc, void (*isr)(void *cookie, void *arg), void *arg)
{
	struct gradcdac_priv *pDev = cookie;

	if ( (adc > 3) || !adc )
		return -1;

	if ( adc & GRADCDAC_ISR_ADC ){
		pDev->isr_adc_arg = arg;
		pDev->isr_adc = isr;
		drvmgr_interrupt_register(pDev->dev, GRADCDAC_IRQ_ADC, "gradcdac_adc", gradcdac_adc_interrupt, pDev);
	}

	if ( adc & GRADCDAC_ISR_DAC ){
		pDev->isr_dac_arg = arg;
		pDev->isr_dac = isr;
		drvmgr_interrupt_register(pDev->dev, GRADCDAC_IRQ_DAC, "gradcdac_dac", gradcdac_dac_interrupt, pDev);
	}

	return 0;
}

void gradcdac_uninstall_irq_handler(void *cookie, int adc)
{
	struct gradcdac_priv *pDev = cookie;

	if ( (adc > 3) || !adc )
		return;

	if ( adc & GRADCDAC_ISR_ADC ){
		drvmgr_interrupt_unregister(pDev->dev, GRADCDAC_IRQ_ADC, gradcdac_adc_interrupt, pDev);
		pDev->isr_adc = NULL;
		pDev->isr_adc_arg = NULL;
	}

	if ( adc & GRADCDAC_ISR_DAC ){
		drvmgr_interrupt_unregister(pDev->dev, GRADCDAC_IRQ_DAC, gradcdac_dac_interrupt, pDev);
		pDev->isr_dac = NULL;
		pDev->isr_dac_arg = NULL;
	}
}

/* Make the ADC circuitry initialize a analogue to digital
 * conversion. The result can be read out by gradcdac_adc_convert_try
 * or gradcdac_adc_convert.
 */
void gradcdac_adc_convert_start(void *cookie)
{
	struct gradcdac_priv *pDev = cookie;

	/* Write to ADC Data Input register to start a conversion */
	pDev->regs->adc_din = 0;
}

/* Tries to read the conversion result. If the circuitry is busy 
 * converting the function return a non-zero value, if the conversion 
 * has successfully finished the function return zero.
 *
 * \param digital_value the resulting converted value is placed here
 * \return zero     = ADC conversion complete, digital_value contain current conversion result
 *         positive = ADC busy, digital_value contain previous conversion result
 *         negative = Conversion request failed.
 */
int gradcdac_adc_convert_try(void *cookie, unsigned short *digital_value)
{
	struct gradcdac_priv *pDev = cookie;
	unsigned int status;

	status = pDev->regs->status;

	if ( digital_value ){
		*digital_value = pDev->regs->adc_din;
	}

	if ( gradcdac_ADC_isOngoing(status) )
		return 1;

	if ( gradcdac_ADC_isCompleted(status) )
		return 0;

	/* Failure */
	return -1;
}

/* Waits until the ADC circuity has finished a digital to analogue 
 * conversion. The Waiting is implemented as a busy loop utilizing
 * 100% CPU load.
 */
int gradcdac_adc_convert(void *cookie, unsigned short *digital_value)
{
	struct gradcdac_priv *pDev = cookie;
	unsigned int status;

	do {
		status=gradcdac_get_status(pDev);
	}while ( gradcdac_ADC_isOngoing(status) );

	if ( digital_value )
		*digital_value = pDev->regs->adc_din;

	if ( gradcdac_ADC_isCompleted(status) )
		return 0;

	return -1;
}

/* Try to make the DAC circuitry initialize a digital to analogue 
 * conversion. If the circuitry is busy by a previous conversion
 * the function return a non-zero value, if the conversion is
 * successfully initialized the function return zero.
 */
int gradcdac_dac_convert_try(void *cookie, unsigned short digital_value)
{
	struct gradcdac_priv *pDev = cookie;
	unsigned int status = pDev->regs->status;

	if ( gradcdac_DAC_isOngoing(status) )
		return -1;

	/* Force a new conversion */
	pDev->regs->dac_dout = digital_value;

	/* Return success */
	return 0;
}

/* Initializes a digital to analogue conversion by waiting until 
 * previous conversions is finished before proceeding with the
 * conversion. The Waiting is implemented as a busy loop utilizing
 * 100% CPU load.
 */
void gradcdac_dac_convert(void *cookie, unsigned short digital_value)
{
	struct gradcdac_priv *pDev = cookie;
	unsigned int status;
	
	do {
		status = gradcdac_get_status(pDev);
	}while( gradcdac_DAC_isOngoing(status) );
	
	pDev->regs->dac_dout = digital_value;
}

unsigned int gradcdac_get_adrinput(void *cookie)
{
	struct gradcdac_priv *pDev = cookie;
	return pDev->regs->adrin;
}

void gradcdac_set_adrinput(void *cookie, unsigned int input)
{
	struct gradcdac_priv *pDev = cookie;
	pDev->regs->adrin = input;
}

unsigned int gradcdac_get_adroutput(void *cookie)
{
	struct gradcdac_priv *pDev = cookie;
	return pDev->regs->adrout;
}

void gradcdac_set_adroutput(void *cookie, unsigned int output)
{
	struct gradcdac_priv *pDev = cookie;
	pDev->regs->adrout = output;
}

unsigned int gradcdac_get_adrdir(void *cookie)
{
	struct gradcdac_priv *pDev = cookie;
	return pDev->regs->adrdir;
}

void gradcdac_set_adrdir(void *cookie, unsigned int dir)
{
	struct gradcdac_priv *pDev = cookie;
	pDev->regs->adrdir = dir;
}

unsigned int gradcdac_get_datainput(void *cookie)
{
	struct gradcdac_priv *pDev = cookie;
	return pDev->regs->data_in;
}

void gradcdac_set_datainput(void *cookie, unsigned int input)
{
	struct gradcdac_priv *pDev = cookie;
	pDev->regs->data_in = input;
}

unsigned int gradcdac_get_dataoutput(void *cookie)
{
	struct gradcdac_priv *pDev = cookie;
	return pDev->regs->data_out;
}

void gradcdac_set_dataoutput(void *cookie, unsigned int output)
{
	struct gradcdac_priv *pDev = cookie;
	pDev->regs->data_out = output;
}

unsigned int gradcdac_get_datadir(void *cookie)
{
	struct gradcdac_priv *pDev = cookie;
	return pDev->regs->data_dir;
}

void gradcdac_set_datadir(void *cookie, unsigned int dir)
{
	struct gradcdac_priv *pDev = cookie;
	pDev->regs->data_dir = dir;
}
