/*
 *  This file contains the GRSPW SpaceWire Driver for LEON2 and LEON3.
 *
 *  COPYRIGHT (c) 2006
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
#include <rtems/malloc.h>
#include <ambapp.h>

#include <drvmgr/drvmgr.h>
#include <drvmgr/ambapp_bus.h>
#include <bsp/grspw.h>

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

typedef struct {
   /* configuration parameters */ 
   struct drvmgr_dev *dev; /* Driver manager device */
   char devName[32]; /* Device Name */
   LEON3_SPACEWIRE_Regs_Map *regs;
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

   /* DMA Area set by user */
   unsigned int rx_dma_area;
   unsigned int tx_data_dma_area;
   unsigned int tx_hdr_dma_area;
   unsigned int bd_dma_area;

   /* statistics */
   spw_stats stat;

   unsigned int _ptr_rxbuf0;
   char *ptr_rxbuf0;
   char *ptr_txdbuf0;
   char *ptr_txhbuf0;
   char *_ptr_bd0, *ptr_bd0;

   char *ptr_rxbuf0_remote;
   char *ptr_txdbuf0_remote;
   char *ptr_txhbuf0_remote;
   char *ptr_bd0_remote;

   unsigned int irq;
   int minor;
   int core_ver;
   int open;
   int running;
   unsigned int core_freq_khz;
   unsigned int rtimeout;

   /* semaphores*/
   rtems_id txsp;
   rtems_id rxsp;

   SPACEWIRE_RXBD *rx;
   SPACEWIRE_TXBD *tx;

   unsigned int rx_remote;
   unsigned int tx_remote;
} GRSPW_DEV;

/* Function pointer called upon timecode receive */
void (*grspw_timecode_callback)
    (void *pDev, void *regs, int minor, unsigned int tc) = NULL;

#ifdef GRSPW_DONT_BYPASS_CACHE
#define _SPW_READ(address) (*(volatile unsigned int *)(address))
#define _MEM_READ8(address) (*(volatile unsigned char *)(address))
#define _MEM_READ32(address) (*(volatile unsigned int *)(address))
#else
static inline unsigned int _SPW_READ(volatile void *addr) {
        unsigned int tmp;
        __asm__ (" lda [%1]1, %0 "
            : "=r"(tmp)
            : "r"(addr)
           );
        return tmp;
}

static inline unsigned int _MEM_READ8(volatile void *addr) {
        unsigned int tmp;
        __asm__ (" lduba [%1]1, %0 "
            : "=r"(tmp)
            : "r"(addr)
           );
        return tmp;
}

static inline unsigned int _MEM_READ32(volatile void *addr) {
        unsigned int tmp;
        __asm__ (" lda [%1]1, %0 "
            : "=r"(tmp)
            : "r"(addr)
           );
        return tmp;
}
#endif

#define MEM_READ8(addr) _MEM_READ8((volatile void *)(addr))
#define MEM_READ32(addr) _MEM_READ32((volatile void *)(addr))
#define SPW_READ(addr) _SPW_READ((volatile void *)(addr))
#define SPW_WRITE(addr,v) (*(volatile unsigned int *)addr)=v

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
#define SPW_TXBD_HC (1 << 16)
#define SPW_TXBD_DC (1 << 17)

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
static int grspw_hw_send(GRSPW_DEV *pDev, unsigned int hlen, char *hdr, unsigned int dlen, char *data, unsigned int options);
static int grspw_hw_receive(GRSPW_DEV *pDev,char *b,int c);
static int grspw_hw_startup (GRSPW_DEV *pDev, int timeout);
static int grspw_hw_stop (GRSPW_DEV *pDev, int rx, int tx);
static void grspw_hw_wait_rx_inactive(GRSPW_DEV *pDev);
static int grspw_hw_waitlink (GRSPW_DEV *pDev, int timeout);
static void grspw_hw_reset(GRSPW_DEV *pDev);
static void grspw_hw_read_config(GRSPW_DEV *pDev);

static void check_rx_errors(GRSPW_DEV *pDev, int ctrl);
static void grspw_rxnext(GRSPW_DEV *pDev);
static void grspw_interrupt(void *arg);
static int grspw_buffer_alloc(GRSPW_DEV *pDev);

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
static int grspw_driver_io_registered = 0;
static rtems_device_major_number grspw_driver_io_major = 0;

/******************* Driver manager interface ***********************/

/* Driver prototypes */
int grspw_register_io(rtems_device_major_number *m);
int grspw_device_init(GRSPW_DEV *pDev);

int grspw_init2(struct drvmgr_dev *dev);
int grspw_init3(struct drvmgr_dev *dev);

struct drvmgr_drv_ops grspw_ops = 
{
	.init = {NULL,  grspw_init2, grspw_init3, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id grspw_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_SPW},
	{VENDOR_GAISLER, GAISLER_SPW2},
	{VENDOR_GAISLER, GAISLER_SPW2_DMA},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info grspw_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_GRSPW_ID,	/* Driver ID */
		"GRSPW_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&grspw_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		0,
	},
	&grspw_ids[0]
};

void grspw_register_drv (void)
{
	SPACEWIRE_DBG("Registering GRSPW driver\n");
	drvmgr_drv_register(&grspw_drv_info.general);
}

int grspw_init2(struct drvmgr_dev *dev)
{
	GRSPW_DEV *priv;

	SPACEWIRE_DBG("GRSPW[%d] on bus %s\n", dev->minor_drv,
		dev->parent->dev->name);
	priv = dev->priv = malloc(sizeof(GRSPW_DEV));
	if ( !priv )
		return DRVMGR_NOMEM;
	memset(priv, 0, sizeof(*priv));
	priv->dev = dev;

	/* This core will not find other cores, so we wait for init2() */

	return DRVMGR_OK;
}

int grspw_init3(struct drvmgr_dev *dev)
{
	GRSPW_DEV *priv;
	char prefix[32];
	rtems_status_code status;

	priv = dev->priv;

	/* Do initialization */

	if ( grspw_driver_io_registered == 0) {
		/* Register the I/O driver only once for all cores */
		if ( grspw_register_io(&grspw_driver_io_major) ) {
			/* Failed to register I/O driver */
			free(dev->priv);
			dev->priv = NULL;
			return DRVMGR_FAIL;
		}

		grspw_driver_io_registered = 1;
	}

	/* I/O system registered and initialized 
	 * Now we take care of device initialization.
	 */

	/* Get frequency in Hz */
	if ( drvmgr_freq_get(dev, DEV_APB_SLV, &priv->core_freq_khz) ) {
		return DRVMGR_FAIL;
	}
	/* Convert from Hz -> kHz */
	priv->core_freq_khz = priv->core_freq_khz / 1000;

	if ( grspw_device_init(priv) ) {
		return DRVMGR_FAIL;
	}

	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if ( drvmgr_get_dev_prefix(dev, prefix) ) {
		/* Failed to get prefix, make sure of a unique FS name
		 * by using the driver minor.
		 */
		sprintf(priv->devName, "/dev/grspw%d", dev->minor_drv);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "/dev/%sgrspw%d", prefix, dev->minor_bus);
	}

	/* Register Device */
	status = rtems_io_register_name(priv->devName, grspw_driver_io_major, dev->minor_drv);
	if (status != RTEMS_SUCCESSFUL) {
		return DRVMGR_FAIL;
	}

	return DRVMGR_OK;
}

/******************* Driver Implementation ***********************/

int grspw_register_io(rtems_device_major_number *m)
{
	rtems_status_code r;

	if ((r = rtems_io_register_driver(0, &grspw_driver, m)) == RTEMS_SUCCESSFUL) {
		SPACEWIRE_DBG("GRSPW driver successfully registered, major: %d\n", *m);
	} else {
		switch(r) {
		case RTEMS_TOO_MANY:
			printk("GRSPW rtems_io_register_driver failed: RTEMS_TOO_MANY\n");
			return -1;
		case RTEMS_INVALID_NUMBER:  
			printk("GRSPW rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n");
			return -1;
		case RTEMS_RESOURCE_IN_USE:
			printk("GRSPW rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n");
			return -1;
		default:
			printk("GRSPW rtems_io_register_driver failed\n");
			return -1;
		}
	}
	return 0;
}

int grspw_device_init(GRSPW_DEV *pDev)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	union drvmgr_key_value *value;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)pDev->dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	pDev->irq = pnpinfo->irq;
	pDev->regs = (LEON3_SPACEWIRE_Regs_Map *)pnpinfo->apb_slv->start;
	pDev->minor = pDev->dev->minor_drv;

	/* Get SpaceWire core version */
	switch( pnpinfo->device ) {
		case GAISLER_SPW:
			pDev->core_ver = 1;
			break;
		case GAISLER_SPW2:
			pDev->core_ver = 2;
			break;
		case GAISLER_SPW2_DMA:
			pDev->core_ver = 3;
			break;
		default:
			return -1;
	}

	/* initialize the code with some resonable values,
	 * actual initialization is done later using ioctl(fd)
	 * on the opened device */
	pDev->config.rxmaxlen = SPACEWIRE_RXPCK_SIZE;
	pDev->txdbufsize = SPACEWIRE_TXD_SIZE;
	pDev->txhbufsize = SPACEWIRE_TXH_SIZE;
	pDev->rxbufsize = SPACEWIRE_RXPCK_SIZE;
	pDev->txbufcnt = SPACEWIRE_TXBUFS_NR;
	pDev->rxbufcnt = SPACEWIRE_RXBUFS_NR;

	pDev->_ptr_rxbuf0 = 0;
	pDev->ptr_rxbuf0 = 0;
	pDev->ptr_txdbuf0 = 0;
	pDev->ptr_txhbuf0 = 0;
	pDev->ptr_bd0 = 0;
	pDev->rx_dma_area = 0;
	pDev->tx_data_dma_area = 0;
	pDev->tx_hdr_dma_area = 0;
	pDev->bd_dma_area = 0;

	/* Get Configuration from Bus resources (Let user override defaults) */

	value = drvmgr_dev_key_get(pDev->dev, "txBdCnt", DRVMGR_KT_INT);
	if ( value )
		pDev->txbufcnt = value->i;

	value = drvmgr_dev_key_get(pDev->dev, "rxBdCnt", DRVMGR_KT_INT);
	if ( value )
		pDev->rxbufcnt = value->i;

	value = drvmgr_dev_key_get(pDev->dev, "txDataSize", DRVMGR_KT_INT);
	if ( value )
		pDev->txdbufsize = value->i;

	value = drvmgr_dev_key_get(pDev->dev, "txHdrSize", DRVMGR_KT_INT);
	if ( value )
		pDev->txhbufsize = value->i;

	value = drvmgr_dev_key_get(pDev->dev, "rxPktSize", DRVMGR_KT_INT);
	if ( value ) {
		pDev->rxbufsize = value->i;
		pDev->config.rxmaxlen = pDev->rxbufsize;
	}

	value = drvmgr_dev_key_get(pDev->dev, "rxDmaArea", DRVMGR_KT_INT);
	if ( value )
		pDev->rx_dma_area = value->i;

	value = drvmgr_dev_key_get(pDev->dev, "txDataDmaArea", DRVMGR_KT_INT);
	if ( value )
		pDev->tx_data_dma_area = value->i;

	value = drvmgr_dev_key_get(pDev->dev, "txHdrDmaArea", DRVMGR_KT_INT);
	if ( value )
		pDev->tx_hdr_dma_area = value->i;

	value = drvmgr_dev_key_get(pDev->dev, "bdDmaArea", DRVMGR_KT_INT);
	if ( value )
		pDev->bd_dma_area = value->i;

	if (grspw_buffer_alloc(pDev)) 
		return RTEMS_NO_MEMORY;

	/* Create semaphores */
	rtems_semaphore_create(
		rtems_build_name('T', 'x', 'S', '0' + pDev->minor), 
		0, 
		RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
		RTEMS_NO_PRIORITY_CEILING, 
		0, 
		&(pDev->txsp));

	rtems_semaphore_create(
		rtems_build_name('R', 'x', 'S', '0' + pDev->minor), 
		0, 
		RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
		RTEMS_NO_PRIORITY_CEILING, 
		0, 
		&(pDev->rxsp));

	grspw_hw_init(pDev);

	return 0;
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

static int grspw_buffer_alloc(GRSPW_DEV *pDev)
{
	/* RX DMA AREA */
	if (pDev->rx_dma_area & 1) {
		/* Address given in remote address */
		pDev->ptr_rxbuf0_remote = (char *)(pDev->rx_dma_area & ~1);
		drvmgr_translate_check(
			pDev->dev,
			DMAMEM_TO_CPU,
			(void *)pDev->ptr_rxbuf0_remote,
			(void **)&pDev->ptr_rxbuf0,
			pDev->rxbufsize * pDev->rxbufcnt);
		
	} else {
		if (pDev->rx_dma_area == 0) {
			if (pDev->_ptr_rxbuf0)
				free((void *)pDev->_ptr_rxbuf0);
			pDev->_ptr_rxbuf0 = (unsigned int) malloc(pDev->rxbufsize * pDev->rxbufcnt+4);
			pDev->ptr_rxbuf0 = (char *)((pDev->_ptr_rxbuf0+7)&~7);
			if ( !pDev->ptr_rxbuf0 )
				return 1;
		} else {
			pDev->ptr_rxbuf0 = (char *)pDev->rx_dma_area;
		}
		drvmgr_translate_check(
			pDev->dev,
			CPUMEM_TO_DMA,
			(void *)pDev->ptr_rxbuf0,
			(void **)&pDev->ptr_rxbuf0_remote,
			pDev->rxbufsize * pDev->rxbufcnt);
	}

	/* TX-DATA DMA AREA */
	if (pDev->tx_data_dma_area & 1) {
		/* Address given in remote address */
		pDev->ptr_txdbuf0_remote = (char*)(pDev->tx_data_dma_area & ~1);
		drvmgr_translate_check(
			pDev->dev,
			DMAMEM_TO_CPU,
			(void *)pDev->ptr_txdbuf0_remote,
			(void **)&pDev->ptr_txdbuf0,
			pDev->txdbufsize * pDev->txbufcnt);
	} else {
		if (pDev->tx_data_dma_area == 0) {
			if (pDev->ptr_txdbuf0)
				free(pDev->ptr_txdbuf0);
			pDev->ptr_txdbuf0 = (char *) malloc(pDev->txdbufsize * pDev->txbufcnt);
			if (!pDev->ptr_txdbuf0)
				return 1;
		} else {
			pDev->ptr_txdbuf0 = (char *)pDev->tx_data_dma_area;
		}
		drvmgr_translate_check(
			pDev->dev,
			CPUMEM_TO_DMA,
			(void *)pDev->ptr_txdbuf0,
			(void **)&pDev->ptr_txdbuf0_remote,
			pDev->txdbufsize * pDev->txbufcnt);
	}

	/* TX-HEADER DMA AREA */
	if (pDev->tx_hdr_dma_area & 1) {
		/* Address given in remote address */
		pDev->ptr_txhbuf0_remote = (char *)(pDev->tx_hdr_dma_area & ~1);
		drvmgr_translate_check(
			pDev->dev,
			DMAMEM_TO_CPU,
			(void *)pDev->ptr_txhbuf0_remote,
			(void **)&pDev->ptr_txhbuf0,
			pDev->txhbufsize * pDev->txbufcnt);
	} else {
		if (pDev->tx_hdr_dma_area == 0) {
			if (pDev->ptr_txhbuf0)
				free(pDev->ptr_txhbuf0);
			pDev->ptr_txhbuf0 = (char *) malloc(pDev->txhbufsize * pDev->txbufcnt);
			if (!pDev->ptr_txhbuf0)
				return 1;
		} else {
			pDev->ptr_txhbuf0 = (char *)pDev->tx_hdr_dma_area;
		}
		drvmgr_translate_check(
			pDev->dev,
			CPUMEM_TO_DMA,
			(void *)pDev->ptr_txhbuf0,
			(void **)&pDev->ptr_txhbuf0_remote,
			pDev->txhbufsize * pDev->txbufcnt);
	}

	/* DMA DESCRIPTOR TABLES */
	if (pDev->bd_dma_area & 1) {
		/* Address given in remote address */
		pDev->ptr_bd0_remote = (char *)(pDev->bd_dma_area & ~1);
		drvmgr_translate_check(
			pDev->dev,
			DMAMEM_TO_CPU,
			(void *)pDev->ptr_bd0_remote,
			(void **)&pDev->ptr_bd0,
			2 * SPACEWIRE_BDTABLE_SIZE);
	} else {
		if (pDev->bd_dma_area == 0) {
			if (pDev->_ptr_bd0)
				free(pDev->_ptr_bd0);
			pDev->_ptr_bd0 =
				rtems_heap_allocate_aligned_with_boundary(
					SPACEWIRE_BDTABLE_SIZE*2, 1024, 0);
			if (!pDev->_ptr_bd0)
				return 1;
			pDev->ptr_bd0 = (char *)pDev->_ptr_bd0;
		} else {
			pDev->ptr_bd0 = (char *)pDev->bd_dma_area;
		}
		drvmgr_translate_check(
			pDev->dev, 
			CPUMEM_TO_DMA,
			(void *)pDev->ptr_bd0,
			(void **)&pDev->ptr_bd0_remote,
			2 * SPACEWIRE_BDTABLE_SIZE);
	}

	return 0;
}

static void grspw_interrupt(void *arg)
{
	GRSPW_DEV *pDev = (GRSPW_DEV *)arg;
	int dmactrl;
	int status;
	int ctrl;
	unsigned int timecode;

	status = SPW_STATUS_READ(pDev);
	/*SPW_STATUS_WRITE(pDev, SPW_STATUS_CE | SPW_STATUS_ER | SPW_STATUS_DE | SPW_STATUS_PE | SPW_STATUS_WE | SPW_STATUS_IA | SPW_STATUS_EE | SPW_STATUS_TO);*/
	SPW_STATUS_WRITE(pDev, status & (SPW_STATUS_CE | SPW_STATUS_ER | SPW_STATUS_DE | SPW_STATUS_PE | SPW_STATUS_WE | SPW_STATUS_IA | SPW_STATUS_EE));

	/* Make sure to put the timecode handling first in order to get the smallest
	 * possible interrupt latency
	 */
	if ( (status & SPW_STATUS_TO) && (grspw_timecode_callback != NULL) ) {
	    /* Timecode received. Let custom function handle this */
	    SPW_STATUS_WRITE(pDev, SPW_STATUS_TO);
	    timecode = SPW_READ(&pDev->regs->time);
	    (grspw_timecode_callback)(pDev,pDev->regs,pDev->minor,timecode);
	}

	/* Clear SPW_DMACTRL_PR if set */
	dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
	/*SPW_WRITE(&pDev->regs->dma0ctrl, dmactrl | SPW_DMACTRL_PR);*/
	SPW_WRITE(&pDev->regs->dma0ctrl, dmactrl);

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
	rtems_device_major_number major,
	rtems_device_minor_number minor,
	void *arg
)
{
	/* Initialize device-common data structures here */
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grspw_open(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void * arg
        ) 
{
	GRSPW_DEV *pDev;
	struct drvmgr_dev *dev;
	SPACEWIRE_DBGC(DBGSPW_IOCALLS, "open [%i,%i]\n", major, minor);

	if ( drvmgr_get_dev(&grspw_drv_info.general, minor, &dev) ) {
		SPACEWIRE_DBG("Wrong minor %d\n", minor);
		return RTEMS_INVALID_NAME;
	}
	pDev = (GRSPW_DEV *)dev->priv;

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

	pDev->config.rm_prot_id = 0;
	pDev->config.keep_source = 0;
	pDev->config.check_rmap_err = 0;
	pDev->config.tx_blocking = 0;
	pDev->config.tx_block_on_full = 0;
	pDev->config.rx_blocking = 0;
	pDev->config.disable_err = 0;
	pDev->config.link_err_irq = 0; 
	pDev->config.event_id = 0;
	pDev->config.rtimeout = 0;

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
	void * arg
	)
{	
	GRSPW_DEV *pDev;
	struct drvmgr_dev *dev;

	if ( drvmgr_get_dev(&grspw_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	pDev = (GRSPW_DEV *)dev->priv;

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
	void		    * arg
	)
{
	rtems_libio_rw_args_t *rw_args;
	unsigned int count = 0;
	GRSPW_DEV *pDev;
	struct drvmgr_dev *dev;
	int status;

	if ( drvmgr_get_dev(&grspw_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	pDev = (GRSPW_DEV *)dev->priv;

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
			if ( pDev->config.rtimeout ) {
				status = rtems_semaphore_obtain(pDev->rxsp, RTEMS_WAIT, pDev->config.rtimeout);
				if ( status == RTEMS_TIMEOUT )
					return RTEMS_TIMEOUT;
			} else {
				rtems_semaphore_obtain(pDev->rxsp, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
			}
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
	void		    * arg
)
{
	rtems_libio_rw_args_t *rw_args;
	GRSPW_DEV *pDev;
	struct drvmgr_dev *dev;

	if ( drvmgr_get_dev(&grspw_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	pDev = (GRSPW_DEV *)dev->priv;

	rw_args = (rtems_libio_rw_args_t *) arg;
	SPACEWIRE_DBGC(DBGSPW_IOCALLS, "write [%i,%i]: buf:0x%x len:%i\n", major, minor, (unsigned int)rw_args->buffer, rw_args->count);

	/* is link up? */
	if ( !pDev->running ) {
		return RTEMS_INVALID_NAME;
	}

	if ((rw_args->count > pDev->txdbufsize) || (rw_args->count < 1) || (rw_args->buffer == NULL)) {
		return RTEMS_INVALID_NAME;
	}

	while ((rw_args->bytes_moved = grspw_hw_send(pDev, 0, NULL, rw_args->count, rw_args->buffer, 0)) == 0) {
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
	void		    * arg
	)
{
	spw_ioctl_pkt_send *args;
	spw_ioctl_packetsize *ps;
	int status;
	unsigned int tmp,mask,nodeaddr,nodemask;
	int timeout;
	rtems_device_driver ret;
	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *) arg;
	GRSPW_DEV *pDev;
	struct drvmgr_dev *dev;

	SPACEWIRE_DBGC(DBGSPW_IOCALLS, "ctrl [%i,%i]\n", major, minor);

	if ( drvmgr_get_dev(&grspw_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	pDev = (GRSPW_DEV *)dev->priv;

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
			if ( pDev->core_ver == 3 )
				break;
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
			if ( pDev->core_ver == 3 )
				break;
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
			SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFEFFFF) | ((unsigned int)ioarg->buffer << 16));
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
			SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFDFFFF) | ((unsigned int)ioarg->buffer << 17));
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
		case SPACEWIRE_IOCTRL_SET_KEEP_SOURCE: 
			SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_KEEP_SOURCE %i \n", (unsigned int)ioarg->buffer);
			if ((unsigned int)ioarg->buffer > 1) {
				return RTEMS_INVALID_NAME;
			}
			pDev->config.keep_source = (unsigned int)ioarg->buffer;
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
			tmp = (SPW_CTRL_READ(pDev) & 0xFFFFFDF7) | ((unsigned int)ioarg->buffer << 9);
			if (tmp & (SPW_CTRL_LI|SPW_CTRL_TQ))
				tmp |= SPW_CTRL_IE;
			SPW_CTRL_WRITE(pDev, tmp);
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
			pDev->rxbufsize = ((ps->rxsize+7)&~7);
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
			(*(spw_config *)ioarg->buffer).keep_source = pDev->config.keep_source;
			(*(spw_config *)ioarg->buffer).rtimeout = pDev->config.rtimeout;
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
			while ((args->sent = grspw_hw_send(pDev, args->hlen, args->hdr, args->dlen, args->data, args->options)) == 0) {
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
			if ( pDev->core_ver != 3 ) {
				SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFFFFFC) | 1);
				if ((SPW_CTRL_READ(pDev) & 3) != 1) {
					return RTEMS_IO_ERROR;
				}
			}
			break;

		case SPACEWIRE_IOCTRL_LINKSTART:
			pDev->config.linkdisabled = 0;
			pDev->config.linkstart = 1;
			if ( pDev->core_ver != 3 ) {
				SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFFFFFC) | 2);
				if ((SPW_CTRL_READ(pDev) & 3) != 2) {
					return RTEMS_IO_ERROR;
				}
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
				drvmgr_freq_get(pDev->dev, DEV_APB_SLV,
					&pDev->core_freq_khz);
				/* Convert from Hz -> kHz */
				pDev->core_freq_khz = pDev->core_freq_khz / 1000;
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
			 *   ¤  -1	   = Default timeout
			 *   ¤  less than -1 = forever
			 *   ¤  0	    = no wait, proceed if link is up
			 *   ¤  positive     = specifies number of system clock ticks that 
			 *		     startup will wait for link to enter ready mode.
			 */
			timeout = (int)ioarg->buffer;
			
			if ( (ret=grspw_hw_startup(pDev,timeout)) != RTEMS_SUCCESSFUL ) {
				return ret;
			}
			pDev->running = 1;
			/* Register interrupt routine and unmask IRQ */
			drvmgr_interrupt_register(pDev->dev, 0, "grspw", grspw_interrupt, pDev);

			break;

		case SPACEWIRE_IOCTRL_STOP:
			if ( !pDev->running ){
				return RTEMS_INVALID_NAME;
			}
			/* Disable interrupts */
			drvmgr_interrupt_unregister(dev, 0, grspw_interrupt, pDev);

			pDev->running = 0;

			/* Stop Receiver and transmitter */
			grspw_hw_stop(pDev,1,1);
			break;

		/* Set time-code control register bits, and Enables/Disables 
		 * Time code interrupt, make sure to connect the callback 
		 * grspw_timecode_callback if using interrupts.
		 */
		case SPACEWIRE_IOCTRL_SET_TCODE_CTRL:
			tmp = (unsigned int)ioarg->buffer;
			mask = tmp & (SPACEWIRE_TCODE_CTRL_IE_MSK|SPACEWIRE_TCODE_CTRL_TT_MSK|SPACEWIRE_TCODE_CTRL_TR_MSK);
			mask <<= 8;
			tmp &= mask;
			tmp = (SPW_CTRL_READ(pDev) & ~(mask | SPW_CTRL_IE)) | tmp;
			if (tmp & (SPW_CTRL_LI|SPW_CTRL_TQ))
				tmp |= SPW_CTRL_IE;
			SPW_CTRL_WRITE(pDev, tmp);
			break;

		/* Set time register and optionaly send a time code */
		case SPACEWIRE_IOCTRL_SET_TCODE:
			tmp = (unsigned int)ioarg->buffer;
			/* Set timecode register */
			if (tmp & SPACEWIRE_TCODE_SET) {
				SPW_WRITE(&pDev->regs->time,
				    ((SPW_READ(&pDev->regs->time) & ~(0xff)) |
				    (tmp & SPACEWIRE_TCODE_TCODE)));
			}
			/* Send timecode directly (tick-in) ? */
			if (tmp & SPACEWIRE_TCODE_TX) {
			    SPW_CTRL_WRITE(pDev,
				((SPW_CTRL_READ(pDev) & ~(SPW_CTRL_TI)) | SPW_CTRL_TI));
			}
			break;

		/* Read time code register and tick-out status bit */
		case SPACEWIRE_IOCTRL_GET_TCODE:
			tmp = (unsigned int)ioarg->buffer;
			if ( !tmp ){
			    return RTEMS_INVALID_NAME;
			}

			/* Copy timecode register */
			if (SPW_READ(&pDev->regs->status) & SPW_STATUS_TO) {
				*(unsigned int *)tmp = (1 << 8) | SPW_READ(&pDev->regs->time);
			} else {
				*(unsigned int *)tmp = SPW_READ(&pDev->regs->time);
			}
			break;

		case SPACEWIRE_IOCTRL_SET_READ_TIMEOUT:
			pDev->config.rtimeout = (unsigned int)ioarg->buffer;
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

	pDev->rx = (SPACEWIRE_RXBD *) pDev->ptr_bd0;
	pDev->tx = (SPACEWIRE_TXBD *) (pDev->ptr_bd0 + SPACEWIRE_BDTABLE_SIZE);

	/* Set up remote addresses */
	pDev->rx_remote = (unsigned int)pDev->ptr_bd0_remote;
	pDev->tx_remote = pDev->rx_remote + SPACEWIRE_BDTABLE_SIZE;

	SPACEWIRE_DBG("hw_init [minor %i]\n", pDev->minor);

	pDev->config.is_rmap = ctrl & SPW_CTRL_RA;
	pDev->config.is_rxunaligned = ctrl & SPW_CTRL_RX;
	pDev->config.is_rmapcrc = ctrl & SPW_CTRL_RC;
	return 0;
}

static int grspw_hw_waitlink (GRSPW_DEV *pDev, int timeout) 
{
	int j;

	/* No actual link interface on a DMA-only GRSPW2 connected to the
	 * SPW router
	 */
	if (pDev->core_ver == 3)
		return 0;

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

	/* Add extra writes to make sure we wait the number of clocks required
	 * after reset
	 */
	SPW_STATUS_WRITE(pDev, SPW_STATUS_TO | SPW_STATUS_CE | SPW_STATUS_ER | SPW_STATUS_DE | SPW_STATUS_PE | 
		SPW_STATUS_WE | SPW_STATUS_IA | SPW_STATUS_EE); /*clear status*/
	SPW_STATUS_WRITE(pDev, SPW_STATUS_TO | SPW_STATUS_CE | SPW_STATUS_ER | SPW_STATUS_DE | SPW_STATUS_PE | 
		SPW_STATUS_WE | SPW_STATUS_IA | SPW_STATUS_EE); /*clear status*/
	SPW_STATUS_WRITE(pDev, SPW_STATUS_TO | SPW_STATUS_CE | SPW_STATUS_ER | SPW_STATUS_DE | SPW_STATUS_PE | 
		SPW_STATUS_WE | SPW_STATUS_IA | SPW_STATUS_EE); /*clear status*/
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
		pDev->tx[i].addr_header = ((unsigned int)&pDev->ptr_txhbuf0_remote[0]) + (i * pDev->txhbufsize);
		pDev->tx[i].addr_data = ((unsigned int)&pDev->ptr_txdbuf0_remote[0]) + (i * pDev->txdbufsize);
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
		pDev->rx[i].addr = ((unsigned int)&pDev->ptr_rxbuf0_remote[0]) + (i * pDev->rxbufsize);
	}
	pDev->rxcur = 0;
	pDev->rxbufcur = -1;
	grspw_set_rxmaxlen(pDev);

	SPW_WRITE(&pDev->regs->dma0txdesc, pDev->tx_remote);
	SPW_WRITE(&pDev->regs->dma0rxdesc, pDev->rx_remote);

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



int grspw_hw_send(GRSPW_DEV *pDev, unsigned int hlen, char *hdr, unsigned int dlen, char *data, unsigned int options) 
{
	unsigned int dmactrl, ctrl;
#ifdef DEBUG_SPACEWIRE_ONOFF
	unsigned int k;
#endif
	rtems_interrupt_level level;
	unsigned int cur = pDev->tx_cur, bdctrl;
	char *txh = pDev->ptr_txhbuf0 + (cur * pDev->txhbufsize);
	char *txd = pDev->ptr_txdbuf0 + (cur * pDev->txdbufsize);
	char *txh_remote = pDev->ptr_txhbuf0_remote + (cur * pDev->txhbufsize);
	char *txd_remote = pDev->ptr_txdbuf0_remote + (cur * pDev->txdbufsize);
	
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
	
	pDev->tx[cur].addr_header = (unsigned int)txh_remote;
	pDev->tx[cur].len = dlen;
	pDev->tx[cur].addr_data = (unsigned int)txd_remote;

	bdctrl = SPW_TXBD_IE | SPW_TXBD_EN | hlen;
	if ( options & GRSPW_PKTSEND_OPTION_HDR_CRC )
		bdctrl |= SPW_TXBD_HC;
	if ( options & GRSPW_PKTSEND_OPTION_DATA_CRC )
		bdctrl |= SPW_TXBD_DC;
	bdctrl |= options & GRSPW_PKTSEND_OPTION_NOCRCLEN_MASK;

	/* Update counters */
	rtems_interrupt_disable(level);
	if (pDev->tx_cur == (pDev->txbufcnt - 1) ) {
		bdctrl |= SPW_TXBD_WR;
	}
	pDev->tx[cur].ctrl = bdctrl;

	dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
	SPW_WRITE(&pDev->regs->dma0ctrl, (dmactrl & SPW_PREPAREMASK_TX) | SPW_DMACTRL_TXEN | SPW_DMACTRL_TXIE);

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

	if ( pDev->config.promiscuous || pDev->config.keep_source ) {
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
/*		if ( 1 ) {*/
			/*printf("RX_MEMCPY(0x%x, 0x%x, 0x%x)\n", (unsigned int)b, (unsigned int)(rxb+pDev->rxbufcur), (unsigned int)rxlen);*/
			memcpy(b, rxb+pDev->rxbufcur, rxlen);
		} else {
			int left = rxlen;
			/* Copy word wise if Aligned */
			if ( (((int)b & 3) == 0) && (((int)(rxb+pDev->rxbufcur) & 3) == 0) ){
				while(left>=32){
					*(int *)(b+0) = MEM_READ32(rxb+pDev->rxbufcur+0);
					*(int *)(b+4) = MEM_READ32(rxb+pDev->rxbufcur+4);
					*(int *)(b+8) = MEM_READ32(rxb+pDev->rxbufcur+8);
					*(int *)(b+12) = MEM_READ32(rxb+pDev->rxbufcur+12);
					*(int *)(b+16) = MEM_READ32(rxb+pDev->rxbufcur+16);
					*(int *)(b+20) = MEM_READ32(rxb+pDev->rxbufcur+20);
					*(int *)(b+24) = MEM_READ32(rxb+pDev->rxbufcur+24);
					*(int *)(b+28) = MEM_READ32(rxb+pDev->rxbufcur+28);
					rxb+=32;
					b+=32;
					left-=32;
				}
				while(left>=4){
					*(int *)b = MEM_READ32(rxb+pDev->rxbufcur);
					rxb+=4;
					b+=4;
					left-=4;
				}
			}
			for(i = 0; i < left; i++) {
				b[i] = MEM_READ8(rxb+pDev->rxbufcur+i);
			}
		}

		pDev->rxbufcur += rxlen;
		if (c >= tmp) {
			SPACEWIRE_DBGC(DBGSPW_RX, "Next descriptor\n");
			grspw_rxnext(pDev);
		}
	} else {
		check_rx_errors(pDev, ctrl);
		grspw_rxnext(pDev);
	}
	return rxlen;
}

static void grspw_rxnext(GRSPW_DEV *pDev) 
{
	unsigned int dmactrl;
	unsigned int cur = pDev->rxcur;
	unsigned int ctrl = 0;
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);

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

	rtems_interrupt_enable(level);
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


static void grspw_print_dev(struct drvmgr_dev *dev, int options)
{
	GRSPW_DEV *pDev = dev->priv;

	/* Print */
	printf("--- GRSPW %s ---\n", pDev->devName);
	printf(" REGS:            0x%x\n", (unsigned int)pDev->regs);
	printf(" IRQ:             %d\n", pDev->irq);
	printf(" CORE VERSION:    %d\n", pDev->core_ver);
	printf(" CTRL:            0x%x\n", pDev->regs->ctrl);
	printf(" STATUS:          0x%x\n", pDev->regs->status);
	printf(" DMA0CTRL:        0x%x\n", pDev->regs->dma0ctrl);
	printf(" TXBD:            0x%x\n", (unsigned int)pDev->tx);
	printf(" RXBD:            0x%x\n", (unsigned int)pDev->rx);
}

void grspw_print(int options)
{
	struct amba_drv_info *drv = &grspw_drv_info;
	struct drvmgr_dev *dev;

	dev = drv->general.dev;
	while(dev) {
		grspw_print_dev(dev, options);
		dev = dev->next_in_drv;
	}
}
