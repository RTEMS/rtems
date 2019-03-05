/**
 * @file
 * @ingroup 1553
 */

/*
 *  COPYRIGHT (c) 2006.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __B1553BRM_H__
#define __B1553BRM_H__

/**
 * @defgroup 1553 B1553BRM
 *
 * @ingroup RTEMSBSPsSharedGRLIB
 *
 * @brief B1553BRM device driver
 *
 * @{
 */

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

struct brm_reg {
    volatile unsigned int ctrl;            /* 0x00 */
    volatile unsigned int oper;            /* 0x04 */
    volatile unsigned int cur_cmd;         /* 0x08 */
    volatile unsigned int imask;           /* 0x0C */
    volatile unsigned int ipend;           /* 0x10 */
    volatile unsigned int ipoint;          /* 0x14 */
    volatile unsigned int bit_reg;         /* 0x18 */
    volatile unsigned int ttag;            /* 0x1C */
    volatile unsigned int dpoint;          /* 0x20 */
    volatile unsigned int sw;              /* 0x24 */
    volatile unsigned int initcount;       /* 0x28 */
    volatile unsigned int mcpoint;         /* 0x2C */
    volatile unsigned int mdpoint;         /* 0x30 */
    volatile unsigned int mbc;             /* 0x34 */
    volatile unsigned int mfilta;          /* 0x38 */
    volatile unsigned int mfiltb;          /* 0x3C */
    volatile unsigned int rt_cmd_leg[16];  /* 0x40-0x80 */
    volatile unsigned int enhanced;        /* 0x84 */

    volatile unsigned int dummy[31];

    volatile unsigned int w_ctrl;          /* 0x100 */
    volatile unsigned int w_irqctrl;       /* 0x104 */
    volatile unsigned int w_ahbaddr;       /* 0x108 */
};

struct bm_msg {
    unsigned short miw;
    unsigned short cw1;
    unsigned short cw2;
    unsigned short sw1;
    unsigned short sw2;
    unsigned short time;
    unsigned short data[32];
};

struct rt_msg {
    unsigned short miw;
    unsigned short time;
    unsigned short data[32];
    unsigned short desc;
};

/*
 * rtaddr[0] and subaddr[0] :  RT address and subaddress (for rt-rt receive addresses)
 * rtaddr[1] and subaddr[1] :  Only for RT-RT. Transmit addresses.
 *
 * wc : word count, or mode code if subaddress 0 or 31.
 *
 * ctrl, bit 0 (TR)      : 1 - transmit, 0 - receive. Ignored for rt-rt
 *       bit 1 (RTRT)    : 1 - rt to rt, 0 - normal
 *       bit 2 (AB)      : 1 - Bus B, 0 - Bus A
 *       bit 4:3 (Retry) : 1 - 1, 2 - 2, 3 - 3, 0 - 4
 *       bit 5 (END)     : End of list
 *       bit 15 (BAME)   : Message error. Set by BRM if protocol error is detected
 *
 * tsw[0] : status word
 * tsw[1] : Only for rt-rt, status word 2
 *
 * data : data to be transmitted, or received data
 *
 */
struct bc_msg {
    unsigned char  rtaddr[2];
    unsigned char  subaddr[2];
    unsigned short wc;
    unsigned short ctrl;
    unsigned short tsw[2];
    unsigned short data[32];
};

/* BC control bits */
#define BC_TR     0x0001
#define BC_RTRT   0x0002
#define BC_BUSA   0x0004
#define BC_EOL    0x0020
#define BC_SKIP   0x0040
#define BC_BAME   0x8000

#define BRM_MBC_IRQ        1                    /* Monitor Block Counter irq */
#define BRM_CBA_IRQ        2                    /* Command Block Accessed irq */
#define BRM_RTF_IRQ        4                    /* Retry Fail irq */
#define BRM_ILLOP_IRQ      8                    /* Illogical Opcode irq */
#define BRM_BC_ILLCMD_IRQ  16                   /* BC Illocigal Command irq */
#define BRM_EOL_IRQ        32                   /* End Of List irq */
#define BRM_RT_ILLCMD_IRQ  128                  /* RT Illegal Command irq */
#define BRM_IXEQ0_IRQ      256                  /* Index Equal Zero irq */
#define BRM_BDRCV_IRQ      512                  /* Broadcast Command Received irq */
#define BRM_SUBAD_IRQ      1024                 /* Subaddress Accessed irq */
#define BRM_MERR_IRQ       2048                 /* Message Error irq */
#define BRM_TAPF_IRQ       8192                 /* Terminal Address Parity Fail irq */
#define BRM_WRAPF_IRQ      16384                /* Wrap Fail irq */
#define BRM_DMAF_IRQ       32768                /* DMA Fail irq */


#define BRM_SET_MODE    0
#define BRM_SET_BUS     1
#define BRM_SET_MSGTO   2
#define BRM_SET_RT_ADDR 3
#define BRM_SET_STD     4
#define BRM_SET_BCE     5
#define BRM_TX_BLOCK    7
#define BRM_RX_BLOCK    8

#define BRM_DO_LIST     10
#define BRM_LIST_DONE   11

#define BRM_CLR_STATUS  12
#define BRM_GET_STATUS  13
#define BRM_SET_EVENTID 14

#define GET_ERROR_DESCRIPTOR(event_in) (event_in>>16)


#define BRM_MODE_BC 0x0
#define BRM_MODE_RT 0x1
#define BRM_MODE_BM 0x2
#define BRM_MODE_BM_RT 0x3 /* both RT and BM */

#define BRM_FREQ_12MHZ 0
#define BRM_FREQ_16MHZ 1
#define BRM_FREQ_20MHZ 2
#define BRM_FREQ_24MHZ 3
#define BRM_FREQ_MASK 0x3

#define CLKDIV_MASK 0xf

#define CLKSEL_MASK 0x7

void b1553brm_register_drv(void);

/* Default initialization of the RT legalization registers. The values in this
 * array are written to the registers on boot driver initialization and when
 * the user set the mode to RT-mode by calling ioctl(BRM_SET_MODE). Thus,
 * update the array first then call ioctl(BRM_SET_MODE) for the changes to have
 * an affect. Note that this affects all B1553BRM RTs in the system.
 */
extern unsigned short b1553brm_rt_cmd_legalize[16];

/* Print information about all BRM devices handled by this driver */
void b1553brm_print(int options);

/* Print information about one BRM device */
void b1553brm_print_dev(struct drvmgr_dev *dev, int options);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __BRM_H__ */

