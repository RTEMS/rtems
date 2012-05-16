/*
 *  This file contains the GRSPW SpaceWire Driver for LEON2 and LEON3.
 *
 *  COPYRIGHT (c) 2007
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * Changes:
 *
 * 2007-09-27, Daniel Hellstrom <daniel@gaisler.com>
 *  Added basic support for GRSPW2 core.
 *
 * 2007-07-12, Daniel Hellstrom <daniel@gaisler.com>
 *  Fixed bug in TXBLOCK mode (normally called flush).
 *
 * 2007-05-28, Daniel Hellstrom <daniel@gaisler.com>
 *  Changed register call from spacewire_register to
 *  grspw_register. Added one parameter (the AMBA bus
 *  pointer) for LEON2 and LEON3 PCI compability.
 *  Typical LEON3 register: grspw_register(&amba_conf);
 *
 * 2007-05-28, Daniel Hellstrom <daniel@gaisler.com>
 *  Changed errno return values, compatible with RASTA
 *  Spacewire driver
 *
 * 2007-05-25, Daniel Hellstrom <daniel@gaisler.com>
 *  Changed name from /dev/spacewire,/dev/spacewire_b...
 *  to /dev/grspw0,/dev/grspw1...
 *
 * 2007-05-24, Daniel Hellstrom <daniel@gaisler.com>
 *  Merged LEON3, LEON2 and RASTA driver to one - this.
 *  The driver is included and configured from grspw_pci.c
 *  and grspw_rasta.c.
 *
 * 2007-05-23, Daniel Hellstrom <daniel@gaisler.com>
 *  Changed open call, now one need to first call open
 *  and then ioctl(fd,START,timeout) in order to setup
 *  hardware for communication.
 *
 * 2007-05-23, Daniel Hellstrom <daniel@gaisler.com>
 *  Added ioctl(fd,SET_COREFREQ,freq_arg), the command
 *  can autodetect the register values disconnect and
 *  timer64. It is still possible to change them manually
 *  by ioctl(fd,SET_{DISCONNECT,TIMER},arg).
 *
 */

/* default name to /dev/grspw0 */
#if !defined(GRSPW_DEVNAME) || !defined(GRSPW_DEVNAME_NO)
 #undef GRSPW_DEVNAME
 #undef GRSPW_DEVNAME_NO
 #define GRSPW_DEVNAME "/dev/grspw0"
 #define GRSPW_DEVNAME_NO(devstr,no) ((devstr)[10]='0'+(no))
#endif

#ifndef GRSPW_PREFIX
 #define GRSPW_PREFIX(name) grspw##name
#endif

/* default to no translation */
#ifndef GRSPW_ADR_TO
 #define memarea_to_hw(x) ((unsigned int)(x))
#endif
#ifndef GRSPW_ADR_FROM
 #define hw_to_memarea(x) ((unsigned int)(x))
#endif

#ifndef GRSPW_REG_INT
 #define GRSPW_REG_INT(handler,irqno,arg) set_vector(handler,irqno+0x10,1)
 #undef  GRSPW_DEFINE_INTHANDLER
 #define GRSPW_DEFINE_INTHANDLER
#endif

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <rtems/bspIo.h>
#include <ambapp.h>
#include <grlib.h>
#include <grspw.h>

#define DBGSPW_IOCALLS 1
#define DBGSPW_TX 2
#define DBGSPW_RX 4
#define DBGSPW_IOCTRL 1
#define DBGSPW_DUMP 16
#define DEBUG_SPACEWIRE_FLAGS (DBGSPW_IOCALLS | DBGSPW_TX | DBGSPW_RX )

/* #define DEBUG_SPACEWIRE_ONOFF */

#ifdef DEBUG_SPACEWIRE_ONOFF
#define SPACEWIRE_DBG(fmt, args...)    do { { printk(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__,## args); }} while(0)
#define SPACEWIRE_DBG2(fmt)            do { { printk(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__); }} while(0)
#define SPACEWIRE_DBGC(c,fmt, args...) do { if (DEBUG_SPACEWIRE_FLAGS & c) { printk(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__,## args);  }} while(0)
#else
#define SPACEWIRE_DBG(fmt, args...)
#define SPACEWIRE_DBG2(fmt, args...)
#define SPACEWIRE_DBGC(c, fmt, args...)
#endif

typedef struct {
   volatile unsigned int ctrl;
   volatile unsigned int status;
   volatile unsigned int nodeaddr;
   volatile unsigned int clkdiv;
   volatile unsigned int destkey;
   volatile unsigned int time;
   volatile unsigned int timer;
   volatile unsigned int pad;

   volatile unsigned int dma0ctrl;
   volatile unsigned int dma0rxmax;
   volatile unsigned int dma0txdesc;
   volatile unsigned int dma0rxdesc;

   /* For GRSPW core 2 and onwards */
   volatile unsigned int dma0addr;

} LEON3_SPACEWIRE_Regs_Map;

typedef struct {
   volatile unsigned int ctrl;
   volatile unsigned int addr;
} SPACEWIRE_RXBD;

typedef struct {
   volatile unsigned int ctrl;
   volatile unsigned int addr_header;
   volatile unsigned int len;
   volatile unsigned int addr_data;
} SPACEWIRE_TXBD;

#define SPACEWIRE_INIT_TIMEOUT 10
#define SPACEWIRE_BDTABLE_SIZE 0x400
#define SPACEWIRE_TXD_SIZE 1024
#define SPACEWIRE_TXH_SIZE 64
#define SPACEWIRE_RXPCK_SIZE 1024
#define SPACEWIRE_TXBUFS_NR 64
#define SPACEWIRE_RXBUFS_NR 128

#define BUFMEM_PER_LINK (SPACEWIRE_TXBUFS_NR*(SPACEWIRE_TXD_SIZE+SPACEWIRE_TXH_SIZE) + SPACEWIRE_RXBUFS_NR*SPACEWIRE_RXPCK_SIZE)

#define SPW_ALIGN(p,c) ((((unsigned int)(p))+((c)-1))&~((c)-1))

typedef struct {
   /* configuration parameters */
   spw_config config;

   unsigned int tx_all_in_use;
   unsigned int tx_sent;
   unsigned int tx_cur;
   unsigned int rxcur;
   unsigned int rxbufcur;
   unsigned int txdbufsize;
   unsigned int txhbufsize;
   unsigned int rxbufsize;
   unsigned int txbufcnt;
   unsigned int rxbufcnt;

   /* statistics */
   spw_stats stat;

   char *ptr_rxbuf0;
   char *ptr_txdbuf0;
   char *ptr_txhbuf0;

   unsigned int irq;
   int minor;
   int core_ver;
   int open;
   int running;
   unsigned int core_freq_khz;


   /* semaphores*/
   rtems_id txsp;
   rtems_id rxsp;

   SPACEWIRE_RXBD *rx;
   SPACEWIRE_TXBD *tx;

#ifdef GRSPW_STATIC_MEM
   unsigned int membase, memend, mem_bdtable;
#else
   char _rxtable[SPACEWIRE_BDTABLE_SIZE*2];
   char _txtable[SPACEWIRE_BDTABLE_SIZE*2];
#endif

   LEON3_SPACEWIRE_Regs_Map *regs;
} GRSPW_DEV;

static int spw_cores;
static int spw_cores2;
static unsigned int sys_freq_khz;
static GRSPW_DEV *grspw_devs;

#ifdef GRSPW_DONT_BYPASS_CACHE
#define _SPW_READ(address) (*(volatile unsigned int *)(address))
#define _MEM_READ(address) (*(volatile unsigned char *)(address))
#else
static unsigned int _SPW_READ(void *addr) {
        unsigned int tmp;
        __asm__ (" lda [%1]1, %0 "
            : "=r"(tmp)
            : "r"(addr)
           );
        return tmp;
}

static unsigned int _MEM_READ(void *addr) {
        unsigned int tmp;
        __asm__ (" lduba [%1]1, %0 "
            : "=r"(tmp)
            : "r"(addr)
           );
        return tmp;

}
#endif

#define MEM_READ(addr) _MEM_READ((void *)(addr))
#define SPW_READ(addr) _SPW_READ((void *)(addr))
#define SPW_WRITE(addr,v) *addr=v

#define SPW_REG(c,r) (c->regs->r)
#define SPW_REG_CTRL(c) SPW_REG(c,ctrl)
#define SPW_REG_STATUS(c) SPW_REG(c,status)
#define SPW_REG_NODEADDR(c) SPW_REG(c,nodeaddr)

#define SPW_CTRL_READ(c)      SPW_READ(&SPW_REG_CTRL(c))
#define SPW_CTRL_WRITE(c,v)   SPW_WRITE(&SPW_REG_CTRL(c),v)
#define SPW_STATUS_READ(c)    SPW_READ(&SPW_REG_STATUS(c))
#define SPW_STATUS_WRITE(c,v) SPW_WRITE(&SPW_REG_STATUS(c),v)

#define SPW_LINKSTATE(c) (((c) >> 21) & 0x7)

#define SPACEWIRE_RXNR(c) ((c&~(SPACEWIRE_BDTABLE_SIZE-1))>>3)
#define SPACEWIRE_TXNR(c) ((c&~(SPACEWIRE_BDTABLE_SIZE-1))>>4)

#define SPW_RXBD_LENGTH 0x1ffffff
#define SPW_RXBD_EN (1 << 25)
#define SPW_RXBD_WR (1 << 26)
#define SPW_RXBD_IE (1 << 27)

#define SPW_RXBD_EEP (1 << 28)
#define SPW_RXBD_EHC (1 << 29)
#define SPW_RXBD_EDC (1 << 30)
#define SPW_RXBD_ETR (1 << 31)

#define SPW_RXBD_ERROR (SPW_RXBD_EEP | \
                        SPW_RXBD_ETR)

#define SPW_RXBD_RMAPERROR (SPW_RXBD_EHC | SPW_RXBD_EDC)

#define SPW_TXBD_LENGTH 0xffffff

#define SPW_TXBD_EN (1 << 12)
#define SPW_TXBD_WR (1 << 13)
#define SPW_TXBD_IE (1 << 14)
#define SPW_TXBD_LE (1 << 15)

#define SPW_TXBD_ERROR (SPW_TXBD_LE)

#define SPW_CTRL_LINKDISABLED (1 << 0)
#define SPW_CTRL_LINKSTART    (1 << 1)
#define SPW_CTRL_AUTOSTART    (1 << 2)
#define SPW_CTRL_IE           (1 << 3)
#define SPW_CTRL_TI           (1 << 4)
#define SPW_CTRL_PM           (1 << 5)
#define SPW_CTRL_RESET        (1 << 6)
#define SPW_CTRL_TQ           (1 << 8)
#define SPW_CTRL_LI           (1 << 9)
#define SPW_CTRL_TT           (1 << 10)
#define SPW_CTRL_TR           (1 << 11)
#define SPW_CTRL_RE           (1 << 16)
#define SPW_CTRL_RD           (1 << 17)

#define SPW_CTRL_RC           (1 << 29)
#define SPW_CTRL_RX           (1 << 30)
#define SPW_CTRL_RA           (1 << 31)

#define SPW_STATUS_TO (1 << 0)
#define SPW_STATUS_CE (1 << 1)
#define SPW_STATUS_ER (1 << 2)
#define SPW_STATUS_DE (1 << 3)
#define SPW_STATUS_PE (1 << 4)
#define SPW_STATUS_WE (1 << 6)
#define SPW_STATUS_IA (1 << 7)
#define SPW_STATUS_EE (1 << 8)

#define SPW_DMACTRL_TXEN (1 << 0)
#define SPW_DMACTRL_RXEN (1 << 1)
#define SPW_DMACTRL_TXIE (1 << 2)
#define SPW_DMACTRL_RXIE (1 << 3)
#define SPW_DMACTRL_AI   (1 << 4)
#define SPW_DMACTRL_PS   (1 << 5)
#define SPW_DMACTRL_PR   (1 << 6)
#define SPW_DMACTRL_TA   (1 << 7)
#define SPW_DMACTRL_RA   (1 << 8)
#define SPW_DMACTRL_AT   (1 << 9)
#define SPW_DMACTRL_RX   (1 << 10)
#define SPW_DMACTRL_RD   (1 << 11)
#define SPW_DMACTRL_NS   (1 << 12)

#define SPW_PREPAREMASK_TX (SPW_DMACTRL_RXEN | SPW_DMACTRL_RXIE | SPW_DMACTRL_PS | SPW_DMACTRL_TA | SPW_DMACTRL_RD | SPW_DMACTRL_NS)
#define SPW_PREPAREMASK_RX (SPW_DMACTRL_TXEN | SPW_DMACTRL_TXIE | SPW_DMACTRL_AI | SPW_DMACTRL_PR | SPW_DMACTRL_RA)

static int grspw_hw_init(GRSPW_DEV *pDev);
static int grspw_hw_send(GRSPW_DEV *pDev, unsigned int hlen, char *hdr, unsigned int dlen, char *data);
static int grspw_hw_receive(GRSPW_DEV *pDev,char *b,int c);
static int grspw_hw_startup (GRSPW_DEV *pDev, int timeout);
static int grspw_hw_stop (GRSPW_DEV *pDev, int rx, int tx);
static void grspw_hw_wait_rx_inactive(GRSPW_DEV *pDev);
static int grspw_hw_waitlink (GRSPW_DEV *pDev, int timeout);
static void grspw_hw_reset(GRSPW_DEV *pDev);
static void grspw_hw_read_config(GRSPW_DEV *pDev);

static void check_rx_errors(GRSPW_DEV *pDev, int ctrl);
static void grspw_rxnext(GRSPW_DEV *pDev);
static void grspw_interrupt(GRSPW_DEV *pDev);

static rtems_device_driver grspw_initialize(
        rtems_device_major_number  major,
        rtems_device_minor_number  minor,
        void                    * arg
        );

static rtems_device_driver grspw_open(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        );

static rtems_device_driver grspw_close(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        );

static rtems_device_driver grspw_read(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        );

static rtems_device_driver grspw_write(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        );

static rtems_device_driver grspw_control(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        );

#define GRSPW_DRIVER_TABLE_ENTRY \
  { grspw_initialize, \
    grspw_open, \
    grspw_close, \
    grspw_read, \
    grspw_write, \
    grspw_control }

static rtems_driver_address_table grspw_driver = GRSPW_DRIVER_TABLE_ENTRY;
static struct ambapp_bus *amba_bus;

int GRSPW_PREFIX(_register)(struct ambapp_bus *bus)
{
        rtems_status_code r;
        rtems_device_major_number m;

        /* Get System clock frequency */
        sys_freq_khz = 0;

        amba_bus = bus;

        /* Auto Detect the GRSPW core frequency by assuming that the system frequency is
         * is the same as the GRSPW core frequency.
         */
#ifndef SYS_FREQ_KHZ
#ifdef LEON3
	/* LEON3: find timer address via AMBA Plug&Play info */
	{
		struct ambapp_apb_info gptimer;
		struct gptimer_regs *tregs;

		if ( ambapp_find_apbslv(&ambapp_plb, VENDOR_GAISLER,
                                        GAISLER_GPTIMER, &gptimer) == 1 ) {
			tregs = (struct gptimer_regs *)gptimer.start;
			sys_freq_khz = (tregs->scaler_reload+1)*1000;
			SPACEWIRE_DBG("GRSPW: detected %dkHZ system frequency\n\r",sys_freq_khz);
		}else{
			sys_freq_khz = 40000; /* Default to 40MHz */
			printk("GRSPW: Failed to detect system frequency\n\r");
		}

	}
#elif defined(LEON2)
        /* LEON2: use hardcoded address to get to timer */
        {
                LEON_Register_Map *regs = (LEON_Register_Map *)0x80000000;
                sys_freq_khz = (regs->Scaler_Reload+1)*1000;
        }
#else
 #error CPU not supported by GRSPW driver
#endif
#else
	/* Use hardcoded frequency */
	sys_freq_khz = SYS_FREQ_KHZ;
#endif

        SPACEWIRE_DBG2("register driver\n");
        if ((r = rtems_io_register_driver(0, &grspw_driver, &m)) == RTEMS_SUCCESSFUL) {
                SPACEWIRE_DBG2("success\n");
                return 0;
        } else {
                switch(r) {
                        case RTEMS_TOO_MANY:
                                SPACEWIRE_DBG2("failed RTEMS_TOO_MANY\n");
                                break;
                        case RTEMS_INVALID_NUMBER:
                                SPACEWIRE_DBG2("failed RTEMS_INVALID_NUMBER\n");
                                break;
                        case RTEMS_RESOURCE_IN_USE:
                                SPACEWIRE_DBG2("failed RTEMS_RESOURCE_IN_USE\n");
                                break;
                        default:
                                SPACEWIRE_DBG("failed %i\n",r);
                                break;
                }
                return 1;
        }

}

/* Get a value at least 6.4us in number of clock cycles */
static unsigned int grspw_calc_timer64(int freq_khz){
	unsigned int timer64 = (freq_khz*64+9999)/10000;
	return timer64 & 0xfff;
}

/* Get a value at least 850ns in number of clock cycles - 3 */
static unsigned int grspw_calc_disconnect(int freq_khz){
	unsigned int disconnect = ((freq_khz*85+99999)/100000) - 3;
	return disconnect & 0x3ff;
}

#if 0
static int grspw_buffer_alloc(GRSPW_DEV *pDev) {
        if (pDev->ptr_rxbuf0) {
                free(pDev->ptr_rxbuf0);
        }
        if (pDev->ptr_txdbuf0) {
                free(pDev->ptr_txdbuf0);
        }
        if (pDev->ptr_txhbuf0) {
                free(pDev->ptr_txhbuf0);
        }
        pDev->ptr_rxbuf0 = (char *) malloc(pDev->rxbufsize * pDev->rxbufcnt);
        pDev->ptr_txdbuf0 = (char *) malloc(pDev->txdbufsize * pDev->txbufcnt);
        pDev->ptr_txhbuf0 = (char *) malloc(pDev->txhbufsize * pDev->txbufcnt);
        if ((pDev->ptr_rxbuf0 == NULL) ||
            (pDev->ptr_txdbuf0 == NULL) || (pDev->ptr_txhbuf0 == NULL)) {
                return 1;
        } else {
                return 0;
        }
}
#endif

static int grspw_buffer_alloc(GRSPW_DEV *pDev)
{
#ifndef GRSPW_STATIC_MEM
        if (pDev->ptr_rxbuf0) {
                free(pDev->ptr_rxbuf0);
        }
        if (pDev->ptr_txdbuf0) {
                free(pDev->ptr_txdbuf0);
        }
        if (pDev->ptr_txhbuf0) {
                free(pDev->ptr_txhbuf0);
        }

        pDev->ptr_rxbuf0 = (char *) malloc(pDev->rxbufsize * pDev->rxbufcnt);
        pDev->ptr_txdbuf0 = (char *) malloc(pDev->txdbufsize * pDev->txbufcnt);
        pDev->ptr_txhbuf0 = (char *) malloc(pDev->txhbufsize * pDev->txbufcnt);
        if ((pDev->ptr_rxbuf0 == NULL) ||
            (pDev->ptr_txdbuf0 == NULL) || (pDev->ptr_txhbuf0 == NULL)) {
                return 1;
        } else {
                return 0;
        }
#else
        if ( (pDev->membase + pDev->rxbufsize*pDev->rxbufcnt + pDev->txdbufsize*pDev->txbufcnt) >= pDev->memend ) {
                return -1;
        }
        pDev->ptr_rxbuf0  = (char *) pDev->membase;
        pDev->ptr_txdbuf0 = pDev->ptr_rxbuf0 + pDev->rxbufsize * pDev->rxbufcnt;
        pDev->ptr_txhbuf0 = pDev->ptr_txdbuf0 + pDev->txdbufsize * pDev->txbufcnt;
        return 0;
#endif

}

#ifdef GRSPW_DEFINE_INTHANDLER
/*
 *  Standard Interrupt handler
 */
static rtems_isr grspw_interrupt_handler(rtems_vector_number v)
{
        int minor;

        for(minor = 0; minor < spw_cores+spw_cores2; minor++) {
                if (v == (grspw_devs[minor].irq+0x10) ) {
                        grspw_interrupt(&grspw_devs[minor]);
                        break;
                }
        }
}
#endif

static void grspw_interrupt(GRSPW_DEV *pDev){
        int dmactrl;
        int status;
        int ctrl;

        status = SPW_STATUS_READ(pDev);
        SPW_STATUS_WRITE(pDev, SPW_STATUS_CE | SPW_STATUS_ER | SPW_STATUS_DE | SPW_STATUS_PE | SPW_STATUS_WE | SPW_STATUS_IA | SPW_STATUS_EE);
        dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
        SPW_WRITE(&pDev->regs->dma0ctrl, dmactrl | SPW_DMACTRL_PR);
        /* If linkinterrupts are enabled check if it was a linkerror irq and then send an event to the
           process set in the config */
        if (pDev->config.link_err_irq) {
                if (status & (SPW_STATUS_CE | SPW_STATUS_ER | SPW_STATUS_DE | SPW_STATUS_PE | SPW_STATUS_WE)) {
                        rtems_event_send(pDev->config.event_id, SPW_LINKERR_EVENT);
                        if (pDev->config.disable_err) {
                                /* disable link*/
                                SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFFFFFC) | SPW_CTRL_LINKDISABLED);
                                pDev->config.linkdisabled = 1;
                                pDev->config.linkstart = 0;
                                pDev->running = 0;
                        }
                }
        }
        if (status & SPW_STATUS_CE) {
                pDev->stat.credit_err++;
        }
        if (status & SPW_STATUS_ER) {
                pDev->stat.escape_err++;
        }
        if (status & SPW_STATUS_DE) {
                pDev->stat.disconnect_err++;
        }
        if (status & SPW_STATUS_PE) {
                pDev->stat.parity_err++;
        }
        if (status & SPW_STATUS_WE) {
                pDev->stat.write_sync_err++;
        }
        if (status & SPW_STATUS_IA) {
                pDev->stat.invalid_address++;
        }
        if (status & SPW_STATUS_EE) {
                pDev->stat.early_ep++;
        }

        /* Check for tx interrupts */
        while( (pDev->tx_sent != pDev->tx_cur) || pDev->tx_all_in_use) {
                /* Has this descriptor been sent? */
                ctrl = SPW_READ((volatile void *)&pDev->tx[pDev->tx_sent].ctrl);
                if ( ctrl & SPW_TXBD_EN ) {
                        break;
                }
                /* Yes, increment status counters & tx_sent so we can use this descriptor to send more packets with */
                pDev->stat.packets_sent++;

                rtems_semaphore_release(pDev->txsp);

                if ( ctrl & SPW_TXBD_LE ) {
                        pDev->stat.tx_link_err++;
                }

                /* step to next descriptor */
                pDev->tx_sent = (pDev->tx_sent + 1) % pDev->txbufcnt;
                pDev->tx_all_in_use = 0; /* not all of the descriptors can be in use since we just freed one. */
        }

        /* Check for rx interrupts */
        if (dmactrl & SPW_DMACTRL_PR) {
                rtems_semaphore_release(pDev->rxsp);
        }
}

static rtems_device_driver grspw_initialize(
        rtems_device_major_number  major,
        rtems_device_minor_number  minor,
        void                      *arg
)
{
        rtems_status_code status;
        int i=0;
        char c;
        GRSPW_DEV *pDev;
        char console_name[20];
        struct ambapp_apb_info dev;

        SPACEWIRE_DBG2("spacewire driver initialization\n");

        /* Copy device name */
        strcpy(console_name,GRSPW_DEVNAME);

        /* Get the number of GRSPW cores */
        i=0; spw_cores = 0; spw_cores2 = 0;

        /* get number of GRSPW cores */
        spw_cores = ambapp_get_number_apbslv_devices(amba_bus, VENDOR_GAISLER,
                                                     GAISLER_SPW);
        spw_cores2 = ambapp_get_number_apbslv_devices(amba_bus, VENDOR_GAISLER,
                                                      GAISLER_SPW2);

        if ( (spw_cores+spw_cores2) < 1 ){
                /* No GRSPW cores around... */
                return RTEMS_SUCCESSFUL;
        }

        /* Allocate memory for all spacewire cores */
        grspw_devs = (GRSPW_DEV *)malloc((spw_cores+spw_cores2) * sizeof(GRSPW_DEV));

        /* Zero out all memory */
        memset(grspw_devs,0,(spw_cores+spw_cores2) * sizeof(GRSPW_DEV));

        /* loop all found spacewire cores */
        i = 0;
        for(minor=0; minor<(spw_cores+spw_cores2); minor++){

                pDev = &grspw_devs[minor];

                /* Get device */
                if ( spw_cores > minor ) {
                        ambapp_find_apbslv_next(amba_bus, VENDOR_GAISLER,
                                                GAISLER_SPW, &dev, minor);
                        pDev->core_ver = 1;
                } else {
                        ambapp_find_apbslv_next(amba_bus, VENDOR_GAISLER,
                                                GAISLER_SPW2, &dev,
                                                minor - spw_cores);
                        pDev->core_ver = 2;
                }

                pDev->regs = (LEON3_SPACEWIRE_Regs_Map *)dev.start;
                pDev->irq = dev.irq;
                pDev->minor = minor;
                pDev->open = 0;

                /* register interrupt routine */
                GRSPW_REG_INT(GRSPW_PREFIX(_interrupt_handler), pDev->irq, pDev);

                SPACEWIRE_DBG("spacewire core at [0x%x]\n", (unsigned int) pDev->regs);

                /* initialize the code with some resonable values,
                   actual initialization is done later using ioctl(fd)
                   on the opened device */
                pDev->config.rxmaxlen = SPACEWIRE_RXPCK_SIZE;
                pDev->txdbufsize = SPACEWIRE_TXD_SIZE;
                pDev->txhbufsize = SPACEWIRE_TXH_SIZE;
                pDev->rxbufsize = SPACEWIRE_RXPCK_SIZE;
                pDev->txbufcnt = SPACEWIRE_TXBUFS_NR;
                pDev->rxbufcnt = SPACEWIRE_RXBUFS_NR;
                pDev->config.check_rmap_err = 0;
                pDev->config.tx_blocking = 0;
                pDev->config.tx_block_on_full = 0;
                pDev->config.rx_blocking = 0;
                pDev->config.disable_err = 0;
                pDev->config.link_err_irq = 0;
                pDev->config.event_id = 0;

                pDev->ptr_rxbuf0 = 0;
                pDev->ptr_txdbuf0 = 0;
                pDev->ptr_txhbuf0 = 0;

#ifdef GRSPW_STATIC_MEM
                GRSPW_CALC_MEMOFS(spw_cores,minor,&pDev->membase,&pDev->memend,&pDev->mem_bdtable);
#endif

                if (grspw_buffer_alloc(pDev))
                        return RTEMS_NO_MEMORY;

        }

        /*  Register Device Names, /dev/grspw0, /dev/grspw1  ... */
        for (i = 0; i < spw_cores+spw_cores2; i++) {
           GRSPW_DEVNAME_NO(console_name,i);
           SPACEWIRE_DBG("registering minor %i as %s\n", i, console_name);
           status = rtems_io_register_name( console_name, major, i);
           if (status != RTEMS_SUCCESSFUL){
              rtems_fatal_error_occurred(status);
           }
        }

        /* Initialize Hardware and semaphores*/
        c = 'a';
        for (i = 0; i < spw_cores+spw_cores2; i++) {
                pDev = &grspw_devs[i];
                rtems_semaphore_create(
                        rtems_build_name('T', 'x', 'S', c),
                        0,
                        RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
                        RTEMS_NO_PRIORITY_CEILING,
                        0,
                        &(pDev->txsp));
                rtems_semaphore_create(
                        rtems_build_name('R', 'x', 'S', c),
                        0,
                        RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
                        RTEMS_NO_PRIORITY_CEILING,
                        0,
                        &(pDev->rxsp));
                c++;
                grspw_hw_init(pDev);
        }

        return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grspw_open(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        )
{
        GRSPW_DEV *pDev;
        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "open [%i,%i]\n", major, minor);
        if ( minor >= (spw_cores+spw_cores2) ) {
                SPACEWIRE_DBG("minor %i too big\n", minor);
                return RTEMS_INVALID_NAME;
        }
        pDev = &grspw_devs[minor];

        if ( pDev->open )
                return RTEMS_RESOURCE_IN_USE;

        /* Mark device open */
        pDev->open = 1;

        pDev->stat.tx_link_err = 0;
        pDev->stat.rx_rmap_header_crc_err = 0;
        pDev->stat.rx_rmap_data_crc_err = 0;
        pDev->stat.rx_eep_err = 0;
        pDev->stat.rx_truncated = 0;
        pDev->stat.parity_err = 0;
        pDev->stat.escape_err = 0;
        pDev->stat.credit_err = 0;
        pDev->stat.write_sync_err = 0;
        pDev->stat.disconnect_err = 0;
        pDev->stat.early_ep = 0;
        pDev->stat.invalid_address = 0;
        pDev->stat.packets_sent = 0;
        pDev->stat.packets_received = 0;

        pDev->running = 0;
        pDev->core_freq_khz = 0;

        /* Reset Core */
        grspw_hw_reset(pDev);

        /* Read default configuration */
        grspw_hw_read_config(pDev);

        return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grspw_close(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        )
{
        GRSPW_DEV *pDev = &grspw_devs[minor];

        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "close [%i,%i]\n", major, minor);
        rtems_semaphore_delete(pDev->txsp);
        rtems_semaphore_delete(pDev->rxsp);

        grspw_hw_stop(pDev,1,1);

        grspw_hw_reset(pDev);

        /* Mark device closed - not open */
        pDev->open = 0;

        return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grspw_read(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        )
{
        GRSPW_DEV *pDev = &grspw_devs[minor];
        rtems_libio_rw_args_t *rw_args;
        unsigned int count = 0;
        rw_args = (rtems_libio_rw_args_t *) arg;

        /* is link up? */
        if ( !pDev->running ) {
                return RTEMS_INVALID_NAME;
        }

        if ((rw_args->count < 1) || (rw_args->buffer == NULL)) {
                return RTEMS_INVALID_NAME;
        }

        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "read  [%i,%i]: buf:0x%x len:%i \n", major, minor, (unsigned int)rw_args->buffer, rw_args->count);

        while ( (count = grspw_hw_receive(pDev, rw_args->buffer, rw_args->count)) == 0) {
		/* wait a moment for any descriptors to get available
		 *
		 * Semaphore is signaled by interrupt handler
		 */
		if (pDev->config.rx_blocking) {
			SPACEWIRE_DBG2("Rx blocking\n");
			rtems_semaphore_obtain(pDev->rxsp, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		} else {
                        SPACEWIRE_DBG2("Rx non blocking\n");
			return RTEMS_RESOURCE_IN_USE;
		}
	}

#ifdef DEBUG_SPACEWIRE_ONOFF
        if (DEBUG_SPACEWIRE_FLAGS & DBGSPW_DUMP) {
                int k;
                for (k = 0; k < count; k++){
                        if (k % 16 == 0) {
                                printf ("\n");
                        }
                        printf ("%.2x(%c) ", rw_args->buffer[k] & 0xff, isprint(rw_args->buffer[k] & 0xff) ? rw_args->buffer[k] & 0xff : ' ');
                }
                printf ("\n");
        }
#endif

        rw_args->bytes_moved = count;
        return RTEMS_SUCCESSFUL;

}

static rtems_device_driver grspw_write(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
)
{
        GRSPW_DEV *pDev = &grspw_devs[minor];
        rtems_libio_rw_args_t *rw_args;
        rw_args = (rtems_libio_rw_args_t *) arg;
        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "write [%i,%i]: buf:0x%x len:%i\n", major, minor, (unsigned int)rw_args->buffer, rw_args->count);

        /* is link up? */
        if ( !pDev->running ) {
                return RTEMS_INVALID_NAME;
        }

        if ((rw_args->count > pDev->txdbufsize) || (rw_args->count < 1) || (rw_args->buffer == NULL)) {
                return RTEMS_INVALID_NAME;
        }

        while ((rw_args->bytes_moved = grspw_hw_send(pDev, 0, NULL, rw_args->count, rw_args->buffer)) == 0) {
                if (pDev->config.tx_block_on_full == 1) {
                        SPACEWIRE_DBG2("Tx Block on full \n");
                        rtems_semaphore_obtain(pDev->txsp, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
                } else {
                        SPACEWIRE_DBG2("Tx non blocking return when full \n");
                        return RTEMS_RESOURCE_IN_USE;
                }
        }
        return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grspw_control(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                    * arg
        )
{
        GRSPW_DEV *pDev = &grspw_devs[minor];
        spw_ioctl_pkt_send *args;
        spw_ioctl_packetsize *ps;
        int status;
        unsigned int tmp,nodeaddr,nodemask;
        int timeout;
        rtems_device_driver ret;
        rtems_libio_ioctl_args_t	*ioarg = (rtems_libio_ioctl_args_t *) arg;
        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "ctrl [%i,%i]\n", major, minor);

        if (!ioarg)
                return RTEMS_INVALID_NAME;


        ioarg->ioctl_return = 0;
        switch(ioarg->command) {
                case SPACEWIRE_IOCTRL_SET_NODEADDR:
                        /*set node address*/
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_NODEADDR %i\n",(unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 255) {
                                return RTEMS_INVALID_NAME;
                        }
                        nodeaddr = ((unsigned int)ioarg->buffer) & 0xff;
                        tmp = SPW_READ(&pDev->regs->nodeaddr);
                        tmp &= 0xffffff00; /* Remove old address */
                        tmp |= nodeaddr;
                        SPW_WRITE(&pDev->regs->nodeaddr, tmp);
                        if ((SPW_READ(&pDev->regs->nodeaddr)&0xff) != nodeaddr) {
                                return RTEMS_IO_ERROR;
                        }
                        pDev->config.nodeaddr = nodeaddr;
                        break;
                case SPACEWIRE_IOCTRL_SET_NODEMASK:
                        /*set node address*/
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_NODEMASK %i\n",(unsigned int)ioarg->buffer);
                        if ( pDev->core_ver > 1 ){
                          if ((unsigned int)ioarg->buffer > 255) {
                                  return RTEMS_INVALID_NAME;
                          }
                          nodemask = ((unsigned int)ioarg->buffer) & 0xff;
                          tmp = SPW_READ(&pDev->regs->nodeaddr);
                          tmp &= 0xffff00ff; /* Remove old mask */
                          tmp |= nodemask<<8;
                          SPW_WRITE(&pDev->regs->nodeaddr, tmp);
                          if (((SPW_READ(&pDev->regs->nodeaddr)>>8)&0xff) != nodemask) {
                                  return RTEMS_IO_ERROR;
                          }
                          pDev->config.nodemask = nodemask;
                        }else{
                          SPACEWIRE_DBG("SPACEWIRE_IOCTRL_SET_NODEMASK: not implemented in GRSPW1 HW\n");
                        }
                        break;
                case SPACEWIRE_IOCTRL_SET_RXBLOCK:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_RXBLOCK %i \n", (unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 1) {
                                return RTEMS_INVALID_NAME;
                        }
                        pDev->config.rx_blocking = (unsigned int)ioarg->buffer;
                        break;
                case SPACEWIRE_IOCTRL_SET_DESTKEY:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_DESTKEY %i\n", (unsigned int)ioarg->buffer);
                        if (!pDev->config.is_rmap) {
                                return RTEMS_NOT_IMPLEMENTED;
                        }
                        if ((unsigned int)ioarg->buffer > 255) {
                                return RTEMS_INVALID_NAME;
                        }
                        SPW_WRITE(&pDev->regs->destkey, (unsigned int)ioarg->buffer);
                        if (SPW_READ(&pDev->regs->destkey) != (unsigned int)ioarg->buffer) {
                                return RTEMS_IO_ERROR;
                        }
                        pDev->config.destkey = (unsigned int)ioarg->buffer;
                        break;
                case SPACEWIRE_IOCTRL_SET_CLKDIV:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_CLKDIV %i\n", (unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 255) {
                                return RTEMS_INVALID_NAME;
                        }
                        tmp = SPW_READ(&pDev->regs->clkdiv);
                        tmp &= ~0xff; /* Remove old Clockdiv Setting */
                        tmp |= ((unsigned int)ioarg->buffer) & 0xff; /* add new clockdiv setting */
                        SPW_WRITE(&pDev->regs->clkdiv, tmp);
                        if (SPW_READ(&pDev->regs->clkdiv) != tmp) {
                                return RTEMS_IO_ERROR;
                        }
                        pDev->config.clkdiv = tmp;
                        break;
                case SPACEWIRE_IOCTRL_SET_CLKDIVSTART:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_CLKDIVSTART %i\n", (unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 255) {
                                return RTEMS_INVALID_NAME;
                        }
                        tmp = SPW_READ(&pDev->regs->clkdiv);
                        tmp &= ~0xff00; /* Remove old Clockdiv Start Setting */
                        tmp |= (((unsigned int)ioarg->buffer) & 0xff)<<8; /* add new clockdiv start setting */
                        SPW_WRITE(&pDev->regs->clkdiv, tmp);
                        if (SPW_READ(&pDev->regs->clkdiv) != tmp) {
                                return RTEMS_IO_ERROR;
                        }
                        pDev->config.clkdiv = tmp;
                        break;
                case SPACEWIRE_IOCTRL_SET_TIMER:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_TIMER %i\n", (unsigned int)ioarg->buffer);
                        if ( pDev->core_ver <= 1 ) {
                          if ((unsigned int)ioarg->buffer > 4095) {
                                  return RTEMS_INVALID_NAME;
                          }
                          SPW_WRITE(&pDev->regs->timer, (SPW_READ(&pDev->regs->timer) & 0xFFFFF000) | ((unsigned int)ioarg->buffer & 0xFFF));
                          if ((SPW_READ(&pDev->regs->timer) & 0xFFF) != (unsigned int)ioarg->buffer) {
                                  return RTEMS_IO_ERROR;
                          }
                          pDev->config.timer = (unsigned int)ioarg->buffer;
                        }else{
                          SPACEWIRE_DBG("SPACEWIRE_IOCTRL_SET_TIMER: removed in GRSPW2 HW\n");
                        }
                        break;
                case SPACEWIRE_IOCTRL_SET_DISCONNECT:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_DISCONNECT %i\n", (unsigned int)ioarg->buffer);
                        if ( pDev->core_ver <= 1 ) {
                          if ((unsigned int)ioarg->buffer > 1023) {
                                  return RTEMS_INVALID_NAME;
                          }
                          SPW_WRITE(&pDev->regs->timer, (SPW_READ(&pDev->regs->timer) & 0xFFC00FFF) | (((unsigned int)ioarg->buffer & 0x3FF) << 12));
                          if (((SPW_READ(&pDev->regs->timer) >> 12) & 0x3FF) != (unsigned int)ioarg->buffer) {
                                  return RTEMS_IO_ERROR;
                          }
                          pDev->config.disconnect = (unsigned int)ioarg->buffer;
                        }else{
                          SPACEWIRE_DBG("SPACEWIRE_IOCTRL_SET_DISCONNECT: not implemented for GRSPW2\n");
                        }
                        break;
                case SPACEWIRE_IOCTRL_SET_PROMISCUOUS:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_PROMISCUOUS %i \n", (unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 1) {
                                return RTEMS_INVALID_NAME;
                        }
                        SPW_CTRL_WRITE(pDev, SPW_CTRL_READ(pDev) | ((unsigned int)ioarg->buffer << 5));
                        if (((SPW_CTRL_READ(pDev) >> 5) & 1) != (unsigned int)ioarg->buffer) {
                                return RTEMS_IO_ERROR;
                        }
                        pDev->config.promiscuous = (unsigned int)ioarg->buffer;
                        break;
                case SPACEWIRE_IOCTRL_SET_RMAPEN:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_RMAPEN %i \n", (unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 1) {
                                return RTEMS_INVALID_NAME;
                        }
                        SPW_CTRL_WRITE(pDev, (SPW_STATUS_READ(pDev) & 0xFFFEFFFF) | ((unsigned int)ioarg->buffer << 16));
                        if (((SPW_CTRL_READ(pDev) >> 16) & 1) != (unsigned int)ioarg->buffer) {
                                return RTEMS_IO_ERROR;
                        }
                        pDev->config.rmapen = (unsigned int)ioarg->buffer;
                        break;
                case SPACEWIRE_IOCTRL_SET_RMAPBUFDIS:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_RMAPBUFDIS %i \n", (unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 1) {
                                return RTEMS_INVALID_NAME;
                        }
                        SPW_CTRL_WRITE(pDev, (SPW_STATUS_READ(pDev) & 0xFFFDFFFF) | ((unsigned int)ioarg->buffer << 17));
                        if (((SPW_CTRL_READ(pDev) >> 17) & 1) != (unsigned int)ioarg->buffer) {
                                return RTEMS_IO_ERROR;
                        }
                        pDev->config.rmapbufdis = (unsigned int)ioarg->buffer;
                        break;
                case SPACEWIRE_IOCTRL_SET_CHECK_RMAP:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_CHECK_RMAP %i \n", (unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 1) {
                                return RTEMS_INVALID_NAME;
                        }
                        pDev->config.check_rmap_err = (unsigned int)ioarg->buffer;
                        break;
                case SPACEWIRE_IOCTRL_SET_RM_PROT_ID:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_RM_PROT_ID %i \n", (unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 1) {
                                return RTEMS_INVALID_NAME;
                        }
                        pDev->config.rm_prot_id = (unsigned int)ioarg->buffer;
                        break;
                case SPACEWIRE_IOCTRL_SET_TXBLOCK:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_TXBLOCK %i \n", (unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 1) {
                                return RTEMS_INVALID_NAME;
                        }
                        pDev->config.tx_blocking = (unsigned int)ioarg->buffer;
                        break;
                case SPACEWIRE_IOCTRL_SET_TXBLOCK_ON_FULL:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_TXBLOCK_ON_FULL %i \n", (unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 1) {
                                return RTEMS_INVALID_NAME;
                        }
                        pDev->config.tx_block_on_full = (unsigned int)ioarg->buffer;
                        break;
                case SPACEWIRE_IOCTRL_SET_DISABLE_ERR:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_DISABLE_ERR %i \n", (unsigned int)ioarg->buffer);
                        if ((unsigned int)ioarg->buffer > 1) {
                                return RTEMS_INVALID_NAME;
                        }
                        pDev->config.disable_err = (unsigned int)ioarg->buffer;
                        break;
                case SPACEWIRE_IOCTRL_SET_LINK_ERR_IRQ:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_LINK_ERR_IRQ %i \n", (unsigned int)ioarg->buffer);
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "CTRL REG: %x\n", SPW_CTRL_READ(pDev));
                        if ((unsigned int)ioarg->buffer > 1) {
                                return RTEMS_INVALID_NAME;
                        }
                        SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFFFDF7) | ((unsigned int)ioarg->buffer << 9) | (pDev->config.link_err_irq << 3));
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "CTRL REG: %x\n", SPW_CTRL_READ(pDev));
                        if (((SPW_CTRL_READ(pDev) >> 9) & 1) != (unsigned int)ioarg->buffer) {
                                return RTEMS_IO_ERROR;
                        }
                        pDev->config.link_err_irq = (unsigned int)ioarg->buffer;
                        break;
                case SPACEWIRE_IOCTRL_SET_EVENT_ID:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_EVENT_ID %i \n", (unsigned int)ioarg->buffer);
                        pDev->config.event_id = (rtems_id)ioarg->buffer;
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "Event id: %i\n", pDev->config.event_id);
                        break;

                /* Change MAX Packet size by:
                 *  - stop RX/TX (if on)
                 *  - wait for hw to complete RX DMA (if on)
                 *  - reallocate buffers with new size
                 *  - tell hw about new size & start RX/TX again (if previously on)
                 */
                case SPACEWIRE_IOCTRL_SET_PACKETSIZE:
                        if (ioarg->buffer == NULL)
                                return RTEMS_INVALID_NAME;
                        ps = (spw_ioctl_packetsize*) ioarg->buffer;
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_RXPACKETSIZE %i \n", (unsigned int)ioarg->buffer);

                        tmp = pDev->running;

                        if ( pDev->running ){
                                /* Stop RX */
                                grspw_hw_stop(pDev,1,1);

                                /* If packetsize fails it is good to know if in running mode */
                                pDev->running = 0;

                                /* Wait for Receiver to finnish pending DMA transfers if any */
                                grspw_hw_wait_rx_inactive(pDev);
                        }

                        /* Save new buffer sizes */
                        pDev->rxbufsize = ps->rxsize;
                        pDev->txdbufsize = ps->txdsize;
                        pDev->txhbufsize = ps->txhsize;
                        pDev->config.rxmaxlen = pDev->rxbufsize;

                        /* Free previous buffers & allocate buffers with new size */
                        if (grspw_buffer_alloc(pDev))
                                return RTEMS_NO_MEMORY;

                        /* if RX was actived before, we reactive it again */
                        if ( tmp ) {
                                if ( (status = grspw_hw_startup(pDev,-1)) != RTEMS_SUCCESSFUL ) {
                                        return status;
                                }
                                pDev->running = 1;
                        }
#if 0
                        /* Rewrite previous config which was wasted due to reset in hw_startup */
                        SPW_WRITE(&pDev->regs->nodeaddr, pDev->config.nodeaddr);
                        SPW_WRITE(&pDev->regs->destkey, pDev->config.destkey);
                        SPW_WRITE(&pDev->regs->clkdiv, pDev->config.clkdiv);
                        SPW_WRITE(&pDev->regs->timer, pDev->config.timer | ( (pDev->config.disconnect & 0x3FF) << 12) );
                        SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & !(SPW_CTRL_LINKSTART | SPW_CTRL_PM | SPW_CTRL_RE | SPW_CTRL_RD | SPW_CTRL_TT | SPW_CTRL_TR)) | \
                                              (pDev->config.promiscuous << 5) | (pDev->config.rmapen << 16) | (pDev->config.rmapbufdis << 17) | \
                                              (pDev->config.linkdisabled) | (pDev->config.linkstart << 1));
#endif
                        break;
                case SPACEWIRE_IOCTRL_GET_CONFIG:
                        if (ioarg->buffer == NULL)
                                return RTEMS_INVALID_NAME;
                        SPACEWIRE_DBG2("SPACEWIRE_IOCTRL_GET_CONFIG \n");
                        (*(spw_config *)ioarg->buffer).nodeaddr = pDev->config.nodeaddr;
                        (*(spw_config *)ioarg->buffer).nodemask = pDev->config.nodemask;
                        (*(spw_config *)ioarg->buffer).destkey = pDev->config.destkey;
                        (*(spw_config *)ioarg->buffer).clkdiv = pDev->config.clkdiv;
                        (*(spw_config *)ioarg->buffer).rxmaxlen = pDev->config.rxmaxlen;
                        (*(spw_config *)ioarg->buffer).timer = pDev->config.timer;
                        (*(spw_config *)ioarg->buffer).disconnect = pDev->config.disconnect;
                        (*(spw_config *)ioarg->buffer).promiscuous = pDev->config.promiscuous;
                        (*(spw_config *)ioarg->buffer).rmapen = pDev->config.rmapen;
                        (*(spw_config *)ioarg->buffer).rmapbufdis = pDev->config.rmapbufdis;
                        (*(spw_config *)ioarg->buffer).check_rmap_err = pDev->config.check_rmap_err;
                        (*(spw_config *)ioarg->buffer).rm_prot_id = pDev->config.rm_prot_id;
                        (*(spw_config *)ioarg->buffer).tx_blocking = pDev->config.tx_blocking;
                        (*(spw_config *)ioarg->buffer).disable_err = pDev->config.disable_err;
                        (*(spw_config *)ioarg->buffer).link_err_irq = pDev->config.link_err_irq;
                        (*(spw_config *)ioarg->buffer).event_id = pDev->config.event_id;
                        (*(spw_config *)ioarg->buffer).is_rmap = pDev->config.is_rmap;
                        (*(spw_config *)ioarg->buffer).is_rmapcrc = pDev->config.is_rmapcrc;
                        (*(spw_config *)ioarg->buffer).is_rxunaligned = pDev->config.is_rxunaligned;
                        (*(spw_config *)ioarg->buffer).linkdisabled = pDev->config.linkdisabled;
                        (*(spw_config *)ioarg->buffer).linkstart = pDev->config.linkstart;
                        (*(spw_config *)ioarg->buffer).rx_blocking = pDev->config.rx_blocking;
                        (*(spw_config *)ioarg->buffer).tx_block_on_full = pDev->config.tx_block_on_full;
                        break;
                case SPACEWIRE_IOCTRL_GET_LINK_STATUS:
                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_GET_STATUS=%i \n", (unsigned int)((SPW_STATUS_READ(pDev) >> 21) & 0x7));
                        *(unsigned int *)ioarg->buffer = (unsigned int )((SPW_STATUS_READ(pDev) >> 21) & 0x7);
                        break;
                case SPACEWIRE_IOCTRL_GET_STATISTICS:
                        if (ioarg->buffer == NULL)
                                return RTEMS_INVALID_NAME;
                        SPACEWIRE_DBG2("SPACEWIRE_IOCTRL_GET_STATISTICS \n");
                        (*(spw_stats *)ioarg->buffer).tx_link_err = pDev->stat.tx_link_err;
                        (*(spw_stats *)ioarg->buffer).rx_rmap_header_crc_err = pDev->stat.rx_rmap_header_crc_err;
                        (*(spw_stats *)ioarg->buffer).rx_rmap_data_crc_err = pDev->stat.rx_rmap_data_crc_err;
                        (*(spw_stats *)ioarg->buffer).rx_eep_err =  pDev->stat.rx_eep_err;
                        (*(spw_stats *)ioarg->buffer).rx_truncated = pDev->stat.rx_truncated;
                        (*(spw_stats *)ioarg->buffer).parity_err = pDev->stat.parity_err;
                        (*(spw_stats *)ioarg->buffer).escape_err = pDev->stat.escape_err;
                        (*(spw_stats *)ioarg->buffer).credit_err = pDev->stat.credit_err;
                        (*(spw_stats *)ioarg->buffer).write_sync_err = pDev->stat.write_sync_err;
                        (*(spw_stats *)ioarg->buffer).disconnect_err = pDev->stat.disconnect_err;
                        (*(spw_stats *)ioarg->buffer).early_ep = pDev->stat.early_ep;
                        (*(spw_stats *)ioarg->buffer).invalid_address = pDev->stat.invalid_address;
                        (*(spw_stats *)ioarg->buffer).packets_sent = pDev->stat.packets_sent;
                        (*(spw_stats *)ioarg->buffer).packets_received = pDev->stat.packets_received;
                        break;
                case SPACEWIRE_IOCTRL_CLR_STATISTICS:
                        SPACEWIRE_DBG2("SPACEWIRE_IOCTRL_CLR_STATISTICS \n");
                        pDev->stat.tx_link_err = 0;
                        pDev->stat.rx_rmap_header_crc_err = 0;
                        pDev->stat.rx_rmap_data_crc_err = 0;
                        pDev->stat.rx_eep_err = 0;
                        pDev->stat.rx_truncated = 0;
                        pDev->stat.parity_err = 0;
                        pDev->stat.escape_err = 0;
                        pDev->stat.credit_err = 0;
                        pDev->stat.write_sync_err = 0;
                        pDev->stat.disconnect_err = 0;
                        pDev->stat.early_ep = 0;
                        pDev->stat.invalid_address = 0;
                        pDev->stat.packets_sent = 0;
                        pDev->stat.packets_received = 0;
                        break;
                case SPACEWIRE_IOCTRL_SEND:
                        if (ioarg->buffer == NULL)
                                return RTEMS_INVALID_NAME;
                        args = (spw_ioctl_pkt_send *)ioarg->buffer;
                        args->sent = 0;

                        /* is link up? */
                        if ( !pDev->running ) {
                                return RTEMS_INVALID_NAME;
                        }

                        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "write [%i,%i]: hlen: %i hbuf:0x%x dlen:%i dbuf:0x%x\n", major, minor,
                                       (unsigned int)args->hlen, (int)args->hdr,(unsigned int)args->dlen, (int)args->data);

                        if ((args->hlen > pDev->txhbufsize) || (args->dlen > pDev->txdbufsize) ||
                            ((args->hlen+args->dlen) < 1) ||
                            ((args->hdr == NULL) && (args->hlen != 0)) || ((args->data == NULL) && (args->dlen != 0))) {
                                return RTEMS_INVALID_NAME;
                        }
                        while ((args->sent = grspw_hw_send(pDev, args->hlen, args->hdr, args->dlen, args->data)) == 0) {
                                if (pDev->config.tx_block_on_full == 1) {
                                        SPACEWIRE_DBG2("Tx Block on full \n");
                                        rtems_semaphore_obtain(pDev->txsp, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
                                } else {
                                        SPACEWIRE_DBG2("Tx non blocking return when full \n");
                                        return RTEMS_RESOURCE_IN_USE;
                                }
                        }
                        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "Tx ioctl return: %i  \n", args->sent);
                        break;

                case SPACEWIRE_IOCTRL_LINKDISABLE:
                        pDev->config.linkdisabled = 1;
                        pDev->config.linkstart = 0;
                        SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFFFFFC) | 1);
                        if ((SPW_CTRL_READ(pDev) & 3) != 1) {
                                return RTEMS_IO_ERROR;
                        }
                        break;

                case SPACEWIRE_IOCTRL_LINKSTART:
                        pDev->config.linkdisabled = 0;
                        pDev->config.linkstart = 1;
                        SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFFFFFC) | 2);
                        if ((SPW_CTRL_READ(pDev) & 3) != 2) {
                                return RTEMS_IO_ERROR;
                        }
                        break;

                /* Calculate timer register from GRSPW Core frequency
                 * Also possible to set disconnect and timer64 from
                 *  - SPACEWIRE_IOCTRL_SET_DISCONNECT
                 *  - SPACEWIRE_IOCTRL_SET_TIMER
                 */
                case SPACEWIRE_IOCTRL_SET_COREFREQ:
                        pDev->core_freq_khz = (unsigned int)ioarg->buffer;
                        if ( pDev->core_freq_khz == 0 ){
                                /* Get GRSPW clock frequency from system clock.
                                 * System clock has been read from timer inited
                                 * by RTEMS loader (mkprom)
                                 */
                                 pDev->core_freq_khz = sys_freq_khz;
                        }

                        /* Only GRSPW1 needs the Timer64 and Disconnect values
                         * GRSPW2 and onwards doesn't have this register.
                         */
                        if ( pDev->core_ver <= 1 ){
                          /* Calculate Timer64 & Disconnect */
                          pDev->config.timer = grspw_calc_timer64(pDev->core_freq_khz);
                          pDev->config.disconnect = grspw_calc_disconnect(pDev->core_freq_khz);

                          /* Set Timer64 & Disconnect Register */
                          SPW_WRITE(&pDev->regs->timer,
                                 (SPW_READ(&pDev->regs->timer) & 0xFFC00000) |
                                 ((pDev->config.disconnect & 0x3FF)<<12) |
                                 (pDev->config.timer & 0xFFF));

                          /* Check that the registers were written successfully */
                          tmp = SPW_READ(&pDev->regs->timer) & 0x003fffff;
                          if ( ((tmp & 0xFFF) != pDev->config.timer) ||
                               (((tmp >> 12) & 0x3FF) != pDev->config.disconnect) ) {
                                  return RTEMS_IO_ERROR;
                          }
                        }
                        break;

                case SPACEWIRE_IOCTRL_START:
                        if ( pDev->running ){
                                return RTEMS_INVALID_NAME;
                        }

                        /* Get timeout from userspace
                         *  timeout:
                         *   ¤  -1           = Default timeout
                         *   ¤  less than -1 = forever
                         *   ¤  0            = no wait, proceed if link is up
                         *   ¤  positive     = specifies number of system clock ticks that
                         *                     startup will wait for link to enter ready mode.
                         */
                        timeout = (int)ioarg->buffer;

                        if ( (ret=grspw_hw_startup(pDev,timeout)) != RTEMS_SUCCESSFUL ) {
                                return ret;
                        }
                        pDev->running = 1;
                        break;

                case SPACEWIRE_IOCTRL_STOP:
                        if ( !pDev->running ){
                                return RTEMS_INVALID_NAME;
                        }
                        pDev->running = 0;

                        /* Stop Receiver and transmitter */
                        grspw_hw_stop(pDev,1,1);
                        break;

                default:
                        return RTEMS_NOT_IMPLEMENTED;
        }

        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "SPW_IOCTRL Return\n");
        return RTEMS_SUCCESSFUL;
}

/* ============================================================================== */

static int grspw_set_rxmaxlen(GRSPW_DEV *pDev) {
        unsigned int rxmax;
        SPW_WRITE(&pDev->regs->dma0rxmax,pDev->config.rxmaxlen); /*set rx maxlength*/
        rxmax = SPW_READ(&pDev->regs->dma0rxmax);
        if (rxmax != pDev->config.rxmaxlen) {
                return 0;
        }
        return 1;
}

static int grspw_hw_init(GRSPW_DEV *pDev) {
        unsigned int ctrl;

        ctrl = SPW_CTRL_READ(pDev);

#ifdef GRSPW_STATIC_MEM
        pDev->rx = (SPACEWIRE_RXBD *) pDev->mem_bdtable;
        pDev->tx = (SPACEWIRE_RXBD *) pDev->mem_bdtable + SPACEWIRE_BDTABLE_SIZE;
#else
        pDev->rx = (SPACEWIRE_RXBD *) SPW_ALIGN(&pDev->_rxtable, SPACEWIRE_BDTABLE_SIZE);
        pDev->tx = (SPACEWIRE_TXBD *) SPW_ALIGN(&pDev->_txtable, SPACEWIRE_BDTABLE_SIZE);
#endif
        SPACEWIRE_DBG("hw_init [minor %i]\n", pDev->minor);

        pDev->config.is_rmap = ctrl & SPW_CTRL_RA;
        pDev->config.is_rxunaligned = ctrl & SPW_CTRL_RX;
        pDev->config.is_rmapcrc = ctrl & SPW_CTRL_RC;
        return 0;
}

static int grspw_hw_waitlink (GRSPW_DEV *pDev, int timeout)
{
        int j;

        if ( timeout == -1 ){
                /* Wait default timeout */
                timeout = SPACEWIRE_INIT_TIMEOUT;
        }

        j=0;
        while (SPW_LINKSTATE(SPW_STATUS_READ(pDev)) != 5) {
                if ( timeout < -1 ) {
                        /* wait forever */
                }else if ( j >= timeout ){
                        /* timeout reached, return fail */
                        return 1;
                }

                /* Sleep for 10 ticks */
                rtems_task_wake_after(10);
                j+=10;
        }
        return 0;
}

static void grspw_hw_reset(GRSPW_DEV *pDev)
{
        SPW_CTRL_WRITE(pDev, SPW_CTRL_RESET); /*reset core*/
        SPW_STATUS_WRITE(pDev, SPW_STATUS_TO | SPW_STATUS_CE | SPW_STATUS_ER | SPW_STATUS_DE | SPW_STATUS_PE |
                         SPW_STATUS_WE | SPW_STATUS_IA | SPW_STATUS_EE); /*clear status*/
        SPW_CTRL_WRITE(pDev, SPW_CTRL_LINKSTART); /*start link core*/
}

static void grspw_hw_read_config(GRSPW_DEV *pDev)
{
        unsigned int tmp;

        tmp = SPW_READ(&pDev->regs->nodeaddr);
        pDev->config.nodeaddr = 0xFF & tmp;
        pDev->config.nodemask = 0xFF & (tmp>>8);
        pDev->config.destkey = 0xFF & SPW_READ(&pDev->regs->destkey);
        pDev->config.clkdiv = 0xFFFF & SPW_READ(&pDev->regs->clkdiv);

        tmp = SPW_CTRL_READ(pDev);
        pDev->config.promiscuous = 1 & (tmp >> 5);
        pDev->config.rmapen = 1 & (tmp >> 16);
        pDev->config.rmapbufdis = 1 & (tmp >> 17);
        pDev->config.is_rmap = 1 & (tmp >> 31);
        pDev->config.is_rxunaligned = 1 & (tmp >> 30);
        pDev->config.is_rmapcrc = 1 & (tmp >> 29);
        pDev->config.linkdisabled = 1 & tmp;
        pDev->config.linkstart = 1 & (tmp >> 1);

        if ( pDev->core_ver <= 1 ){
          tmp = SPW_READ(&pDev->regs->timer);
          pDev->config.timer = 0xFFF & tmp;
          pDev->config.disconnect = 0x3FF & (tmp >> 12);
        }else{
          pDev->config.timer = 0;
          pDev->config.disconnect = 0;
        }

        return;
}

/* timeout is given in ticks */
static int grspw_hw_startup (GRSPW_DEV *pDev, int timeout)
{
        int i;
        unsigned int dmactrl;

        SPW_WRITE(&pDev->regs->status, (SPW_STATUS_TO|SPW_STATUS_CE|SPW_STATUS_ER|SPW_STATUS_DE|SPW_STATUS_PE|SPW_STATUS_WE|SPW_STATUS_IA|SPW_STATUS_EE)); /*clear status*/

        if (grspw_hw_waitlink(pDev,timeout)) {
                SPACEWIRE_DBG2("Device open. Link is not up\n");
                return RTEMS_TIMEOUT;
        }

        SPW_WRITE(&pDev->regs->dma0ctrl, SPW_DMACTRL_PS | SPW_DMACTRL_PR | SPW_DMACTRL_TA | SPW_DMACTRL_RA); /*clear status, set ctrl*/


        if ((dmactrl = SPW_READ(&pDev->regs->dma0ctrl)) != 0) {
                SPACEWIRE_DBG2("DMACtrl is not cleared\n");
                return RTEMS_IO_ERROR;
        }

        /* prepare transmit buffers */
        for (i = 0; i < pDev->txbufcnt; i++) {
                pDev->tx[i].ctrl = 0;
                pDev->tx[i].addr_header = memarea_to_hw(((unsigned int)&pDev->ptr_txhbuf0[0]) + (i * pDev->txhbufsize));
                pDev->tx[i].addr_data = memarea_to_hw(((unsigned int)&pDev->ptr_txdbuf0[0]) + (i * pDev->txdbufsize));
        }
        pDev->tx_cur = 0;
        pDev->tx_sent = 0;
        pDev->tx_all_in_use = 0;

        /* prepare receive buffers */
        for (i = 0; i < pDev->rxbufcnt; i++) {
                if (i+1 == pDev->rxbufcnt) {
                        pDev->rx[i].ctrl = SPW_RXBD_IE | SPW_RXBD_EN | SPW_RXBD_WR;
                } else {
                        pDev->rx[i].ctrl = SPW_RXBD_IE | SPW_RXBD_EN;
                }
                pDev->rx[i].addr = memarea_to_hw(((unsigned int)&pDev->ptr_rxbuf0[0]) + (i * pDev->rxbufsize));
        }
        pDev->rxcur = 0;
        pDev->rxbufcur = -1;
        grspw_set_rxmaxlen(pDev);

        SPW_WRITE(&pDev->regs->dma0txdesc, memarea_to_hw((unsigned int) pDev->tx));
        SPW_WRITE(&pDev->regs->dma0rxdesc, memarea_to_hw((unsigned int) pDev->rx));

        /* start RX */
        dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
        SPW_WRITE(&pDev->regs->dma0ctrl, (dmactrl & SPW_PREPAREMASK_RX) | SPW_DMACTRL_RD | SPW_DMACTRL_RXEN | SPW_DMACTRL_NS | SPW_DMACTRL_TXIE | SPW_DMACTRL_RXIE);

        SPACEWIRE_DBGC(DBGSPW_TX,"0x%x: setup complete\n", (unsigned int)pDev->regs);
        return RTEMS_SUCCESSFUL;
}

/* Wait until the receiver is inactive */
static void grspw_hw_wait_rx_inactive(GRSPW_DEV *pDev)
{
        while( SPW_READ(&pDev->regs->dma0ctrl) & SPW_DMACTRL_RX ){
                /* switching may be needed:
                 *  - low frequency GRSPW
                 *  - mega packet incoming
                 */
                rtems_task_wake_after(1);
        }
}

/* Stop the rx or/and tx by disabling the receiver/transmitter */
static int grspw_hw_stop (GRSPW_DEV *pDev, int rx, int tx)
{
        unsigned int dmactrl;

        /* stop rx and/or tx */
        dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
        if ( rx ) {
                dmactrl &= ~(SPW_DMACTRL_RXEN|SPW_DMACTRL_RXIE|SPW_DMACTRL_RD);
        }
        if ( tx ) {
                dmactrl &= ~(SPW_DMACTRL_TXEN|SPW_DMACTRL_TXIE);
        }
        /*SPW_WRITE(&pDev->regs->dma0ctrl, (dmactrl & SPW_PREPAREMASK_RX) & ~(SPW_DMACTRL_RD | SPW_DMACTRL_RXEN) & ~(SPW_DMACTRL_TXEN));*/

        /* don't clear status flags */
        dmactrl &= ~(SPW_DMACTRL_RA|SPW_DMACTRL_PR|SPW_DMACTRL_AI);
        SPW_WRITE(&pDev->regs->dma0ctrl, dmactrl);
        return RTEMS_SUCCESSFUL;
}

int grspw_hw_send(GRSPW_DEV *pDev, unsigned int hlen, char *hdr, unsigned int dlen, char *data)
{

        unsigned int dmactrl, ctrl;
#ifdef DEBUG_SPACEWIRE_ONOFF
				unsigned int k;
#endif
        rtems_interrupt_level level;
        unsigned int cur = pDev->tx_cur;
        char *txh = pDev->ptr_txhbuf0 + (cur * pDev->txhbufsize);
        char *txd = pDev->ptr_txdbuf0 + (cur * pDev->txdbufsize);

        ctrl = SPW_READ((volatile void *)&pDev->tx[cur].ctrl);

        if (ctrl & SPW_TXBD_EN) {
                return 0;
        }

        memcpy(&txd[0], data, dlen);
        memcpy(&txh[0], hdr, hlen);

#ifdef DEBUG_SPACEWIRE_ONOFF
        if (DEBUG_SPACEWIRE_FLAGS & DBGSPW_DUMP) {
                for (k = 0; k < hlen; k++){
                        if (k % 16 == 0) {
                                printf ("\n");
                        }
                        printf ("%.2x(%c) ",txh[k] & 0xff,isprint(txh[k] & 0xff) ? txh[k] & 0xff : ' ');
                }
                printf ("\n");
        }
        if (DEBUG_SPACEWIRE_FLAGS & DBGSPW_DUMP) {
                for (k = 0; k < dlen; k++){
                        if (k % 16 == 0) {
                                printf ("\n");
                        }
                        printf ("%.2x(%c) ",txd[k] & 0xff,isprint(txd[k] & 0xff) ? txd[k] & 0xff : ' ');
                }
                printf ("\n");
        }
#endif

        pDev->tx[cur].addr_header = memarea_to_hw((unsigned int)txh);
        pDev->tx[cur].len = dlen;
        pDev->tx[cur].addr_data = memarea_to_hw((unsigned int)txd);
        if (pDev->tx_cur == (pDev->txbufcnt - 1) ) {
                pDev->tx[cur].ctrl = SPW_TXBD_IE | SPW_TXBD_WR | SPW_TXBD_EN | hlen;
        } else {
                pDev->tx[cur].ctrl = SPW_TXBD_IE | SPW_TXBD_EN | hlen;
        }

        dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
        SPW_WRITE(&pDev->regs->dma0ctrl, (dmactrl & SPW_PREPAREMASK_TX) | SPW_DMACTRL_TXEN | SPW_DMACTRL_TXIE);

        /* Update counters */
        rtems_interrupt_disable(level);

        pDev->tx_cur = (pDev->tx_cur + 1) % pDev->txbufcnt;
        if (pDev->tx_cur == pDev->tx_sent) {
                pDev->tx_all_in_use = 1;
        }
        rtems_interrupt_enable(level);

        /* In blocking mode wait until message is sent */
	if (pDev->config.tx_blocking) {
		while ( SPW_READ(&pDev->regs->dma0ctrl) & SPW_DMACTRL_TXEN) {
			/* if changed to blocking mode  */
                        SPACEWIRE_DBGC(DBGSPW_TX, "Tx blocking\n");
                        rtems_semaphore_obtain(pDev->txsp, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		}
	}
        SPACEWIRE_DBGC(DBGSPW_TX, "0x%x: transmitted <%i> bytes\n", (unsigned int) pDev->regs, dlen+hlen);
        return hlen + dlen;
}

static int grspw_hw_receive(GRSPW_DEV *pDev, char *b, int c) {
        unsigned int len, rxlen, ctrl;
        unsigned int cur;
        unsigned int tmp;
        unsigned int dump_start_len;
        int i;
        char *rxb;

        if ( pDev->config.promiscuous ) {
		dump_start_len = 0; /* make sure address and prot can be read in promiscuous mode */
	} else if (pDev->config.rm_prot_id) {
		dump_start_len = 2; /* skip source address and protocol id */
	} else {
		dump_start_len = 1; /* default: skip only source address */
	}

        rxlen = 0;
        cur = pDev->rxcur;
        rxb = pDev->ptr_rxbuf0 + (cur * pDev->rxbufsize);

        SPACEWIRE_DBGC(DBGSPW_RX, "0x%x: waitin packet at pos %i\n", (unsigned int) pDev->regs, cur);

        ctrl = SPW_READ((volatile void *)&pDev->rx[cur].ctrl);
        if (ctrl & SPW_RXBD_EN) {
                return rxlen;
        }
        SPACEWIRE_DBGC(DBGSPW_RX, "checking packet\n");

        len = SPW_RXBD_LENGTH & ctrl;
        if (!((ctrl & SPW_RXBD_ERROR) || (pDev->config.check_rmap_err && (ctrl & SPW_RXBD_RMAPERROR)))) {
                if (pDev->rxbufcur == -1) {
                        SPACEWIRE_DBGC(DBGSPW_RX, "incoming packet len %i\n", len);
                        pDev->stat.packets_received++;
                        pDev->rxbufcur = dump_start_len;
                }
                rxlen = tmp = len - pDev->rxbufcur;
                SPACEWIRE_DBGC(DBGSPW_RX, "C %i\n", c);
                SPACEWIRE_DBGC(DBGSPW_RX, "Dump %i\n", dump_start_len);
                SPACEWIRE_DBGC(DBGSPW_RX, "Bufcur %i\n", pDev->rxbufcur);
                SPACEWIRE_DBGC(DBGSPW_RX, "Rxlen %i\n", rxlen );
                if (rxlen > c) {
                        rxlen = c;
                }
                if (CPU_SPARC_HAS_SNOOPING) {
                        memcpy(b, rxb+pDev->rxbufcur, rxlen);
                } else {
                        for(i = 0; i < rxlen; i++) {
                                b[i] = MEM_READ(rxb+pDev->rxbufcur);
                        }
                }

                pDev->rxbufcur += rxlen;
                if (c >= tmp) {
                        SPACEWIRE_DBGC(DBGSPW_RX, "Next descriptor\n");
                        grspw_rxnext(pDev);
                }
        } else {
                check_rx_errors(pDev, ctrl);
        }
        return rxlen;
}

static void grspw_rxnext(GRSPW_DEV *pDev)
{
        unsigned int dmactrl;
        unsigned int cur = pDev->rxcur;
        unsigned int ctrl = 0;
        if (cur == (pDev->rxbufcnt - 1)) {
                pDev->rx[cur].ctrl = ctrl | SPW_RXBD_EN | SPW_RXBD_IE | SPW_RXBD_WR;
                cur = 0;
        } else {
                pDev->rx[cur].ctrl = ctrl | SPW_RXBD_EN | SPW_RXBD_IE;
                cur++;
        }

        pDev->rxcur = cur;
        pDev->rxbufcur = -1;

        /* start RX */
        dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
        SPW_WRITE(&pDev->regs->dma0ctrl, (dmactrl & SPW_PREPAREMASK_RX) | SPW_DMACTRL_RD | SPW_DMACTRL_RXEN | SPW_DMACTRL_RXIE | SPW_DMACTRL_NS);

}

static void check_rx_errors(GRSPW_DEV *pDev, int ctrl)
{
        if (ctrl & SPW_RXBD_EEP) {
                pDev->stat.rx_eep_err++;
        }
        if (ctrl & SPW_RXBD_EHC) {
                if (pDev->config.check_rmap_err) {
                        pDev->stat.rx_rmap_header_crc_err++;
                }
        }
        if (ctrl & SPW_RXBD_EDC) {
                if (pDev->config.check_rmap_err) {
                        pDev->stat.rx_rmap_data_crc_err++;
                }
        }
        if (ctrl & SPW_RXBD_ETR) {
                pDev->stat.rx_truncated++;
        }
}


