/*
 *  BRM driver
 *
 *  COPYRIGHT (c) 2006.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/********** Set defaults **********/

/* default to 16K memory layout */
#define DMA_MEM_128K
#if !defined(DMA_MEM_128K)
 #define DMA_MEM_16K
#endif

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <rtems/bspIo.h>

#include <drvmgr/drvmgr.h>
#include <grlib/b1553brm.h>
#include <grlib/ambapp.h>
#include <grlib/ambapp_bus.h>

#include <grlib/grlib_impl.h>

/* Uncomment for debug output */
/*#define DEBUG 1
#define FUNCDEBUG 1*/
#undef DEBUG
#undef FUNCDEBUG

/* EVENT_QUEUE_SIZE sets the size of the event queue
 */
#define EVENT_QUEUE_SIZE           1024  


#define INDEX(x) ( x&(EVENT_QUEUE_SIZE-1) )

#if 0
#define DBG(x...) printk(x)
#else
#define DBG(x...) 
#endif

#ifdef FUNCDEBUG
#define FUNCDBG(x...) printk(x)
#else
#define FUNCDBG(x...) 
#endif

#define READ_REG(address) (*(volatile unsigned int *)address)
#define READ_DMA(address) grlib_read_uncached16((unsigned int)address)

static rtems_device_driver brm_initialize(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver brm_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver brm_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver brm_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver brm_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver brm_control(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);

#define BRM_DRIVER_TABLE_ENTRY { brm_initialize, brm_open, brm_close, brm_read, brm_write, brm_control }

static rtems_driver_address_table b1553brm_driver = BRM_DRIVER_TABLE_ENTRY;

struct msg {
	unsigned short miw;
	unsigned short time;
	unsigned short data[32];
};
#if defined(DMA_MEM_128K)
struct circ_buf {
	struct msg msgs[9];
};
#elif defined(DMA_MEM_16K)
/* two message queue */
struct circ_buf_2 {
	struct msg msgs[2];
};
/* one message queue */
struct circ_buf_1 {
	struct msg msgs[1];
};
#endif

struct irq_log_list {
        volatile unsigned short iiw;
        volatile unsigned short iaw;
};

typedef struct { 

	struct drvmgr_dev *dev; /* Driver manager device */
	char devName[32]; /* Device Name */
	struct brm_reg *regs;

	unsigned int memarea_base;
	unsigned int memarea_base_remote;
	unsigned int cfg_clksel;
	unsigned int cfg_clkdiv;
	unsigned int cfg_freq;

	/* BRM descriptors */
	struct desc_table {
		volatile unsigned short ctrl;        
		volatile unsigned short top;
		volatile unsigned short cur;
		volatile unsigned short bot;
	} *desc;

	volatile unsigned short *mem;
	/* bc mem struct */
	struct {
		/* BC Descriptors */
		struct {
			unsigned short ctrl; /* control */
			unsigned short cw1;  /* Command word 1*/
			unsigned short cw2;  /* Command word 1*/
			unsigned short dptr; /* Data pointer in halfword offset from bcmem */
			unsigned short tsw[2]; /* status word 1 & 2 */
			unsigned short ba;     /* branch address */
			unsigned short timer;  /* timer value */
		} descs[128]; /* 2k (1024 half words) */

		/* message data */
		struct {
			unsigned short data[32]; /* 1 message's data */
		} msg_data[128]; /* 8k */

#if defined(DMA_MEM_128K)
		/* offset to last 64bytes of 128k */
		unsigned short unused[(64*1024-(128*8+128*32))-16*2];
#elif defined(DMA_MEM_16K)
		unsigned short unused[(8*1024-(128*8+128*32))-16*2];
#endif
		/* interrupt log at 64 bytes from end */
		struct irq_log_list irq_logs[16];
	} *bcmem;

#if defined(DMA_MEM_128K)
	/* Memory structure of a RT being inited, just used
	 * for RT initialization.
	 *
	 * *mesgs[32] fit each minimally 8 messages per sub address.
	 */
	struct {
		/* RX Sub Address descriptors */
		struct desc_table rxsubs[32];
		/* TX Sub Address descriptors */
		struct desc_table txsubs[32];
		/* RX mode code descriptors */
		struct desc_table rxmodes[32];
		/* TX mode code descriptors */
		struct desc_table txmodes[32];

		/* RX Sub Address messages */
		struct circ_buf rxsuba_msgs[32];
		/* TX Sub Address messages */
		struct circ_buf txsuba_msgs[32];
		/* RX Mode Code messages */
		struct circ_buf rxmode_msgs[32];
		/* RX Mode Code messages */
		struct circ_buf txmode_msgs[32];

		/* offset to last 64bytes of 128k: tot-used-needed */
		unsigned short unused[(64*1024-(4*32*4+4*32*9*34))-16*2];

		/* interrupt log at 64 bytes from end */
		struct irq_log_list irq_logs[16];
	} *rtmem;
#elif defined(DMA_MEM_16K)
	/* Memory structure of a RT being inited, just used
	 * for RT initialization.
	 *
	 * circ_buf_2 *mesgs[32] fit each minimally 2 messages per queue.
	 * circ_buf_1 *mesgs[32] fit each minimally 1 messages per queue.
	 */
	struct {
		/* RX Sub Address descriptors */
		struct desc_table rxsubs[32];
		/* TX Sub Address descriptors */
		struct desc_table txsubs[32];
		/* RX mode code descriptors */
		struct desc_table rxmodes[32];
		/* TX mode code descriptors */
		struct desc_table txmodes[32];

		/* RX Sub Address messages */
		struct circ_buf_2 rxsuba_msgs[32];
		/* TX Sub Address messages */
		struct circ_buf_2 txsuba_msgs[32];
		/* RX Mode Code messages */
		struct circ_buf_2 rxmode_msgs[32];
		/* RX Mode Code messages */
		struct circ_buf_1 txmode_msgs[32];

		/* offset to last 64bytes of 16k: tot-used-needed */
		unsigned short unused[8*1024 -(4*32*4 +3*32*2*34 +1*32*1*34) -16*2];

		/* interrupt log at 64 bytes from end */
		struct irq_log_list irq_logs[16];
	} *rtmem;
#else
	#error You must define one DMA_MEM_???K
#endif

	/* Interrupt log list */
	struct irq_log_list *irq_log;
	unsigned int irq;

	/* Received events waiting to be read */
	struct rt_msg *rt_event;
	struct bm_msg *bm_event;

	unsigned int head, tail;

	unsigned int last_read[128];
	unsigned int written[32];

	struct bc_msg *cur_list;

	int tx_blocking, rx_blocking;

	rtems_id rx_sem, tx_sem, dev_sem;
	int minor;
	int irqno;
	unsigned int mode;
#ifdef DEBUG    		
	unsigned int log[EVENT_QUEUE_SIZE*4];
	unsigned int log_i;
#endif

	rtems_id event_id; /* event that may be signalled upon errors, needs to be set through ioctl command BRM_SET_EVENTID */
	unsigned int status;
	int bc_list_fail;
} brm_priv;

static void b1553brm_interrupt(void *arg);
static rtems_device_driver rt_init(brm_priv *brm);

#define OFS(ofs) (((unsigned int)&ofs & 0x1ffff)>>1)

static int b1553brm_driver_io_registered = 0;
static rtems_device_major_number b1553brm_driver_io_major = 0;

/******************* Driver manager interface ***********************/

/* Driver prototypes */
int b1553brm_register_io(rtems_device_major_number *m);
int b1553brm_device_init(brm_priv *pDev);

int b1553brm_init2(struct drvmgr_dev *dev);
int b1553brm_init3(struct drvmgr_dev *dev);
int b1553brm_remove(struct drvmgr_dev *dev);

struct drvmgr_drv_ops b1553brm_ops = 
{
	.init = {NULL, b1553brm_init2, b1553brm_init3, NULL},
	.remove = b1553brm_remove,
	.info = NULL
};

struct amba_dev_id b1553brm_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_B1553BRM},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info b1553brm_drv_info =
{
	{
		DRVMGR_OBJ_DRV,				/* Driver */
		NULL,					/* Next driver */
		NULL,					/* Device list */
		DRIVER_AMBAPP_GAISLER_B1553BRM_ID,	/* Driver ID */
		"B1553BRM_DRV",				/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,			/* Bus Type */
		&b1553brm_ops,
		NULL,					/* Funcs */
		0,					/* No devices yet */
		0,
	},
	&b1553brm_ids[0]
};

void b1553brm_register_drv (void)
{
	DBG("Registering B1553BRM driver\n");
	drvmgr_drv_register(&b1553brm_drv_info.general);
}

int b1553brm_init2(struct drvmgr_dev *dev)
{
	brm_priv *priv;

	DBG("B1553BRM[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);
	priv = dev->priv = grlib_calloc(1, sizeof(*priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* This core will not find other cores, so we wait for init2() */

	return DRVMGR_OK;
}

int b1553brm_init3(struct drvmgr_dev *dev)
{
	brm_priv *priv;
	char prefix[32];
	rtems_status_code status;

	priv = dev->priv;

	/* Do initialization */

	if ( b1553brm_driver_io_registered == 0) {
		/* Register the I/O driver only once for all cores */
		if ( b1553brm_register_io(&b1553brm_driver_io_major) ) {
			/* Failed to register I/O driver */
			dev->priv = NULL;
			return DRVMGR_FAIL;
		}

		b1553brm_driver_io_registered = 1;
	}

	/* I/O system registered and initialized 
	 * Now we take care of device initialization.
	 */

	if ( b1553brm_device_init(priv) ) {
		return DRVMGR_FAIL;
	}

	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if ( drvmgr_get_dev_prefix(dev, prefix) ) {
		/* Failed to get prefix, make sure of a unique FS name
		 * by using the driver minor.
		 */
		sprintf(priv->devName, "/dev/b1553brm%d", dev->minor_drv);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "/dev/%sb1553brm%d", prefix, dev->minor_bus);
	}

	/* Register Device */
	status = rtems_io_register_name(priv->devName, b1553brm_driver_io_major, dev->minor_drv);
	if (status != RTEMS_SUCCESSFUL) {
		return DRVMGR_FAIL;
	}

	return DRVMGR_OK;
}

int b1553brm_remove(struct drvmgr_dev *dev)
{
	/* Stop more tasks to open driver */

	/* Throw out all tasks using this driver */

	/* Unregister I/O node */

	/* Unregister and disable Interrupt */

	/* Free device memory */

	/* Return sucessfully */

	return DRVMGR_OK;
}

/******************* Driver Implementation ***********************/

int b1553brm_register_io(rtems_device_major_number *m)
{
	rtems_status_code r;

	if ((r = rtems_io_register_driver(0, &b1553brm_driver, m)) == RTEMS_SUCCESSFUL) {
		DBG("B1553BRM driver successfully registered, major: %d\n", *m);
	} else {
		switch(r) {
		case RTEMS_TOO_MANY:
			printk("B1553BRM rtems_io_register_driver failed: RTEMS_TOO_MANY\n");
			return -1;
		case RTEMS_INVALID_NUMBER:  
			printk("B1553BRM rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n");
			return -1;
		case RTEMS_RESOURCE_IN_USE:
			printk("B1553BRM rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n");
			return -1;
		default:
			printk("B1553BRM rtems_io_register_driver failed\n");
			return -1;
		}
	}
	return 0;
}

int b1553brm_device_init(brm_priv *pDev)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	union drvmgr_key_value *value;
	unsigned int mem;
	int size;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)pDev->dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	pDev->irqno = pnpinfo->irq;
	/* Two versions of the BRM core. One where the registers are accessed using the AHB bus 
	 * and one where the APB bus is used
	 */
	if ( pnpinfo->ahb_slv ) {
		/* Registers accessed over AHB */
		pDev->regs = (struct brm_reg *)pnpinfo->ahb_slv->start[0];
	} else {
		/* Registers accessed over APB */
		pDev->regs = (struct brm_reg *)pnpinfo->apb_slv->start;
	}
	pDev->minor = pDev->dev->minor_drv;
#ifdef DEBUG
	pDev->log_i = 0;	
	memset(pDev->log,0,sizeof(pDev->log));
#endif

#ifdef DMA_MEM_128K
	size = 128 * 1024;
#else
	size = 16 * 1024;
#endif

	/* Get memory configuration from bus resources */
	value = drvmgr_dev_key_get(pDev->dev, "dmaBaseAdr", DRVMGR_KT_POINTER);
	if (value)
		mem = (unsigned int)value->ptr;

	if (value && (mem & 1)) {
		/* Remote address, address as BRM looks at it. */

		/* Translate the base address into an address that the the CPU can understand */
		pDev->memarea_base_remote = mem & ~1;
		drvmgr_translate_check(pDev->dev, DMAMEM_TO_CPU,
					(void *)pDev->memarea_base_remote,
					(void **)&pDev->memarea_base,
					size);
	} else {
		if (!value) {
			/* Use dynamically allocated memory + 128k for
			 * alignment
			 */
			mem = (unsigned int)grlib_malloc(size + 128 * 1024);
			if (!mem){
				printk("BRM: Failed to allocate HW memory\n\r");
				return -1;
			}
			/* align memory to 128k boundary */
			pDev->memarea_base = (mem + 0x1ffff) & ~0x1ffff;
		} else {
			pDev->memarea_base = mem;
		}

		/* Translate the base address into an address that the BRM core can understand */
		drvmgr_translate_check(pDev->dev, CPUMEM_TO_DMA,
					(void *)pDev->memarea_base,
					(void **)&pDev->memarea_base_remote,
					size);
	}

	/* clear the used memory */
	memset((char *)pDev->memarea_base, 0, size);

	/* Set base address of all descriptors */
	pDev->desc = (struct desc_table *) pDev->memarea_base;
	pDev->mem = (volatile unsigned short *) pDev->memarea_base;
	pDev->irq_log	= (struct irq_log_list *)(pDev->memarea_base + (0xFFE0<<1)); /* last 64byte */

	pDev->bm_event = NULL;
	pDev->rt_event = NULL;

	pDev->cfg_clksel = 0;
	pDev->cfg_clkdiv = 0;
	pDev->cfg_freq = BRM_FREQ_24MHZ;

	value = drvmgr_dev_key_get(pDev->dev, "clkSel", DRVMGR_KT_INT);
	if ( value ) {
		pDev->cfg_clksel = value->i & CLKSEL_MASK;
	}

	value = drvmgr_dev_key_get(pDev->dev, "clkDiv", DRVMGR_KT_INT);
	if ( value ) {
		pDev->cfg_clkdiv = value->i & CLKDIV_MASK;
	}

	value = drvmgr_dev_key_get(pDev->dev, "coreFreq", DRVMGR_KT_INT);
	if ( value ) {
		pDev->cfg_freq = value->i & BRM_FREQ_MASK;
	}

	/* Sel clock so that we can write to BRM's registers */
	pDev->regs->w_ctrl = (pDev->cfg_clksel<<9) | (pDev->cfg_clkdiv<<5);
	/* Reset BRM core */
	pDev->regs->w_ctrl = 1<<10 | READ_REG(&pDev->regs->w_ctrl);

	/* RX Semaphore created with count = 0 */
	if ( rtems_semaphore_create(rtems_build_name('B', 'M', 'R', '0' + pDev->minor),
		0,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING, 
		0,
		&pDev->rx_sem) != RTEMS_SUCCESSFUL ) {
		printk("BRM: Failed to create rx semaphore\n");
		return RTEMS_INTERNAL_ERROR;
	}

	/* TX Semaphore created with count = 1 */
	if ( rtems_semaphore_create(rtems_build_name('B', 'M', 'T', '0' + pDev->minor),
		1,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING, 
		0,
		&pDev->tx_sem) != RTEMS_SUCCESSFUL ){
		printk("BRM: Failed to create tx semaphore\n");
		return RTEMS_INTERNAL_ERROR;
	}

	/* Device Semaphore created with count = 1 */
	if ( rtems_semaphore_create(rtems_build_name('B', 'M', 'D', '0' + pDev->minor),
		1,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING, 
		0,
		&pDev->dev_sem) != RTEMS_SUCCESSFUL ){
		printk("BRM: Failed to create device semaphore\n");
		return RTEMS_INTERNAL_ERROR;
	}

	/* Default to RT-mode */
	rt_init(pDev);

	return 0;
}

static int odd_parity(unsigned int data) {
    unsigned int i=0;

    while(data)
    {
        i++;
        data &= (data - 1);
    } 

    return !(i&1);
}

static void start_operation(brm_priv *brm) {
	unsigned int ctrl = READ_REG(&brm->regs->ctrl);
	brm->regs->ctrl = ctrl | 0x8000;
}

static void stop_operation(brm_priv *brm) {
	unsigned int ctrl = READ_REG(&brm->regs->ctrl);
  brm->regs->ctrl = ctrl & ~0x8000;
}

static int is_executing(brm_priv *brm) {
	unsigned int ctrl = READ_REG(&brm->regs->ctrl);
	return ((ctrl>>15) & 1);
}

static void clr_int_logs(struct irq_log_list *logs){
	int i;
	for(i=0; i<16; i++){
		logs[i].iiw = 0xffff;
		logs[i].iaw = 0x0;
	}
}

unsigned short b1553brm_rt_cmd_legalize[16] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0xffff,
	0xffff,
	0xffff,
	0xfffd,
	0xfe01,
	0xfff2,
	0xffff,
	0xfffd,
	0xfe05,
	0xffff,
};

static rtems_device_driver rt_init(brm_priv *brm) {
	unsigned int i;

	brm->head = brm->tail = 0;
	brm->rx_blocking = brm->tx_blocking = 1;

	if ( brm->bm_event )
		free(brm->bm_event);
	brm->bm_event = NULL;

	if ( brm->rt_event )
		free(brm->rt_event);
	
	brm->bcmem = NULL;
	brm->rtmem = (void *)brm->mem;

	brm->rt_event = grlib_malloc(EVENT_QUEUE_SIZE*sizeof(*brm->rt_event));
  
	if (brm->rt_event == NULL) {
		DBG("BRM driver failed to allocated memory.");
		return RTEMS_NO_MEMORY;
	}

	brm->irq_log = (struct irq_log_list *)&brm->rtmem->irq_logs[0];

	brm->regs->ctrl	  = 0x1912;  /* enable both buses, circular 1 bufmode, broadcast, interrupt log */
	brm->regs->oper	  = 0x0900;  /* configure as RT, with addr 1 */
	brm->regs->imask	 = BRM_RT_ILLCMD_IRQ|BRM_SUBAD_IRQ|BRM_TAPF_IRQ|BRM_DMAF_IRQ|BRM_WRAPF_IRQ|BRM_MERR_IRQ;
	brm->regs->dpoint	= 0;
	brm->regs->ipoint	= OFS(brm->rtmem->irq_logs[0]);
	brm->regs->enhanced  = 0x0000 | brm->cfg_freq;  /* BRM clocked with freq = 12,16,20 or 24MHz */
	brm->regs->w_ctrl	= (brm->cfg_clksel<<9) | (brm->cfg_clkdiv<<5) | 1;
	brm->regs->w_irqctrl = 6;
	brm->regs->w_ahbaddr = brm->memarea_base_remote;
		
	clr_int_logs(brm->irq_log);

	/* Initialize the Legalize register with standard values */
	for (i = 0; i < 16; i++) {
		brm->regs->rt_cmd_leg[i] = b1553brm_rt_cmd_legalize[i];
	}

	/* Init descriptor table 
	 * 
	 * Each circular buffer has room for 8 messages with up to 34 (32 data + miw + time) words (16b) in each.
	 * The buffers must separated by 34 words.
	 */

 
	/* RX Sub-address 0 - 31 */
	for (i = 0; i < 32; i++) {
		brm->rtmem->rxsubs[i].ctrl = 0x00E0;				/* Interrupt: INTX, IWA, and IBRD */
		brm->rtmem->rxsubs[i].top  = OFS(brm->rtmem->rxsuba_msgs[i]);		     /* Top address */
		brm->rtmem->rxsubs[i].cur  = OFS(brm->rtmem->rxsuba_msgs[i]);		     /* Current address */
		brm->rtmem->rxsubs[i].bot  = OFS(brm->rtmem->rxsuba_msgs[i+1]) - sizeof(struct msg)/2; /* Bottom address */	
		brm->last_read[i] = OFS(brm->rtmem->rxsuba_msgs[i]);
	}
	/* TX Sub-address 0 - 31 */
	for (i = 0; i < 32; i++) {
		brm->rtmem->txsubs[i].ctrl  = 0x0060;					/* Interrupt: IWA and IBRD */
		brm->rtmem->txsubs[i].top   = OFS(brm->rtmem->txsuba_msgs[i]);			  /* Top address */
		brm->rtmem->txsubs[i].cur   = OFS(brm->rtmem->txsuba_msgs[i]);			  /* Current address */
		brm->rtmem->txsubs[i].bot   = OFS(brm->rtmem->txsuba_msgs[i+1]) - sizeof(struct msg)/2;	/* Bottom address */
		brm->last_read[i+32]  = OFS(brm->rtmem->txsuba_msgs[i]);
		brm->written[i] = OFS(brm->rtmem->txsuba_msgs[i]);
	}
	/* RX mode code 0 - 31 */
	for (i = 0; i < 32; i++) {
		brm->rtmem->rxmodes[i].ctrl = 0x00E0;					/* Interrupt: INTX, IWA, and IBRD */
		brm->rtmem->rxmodes[i].top  = OFS(brm->rtmem->rxmode_msgs[i]);			  /* Top address */
		brm->rtmem->rxmodes[i].cur  = OFS(brm->rtmem->rxmode_msgs[i]);			  /* Current address */
		brm->rtmem->rxmodes[i].bot  = OFS(brm->rtmem->rxmode_msgs[i+1]) - sizeof(struct msg)/2;	/* Bottom address */
		brm->last_read[i+64] = OFS(brm->rtmem->rxmode_msgs[i]);
	}   
	/* TX mode code 0 - 31 */
	for (i = 0; i < 32; i++) {
		brm->rtmem->txmodes[i].ctrl = 0x0060;					/* Interrupt: IWA and IBRD */
		brm->rtmem->txmodes[i].top  = OFS(brm->rtmem->txmode_msgs[i]);			  /* Top address */
		brm->rtmem->txmodes[i].cur  = OFS(brm->rtmem->txmode_msgs[i]);			  /* Current address */
		brm->rtmem->txmodes[i].bot  = OFS(brm->rtmem->txmode_msgs[i+1]) - sizeof(struct msg)/2;	/* Bottom address */
		brm->last_read[i+96] = OFS(brm->rtmem->txmode_msgs[i]);
	}

#ifdef DEBUG
	printk("b1553BRM DMA_AREA: 0x%x\n", (unsigned int)brm->rtmem);
	printk("LOG: 0x%x\n", &brm->log[0]);
	printk("LOG_I: 0x%x\n", &brm->log_i);
#endif

	brm->mode = BRM_MODE_RT;

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver bc_init(brm_priv *brm){

	if ( brm->bm_event )
		free(brm->bm_event);
	brm->bm_event = NULL;

	if ( brm->rt_event )
		free(brm->rt_event);
	brm->rt_event = NULL;
	
	brm->bcmem = (void *)brm->mem;
	brm->rtmem = NULL;
	brm->irq_log = (struct irq_log_list *)&brm->bcmem->irq_logs[0];
	
	brm->head = brm->tail = 0;
	brm->rx_blocking = brm->tx_blocking = 1;
	
	brm->regs->ctrl	  = 0x0006;  /* ping pong enable and enable interrupt log */
	brm->regs->oper	  = 0x0800;  /* configure as BC */
	brm->regs->imask	 = BRM_EOL_IRQ|BRM_BC_ILLCMD_IRQ|BRM_ILLOP_IRQ|BRM_DMAF_IRQ|BRM_WRAPF_IRQ|BRM_MERR_IRQ;
	brm->regs->dpoint	= 0;
	brm->regs->ipoint	= OFS(brm->bcmem->irq_logs[0]);
	brm->regs->enhanced  = 0x0000 | (brm->cfg_freq&BRM_FREQ_MASK);  /* freq = 24 */
	brm->regs->w_ctrl	= (brm->cfg_clksel<<9) | (brm->cfg_clkdiv<<5) | 1;
	brm->regs->w_irqctrl = 6;
	brm->regs->w_ahbaddr = brm->memarea_base_remote;
	
	clr_int_logs(brm->irq_log);
	
	brm->mode = BRM_MODE_BC;
	
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver bm_init(brm_priv *brm) {


	brm->head = brm->tail = 0;
	brm->rx_blocking = brm->tx_blocking = 1;

	if ( brm->rt_event )
		free(brm->rt_event);
	brm->rt_event = NULL;

	if ( brm->bm_event )
		free(brm->bm_event);
	
	brm->bcmem = NULL;
	brm->rtmem = NULL;
	
	brm->bm_event	 = grlib_malloc(EVENT_QUEUE_SIZE*sizeof(*brm->bm_event));
 
	if (brm->bm_event == NULL) {
		DBG("BRM driver failed to allocated memory.");
		return RTEMS_NO_MEMORY;
	}

	/* end of 16K, fits all current modes (128K, 16K) */
	brm->irq_log = (struct irq_log_list *)&brm->mem[8*1024-16*2];

	brm->regs->ctrl	  = 0x0006;  /* ping pong enable and enable interrupt log */
	brm->regs->oper	  = 0x0A00;  /* configure as BM */
	brm->regs->imask	= BRM_MBC_IRQ|BRM_MERR_IRQ|BRM_DMAF_IRQ;
	brm->regs->dpoint	= 0;
	brm->regs->ipoint	= OFS(brm->mem[8*1024-16*2]);
	brm->regs->mcpoint   = 0;	   /* Command pointer */
	brm->regs->mdpoint   = 0x100;   /* Data pointer */
	brm->regs->mbc	   = 1;	   /* Block count */
	brm->regs->enhanced  = 0x0000 | (brm->cfg_freq&BRM_FREQ_MASK);  /* freq = 24 */
	brm->regs->w_ctrl	= (brm->cfg_clksel<<9) | (brm->cfg_clkdiv<<5) | 1;
	brm->regs->w_irqctrl = 6;
	brm->regs->w_ahbaddr = brm->memarea_base_remote;
	
	clr_int_logs(brm->irq_log);
	
	brm->mode = BRM_MODE_BM;
	
	return RTEMS_SUCCESSFUL;
}


static rtems_device_driver brm_initialize(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver brm_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg) {
	brm_priv *brm;
	struct drvmgr_dev *dev;

	FUNCDBG("brm_open\n");

	if ( drvmgr_get_dev(&b1553brm_drv_info.general, minor, &dev) ) {
		DBG("Wrong minor %d\n", minor);
		return RTEMS_UNSATISFIED;
	}
	brm = (brm_priv *)dev->priv;

	if (rtems_semaphore_obtain(brm->dev_sem, RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT) != RTEMS_SUCCESSFUL) {
		DBG("brm_open: resource in use\n");
		return RTEMS_RESOURCE_IN_USE; /* EBUSY */
	}

	/* Set defaults */
	brm->event_id = 0;

	start_operation(brm);

	/* Register interrupt routine */
	if ( drvmgr_interrupt_register(brm->dev, 0, "b1553brm", b1553brm_interrupt, brm) ) {
		rtems_semaphore_release(brm->dev_sem);
		return RTEMS_UNSATISFIED;
	}

	return RTEMS_SUCCESSFUL;
}
 
static rtems_device_driver brm_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	brm_priv *brm;
	struct drvmgr_dev *dev;

	FUNCDBG("brm_close");
	
	if ( drvmgr_get_dev(&b1553brm_drv_info.general, minor, &dev) ) {
		return RTEMS_UNSATISFIED;
	}
	brm = (brm_priv *)dev->priv;

	drvmgr_interrupt_unregister(brm->dev, 0, b1553brm_interrupt, brm);

	stop_operation(brm);
	rtems_semaphore_release(brm->dev_sem);

	return RTEMS_SUCCESSFUL;
}
 
static int get_rt_messages(brm_priv *brm, void *buf, unsigned int msg_count)
{
	struct rt_msg *dest = (struct rt_msg *) buf;
	int count = 0;

	if (brm->head == brm->tail) {
		return 0;
	}

	do {

		DBG("rt read - head: %d, tail: %d\n", brm->head, brm->tail);
		dest[count++] = brm->rt_event[INDEX(brm->tail++)];
	} while (brm->head != brm->tail && count < msg_count);

	return count;
}

static int get_bm_messages(brm_priv *brm, void *buf, unsigned int msg_count)
{
	struct bm_msg *dest = (struct bm_msg *) buf;
	int count = 0;

	if (brm->head == brm->tail) {
		return 0;
	}

	do {

		DBG("bm read - head: %d, tail: %d\n", brm->head, brm->tail);
		dest[count++] = brm->bm_event[INDEX(brm->tail++)];

	} while (brm->head != brm->tail && count < msg_count);

	return count;
}

static rtems_device_driver brm_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_libio_rw_args_t *rw_args;
	int count = 0;
	brm_priv *brm;
	struct drvmgr_dev *dev;
	int (*get_messages)(brm_priv *brm, void *buf, unsigned int count);

	if ( drvmgr_get_dev(&b1553brm_drv_info.general, minor, &dev) ) {
		return RTEMS_UNSATISFIED;
	}
	brm = (brm_priv *)dev->priv;

	if ( ! (brm->mode & (BRM_MODE_RT | BRM_MODE_BM)) ){
		return RTEMS_INVALID_NAME;
	}

	rw_args = (rtems_libio_rw_args_t *) arg;

	if ( ((READ_REG(&brm->regs->oper)>>8) & 3) == 1 ) { /* RT */
		get_messages = get_rt_messages;
	} else { /* BM */
		get_messages = get_bm_messages;
	}

	FUNCDBG("brm_read [%i,%i]: buf: 0x%x len: %i\n",major, minor, (unsigned int)rw_args->buffer,rw_args->count);

	while ( (count=get_messages(brm,rw_args->buffer, rw_args->count)) == 0 ) {
		if (brm->rx_blocking) {
			rtems_semaphore_obtain(brm->rx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		} else {
			/* Translates to EBUSY */
			return RTEMS_RESOURCE_IN_USE;
		}
	}

	rw_args->bytes_moved = count;
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver brm_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_libio_rw_args_t *rw_args;
	struct rt_msg *source;
	unsigned int count=0, current, next, descriptor, wc, suba;
	brm_priv *brm;
	struct drvmgr_dev *dev;
  
	if ( drvmgr_get_dev(&b1553brm_drv_info.general, minor, &dev) ) {
		return RTEMS_UNSATISFIED;
	}
	brm = (brm_priv *)dev->priv;
	
	if ( ! (brm->mode & BRM_MODE_RT) ){
		return RTEMS_INVALID_NAME;
	}
	
	rw_args = (rtems_libio_rw_args_t *) arg;
	source = (struct rt_msg *) rw_args->buffer;

	FUNCDBG("brm_write [%i,%i]: buf: 0x%x len: %i\n",major, minor, (unsigned int)rw_args->buffer,rw_args->count);

	do {

		descriptor = source[count].desc & 0x7F;
		suba       = descriptor-32;
		wc         = source[count].miw >> 11;
		wc = wc ? wc : 32;

		/* Only subaddress transmission is allowed with write */
		if (descriptor < 32 || descriptor >= 64)
			return RTEMS_INVALID_NAME;

		current = brm->desc[descriptor].cur; 
		next = brm->written[suba] + 2 + wc;

		if (brm->written[suba] < current) {

			if (next > current) {

				/* No room in transmission buffer */ 
				if (brm->tx_blocking && count == 0) {
					rtems_semaphore_obtain(brm->tx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
				} else if ( count > 0 ) {
					/* return the number of messages sent so far */
					break;
				} else {	
					/* Translates to posix EBUSY */
					return RTEMS_RESOURCE_IN_USE;
				}
			}
		}

		memcpy((void *)&brm->mem[brm->written[suba]], &source[count], (2+wc)*2);

		count++;

		if (next >= brm->desc[descriptor].bot) {
			next = brm->desc[descriptor].top;
		}
		brm->written[suba] = next;

	}  while (count < rw_args->count);

	rw_args->bytes_moved = count; 

	if (count >= 0) {
		return RTEMS_SUCCESSFUL;
	}
	return RTEMS_UNSATISFIED;
}

static rtems_device_driver brm_control(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
    
	unsigned int i=0;
	unsigned short ctrl, oper, cw1, cw2;
	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *) arg;
	unsigned int *data = ioarg->buffer;
	struct bc_msg *cmd_list = (struct bc_msg *) ioarg->buffer;
  	brm_priv *brm;
	struct drvmgr_dev *dev;
	rtems_device_driver ret;
	int len, msglen;

	FUNCDBG("brm_control[%d]: [%i,%i]\n", minor, major, minor);

	if ( drvmgr_get_dev(&b1553brm_drv_info.general, minor, &dev) ) {
		return RTEMS_UNSATISFIED;
	}
	brm = (brm_priv *)dev->priv;

	if (!ioarg) {
		DBG("brm_control: invalid argument\n");
		return RTEMS_INVALID_NAME;
	}

	ioarg->ioctl_return = 0;
	switch (ioarg->command) {

		case BRM_SET_MODE:
		if ( data[0] > 2 )
			return RTEMS_INVALID_NAME;
		stop_operation(brm);
		if (data[0] == 0) {
			ret = bc_init(brm);
		} else if (data[0] == 1) { 
			ret = rt_init(brm);				
		} else if (data[0] == 2) { 
			ret = bm_init(brm);						
		} else {
			ret = RTEMS_INVALID_NAME;
		}
		if ( ret != RTEMS_SUCCESSFUL)
			return ret;

		if ( brm->mode & (BRM_MODE_RT | BRM_MODE_BM ) )
			start_operation(brm);
		break;

		case BRM_SET_BUS:
		stop_operation(brm);
		ctrl = READ_REG(&brm->regs->ctrl);
		ctrl &= 0xE7FF;                               /* Clear bit 12-11 ...      */
		ctrl |= (data[0]&0x3)<<11;                    /* ... OR in new bus status */
		brm->regs->ctrl = ctrl;
		start_operation(brm);
		break;

		case BRM_SET_MSGTO:
		stop_operation(brm);
		ctrl = READ_REG(&brm->regs->ctrl);
		ctrl &= 0xFDFF;                               /* Clear bit 9 ...          */
		ctrl |= (data[0]&1)<<9;                       /* ... OR in new MSGTO      */
		brm->regs->ctrl = ctrl;
		start_operation(brm);
		break;

		case BRM_SET_RT_ADDR:   
		stop_operation(brm);
		oper = READ_REG(&brm->regs->oper);
		oper &= 0x03FF;                               /* Clear bit 15-10 ...      */
		oper |= (data[0]&0x1f)<<11;                   /* ... OR in new address    */
		oper |= odd_parity(data[0]&0x1f)<<10;         /* ... OR in parity         */
		brm->regs->oper = oper;
		start_operation(brm);
		break;

		case BRM_SET_STD:   
		stop_operation(brm);
		ctrl = READ_REG(&brm->regs->ctrl);
		ctrl &= 0xFF7F;                               /* Clear bit 7 ...           */
		ctrl |= (data[0]&1)<<7;                       /* ... OR in new ABSTD (1=A) */
		brm->regs->ctrl = ctrl;
		start_operation(brm);
		break;

		case BRM_SET_BCE:
		stop_operation(brm);
		ctrl = READ_REG(&brm->regs->ctrl);
		ctrl &= 0xFFEF;                               /* Clear bit 4 ...           */
		ctrl |= (data[0]&1)<<4;                       /* ... OR in new BCE         */
		brm->regs->ctrl = ctrl;
		start_operation(brm);
		break;

		case BRM_TX_BLOCK:
		brm->tx_blocking     = data[0];      
		break;

		case BRM_RX_BLOCK: 
		brm->rx_blocking     = data[0];   
		break;

		case BRM_DO_LIST:
		if ( brm->mode != BRM_MODE_BC ){
			return RTEMS_INVALID_NAME;
		}

		/* Check if we are bus controller */
		if ( ((READ_REG(&brm->regs->oper)>>8) & 3) != 0 ) {
			return RTEMS_INVALID_NAME;
		}

		/* Already processing list? */
		if (is_executing(brm)) {
			return RTEMS_RESOURCE_IN_USE;
		}

		/* clear any earlier releases */
		rtems_semaphore_obtain(brm->tx_sem, RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT);

		brm->bc_list_fail = 0;
		brm->cur_list = cmd_list;
		brm->regs->dpoint = 0;

		i = 0;
		while ( (cmd_list[i].ctrl & BC_EOL) == 0) {

			ctrl = (4<<12) | (((cmd_list[i].ctrl&BC_BUSA)==BC_BUSA)<<9) | (((cmd_list[i].ctrl&BC_RTRT)==BC_RTRT)<<8);

			if (cmd_list[i].ctrl&BC_RTRT) {
				cw1 = (cmd_list[i].rtaddr[0]<<11) | (0<<10) | (cmd_list[i].subaddr[0]<<5) | (cmd_list[i].wc & 0x1f); /* receive cw */
				cw2 = (cmd_list[i].rtaddr[1]<<11) | (1<<10) | (cmd_list[i].subaddr[1]<<5) | (cmd_list[i].wc & 0x1f); /* transmit cw */
			} else {
				cw1 = (cmd_list[i].rtaddr[0]<<11) | (((cmd_list[i].ctrl&BC_TR)==BC_TR)<<10) | (cmd_list[i].subaddr[0]<<5) | (cmd_list[i].wc&0x1f);
				cw2 = 0;
			}

			/* Set up command block */
			brm->bcmem->descs[i].ctrl = ctrl;
			brm->bcmem->descs[i].cw1 = cw1;
			brm->bcmem->descs[i].cw2 = cw2;
			/* data pointer:
			 * (&brm->bcmem->msg_data[i].data[0] & 0x1ffff) / 2 
			 */
			brm->bcmem->descs[i].dptr = 1024+i*32;  /* data pointer */
			brm->bcmem->descs[i].tsw[0] = 0;
			brm->bcmem->descs[i].tsw[1] = 0;
			brm->bcmem->descs[i].ba = 0;
			brm->bcmem->descs[i].timer = 0;

			msglen = cmd_list[i].wc;
			if ( msglen == 0 ) 
				msglen = 32;
			memcpy((void *)&brm->bcmem->msg_data[i].data[0], &cmd_list[i].data[0], msglen*2);

			i++;
		}

		brm->bcmem->descs[i].ctrl = 0; /* end of list */

		start_operation(brm);        
		break;  

		case BRM_LIST_DONE:

		if ( brm->mode != BRM_MODE_BC ){
			return RTEMS_INVALID_NAME;
		}
				
		/* Check if we are bus controller */
		if ( ((READ_REG(&brm->regs->oper)>>8) & 3) != 0 ) {
			return RTEMS_INVALID_NAME;
		}

		if (is_executing(brm)) {

			data[0] = 0;
			if (brm->tx_blocking) {
				rtems_semaphore_obtain(brm->tx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
				data[0] = 1;
				if ( brm->bc_list_fail ){
					return RTEMS_INVALID_NAME;
				}
			} else {
				return RTEMS_RESOURCE_IN_USE;
			}
		} else {
			data[0] = 1; /* done */
		}

		/* copy finished list results back into bc_msg array */
		i = 0;
		while ( (brm->cur_list[i].ctrl & BC_EOL) == 0) {
			if (READ_DMA(&brm->bcmem->descs[i].ctrl) & 1) {
				brm->cur_list[i].ctrl |= 0x8000; /* Set BAME */ 
			}
			if (brm->cur_list[i].ctrl & BC_TR) {
				/* RT Transmit command, copy received data */
				len = brm->cur_list[i].wc;
				if ( len == 0 )
					len = 32;
				while ( len-- > 0) {
					brm->cur_list[i].data[len] = READ_DMA(&brm->bcmem->msg_data[i].data[len]);
				}
			}
			brm->cur_list[i].tsw[0] = READ_DMA(&brm->bcmem->descs[i].tsw[0]);
			brm->cur_list[i].tsw[1] = READ_DMA(&brm->bcmem->descs[i].tsw[1]);

			i++;
		}
		break;

		case BRM_CLR_STATUS:
		brm->status = 0;
		break;

		case BRM_GET_STATUS: /* copy status */
		if ( !ioarg->buffer )
			return RTEMS_INVALID_NAME;

		*(unsigned int *)ioarg->buffer = brm->status;
		break;
    
		case BRM_SET_EVENTID:
		brm->event_id = (rtems_id)ioarg->buffer;
		break;

		default:
		return RTEMS_NOT_DEFINED;
	}
	return RTEMS_SUCCESSFUL;
}

static void b1553brm_interrupt(void *arg)
{
	brm_priv *brm = arg;
	unsigned short descriptor, current, pending, miw, wc, tmp, ctrl;
	unsigned short msgadr, iaw, iiw;
	int len;
	int signal_event=0, wake_rx_task=0, wake_tx_task=0;
	unsigned int event_status=0;
	int accessed;
	#define SET_ERROR_DESCRIPTOR(descriptor) (event_status = (event_status & 0x0000ffff) | descriptor<<16)
 		
	while( (iiw=READ_DMA(&brm->irq_log[brm->irq].iiw)) != 0xffff ){
		iaw=READ_DMA(&brm->irq_log[brm->irq].iaw);
		
		/* indicate that the interrupt log entry has been processed */
		brm->irq_log[brm->irq].iiw = 0xffff;

		/* Interpret interrupt log entry  */
		descriptor = iaw >> 2;
		pending    = iiw;
		brm->irq = (brm->irq + 1) % 16;
		
		/* Clear the log so that we */


		/* Subaddress accessed irq (RT only) 
		 *
		 * Can be either a receive or transmit command
		 * as well as a mode code.
		*/
		if (pending & BRM_SUBAD_IRQ) {

			/* Pointer to next free message in circular buffer */
			current = READ_DMA(&brm->desc[descriptor].cur);
			ctrl = READ_DMA(&brm->desc[descriptor].ctrl);
#ifdef DEBUG
			brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = (0xff<<16);
			brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = current;
			brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = ctrl;
			brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = 0;
#endif
			accessed = ctrl & 0x10;
			/* Note that current may be equal to bot and top when 
			 * circular buffer one can handle one message.
			 */
			if ( accessed )
			  do {
			  	msgadr = brm->last_read[descriptor];

				/* Get word count */
				miw = READ_DMA(&brm->mem[msgadr]);
				wc  = miw >> 11;

				/* Data received */
				if (descriptor < 32) {
					wc = wc ? wc : 32;
				}
				/* Data transmitted */ 
				else if (descriptor < 64) {
					wc = wc ? wc : 32;  
					wake_tx_task=1;
				}
				/* RX Mode code */
				else if (descriptor < 96) {
					wc = (wc>>4);
				}
				/* TX Mode code */
				else if (descriptor < 128) {
					wc = (wc>>4);
				}

#ifdef DEBUG            
				brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = (descriptor << 16) | wc; 
				brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = current;
				brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = msgadr;
#endif

				/* If there is room in the event queue, copy the event there */
				if (brm->head - brm->tail != EVENT_QUEUE_SIZE) {

					/* Copy to event queue */
					brm->rt_event[INDEX(brm->head)].miw = READ_DMA(&brm->mem[msgadr]);
					brm->rt_event[INDEX(brm->head)].time = READ_DMA(&brm->mem[msgadr+1]);
					len = wc;
					while( len-- > 0){
						brm->rt_event[INDEX(brm->head)].data[len] = READ_DMA(&brm->mem[msgadr+2+len]);
					}
					brm->rt_event[INDEX(brm->head)].desc = descriptor;
					brm->head++;
				}
				else {
					/* Indicate overrun */
					brm->rt_event[INDEX(brm->head)].desc |= 0x8000;
				}

				msgadr += (2+wc);

				if (msgadr >= READ_DMA(&brm->desc[descriptor].bot)) {
					msgadr = READ_DMA(&brm->desc[descriptor].top);
				}
				brm->last_read[descriptor] = msgadr;

#ifdef DEBUG
				brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = msgadr;
#endif
				wake_rx_task = 1;
			  } while ( (msgadr=brm->last_read[descriptor]) != current );
		}

		if (pending & BRM_EOL_IRQ) {  
			wake_tx_task = 1;
		}

		if (pending & BRM_BC_ILLCMD_IRQ) {
			brm->bc_list_fail = 1;
			wake_tx_task = 1;
			SET_ERROR_DESCRIPTOR(descriptor);
			FUNCDBG("BRM: ILLCMD IRQ\n\r");
		}

		/* Monitor irq */
		if (pending & BRM_MBC_IRQ) { 

			stop_operation(brm);
			brm->regs->mbc = 1;
			start_operation(brm);

			/* If there is room in the event queue, copy the event there */
			if (brm->head - brm->tail != EVENT_QUEUE_SIZE) {

				/* Copy to event queue */

				brm->bm_event[INDEX(brm->head)].miw  =  READ_DMA(&brm->mem[0]);
				brm->bm_event[INDEX(brm->head)].cw1  =  READ_DMA(&brm->mem[1]);
				brm->bm_event[INDEX(brm->head)].cw2  =  READ_DMA(&brm->mem[2]);
				brm->bm_event[INDEX(brm->head)].sw1  =  READ_DMA(&brm->mem[4]);
				brm->bm_event[INDEX(brm->head)].sw2  =  READ_DMA(&brm->mem[5]);
				brm->bm_event[INDEX(brm->head)].time =  READ_DMA(&brm->mem[6]);

				len = 32;
				while ( len-- ){
					brm->bm_event[INDEX(brm->head)].data[len] =  READ_DMA(&brm->mem[0x100+len]);
					len--;
					brm->bm_event[INDEX(brm->head)].data[len] =  READ_DMA(&brm->mem[0x100+len]);
					len--;
					brm->bm_event[INDEX(brm->head)].data[len] =  READ_DMA(&brm->mem[0x100+len]);
					len--;
					brm->bm_event[INDEX(brm->head)].data[len] =  READ_DMA(&brm->mem[0x100+len]);
				}
/*				memcpy((void *)brm->bm_event[INDEX(brm->head)].data, &brm->mem[0x100], 32);*/

#ifdef DEBUG
				brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = READ_REG(&brm->regs->mbc) & 0xffff;
				brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = READ_DMA(&brm->mem[0]);
				brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = READ_DMA(&brm->mem[1]);
				brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = READ_DMA(&brm->mem[4]);
#endif

				brm->head++;

			}
			else {
				/* Indicate overrun */
				brm->bm_event[INDEX(brm->head)].miw |= 0x8000;
			}

			/* Wake any blocking thread */
			wake_rx_task = 1;
		}

		/* The reset of the interrupts 
		 * cause a event to be signalled
		 * so that user can handle error.
		 */
		if ( pending & BRM_RT_ILLCMD_IRQ){
			FUNCDBG("BRM: BRM_RT_ILLCMD_IRQ\n\r");
			brm->status |= BRM_RT_ILLCMD_IRQ;
			event_status |= BRM_RT_ILLCMD_IRQ;
			SET_ERROR_DESCRIPTOR(descriptor);
			signal_event=1;
		}

		if ( pending & BRM_ILLOP_IRQ){
			FUNCDBG("BRM: BRM_ILLOP_IRQ\n\r");
			brm->bc_list_fail = 1;
			wake_tx_task = 1;
			event_status |= BRM_ILLOP_IRQ;
			SET_ERROR_DESCRIPTOR(descriptor);			
			signal_event=1;
		}

		if ( pending & BRM_MERR_IRQ){
			FUNCDBG("BRM: BRM_MERR_IRQ\n\r");
			event_status |= BRM_MERR_IRQ;
			SET_ERROR_DESCRIPTOR(descriptor);
			signal_event=1;
		}
		/* Clear Block Accessed Bit */ 
		tmp = READ_DMA(&brm->desc[descriptor].ctrl);
		brm->desc[descriptor].ctrl = tmp & ~0x10;
#ifdef DEBUG
		brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = (0xfe<<16);
		brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = 0;
		brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = tmp & ~0x10;
		brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = tmp;
#endif
	} /* While */

	/* clear interrupt flags & handle Hardware errors */
	pending = READ_REG(&brm->regs->ipend);

	if ( pending & BRM_DMAF_IRQ){
		FUNCDBG("BRM: BRM_DMAF_IRQ\n\r");
		event_status |= BRM_DMAF_IRQ;
		signal_event=1;
	}

	if ( pending & BRM_WRAPF_IRQ){
		FUNCDBG("BRM: BRM_WRAPF_IRQ\n\r");
		event_status |= BRM_WRAPF_IRQ;
		signal_event=1;
	}

	if ( pending & BRM_TAPF_IRQ){
		FUNCDBG("BRM: BRM_TAPF_IRQ\n\r");
		event_status |= BRM_TAPF_IRQ;
		signal_event=1;
	}

	/* Copy current mask to status mask */
	if ( event_status ){
		if ( event_status & 0xffff0000 )
			brm->status &= 0x0000ffff;
		brm->status |= event_status;
	}

	/* Wake any blocked rx thread only on receive interrupts */
	if ( wake_rx_task ) {
		rtems_semaphore_release(brm->rx_sem);
	}

	/* Wake any blocked tx thread only on transmit interrupts */
	if ( wake_tx_task ) {
		rtems_semaphore_release(brm->tx_sem);
	}        

	/* signal event once */
	if ( signal_event && (brm->event_id!=0) ){
		rtems_event_send(brm->event_id, event_status);
	}

}

void b1553brm_print_dev(struct drvmgr_dev *dev, int options)
{
	brm_priv *pDev = dev->priv;
	struct brm_reg *regs = pDev->regs;

	/* Print */
	printf("--- B1553BRM[%d] %s ---\n", pDev->minor, pDev->devName);
	printf(" REGS:            0x%x\n", (unsigned int)pDev->regs);
	printf(" IRQ:             %d\n", pDev->irqno);
	switch (pDev->mode) {
		case BRM_MODE_BC:
			printf(" MODE:            BC\n");
			printf(" DESCS:           0x%x\n", (unsigned int)&pDev->bcmem->descs[0]);
			printf(" DATA:            0x%x\n", (unsigned int)&pDev->bcmem->msg_data[0].data[0]);
			printf(" IRQLOG:          0x%x\n", (unsigned int)&pDev->bcmem->irq_logs[0]);
			break;
		case BRM_MODE_BM:
			printf(" MODE:            BM\n");
			break;
		case BRM_MODE_RT:
			printf(" MODE:            RT\n");
			printf(" RXSUBS:          0x%x\n", (unsigned int)&pDev->rtmem->rxsubs[0]);
			printf(" TXSUBS:          0x%x\n", (unsigned int)&pDev->rtmem->txsubs[0]);
			printf(" RXMODES:         0x%x\n", (unsigned int)&pDev->rtmem->rxmodes[0]);
			printf(" TXOMODES:        0x%x\n", (unsigned int)&pDev->rtmem->txmodes[0]);
			printf(" RXSUBS MSGS:     0x%x\n", (unsigned int)&pDev->rtmem->rxsuba_msgs[0]);
			printf(" TXSUBS MSGS:     0x%x\n", (unsigned int)&pDev->rtmem->txsuba_msgs[0]);
			printf(" RXMODES MSGS:    0x%x\n", (unsigned int)&pDev->rtmem->rxmode_msgs[0]);
			printf(" TXMODES MSGS:    0x%x\n", (unsigned int)&pDev->rtmem->txmode_msgs[0]);
			printf(" IRQLOG:          0x%x\n", (unsigned int)&pDev->rtmem->irq_logs[0]);
			break;
	}
	printf(" CTRL:            0x%x\n", regs->ctrl);
	printf(" OPER:            0x%x\n", regs->oper);
	printf(" CUR_CMD:         0x%x\n", regs->cur_cmd);
	printf(" IMASK:           0x%x\n", regs->imask);
	printf(" IPEND:           0x%x\n", regs->ipend);
	printf(" IPOINT:          0x%x\n", regs->ipoint);
	printf(" BIT_REG:         0x%x\n", regs->bit_reg);
	printf(" TTAG:            0x%x\n", regs->ttag);
	printf(" DPOINT:          0x%x\n", regs->dpoint);
	printf(" SW:              0x%x\n", regs->sw);
	printf(" INITCOUNT:       0x%x\n", regs->initcount);
	printf(" MCPOINT:         0x%x\n", regs->mcpoint);
	printf(" MDPOINT:         0x%x\n", regs->mdpoint);
	printf(" MBC:             0x%x\n", regs->mbc);
	printf(" MFILTA:          0x%x\n", regs->mfilta);
	printf(" MFILTB:          0x%x\n", regs->mfiltb);
	printf(" ENHANCED:        0x%x\n", regs->enhanced);
	printf(" W_CTRL:          0x%x\n", regs->w_ctrl);
	printf(" W_IRQCTRL:       0x%x\n", regs->w_irqctrl);
	printf(" W_AHBADDR:       0x%x\n", regs->w_ahbaddr);
}

void b1553brm_print(int options)
{
	struct amba_drv_info *drv = &b1553brm_drv_info;
	struct drvmgr_dev *dev;

	dev = drv->general.dev;
	while(dev) {
		b1553brm_print_dev(dev, options);
		dev = dev->next_in_drv;
	}
}
