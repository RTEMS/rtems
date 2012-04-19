/*
 *  Macros used for grcan controller
 *
 *  COPYRIGHT (c) 2007.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef __GRCAN_H__
#define __GRCAN_H__

#include <ambapp.h>

#ifdef __cplusplus
extern "C" {
#endif

struct grcan_regs {
    volatile unsigned int conf;          /* 0x00 */
    volatile unsigned int stat;          /* 0x04 */
    volatile unsigned int ctrl;          /* 0x08 */
    volatile unsigned int dummy0[3];     /* 0x0C-0x014 */
    volatile unsigned int smask;         /* 0x18 */
    volatile unsigned int scode;         /* 0x1C */

    volatile unsigned int dummy1[56];    /* 0x20-0xFC */

    volatile unsigned int pimsr;         /* 0x100 */
    volatile unsigned int pimr;          /* 0x104 */
    volatile unsigned int pisr;          /* 0x108 */
    volatile unsigned int pir;           /* 0x10C */
    volatile unsigned int imr;           /* 0x110 */
    volatile unsigned int picr;          /* 0x114 */

    volatile unsigned int dummy2[58];    /* 0x118-0x1FC */

    volatile unsigned int tx0ctrl;       /* 0x200 */
    volatile unsigned int tx0addr;       /* 0x204 */
    volatile unsigned int tx0size;       /* 0x208 */
    volatile unsigned int tx0wr;         /* 0x20C */
    volatile unsigned int tx0rd;         /* 0x210 */
    volatile unsigned int tx0irq;        /* 0x214 */

    volatile unsigned int dummy3[58];    /* 0x218-0x2FC */

    volatile unsigned int rx0ctrl;       /* 0x300 */
    volatile unsigned int rx0addr;       /* 0x304 */
    volatile unsigned int rx0size;       /* 0x308 */
    volatile unsigned int rx0wr;         /* 0x30C */
    volatile unsigned int rx0rd;         /* 0x310 */
    volatile unsigned int rx0irq;        /* 0x314 */
    volatile unsigned int rx0mask;       /* 0x318 */
    volatile unsigned int rx0code;       /* 0x31C */
};

struct grcan_stats {
	unsigned int passive_cnt;
	unsigned int overrun_cnt;
	unsigned int rxsync_cnt;
	unsigned int txsync_cnt;
  unsigned int txloss_cnt;
  unsigned int ahberr_cnt;
  unsigned int ints;
};

struct grcan_timing {
	unsigned char scaler;
	unsigned char ps1;
	unsigned char ps2;
	unsigned int  rsj;
	unsigned char bpr;
};

struct grcan_selection {
	int selection;
	int enable0;
	int enable1;
};

struct grcan_filter {
	unsigned long long mask;
	unsigned long long code;
};

/* CAN MESSAGE */
typedef struct {
	char extended; /* 1= Extended Frame (29-bit id), 0= STD Frame (11-bit id) */
	char rtr; /* RTR - Remote Transmission Request */
	char unused; /* unused */
	unsigned char len;
	unsigned char data[8];
	unsigned int id;
} CANMsg;

#define GRCAN_CFG_ABORT      0x00000001
#define GRCAN_CFG_ENABLE0    0x00000002
#define GRCAN_CFG_ENABLE1    0x00000004
#define GRCAN_CFG_SELECTION  0x00000008
#define GRCAN_CFG_SILENT     0x00000010
#define GRCAN_CFG_BPR        0x00000300
#define GRCAN_CFG_RSJ        0x00007000
#define GRCAN_CFG_PS1        0x00f00000
#define GRCAN_CFG_PS2        0x000f0000
#define GRCAN_CFG_SCALER     0xff000000

#define GRCAN_CFG_BPR_BIT    8
#define GRCAN_CFG_RSJ_BIT    12
#define GRCAN_CFG_PS1_BIT    20
#define GRCAN_CFG_PS2_BIT    16
#define GRCAN_CFG_SCALER_BIT 24

#define GRCAN_CTRL_RESET  0x2
#define GRCAN_CTRL_ENABLE 0x1

#define GRCAN_TXCTRL_ENABLE 1
#define GRCAN_TXCTRL_ONGOING 1

#define GRCAN_RXCTRL_ENABLE 1
#define GRCAN_RXCTRL_ONGOING 1

/* Relative offset of IRQ sources to AMBA Plug&Play */
#define GRCAN_IRQ_IRQ 0
#define GRCAN_IRQ_TXSYNC 1
#define GRCAN_IRQ_RXSYNC 2

#define GRCAN_ERR_IRQ        0x1
#define GRCAN_OFF_IRQ        0x2
#define GRCAN_OR_IRQ         0x4
#define GRCAN_RXAHBERR_IRQ   0x8
#define GRCAN_TXAHBERR_IRQ   0x10
#define GRCAN_RXIRQ_IRQ      0x20
#define GRCAN_TXIRQ_IRQ      0x40
#define GRCAN_RXFULL_IRQ     0x80
#define GRCAN_TXEMPTY_IRQ    0x100
#define GRCAN_RX_IRQ         0x200
#define GRCAN_TX_IRQ         0x400
#define GRCAN_RXSYNC_IRQ     0x800
#define GRCAN_TXSYNC_IRQ     0x1000
#define GRCAN_RXERR_IRQ      0x2000
#define GRCAN_TXERR_IRQ      0x4000
#define GRCAN_RXMISS_IRQ     0x8000
#define GRCAN_TXLOSS_IRQ     0x10000

#define GRCAN_STAT_PASS      0x1
#define GRCAN_STAT_OFF       0x2
#define GRCAN_STAT_OR        0x4
#define GRCAN_STAT_AHBERR    0x8
#define GRCAN_STAT_ACTIVE    0x10
#define GRCAN_STAT_RXERRCNT  0xff00
#define GRCAN_STAT_TXERRCNT  0xff0000

/* IOCTL Commands controlling operational
 * mode
 */
#define GRCAN_IOC_START          1   /* Bring the link up after open or bus-off */
#define GRCAN_IOC_STOP           2   /* stop to change baud rate/config or closing down */
#define GRCAN_IOC_ISSTARTED      3   /* return RTEMS_SUCCESSFUL when started, othervise EBUSY */
#define GRCAN_IOC_FLUSH          4   /* Waits until all TX messages has been sent */

/* IOCTL Commands that require connection
 * to be stopped
 */
#define GRCAN_IOC_SET_SILENT     16  /* enable silent mode read only state */
#define GRCAN_IOC_SET_ABORT      17  /* enable/disable stopping link on AHB Error */
#define GRCAN_IOC_SET_SELECTION  18  /* Set Enable0,Enable1,Selection */
#define GRCAN_IOC_SET_SPEED      19  /* Set baudrate by using driver's baud rate timing calculation routines */
#define GRCAN_IOC_SET_BTRS       20  /* Set baudrate by specifying the timing registers manually */

/* IOCTL Commands can be called whenever */
#define GRCAN_IOC_SET_RXBLOCK    32  /* Enable/disable Blocking on reception (until at least one message has been received) */
#define GRCAN_IOC_SET_TXBLOCK    33  /* Enable/disable Blocking on transmission (until at least one message has been transmitted) */
#define GRCAN_IOC_SET_TXCOMPLETE 34  /* Enable/disable Blocking until all requested messages has been sent */
#define GRCAN_IOC_SET_RXCOMPLETE 35  /* Enable/disable Blocking until all requested has been received */
#define GRCAN_IOC_GET_STATS      36  /* Get Statistics */
#define GRCAN_IOC_CLR_STATS      37  /* Clear Statistics */
#define GRCAN_IOC_SET_AFILTER    38  /* Set Acceptance filters, provide pointer to "struct grcan_filter" or NULL to disable filtering (let all messages pass) */
#define GRCAN_IOC_SET_SFILTER    40  /* Set Sync Messages RX/TX filters, NULL disables the IRQ completely */
#define GRCAN_IOC_GET_STATUS     41  /* Get status register of GRCAN core */

struct grcan_device_info {
  unsigned int base_address;
  int irq;
};

/* Use hard coded addresses and IRQs to find hardware */
int grcan_register_abs(struct grcan_device_info *devices, int dev_cnt);

/* Use prescanned AMBA Plug&Play information to find all GRFIFO cores */
int grcan_register(struct ambapp_bus *abus);
#if 0
void grcan_register(unsigned int baseaddr, unsigned int ram_base);
void grcan_interrupt_handler(rtems_vector_number v);
#endif

#ifdef __cplusplus
}
#endif

#endif
