/*
 *  B1553RT driver implmenetation
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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <rtems/bspIo.h>

#include <drvmgr/drvmgr.h>
#include <grlib/b1553rt.h>
#include <grlib/ambapp.h>
#include <grlib/ambapp_bus.h>

#include <grlib/grlib_impl.h>

/* Uncomment for debug output */
/*#define DEBUG 1*/

/*
  #define FUNCDEBUG 1*/
/*#undef DEBUG*/
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

#define READ_DMA(address) grlib_read_uncached16((unsigned int)address)

static rtems_device_driver rt_initialize(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver rt_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver rt_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver rt_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver rt_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver rt_control(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);

#define RT_DRIVER_TABLE_ENTRY { rt_initialize, rt_open, rt_close, rt_read, rt_write, rt_control }

static rtems_driver_address_table b1553rt_driver = RT_DRIVER_TABLE_ENTRY;

typedef struct { 

    struct drvmgr_dev *dev; /* Driver manager device */
    char devName[32];     /* Device Name */

    struct rt_reg *regs;
    unsigned int ctrl_copy; /* Local copy of config register */

    unsigned int cfg_freq;

    unsigned int memarea_base;
    unsigned int memarea_base_remote;

    volatile unsigned short *mem;

    /* Received events waiting to be read */
    struct rt_msg *rt_event;
    unsigned int head, tail;

    int rx_blocking;

    rtems_id rx_sem, tx_sem, dev_sem;
    int minor;
    int irqno;

#ifdef DEBUG    		
    unsigned int log[EVENT_QUEUE_SIZE*4];
    unsigned int log_i;
#endif

    unsigned int status;
    rtems_id event_id; /* event that may be signalled upon errors, needs to be set through ioctl command RT_SET_EVENTID */

} rt_priv;

static void b1553rt_interrupt(void *arg);
static rtems_device_driver rt_init(rt_priv *rt);

#define OFS(ofs) (((unsigned int)&ofs & 0x1ffff)>>1)

static int b1553rt_driver_io_registered = 0;
static rtems_device_major_number b1553rt_driver_io_major = 0;

/******************* Driver manager interface ***********************/

/* Driver prototypes */
int b1553rt_register_io(rtems_device_major_number *m);
int b1553rt_device_init(rt_priv *pDev);

int b1553rt_init2(struct drvmgr_dev *dev);
int b1553rt_init3(struct drvmgr_dev *dev);
int b1553rt_remove(struct drvmgr_dev *dev);

struct drvmgr_drv_ops b1553rt_ops = 
{
    .init = {NULL, b1553rt_init2, b1553rt_init3, NULL},
    .remove = b1553rt_remove,
    .info = NULL
};

struct amba_dev_id b1553rt_ids[] = 
{
    {VENDOR_GAISLER, GAISLER_B1553RT},
    {0, 0}		/* Mark end of table */
};

struct amba_drv_info b1553rt_drv_info =
{
    {
	DRVMGR_OBJ_DRV,				/* Driver */
        NULL,					/* Next driver */
        NULL,					/* Device list */
        DRIVER_AMBAPP_GAISLER_B1553RT_ID,	/* Driver ID */
        "B1553RT_DRV",				/* Driver Name */
        DRVMGR_BUS_TYPE_AMBAPP,			/* Bus Type */
        &b1553rt_ops,
        NULL,					/* Funcs */
        0,					/* No devices yet */
	0,

    },
    &b1553rt_ids[0]
};

void b1553rt_register_drv (void)
{
    DBG("Registering B1553RT driver\n");
    drvmgr_drv_register(&b1553rt_drv_info.general);
}

int b1553rt_init2(struct drvmgr_dev *dev)
{
    rt_priv *priv;

    DBG("B1553RT[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);
    priv = dev->priv = grlib_calloc(1, sizeof(*priv));
    if ( !priv )
        return DRVMGR_NOMEM;
    priv->dev = dev;

    /* This core will not find other cores, so we wait for init2() */

    return DRVMGR_OK;
}

int b1553rt_init3(struct drvmgr_dev *dev)
{
    rt_priv *priv;
    char prefix[32];
    rtems_status_code status;

    priv = dev->priv;

    /* Do initialization */

    if ( b1553rt_driver_io_registered == 0) {
        /* Register the I/O driver only once for all cores */
        if ( b1553rt_register_io(&b1553rt_driver_io_major) ) {
            /* Failed to register I/O driver */
            dev->priv = NULL;
            return DRVMGR_FAIL;
        }

        b1553rt_driver_io_registered = 1;
    }

    /* I/O system registered and initialized 
     * Now we take care of device initialization.
     */

    if ( b1553rt_device_init(priv) ) {
        return DRVMGR_FAIL;
    }

    /* Get Filesystem name prefix */
    prefix[0] = '\0';
    if ( drvmgr_get_dev_prefix(dev, prefix) ) {
        /* Failed to get prefix, make sure of a unique FS name
         * by using the driver minor.
         */
        sprintf(priv->devName, "/dev/b1553rt%d", dev->minor_drv);
    } else {
        /* Got special prefix, this means we have a bus prefix
         * And we should use our "bus minor"
         */
        sprintf(priv->devName, "/dev/%sb1553rt%d", prefix, dev->minor_bus);
    }

    /* Register Device */
    status = rtems_io_register_name(priv->devName, b1553rt_driver_io_major, dev->minor_drv);
    if (status != RTEMS_SUCCESSFUL) {
        return DRVMGR_FAIL;
    }

    return DRVMGR_OK;
}

int b1553rt_remove(struct drvmgr_dev *dev)
{
    /* Stop more tasks to open driver */

    /* Throw out all tasks using this driver */

    /* Unregister I/O node */

    /* Unregister and disable Interrupt */

    /* Free device memory */

    /* Return sucessfully */

    return DRVMGR_FAIL;
}

/******************* Driver Implementation ***********************/

int b1553rt_register_io(rtems_device_major_number *m)
{
    rtems_status_code r;

    if ((r = rtems_io_register_driver(0, &b1553rt_driver, m)) == RTEMS_SUCCESSFUL) {
        DBG("B1553RT driver successfully registered, major: %d\n", *m);
    } else {
        switch(r) {
        case RTEMS_TOO_MANY:
            printk("B1553RT rtems_io_register_driver failed: RTEMS_TOO_MANY\n");
            return -1;
        case RTEMS_INVALID_NUMBER:  
            printk("B1553RT rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n");
            return -1;
        case RTEMS_RESOURCE_IN_USE:
            printk("B1553RT rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n");
            return -1;
        default:
            printk("B1553RT rtems_io_register_driver failed\n");
            return -1;
        }
    }
    return 0;
}

int b1553rt_device_init(rt_priv *pDev)
{
    struct amba_dev_info *ambadev;
    struct ambapp_core *pnpinfo;
    union drvmgr_key_value *value;
    unsigned int mem;
    unsigned int sys_freq_hz;

    /* Get device information from AMBA PnP information */
    ambadev = (struct amba_dev_info *)pDev->dev->businfo;
    if ( ambadev == NULL ) {
        return -1;
    }
    pnpinfo = &ambadev->info;
    pDev->irqno = pnpinfo->irq;
    pDev->regs = (struct rt_reg *)pnpinfo->apb_slv->start;
    pDev->minor = pDev->dev->minor_drv;

#ifdef DEBUG
    pDev->log_i = 0;	
    memset(pDev->log,0,sizeof(pDev->log));
    printf("LOG: 0x%x\n", &pDev->log[0]);
    printf("LOG_I: 0x%x\n", &pDev->log_i);
#endif	

    /* Get memory configuration from bus resources */
    value = drvmgr_dev_key_get(pDev->dev, "dmaBaseAdr", DRVMGR_KT_POINTER);
    if (value)
        mem = (unsigned int)value->ptr;

    if (value && (mem & 1)) {
        /* Remote address, address as RT looks at it. */

        /* Translate the base address into an address that the the CPU can understand */
        pDev->memarea_base = mem & ~1;
        drvmgr_translate_check(pDev->dev, DMAMEM_TO_CPU,
                                (void *)pDev->memarea_base_remote,
                                (void **)&pDev->memarea_base,
                                4 * 1024);
    } else {
        if (!value) {
            /* Use dynamically allocated memory,
             * 4k DMA memory + 4k for alignment 
             */
            mem = (unsigned int)grlib_malloc(4 * 1024 * 2);
            if ( !mem ){
                printk("RT: Failed to allocate HW memory\n\r");
                return -1;
            }
            /* align memory to 4k boundary */
            pDev->memarea_base = (mem + 0xfff) & ~0xfff;
        } else {
            pDev->memarea_base = mem;
        }

        /* Translate the base address into an address that the RT core can understand */
        drvmgr_translate_check(pDev->dev, CPUMEM_TO_DMA,
                               (void *)pDev->memarea_base,
                               (void **)&pDev->memarea_base_remote,
                               4 * 1024);
    }

    /* clear the used memory */
    memset((char *)pDev->memarea_base, 0, 4 * 1024);

    /* Set base address of all descriptors */
    pDev->memarea_base = (unsigned int)mem;
    pDev->mem = (volatile unsigned short *)pDev->memarea_base;

    pDev->rt_event = NULL;

    /* The RT is always clocked at the same frequency as the bus 
     * If the frequency doesnt match it is defaulted to 24MHz, 
     * user can always override it.
     */
    pDev->cfg_freq = RT_FREQ_24MHZ;

    /* Get frequency in Hz */
    if ( drvmgr_freq_get(pDev->dev, DEV_APB_SLV, &sys_freq_hz) == 0 ) {
        if ( sys_freq_hz == 20000000 ) {
            pDev->cfg_freq = RT_FREQ_20MHZ;
        } else if ( sys_freq_hz == 16000000 ) {
            pDev->cfg_freq = RT_FREQ_16MHZ;
        } else if ( sys_freq_hz == 12000000 ) {
            pDev->cfg_freq = RT_FREQ_12MHZ;
        }
    }

    value = drvmgr_dev_key_get(pDev->dev, "coreFreq", DRVMGR_KT_INT);
    if ( value ) {
        pDev->cfg_freq = value->i & RT_FREQ_MASK;
    }

    /* RX Semaphore created with count = 0 */
    if ( rtems_semaphore_create(rtems_build_name('R', 'T', '0', '0' + pDev->minor),
                                0,
                                RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING, 
                                0,
                                &pDev->rx_sem) != RTEMS_SUCCESSFUL ) {
        printk("RT: Failed to create rx semaphore\n");
        return RTEMS_INTERNAL_ERROR;
    }

    /* Device Semaphore created with count = 1 */
    if ( rtems_semaphore_create(rtems_build_name('R', 'T', '0', '0' + pDev->minor),
                                1,
                                RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING, 
                                0,
                                &pDev->dev_sem) != RTEMS_SUCCESSFUL ){
        printk("RT: Failed to create device semaphore\n");
        return RTEMS_INTERNAL_ERROR;
    }

    /* Default to RT-mode */
    rt_init(pDev);

    return 0;
}

static int odd_parity(unsigned int data)
{
    unsigned int i=0;

    while(data)
    {
        i++;
        data &= (data - 1);
    } 

    return !(i&1);
}

static void start_operation(rt_priv *rt)
{

}

static void stop_operation(rt_priv *rt)
{

}

static void set_extmdata_en(rt_priv *rt, int extmdata)
{
    if ( extmdata )
        extmdata = 1;    
    rt->ctrl_copy = (rt->ctrl_copy & ~(1<<16)) | (extmdata<<16);
    rt->regs->ctrl = rt->ctrl_copy;
}

static void set_vector_word(rt_priv *rt, unsigned short vword)
{
    rt->regs->vword = vword;
}

/* Set clock speed */
static void set_clkspd(rt_priv *rt, int spd)
{
    rt->ctrl_copy = (rt->ctrl_copy & ~0xC0) | (spd<<6);
    rt->regs->ctrl = rt->ctrl_copy;
    asm volatile("nop"::);
    rt->regs->ctrl = rt->ctrl_copy | (1<<20);
}

static void set_rtaddr(rt_priv *rt, int addr)
{
    rt->ctrl_copy = (rt->ctrl_copy & ~0x3F00) | (addr << 8) | (odd_parity(addr)<<13);
    rt->regs->ctrl = rt->ctrl_copy;
}

static void set_broadcast_en(rt_priv *rt, int data)
{
    rt->ctrl_copy = (rt->ctrl_copy & ~0x40000) | (data<<18);
    rt->regs->ctrl = rt->ctrl_copy;
}

static rtems_device_driver rt_init(rt_priv *rt)
{
    rt->rx_blocking = 1;

    if ( rt->rt_event )
        free(rt->rt_event);
    rt->rt_event = NULL;

    rt->rt_event = grlib_malloc(EVENT_QUEUE_SIZE*sizeof(*rt->rt_event));

    if (rt->rt_event == NULL) {
        DBG("RT driver failed to allocated memory.");
        return RTEMS_NO_MEMORY;
    }

    rt->ctrl_copy = rt->regs->ctrl & 0x3F00; /* Keep rtaddr and rtaddrp */
    rt->ctrl_copy |= 0x3C0D0; /* broadcast disabled, extmdata=1, writetsw = writecmd = 1 */
    rt->regs->ctrl = rt->ctrl_copy; 

    /* Set Clock speed */
    set_clkspd(rt, rt->cfg_freq);

    rt->regs->addr = rt->memarea_base_remote;
    rt->regs->ipm  = 0x70000; /* Enable RT RX, MEM Failure and AHB Error interrupts */

    DBG("B1553RT DMA_AREA: 0x%x\n", (unsigned int)rt->mem);

    return RTEMS_SUCCESSFUL;
}


static rtems_device_driver rt_initialize(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
    return RTEMS_SUCCESSFUL;
}

static rtems_device_driver rt_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg) {
    rt_priv *rt;
    struct drvmgr_dev *dev;

    FUNCDBG("rt_open\n");

    if ( drvmgr_get_dev(&b1553rt_drv_info.general, minor, &dev) ) {
        DBG("Wrong minor %d\n", minor);
        return RTEMS_UNSATISFIED;
    }
    rt = (rt_priv *)dev->priv;

    if (rtems_semaphore_obtain(rt->dev_sem, RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT) != RTEMS_SUCCESSFUL) {
        DBG("rt_open: resource in use\n");
        return RTEMS_RESOURCE_IN_USE; /* EBUSY */
    }

    /* Set defaults */
    rt->event_id = 0;

    start_operation(rt);

    /* Register interrupt routine */
    if (drvmgr_interrupt_register(rt->dev, 0, "b1553rt", b1553rt_interrupt, rt)) {
        rtems_semaphore_release(rt->dev_sem);
        return -1;
    }


    return RTEMS_SUCCESSFUL;
}
 
static rtems_device_driver rt_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
    rt_priv *rt;
    struct drvmgr_dev *dev;

    FUNCDBG("rt_close");

    if ( drvmgr_get_dev(&b1553rt_drv_info.general, minor, &dev) ) {
        return RTEMS_UNSATISFIED;
    }
    rt = (rt_priv *)dev->priv;

    drvmgr_interrupt_unregister(rt->dev, 0, b1553rt_interrupt, rt);

    stop_operation(rt);
    rtems_semaphore_release(rt->dev_sem);

    return RTEMS_SUCCESSFUL;
}
 
static int get_messages(rt_priv *rt, void *buf, unsigned int msg_count)
{

    struct rt_msg *dest = (struct rt_msg *) buf;
    int count = 0;

    if (rt->head == rt->tail) {
        return 0;
    }

    do {

        DBG("rt read - head: %d, tail: %d\n", rt->head, rt->tail);
        dest[count++] = rt->rt_event[INDEX(rt->tail++)];

    } while (rt->head != rt->tail && count < msg_count);

    return count;

}
static rtems_device_driver rt_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
    rtems_libio_rw_args_t *rw_args;
    int count = 0;
    rt_priv *rt;
    struct drvmgr_dev *dev;

    if ( drvmgr_get_dev(&b1553rt_drv_info.general, minor, &dev) ) {
        return RTEMS_UNSATISFIED;
    }
    rt = (rt_priv *)dev->priv;

    rw_args = (rtems_libio_rw_args_t *) arg;

    FUNCDBG("rt_read [%i,%i]: buf: 0x%x, len: %i\n",major, minor, (unsigned int)rw_args->buffer, rw_args->count);

    while ( (count = get_messages(rt,rw_args->buffer, rw_args->count)) == 0 ) {

        if (rt->rx_blocking) {
            rtems_semaphore_obtain(rt->rx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
        } else {
            /* Translates to EBUSY */
            return RTEMS_RESOURCE_IN_USE;
        }
    }

    rw_args->bytes_moved = count;
    return RTEMS_SUCCESSFUL;
}

static rtems_device_driver rt_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
    rtems_libio_rw_args_t *rw_args;
    struct rt_msg *source;
    rt_priv *rt;
    struct drvmgr_dev *dev;
    unsigned int descriptor, suba, wc;

    if ( drvmgr_get_dev(&b1553rt_drv_info.general, minor, &dev) ) {
        return RTEMS_UNSATISFIED;
    }
    rt = (rt_priv *)dev->priv;

    rw_args = (rtems_libio_rw_args_t *) arg;

    if ( rw_args->count != 1 ) {
        return RTEMS_INVALID_NAME;
    }

    source = (struct rt_msg *) rw_args->buffer;

    descriptor = source[0].desc & 0x7F;
    suba       = descriptor-32;
    wc         = source[0].miw >> 11;
    wc         = wc ? wc : 32;

    FUNCDBG("rt_write [%i,%i]: buf: 0x%x\n",major, minor, (unsigned int)rw_args->buffer);

    memcpy((void *)&rt->mem[0x400 + suba*32], &source[0].data[0], wc*2);

    rw_args->bytes_moved = 1; 

    return RTEMS_SUCCESSFUL;

}

static rtems_device_driver rt_control(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
    rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *) arg;
    unsigned int *data = ioarg->buffer;

    rt_priv *rt;
    struct drvmgr_dev *dev;

    FUNCDBG("rt_control[%d]: [%i,%i]\n", minor, major, minor);

    if ( drvmgr_get_dev(&b1553rt_drv_info.general, minor, &dev) ) {
        return RTEMS_UNSATISFIED;
    }
    rt = (rt_priv *)dev->priv;

    if (!ioarg) {
        DBG("rt_control: invalid argument\n");
        return RTEMS_INVALID_NAME;
    }

    ioarg->ioctl_return = 0;
    switch (ioarg->command) {

    case RT_SET_ADDR:   
        set_rtaddr(rt, data[0]);
        break;

    case RT_SET_BCE:
        set_broadcast_en(rt, data[0]);
        break;

    case RT_SET_VECTORW:
        set_vector_word(rt, data[0]);
        break;

    case RT_SET_EXTMDATA:
        set_extmdata_en(rt, data[0]);
        break;

    case RT_RX_BLOCK: 
        rt->rx_blocking     = data[0];   
        break;

    case RT_CLR_STATUS:
        rt->status = 0;
        break;

    case RT_GET_STATUS: /* copy status */
        if ( !ioarg->buffer )
            return RTEMS_INVALID_NAME;
            
        *(unsigned int *)ioarg->buffer = rt->status;
        break;

    case RT_SET_EVENTID:
        rt->event_id = (rtems_id)ioarg->buffer;
        break;

    default:
        return RTEMS_NOT_IMPLEMENTED;
    }

    return RTEMS_SUCCESSFUL;
}

static void b1553rt_interrupt(void *arg)
{
    rt_priv *rt = arg;
    unsigned short descriptor;
    int signal_event=0, wake_rx_task=0;
    unsigned int event_status=0;
    unsigned int wc, irqv, cmd, tsw, suba, tx, miw, i;
    unsigned int ipend;

    #define SET_ERROR_DESCRIPTOR(descriptor) (event_status = (event_status & 0x0000ffff) | descriptor<<16)
    ipend = rt->regs->ipm;

    if (ipend == 0) {
      /* IRQ mask has been cleared, we must have been reset */
      /* Restore ctrl registers */
      rt->regs->ctrl = rt->ctrl_copy;
      rt->regs->addr = rt->memarea_base_remote;
      rt->regs->ipm = 0x70000;
      /* Send reset mode code event */
      if (rt->head - rt->tail != EVENT_QUEUE_SIZE) {
	miw = (8<<11);
	descriptor = 64 + 32 + 8;
	rt->rt_event[INDEX(rt->head)].miw = miw;
	rt->rt_event[INDEX(rt->head)].time = 0;
	rt->rt_event[INDEX(rt->head)].desc = descriptor;
	rt->head++;
      }
    }

    if ( ipend & 0x1 ) {
      /* RT IRQ */
      if (rt->head - rt->tail != EVENT_QUEUE_SIZE) {

        irqv = rt->regs->irq;
        cmd  = irqv >> 7;
        wc   = cmd & 0x1F;       /* word count / mode code  */
        suba = irqv & 0x1F;      /* sub address (0-31) */
        tx   = (irqv >> 5) & 1;   
 
        /* read status word */
        tsw = READ_DMA(&rt->mem[tx*0x3E0+suba]); 

        /* Build Message Information Word (B1553BRM-style) */
        miw = (wc<<11) | (tsw&RT_TSW_BUS)>>4 | !(tsw&RT_TSW_OK)>>7 | (tsw&RT_TSW_ILL)>>5 | 
            (tsw&RT_TSW_PAR)>>5 | (tsw&RT_TSW_MAN)>>7;

        descriptor = (tx << 5) | suba;

        /* Mode codes */
        if (suba == 0 || suba == 31) {
            descriptor = 64 + (tx*32) + wc;
        }
          
        /* Data received or transmitted */
        if (descriptor < 64) {
            wc = wc ? wc : 32;   /* wc = 0 means 32 words transmitted */
        }
        /* RX Mode code */
        else if (descriptor < 96) {
            wc = (wc>>4);
        }
        /* TX Mode code */
        else if (descriptor < 128) {
            wc = (wc>>4);
        }

        /* Copy to event queue */
        rt->rt_event[INDEX(rt->head)].miw  = miw;
        rt->rt_event[INDEX(rt->head)].time = 0;

        for (i = 0; i < wc; i++) {
            rt->rt_event[INDEX(rt->head)].data[i] = READ_DMA(&rt->mem[tx*0x400 + suba*32 + i]);
        }
        rt->rt_event[INDEX(rt->head)].desc = descriptor;
        rt->head++;


        /* Handle errors */
        if ( tsw & RT_TSW_ILL){
            FUNCDBG("RT: RT_ILLCMD\n\r");
            rt->status |= RT_ILLCMD_IRQ;
            event_status |= RT_ILLCMD_IRQ;
            SET_ERROR_DESCRIPTOR(descriptor);
            signal_event=1;
        }
            
        if ( !(tsw & RT_TSW_OK) ) {
            FUNCDBG("RT: RT_MERR_IRQ\n\r");
            rt->status |= RT_MERR_IRQ;
            event_status |= RT_MERR_IRQ;
            SET_ERROR_DESCRIPTOR(descriptor);
            signal_event=1;
        }
	
      }
      else {
        /* Indicate overrun */
        rt->rt_event[INDEX(rt->head)].desc |= 0x8000;
      }
    }

    if ( ipend & 0x2 ) {
      /* Memory failure IRQ */
      FUNCDBG("B1553RT: Memory failure\n");
			event_status |= RT_DMAF_IRQ;
			signal_event=1;
    }

    if ( ipend & 0x4 ) {
      /* AHB Error */
      FUNCDBG("B1553RT: AHB ERROR\n");
			event_status |= RT_DMAF_IRQ;
			signal_event=1;
    }

#ifdef DEBUG            
    rt->log[rt->log_i++ % EVENT_QUEUE_SIZE] = descriptor; 
    rt->log[rt->log_i++ % EVENT_QUEUE_SIZE] = cmd;
    rt->log[rt->log_i++ % EVENT_QUEUE_SIZE] = miw;
    rt->log[rt->log_i++ % EVENT_QUEUE_SIZE] = tsw;
#endif

    wake_rx_task = 1;

    /* Wake any blocked rx thread only on receive interrupts */
    if ( wake_rx_task ) {
        rtems_semaphore_release(rt->rx_sem);
    }

    /* Copy current mask to status mask */
    if ( event_status ) {
        if ( event_status & 0xffff0000 )
            rt->status &= 0x0000ffff;
        rt->status |= event_status;
    }

    /* signal event once */
    if ( signal_event && (rt->event_id != 0) ) {
        rtems_event_send(rt->event_id, event_status);
    }

}

void b1553rt_print_dev(struct drvmgr_dev *dev, int options)
{
    rt_priv *pDev = dev->priv;

    /* Print */
    printf("--- B1553RT[%d] %s ---\n", pDev->minor, pDev->devName);
    printf(" REGS:            0x%x\n", (unsigned int)pDev->regs);
    printf(" IRQ:             %d\n", pDev->irqno);

}

void b1553rt_print(int options)
{
    struct amba_drv_info *drv = &b1553rt_drv_info;
    struct drvmgr_dev *dev;

    dev = drv->general.dev;
    while(dev) {
        b1553rt_print_dev(dev, options);
        dev = dev->next_in_drv;
    }
}
