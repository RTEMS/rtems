/*
 *  BRM driver
 *
 *  COPYRIGHT (c) 2006.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

/********** Set defaults **********/

/* basic bus/interface of device,
 * Default to direct accessed AMBA bus.
 */
#ifndef B1553BRM_NO_AMBA
 #define B1553BRM_AMBA
 #undef B1553BRM_PCI
#endif

/* default name to /dev/brm */
#if !defined(B1553BRM_DEVNAME) || !defined(B1553BRM_DEVNAME_NO)
 #undef B1553BRM_DEVNAME
 #undef B1553BRM_DEVNAME_NO
 #define B1553BRM_DEVNAME "/dev/brm0"
 #define B1553BRM_DEVNAME_NO(devstr,no) ((devstr)[8]='0'+(no))
#endif

#ifndef B1553BRM_PREFIX
 #define B1553BRM_PREFIX(name) b1553brm##name
#endif

/* default to no translation */
#ifndef B1553BRM_ADR_TO
 #define memarea_to_hw(x) ((unsigned int)(x))
#endif

#ifndef B1553BRM_REG_INT
 #define B1553BRM_REG_INT(handler,irqno,arg) set_vector(handler,(irqno)+0x10,1)
  #undef  B1553BRM_DEFINE_INTHANDLER
  #define B1553BRM_DEFINE_INTHANDLER
#endif

/* default to 128K memory layout */
#if !defined(DMA_MEM_16K)
 #define DMA_MEM_128K
#endif

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <rtems/bspIo.h>

#include <b1553brm.h>
#include <ambapp.h>

/* Uncomment for debug output */
/*#define DEBUG 1
#define FUNCDEBUG 1*/
#undef DEBUG
#undef FUNCDEBUG

/* EVENT_QUEUE_SIZE sets the size of the event queue
 */
#define EVENT_QUEUE_SIZE           1024


#define INDEX(x) ( x&(EVENT_QUEUE_SIZE-1) )

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

#ifdef FUNCDEBUG
#define FUNCDBG(x...) printk(x)
#else
#define FUNCDBG(x...)
#endif

#define READ_REG(address) _BRM_REG_READ16((unsigned int)address)
#define READ_DMA(address) _BRM_REG_READ16((unsigned int)address)
static __inline__ unsigned short _BRM_REG_READ16(unsigned int addr) {
	unsigned short tmp;
	__asm__ (" lduha [%1]1, %0 "
	  : "=r"(tmp)
	  : "r"(addr)
	);
	return tmp;
}

static rtems_device_driver brm_initialize(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver brm_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver brm_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver brm_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver brm_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver brm_control(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);

#define BRM_DRIVER_TABLE_ENTRY { brm_initialize, brm_open, brm_close, brm_read, brm_write, brm_control }

static rtems_driver_address_table brm_driver = BRM_DRIVER_TABLE_ENTRY;

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

    unsigned int memarea_base;
    struct brm_reg *regs;

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

static int brm_cores;
static unsigned int allbrm_memarea;
static brm_priv *brms;
static struct ambapp_bus *amba_bus;
static unsigned int	allbrm_cfg_clksel;
static unsigned int allbrm_cfg_clkdiv;
static unsigned int allbrm_cfg_freq;

static void brm_interrupt(brm_priv *brm);
#ifdef B1553BRM_DEFINE_INTHANDLER
static void b1553brm_interrupt_handler(rtems_vector_number v);
#endif

#define OFS(ofs) (((unsigned int)&ofs & 0x1ffff)>>1)

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

#ifdef LEON3
#ifndef DONT_DEF_RAMON
int brm_register_leon3_ramon_fpga(void){
	/* Clock div & Clock sel is NOT available.
	 * The BRM is always clocked with 24MHz.
	 * 3 in BRM enhanced register will select 24MHz
	 */
	return b1553brm_register(&ambapp_plb, 0, 0, 3);
}

int brm_register_leon3_ramon_asic(void){
	/* Clock div & Clock sel is available.
	 * Clkdiv only matter when clksel is 1.
	 * clksel=2, clkdiv=don't care, brm_frq=24MHz
	 *
	 * 3 in BRM enhanced register will select 24MHz
	 */
	return b1553brm_register(&ambapp_plb, 2, 0, 3);
}
#endif
#endif

int B1553BRM_PREFIX(_register)(struct ambapp_bus *bus, unsigned int clksel, unsigned int clkdiv, unsigned int brm_freq)
{
    rtems_status_code r;
    rtems_device_major_number m;

    FUNCDBG("brm_register:\n\r");

		/* save amba bus pointer */
		amba_bus = bus;
		if ( !bus ){
			printk("brm_register: bus is NULL\n\r");
			return 1;
		}

#ifdef B1553BRM_LOCAL_MEM
		allbrm_memarea = B1553BRM_LOCAL_MEM_ADR;
#else
		allbrm_memarea = 0;
#endif

		/* Save clksel, clkdiv and brm_freq for later use */
		allbrm_cfg_clksel = clksel & CLKSEL_MASK;
		allbrm_cfg_clkdiv = clkdiv & CLKDIV_MASK;
		allbrm_cfg_freq = brm_freq & BRM_FREQ_MASK;

    if ((r = rtems_io_register_driver(0, &brm_driver, &m)) == RTEMS_SUCCESSFUL) {
        DBG("BRM: driver successfully registered, major: %d\n",m);

    } else {
        switch(r) {
        case RTEMS_TOO_MANY:
            printk("BRM rtems_io_register_driver failed: RTEMS_TOO_MANY\n"); break;
        case RTEMS_INVALID_NUMBER:
            printk("BRM rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n"); break;
        case RTEMS_RESOURCE_IN_USE:
            printk("BRM rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n"); break;
        default:
           printk("BRM rtems_io_register_driver failed\n");
        }
				return 1;
    }
		return 0;
}

static void clr_int_logs(struct irq_log_list *logs){
	int i;
	for(i=0; i<16; i++){
		logs[i].iiw = 0xffff;
		logs[i].iaw = 0x0;
	}
}

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

	brm->rt_event = (struct rt_msg *) malloc(EVENT_QUEUE_SIZE*sizeof(struct rt_msg));

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
	brm->regs->enhanced  = 0x0000 | allbrm_cfg_freq;  /* BRM clocked with freq = 12,16,20 or 24MHz */
	brm->regs->w_ctrl	= (allbrm_cfg_clksel<<9) | (allbrm_cfg_clkdiv<<5) | 1;
	brm->regs->w_irqctrl = 6;
	brm->regs->w_ahbaddr = (unsigned int) memarea_to_hw(brm->memarea_base);

	clr_int_logs(brm->irq_log);

	/* Legalize all commands */
	for (i = 0; i < 16; i++) {
		brm->regs->rt_cmd_leg[i] = 0;
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
	printk("Set BC interrupt log: 0x%lx, 0x%lx, 0x%lx\n",OFS(brm->bcmem->irq_logs[0]),&brm->bcmem->irq_logs[0],brm->bcmem);
	brm->regs->ipoint	= OFS(brm->bcmem->irq_logs[0]);
	brm->regs->enhanced  = 0x0000 | (allbrm_cfg_freq&0x3);  /* freq = 24 */
	brm->regs->w_ctrl	= (allbrm_cfg_clksel<<9) | (allbrm_cfg_clkdiv<<5) | 1;
	brm->regs->w_irqctrl = 6;
	brm->regs->w_ahbaddr = (unsigned int) memarea_to_hw(brm->memarea_base);

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

	brm->bm_event	 = (struct bm_msg *) malloc(EVENT_QUEUE_SIZE*sizeof(struct bm_msg));

	if (brm->bm_event == NULL) {
		DBG("BRM driver failed to allocated memory.");
		return RTEMS_NO_MEMORY;
	}

	/* end of 16K, fits all current modes (128K, 16K) */
	brm->irq_log = (struct irq_log_list *)&brm->mem[8*1024-16*2];

	brm->regs->ctrl	  = 0x0006;  /* ping pong enable and enable interrupt log */
	brm->regs->oper	  = 0x0A00;  /* configure as BM */
	brm->regs->imask	= BRM_MBC_IRQ|BRM_MERR_IRQ|BRM_DMAF_IRQ|BRM_MERR_IRQ;
	brm->regs->dpoint	= 0;
	brm->regs->ipoint	= OFS(brm->mem[8*1024-16*2]);
	brm->regs->mcpoint   = 0;	   /* Command pointer */
	brm->regs->mdpoint   = 0x100;   /* Data pointer */
	brm->regs->mbc	   = 1;	   /* Block count */
	brm->regs->enhanced  = 0x0000 | (allbrm_cfg_freq&0x3);  /* freq = 24 */
	brm->regs->w_ctrl	= (allbrm_cfg_clksel<<9) | (allbrm_cfg_clkdiv<<5) | 1;
	brm->regs->w_irqctrl = 6;
	brm->regs->w_ahbaddr = (unsigned int) memarea_to_hw(brm->memarea_base);

	clr_int_logs(brm->irq_log);

	brm->mode = BRM_MODE_BM;

	return RTEMS_SUCCESSFUL;
}


static rtems_device_driver brm_initialize(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_status_code status;
	int dev_cnt;
	char fs_name[20];
	brm_priv *brm;
	struct ambapp_ahb_info ambadev;
	char *mem;

	FUNCDBG("brm_initialize\n");

	brm_cores = 0;
	strcpy(fs_name,B1553BRM_DEVNAME);

	/* Find all BRM devices */
	dev_cnt = ambapp_get_number_ahbslv_devices(amba_bus, VENDOR_GAISLER, GAISLER_B1553BRM);
	if ( dev_cnt < 1 ){
		/* Failed to find any CAN cores! */
		printk("BRM: Failed to find any BRM cores\n\r");
		return -1;
	}

	/* allocate & zero memory for the brm devices */
	brms = (brm_priv *)malloc(sizeof(*brms)*dev_cnt);
	if ( !brms ){
		printk("BRM: Failed to allocate SW memory\n\r");
		return -1;
	}
	memset(brms,0,sizeof(*brms)*dev_cnt);

	/* Allocate memory for all device's descriptors,
	 * they must be aligned to a XXX byte boundary.
	 */
	#define BRM_DESCS_PER_CTRL 128
	if ( allbrm_memarea ){
		mem = (char *)allbrm_memarea;
	}else{
 		/* sizeof(struct desc_table) * BRM_DESCS_PER_CTRL * dev_cnt */
		mem = (char *)malloc( (128*1024) * (dev_cnt+1)); /* 128k per core + 128k for alignment */
		if ( !mem ){
			free(brms);
			printk("BRM: Failed to allocate HW memory\n\r");
			return -1;
		}

		/* align memory to 128k boundary */
		mem = (char *)(((unsigned int)mem+0x1ffff) & ~0x1ffff);
	}

	/* clear the used memory */
	memset(mem,0,(128*1024) * dev_cnt);

	/* initialize each brm device, one at a time */
	for(minor=0; minor<dev_cnt; minor++){
		brm = &brms[minor];

		/* Get AMBA AHB device info from Plug&Play */
		ambapp_find_ahbslv_next(amba_bus, VENDOR_GAISLER,
		                        GAISLER_B1553BRM, &ambadev, minor);

		/* Copy Basic HW info */
		brm->regs = (void *)ambadev.start[0];
		brm->irqno = ambadev.irq;
		brm->minor = minor;
		brm->irq = 0;
#ifdef DEBUG
		brm->log_i = 0;
		memset(brm->log,0,sizeof(brm->log));
#endif

		/* Set unique name */
		B1553BRM_DEVNAME_NO(fs_name,minor);

    DBG("Registering BRM core at [0x%x] irq %d, minor %d as %s\n",brm->regs,brm->irqno,minor,fs_name);

		/* Bind filesystem name to device number (minor) */
		status = rtems_io_register_name(fs_name, major, minor);
    if (status != RTEMS_SUCCESSFUL)
			rtems_fatal_error_occurred(status);

		/* RX Semaphore created with count = 0 */
		if ( rtems_semaphore_create(rtems_build_name('B', 'M', 'R', '0'+minor),
		         0,
		         RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		         0,
		         &brm->rx_sem) != RTEMS_SUCCESSFUL ){
      printk("BRM: Failed to create rx semaphore\n");
	  	return RTEMS_INTERNAL_ERROR;
    }

			/* TX Semaphore created with count = 1 */
		if ( rtems_semaphore_create(rtems_build_name('B', 'M', 'T', '0'+minor),
		         1,
		         RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		         0,
		         &brm->tx_sem) != RTEMS_SUCCESSFUL ){
      printk("BRM: Failed to create tx semaphore\n");
	  	return RTEMS_INTERNAL_ERROR;
    }

		/* Device Semaphore created with count = 1 */
		if ( rtems_semaphore_create(rtems_build_name('B', 'M', 'D', '0'+minor),
		         1,
		         RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		         0,
		         &brm->dev_sem) != RTEMS_SUCCESSFUL ){
      printk("BRM: Failed to create device semaphore\n");
	  	return RTEMS_INTERNAL_ERROR;
    }


		/* Set base address of all descriptors */
		brm->memarea_base = (unsigned int)&mem[(128*1024) * minor];
		brm->desc = (struct desc_table *) brm->memarea_base;
		brm->mem = (volatile unsigned short *) brm->memarea_base;
 	  brm->irq_log	= (struct irq_log_list *)(brm->memarea_base + (0xFFE0<<1)); /* last 64byte */

		brm->bm_event = NULL;
		brm->rt_event = NULL;

		/* Sel clock so that we can write to BRM's registers */
		brm->regs->w_ctrl = (allbrm_cfg_clksel<<9) | (allbrm_cfg_clkdiv<<5);
		/* Reset BRM core */
		brm->regs->w_ctrl = 1<<10 | READ_REG(&brm->regs->w_ctrl);

		/* Register interrupt handler */
		B1553BRM_REG_INT(B1553BRM_PREFIX(_interrupt_handler), brm->irqno, brm);

		rt_init(brm);

		DBG("BRM: LOG: 0x%lx, 0x%lx\n\r",brm->log,brm);
	}

	/* save number of BRM cores found */
	brm_cores = dev_cnt;

	DBG("BRM initialisation done.\n");

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver brm_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg) {
		brm_priv *brm;

    FUNCDBG("brm_open\n");

    if (minor >= brm_cores) {
        DBG("Wrong minor %d\n", minor);
        return RTEMS_UNSATISFIED; /* ENODEV */
    }

		brm = &brms[minor];

    if (rtems_semaphore_obtain(brm->dev_sem, RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT) != RTEMS_SUCCESSFUL) {
        DBG("brm_open: resource in use\n");
        return RTEMS_RESOURCE_IN_USE; /* EBUSY */
    }

		/* Set defaults */
		brm->event_id = 0;

    start_operation(brm);

    return RTEMS_SUCCESSFUL;
}

static rtems_device_driver brm_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	brm_priv *brm = &brms[minor];
	FUNCDBG("brm_close");

	stop_operation(brm);
	rtems_semaphore_release(brm->dev_sem);

	return RTEMS_SUCCESSFUL;
}

static int get_rt_messages(brm_priv *brm, void *buf, unsigned int msg_count) {

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

static int get_bm_messages(brm_priv *brm, void *buf, unsigned int msg_count) {

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
		brm_priv *brm = &brms[minor];
    int (*get_messages)(brm_priv *brm, void *buf, unsigned int count);

		if ( ! (brm->mode & (BRM_MODE_RT | BRM_MODE_BM)) ){
			return RTEMS_INVALID_NAME;
		}

    rw_args = (rtems_libio_rw_args_t *) arg;

    if ( ((READ_REG(&brm->regs->oper)>>8) & 3) == 1 ) { /* RT */
        get_messages = get_rt_messages;
    }
    else { /* BM */
        get_messages = get_bm_messages;
    }


    FUNCDBG("brm_read [%i,%i]: buf: 0x%x len: %i\n",major, minor, (unsigned int)rw_args->buffer,rw_args->count);

    while ( (count=get_messages(brm,rw_args->buffer, rw_args->count)) == 0 ) {

        if (brm->rx_blocking) {
            rtems_semaphore_obtain(brm->rx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
        }
        else {
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
		brm_priv *brm = &brms[minor];

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
                }
								else if ( brm->tx_blocking && (count != 0) ){
									/* return the number of messages sent so far */
									break;
								}
                else {
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
		brm_priv *brm = &brms[minor];
		rtems_device_driver ret;
		int len;

    FUNCDBG("brm_control[%d]: [%i,%i]\n",minor,major, minor);

    if (!ioarg) {
        DBG("brm_control: invalid argument\n");
        return RTEMS_INVALID_NAME;
    }

    ioarg->ioctl_return = 0;
    switch(ioarg->command) {

    case BRM_SET_MODE:
			if ( data[0] > 2 )
				return RTEMS_INVALID_NAME;
      stop_operation(brm);
        if (data[0] == 0) {
            ret = bc_init(brm);
        }
        else if (data[0] == 1) {
            ret = rt_init(brm);
        }
        else if (data[0] == 2) {
            ret = bm_init(brm);
        }else
					ret = RTEMS_INVALID_NAME;

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
            }
            else {
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

            memcpy((void *)&brm->bcmem->msg_data[i].data[0], &cmd_list[i].data[0], cmd_list[i].wc*2);

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
            }else{
							return RTEMS_RESOURCE_IN_USE;
						}


        }
        else {
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
								while( len-- > 0){
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

#ifdef B1553BRM_DEFINE_INTHANDLER
static void b1553brm_interrupt_handler(rtems_vector_number v){
	int i;
	/* find minor */
	for(i=0; i<brm_cores; i++){
		if ( (brms[i].irqno+0x10) == v ){
			brm_interrupt(&brms[i]);
			return;
		}
	}
}
#endif

static void brm_interrupt(brm_priv *brm) {
  unsigned short descriptor, current, pending, miw, wc, tmp;
	unsigned short msgadr, iaw, iiw;
	int len;
	int signal_event=0;
	unsigned int event_status=0;
	#define SET_ERROR_DESCRIPTOR(descriptor) (event_status = (event_status & 0x0000ffff) | descriptor<<16)

	while( (iiw=READ_REG(&brm->irq_log[brm->irq].iiw)) != 0xffff ){
		iaw=READ_REG(&brm->irq_log[brm->irq].iaw);

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

        while  ( (msgadr=brm->last_read[descriptor]) != current) {

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
                rtems_semaphore_release(brm->tx_sem);
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

            if (msgadr >= brm->desc[descriptor].bot) {
                msgadr = brm->desc[descriptor].top;
            }
						brm->last_read[descriptor] = msgadr;

#ifdef DEBUG
            brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = msgadr;
#endif

            /* Wake any blocked rx thread */
            rtems_semaphore_release(brm->rx_sem);

        }

    }

    if (pending & BRM_EOL_IRQ) {
        rtems_semaphore_release(brm->tx_sem);
    }

    if (pending & BRM_BC_ILLCMD_IRQ) {
				brm->bc_list_fail = 1;
        rtems_semaphore_release(brm->tx_sem);
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
/*            memcpy((void *)brm->bm_event[INDEX(brm->head)].data, &brm->mem[0x100], 32);*/

#ifdef DEBUG
            brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = READ_REG(&brm->regs->mbc);
            brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = READ_DMA(&brm->mem[0]);
            brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = READ_DMA(&brm->mem[1]);
            brm->log[brm->log_i++ % EVENT_QUEUE_SIZE] = READ_DMA(&brm->mem[4]);
#endif

            brm->head++;

        }
        else {
            /* Indicate overrun */
            brm->rt_event[INDEX(brm->head)].miw |= 0x8000;
        }

        /* Wake any blocking thread */
        rtems_semaphore_release(brm->rx_sem);

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
			rtems_semaphore_release(brm->tx_sem);
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
    tmp = READ_REG(&brm->desc[descriptor].ctrl);
    brm->desc[descriptor].ctrl = tmp & ~0x10;

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

	/* signal event once */
	if ( signal_event && (brm->event_id!=0) ){
		rtems_event_send(brm->event_id, event_status);
	}

}
