/*
 * Coldfire MCF5272 definitions.
 * Contents of this file based on information provided in
 * Motorola MCF5272 User's Manual.
 *
 * Copyright (C) 2004 Jay Monkman <jtm@lopingdog.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.OARcorp.com/rtems/license.html.
 *
 * @(#) mcf5206e.h,v 1.1 2001/10/26 19:32:40 joel Exp
 */

#ifndef __MCF5272_H__
#define __MCF5272_H__

#ifndef ASM
#include <rtems.h>
#endif

#define bit(x) (1 << (x))

#define MCF5272_SIM_BASE(mbar)    ((mbar) + 0x0000)
#define MCF5272_INT_BASE(mbar)    ((mbar) + 0x0020)
#define MCF5272_CS_BASE(mbar)     ((mbar) + 0x0040)
#define MCF5272_GPIO_BASE(mbar)   ((mbar) + 0x0080)
#define MCF5272_QSPI_BASE(mbar)   ((mbar) + 0x00A0)
#define MCF5272_PWM_BASE(mbar)    ((mbar) + 0x00C0)
#define MCF5272_DMAC_BASE(mbar)   ((mbar) + 0x00E0)
#define MCF5272_UART0_BASE(mbar)  ((mbar) + 0x0100)
#define MCF5272_UART1_BASE(mbar)  ((mbar) + 0x0140)
#define MCF5272_SDRAMC_BASE(mbar) ((mbar) + 0x0180)
#define MCF5272_TIMER_BASE(mbar)  ((mbar) + 0x0200)
#define MCF5272_PLIC_BASE(mbar)   ((mbar) + 0x0300)
#define MCF5272_ENET_BASE(mbar)   ((mbar) + 0x0840)
#define MCF5272_USB_BASE(mbar)    ((mbar) + 0x1000)


/* RAMBAR - SRAM Base Address Register */
#define MCF5272_RAMBAR_BA  (0xfffff000) /* SRAM Base Address */
#define MCF5272_RAMBAR_WP  (0x00000100) /* Write Protect */
#define MCF5272_RAMBAR_CI  (0x00000020) /* CPU Space mask */
#define MCF5272_RAMBAR_SC  (0x00000010) /* Supervisor Code Space Mask */
#define MCF5272_RAMBAR_SD  (0x00000008) /* Supervisor Data Space Mask */
#define MCF5272_RAMBAR_UC  (0x00000004) /* User Code Space Mask */
#define MCF5272_RAMBAR_UD  (0x00000002) /* User Data Space Mask */
#define MCF5272_RAMBAR_V   (0x00000001) /* Contents of RAMBAR are valid */

/* MBAR - Module Base Address Register */
#define MCF5272_MBAR_BA  (0xffff0000) /* Base Address */
#define MCF5272_MBAR_SC  (0x00000010) /* Supervisor Code Space Mask */
#define MCF5272_MBAR_SD  (0x00000008) /* Supervisor Data Space Mask */
#define MCF5272_MBAR_UC  (0x00000004) /* User Code Space Mask */
#define MCF5272_MBAR_UD  (0x00000002) /* User Data Space Mask */
#define MCF5272_MBAR_V   (0x00000001) /* Contents of MBAR are valid */

/* CACR - Cache Control Register */
#define MCF5272_CACR_CENB  (0x80000000) /* Cache Enable */
#define MCF5272_CACR_CPDI  (0x10000000) /* Disable CPUSHL Invalidation */
#define MCF5272_CACR_CFRZ  (0x08000000) /* Cache Freeze */
#define MCF5272_CACR_CINV  (0x01000000) /* Cache Invalidate */
#define MCF5272_CACR_CEIB  (0x00000400) /* Cache Enable Noncacheable
                                           instruction bursting */
#define MCF5272_CACR_DCM   (0x00000200) /* Default cache mode - noncacheable*/
#define MCF5272_CACR_DBWE  (0x00000100) /* Default Buffered Write Enable */
#define MCF5272_CACR_DWP   (0x00000020) /* Default Write Protection */
#define MCF5272_CACR_CLNF  (0x00000003) /* Cache Line Fill */

/* ACRx - Cache Access Control Registers */
#define MCF5272_ACR_BA        (0xff000000) /* Address Base */
#define MCF5272_ACR_BAM       (0x00ff0000) /* Address Mask */
#define MCF5272_ACR_EN        (0x00008000) /* Enable */
#define MCF5272_ACR_SM_USR    (0x00000000) /* Match if user mode */
#define MCF5272_ACR_SM_SVR    (0x00002000) /* Match if supervisor mode */
#define MCF5272_ACR_SM_ANY    (0x00004000) /* Match Always */
#define MCF527_ACR_CM         (0x00000040) /* Cache Mode (1 - noncacheable) */
#define MCF5272_ACR_BWE       (0x00000020) /* Buffered Write Enable */
#define MCF5272_ACR_WP        (0x00000004) /* Write Protect */
#define MCF5272_ACR_BASE(base) ((base) & MCF5272_ACR_BA)
#define MCF5272_ACR_MASK(mask) (((mask) >> 8) & MCF5272_ACR_BAM)


#define MCF5272_ICR1_INT1_PI     (bit(31))
#define MCF5272_ICR1_INT1_IPL(x) ((x) << 28)
#define MCF5272_ICR1_INT1_MASK   ((7) << 28)
#define MCF5272_ICR1_INT2_PI     (bit(27))
#define MCF5272_ICR1_INT2_IPL(x) ((x) << 24)
#define MCF5272_ICR1_INT2_MASK   ((7) << 24)
#define MCF5272_ICR1_INT3_PI     (bit(23))
#define MCF5272_ICR1_INT3_IPL(x) ((x) << 20)
#define MCF5272_ICR1_INT3_MASK   ((7) << 20)
#define MCF5272_ICR1_INT3_PI     (bit(19))
#define MCF5272_ICR1_INT3_IPL(x) ((x) << 16)
#define MCF5272_ICR1_INT3_MASK   ((7) << 16)
#define MCF5272_ICR1_TMR0_PI     (bit(15))
#define MCF5272_ICR1_TMR0_IPL(x) ((x) << 12)
#define MCF5272_ICR1_TMR0_MASK   ((7) << 12)
#define MCF5272_ICR1_TMR1_PI     (bit(11))
#define MCF5272_ICR1_TMR1_IPL(x) ((x) << 8)
#define MCF5272_ICR1_TMR1_MASK   ((7) << 8)
#define MCF5272_ICR1_TMR2_PI     (bit(7))
#define MCF5272_ICR1_TMR2_IPL(x) ((x) << 4)
#define MCF5272_ICR1_TMR2_MASK   ((7) << 4)
#define MCF5272_ICR1_TMR3_PI     (bit(3))
#define MCF5272_ICR1_TMR3_IPL(x) ((x) << 0)
#define MCF5272_ICR1_TMR3_MASK   ((7) << 0)

#define MCF5272_ICR3_USB4_PI     (bit(31))
#define MCF5272_ICR3_USB4_IPL(x) ((x) << 28)
#define MCF5272_ICR3_USB4_MASK   ((7) << 28)
#define MCF5272_ICR3_USB5_PI     (bit(27))
#define MCF5272_ICR3_USB5_IPL(x) ((x) << 24)
#define MCF5272_ICR3_USB5_MASK   ((7) << 24)
#define MCF5272_ICR3_USB6_PI     (bit(23))
#define MCF5272_ICR3_USB6_IPL(x) ((x) << 20)
#define MCF5272_ICR3_USB6_MASK   ((7) << 20)
#define MCF5272_ICR3_USB7_PI     (bit(19))
#define MCF5272_ICR3_USB7_IPL(x) ((x) << 16)
#define MCF5272_ICR3_USB7_MASK   ((7) << 16)
#define MCF5272_ICR3_DMA_PI     (bit(15))
#define MCF5272_ICR3_DMA_IPL(x) ((x) << 12)
#define MCF5272_ICR3_DMA_MASK   ((7) << 12)
#define MCF5272_ICR3_ERX_PI     (bit(11))
#define MCF5272_ICR3_ERX_IPL(x) ((x) << 8)
#define MCF5272_ICR3_ERX_MASK   ((7) << 8)
#define MCF5272_ICR3_ETX_PI     (bit(7))
#define MCF5272_ICR3_ETX_IPL(x) ((x) << 4)
#define MCF5272_ICR3_ETX_MASK   ((7) << 4)
#define MCF5272_ICR3_ENTC_PI     (bit(3))
#define MCF5272_ICR3_ENTC_IPL(x) ((x) << 0)
#define MCF5272_ICR3_ENTC_MASK   ((7) << 0)


#define MCF5272_USR_RB        (bit(7))
#define MCF5272_USR_FE        (bit(6))
#define MCF5272_USR_PE        (bit(5))
#define MCF5272_USR_OE        (bit(4))
#define MCF5272_USR_TXEMP     (bit(3))
#define MCF5272_USR_TXRDY     (bit(2))
#define MCF5272_USR_FFULL     (bit(1))
#define MCF5272_USR_RXRDY     (bit(0))

#define MCF5272_TMR_PS_MASK     0xff00
#define MCF5272_TMR_PS_SHIFT    8
#define MCF5272_TMR_CE_DISABLE  (0 << 6)
#define MCF5272_TMR_CE_RISING   (1 << 6)
#define MCF5272_TMR_CE_FALLING  (2 << 6)
#define MCF5272_TMR_CE_ANY      (3 << 6)
#define MCF5272_TMR_OM          (bit(5))
#define MCF5272_TMR_ORI         (bit(4))
#define MCF5272_TMR_FRR         (bit(3))
#define MCF5272_TMR_CLK_STOP    (0 << 1)
#define MCF5272_TMR_CLK_MSTR    (1 << 1)
#define MCF5272_TMR_CLK_MSTR16  (2 << 1)
#define MCF5272_TMR_CLK_TIN     (3 << 1)
#define MCF5272_TMR_RST         (bit(0))
#define MCF5272_TER_REF (bit(1))
#define MCF5272_TER_CAP (bit(0))

#define MCF5272_SCR_PRI         (bit(8))
#define MCF5272_SCR_AR          (bit(7))
#define MCF5272_SCR_SRST        (bit(6))
#define MCF5272_SCR_BUSLOCK     (bit(3))
#define MCF5272_SCR_HWR_128     (0)
#define MCF5272_SCR_HWR_256     (1)
#define MCF5272_SCR_HWR_512     (2)
#define MCF5272_SCR_HWR_1024    (3)
#define MCF5272_SCR_HWR_2048    (4)
#define MCF5272_SCR_HWR_4096    (5)
#define MCF5272_SCR_HWR_8192    (6)
#define MCF5272_SCR_HWR_16384   (7)

#define MCF5272_SPR_ADC         (bit(15))
#define MCF5272_SPR_WPV         (bit(15))
#define MCF5272_SPR_SMV         (bit(15))
#define MCF5272_SPR_PE          (bit(15))
#define MCF5272_SPR_HWT         (bit(15))
#define MCF5272_SPR_RPV         (bit(15))
#define MCF5272_SPR_EXT         (bit(15))
#define MCF5272_SPR_SUV         (bit(15))
#define MCF5272_SPR_ADCEN       (bit(15))
#define MCF5272_SPR_WPVEN       (bit(15))
#define MCF5272_SPR_SMVEN       (bit(15))
#define MCF5272_SPR_PEEN        (bit(15))
#define MCF5272_SPR_HWTEN       (bit(15))
#define MCF5272_SPR_RPVEN       (bit(15))
#define MCF5272_SPR_EXTEN       (bit(15))
#define MCF5272_SPR_SUVEN       (bit(15))

#define MCF5272_ENET_TX_RT      (bit(25))
#define MCF5272_ENET_ETHERN_EN  (bit(1))
#define MCF5272_ENET_RESET      (bit(0))

#define MCF5272_ENET_EIR_HBERR  (bit(31))
#define MCF5272_ENET_EIR_BABR   (bit(30))
#define MCF5272_ENET_EIR_BABT   (bit(29))
#define MCF5272_ENET_EIR_GRA    (bit(28))
#define MCF5272_ENET_EIR_TXF    (bit(27))
#define MCF5272_ENET_EIR_TXB    (bit(26))
#define MCF5272_ENET_EIR_RXF    (bit(25))
#define MCF5272_ENET_EIR_RXB    (bit(24))
#define MCF5272_ENET_EIR_MII    (bit(23))
#define MCF5272_ENET_EIR_EBERR  (bit(22))
#define MCF5272_ENET_EIR_UMINT  (bit(21))

#define MCF5272_ENET_RCR_PROM   (bit(3))
#define MCF5272_ENET_RCR_MII    (bit(2))
#define MCF5272_ENET_RCR_DRT    (bit(1))
#define MCF5272_ENET_RCR_LOOP   (bit(0))

#define MCF5272_ENET_TCR_FDEN   (bit(2))
#define MCF5272_ENET_TCR_HBC    (bit(1))
#define MCF5272_ENET_TCR_GTS    (bit(0))


#ifndef ASM
typedef struct {
    volatile unsigned32 mbar;       /* READ ONLY!! */

    volatile unsigned16 scr;
    volatile unsigned16 _res0;

    volatile unsigned16 _res1;
    volatile unsigned16 spr;

    volatile unsigned32 pmr;

    volatile unsigned16 _res2;
    volatile unsigned16 alpr;

    volatile unsigned32 dir;
} sim_regs_t;

typedef struct {
    volatile unsigned32 icr1;
    volatile unsigned32 icr2;
    volatile unsigned32 icr3;
    volatile unsigned32 icr4;
    volatile unsigned32 isr;
    volatile unsigned32 pitr;
    volatile unsigned32 piwr;
    volatile unsigned8  _res0[3];
    volatile unsigned8  pivr;
} intctrl_regs_t;

typedef struct {
    volatile unsigned32 csbr0;
    volatile unsigned32 csor0;
    volatile unsigned32 csbr1;
    volatile unsigned32 csor1;
    volatile unsigned32 csbr2;
    volatile unsigned32 csor2;
    volatile unsigned32 csbr3;
    volatile unsigned32 csor3;
    volatile unsigned32 csbr4;
    volatile unsigned32 csor4;
    volatile unsigned32 csbr5;
    volatile unsigned32 csor5;
    volatile unsigned32 csbr6;
    volatile unsigned32 csor6;
    volatile unsigned32 csbr7;
    volatile unsigned32 csor7;
} chipsel_regs_t;

typedef struct {
    volatile unsigned32 pacnt;

    volatile unsigned16 paddr;
    volatile unsigned16 _res0;

    volatile unsigned16 _res1;
    volatile unsigned16 padat;

    volatile unsigned32 pbcnt;

    volatile unsigned16 pbddr;
    volatile unsigned16 _res2;

    volatile unsigned16 _res3;
    volatile unsigned16 pbdat;

    volatile unsigned16 pcddr; 
    volatile unsigned16 _res4;

    volatile unsigned16 _res5;
    volatile unsigned16 pcdat;

    volatile unsigned32 pdcnt;
} gpio_regs_t;

typedef struct {
    volatile unsigned32 qmr;
    volatile unsigned32 qdlyr;
    volatile unsigned32 qwr;
    volatile unsigned32 qir;
    volatile unsigned32 qar;
    volatile unsigned32 qdr;
} qspi_regs_t;

typedef struct {
    volatile unsigned8 pwcr1;
    volatile unsigned8 _res0[3];

    volatile unsigned8 pwcr2;
    volatile unsigned8 _res1[3];

    volatile unsigned8 pwcr3;
    volatile unsigned8 _res2[3];

    volatile unsigned8 pwwd1;
    volatile unsigned8 _res3[3];

    volatile unsigned8 pwwd2;
    volatile unsigned8 _res4[3];

    volatile unsigned8 pwwd3;
    volatile unsigned8 _res5[3];
} pwm_regs_t;

typedef struct {
    volatile unsigned32 dcmr;
    
    volatile unsigned16 _res0;
    volatile unsigned16 dcir;

    volatile unsigned32 dbcr;

    volatile unsigned32 dsar;

    volatile unsigned32 ddar;
} dma_regs_t;

typedef struct {
    volatile unsigned8 umr;              /* 0x000 */
    volatile unsigned8 _res0[3];

    volatile unsigned8 ucsr;             /* 0x004 */
    volatile unsigned8 _res2[3];

    volatile unsigned8 ucr;              /* 0x008 */
    volatile unsigned8 _res3[3];

    volatile unsigned8 udata;            /* 0x00c */
    volatile unsigned8 _res4[3];

    volatile unsigned8 uccr;             /* 0x010 */
    volatile unsigned8 _res6[3];

    volatile unsigned8 uisr;             /* 0x014 */
    volatile unsigned8 _res8[3];

    volatile unsigned8 ubg1;             /* 0x018 */
    volatile unsigned8 _res10[3];

    volatile unsigned8 ubg2;             /* 0x01c */
    volatile unsigned8 _res11[3];

    volatile unsigned8 uabr1;            /* 0x020 */
    volatile unsigned8 _res12[3];

    volatile unsigned8 uabr2;            /* 0x024 */
    volatile unsigned8 _res13[3];

    volatile unsigned8 utxfcsr;          /* 0x028 */
    volatile unsigned8 _res14[3];

    volatile unsigned8 urxfcsr;          /* 0x02c */
    volatile unsigned8 _res15[3];

    volatile unsigned8 ufpdn;            /* 0x030 */
    volatile unsigned8 _res16[3];

    volatile unsigned8 uip;              /* 0x034 */
    volatile unsigned8 _res17[3];

    volatile unsigned8 uop1;             /* 0x038 */
    volatile unsigned8 _res18[3];

    volatile unsigned8 uop0;             /* 0x03c */
    volatile unsigned8 _res19[3];
} uart_regs_t;

typedef struct {
    volatile unsigned16 tmr0;
    volatile unsigned16 _res0;

    volatile unsigned16 trr0;
    volatile unsigned16 _res1;

    volatile unsigned16 tcap0;
    volatile unsigned16 _res2;

    volatile unsigned16 tcn0;
    volatile unsigned16 _res3;

    volatile unsigned16 ter0;
    volatile unsigned16 _res4;

    volatile unsigned8  _res40[12];

    volatile unsigned16 tmr1;
    volatile unsigned16 _res5;

    volatile unsigned16 trr1;
    volatile unsigned16 _res6;

    volatile unsigned16 tcap1;
    volatile unsigned16 _res7;

    volatile unsigned16 tcn1;
    volatile unsigned16 _res8;

    volatile unsigned16 ter1;
    volatile unsigned16 _res9;

    volatile unsigned8  _res91[12];

    volatile unsigned16 tmr2;
    volatile unsigned16 _res10;

    volatile unsigned16 trr2;
    volatile unsigned16 _res11;

    volatile unsigned16 tcap2;
    volatile unsigned16 _res12;

    volatile unsigned16 tcn2;
    volatile unsigned16 _res13;

    volatile unsigned16 ter2;
    volatile unsigned16 _res14;

    volatile unsigned8  _res140[12];

    volatile unsigned16 tmr3;
    volatile unsigned16 _res15;

    volatile unsigned16 trr3;
    volatile unsigned16 _res16;

    volatile unsigned16 tcap3;
    volatile unsigned16 _res17;

    volatile unsigned16 tcn3;
    volatile unsigned16 _res18;

    volatile unsigned16 ter3;
    volatile unsigned16 _res19;

    volatile unsigned8  _res190[12];

    volatile unsigned16 wrrr;
    volatile unsigned16 _res20;

    volatile unsigned16 wirr;
    volatile unsigned16 _res21;

    volatile unsigned16 wcr;
    volatile unsigned16 _res22;

    volatile unsigned16 wer;
    volatile unsigned16 _res23;
} timer_regs_t;

typedef struct {
    volatile unsigned32 p0b1rr;
    volatile unsigned32 p1b1rr;
    volatile unsigned32 p2b1rr;
    volatile unsigned32 p3b1rr;
    volatile unsigned32 p0b2rr;
    volatile unsigned32 p1b2rr;
    volatile unsigned32 p2b2rr;
    volatile unsigned32 p3b2rr;

    volatile unsigned8  p0drr;
    volatile unsigned8  p1drr;
    volatile unsigned8  p2drr;
    volatile unsigned8  p3drr;

    volatile unsigned32 p0b1tr;
    volatile unsigned32 p1b1tr;
    volatile unsigned32 p2b1tr;
    volatile unsigned32 p3b1tr;
    volatile unsigned32 p0b2tr;
    volatile unsigned32 p1b2tr;
    volatile unsigned32 p2b2tr;
    volatile unsigned32 p3b2tr;

    volatile unsigned8  p0dtr;
    volatile unsigned8  p1dtr;
    volatile unsigned8  p2dtr;
    volatile unsigned8  p3dtr;

    volatile unsigned16 p0cr;
    volatile unsigned16 p1cr;
    volatile unsigned16 p2cr;
    volatile unsigned16 p3cr;
    volatile unsigned16 p0icr;
    volatile unsigned16 p1icr;
    volatile unsigned16 p2icr;
    volatile unsigned16 p3icr;
    volatile unsigned16 p0gmr;
    volatile unsigned16 p1gmr;
    volatile unsigned16 p2gmr;
    volatile unsigned16 p3gmr;
    volatile unsigned16 p0gmt;
    volatile unsigned16 p1gmt;
    volatile unsigned16 p2gmt;
    volatile unsigned16 p3gmt;

    volatile unsigned8  _res0;
    volatile unsigned8  pgmts;
    volatile unsigned8  pgmta;
    volatile unsigned8  _res1;
    volatile unsigned8  p0gcir;
    volatile unsigned8  p1gcir;
    volatile unsigned8  p2gcir;
    volatile unsigned8  p3gcir;
    volatile unsigned8  p0gcit;
    volatile unsigned8  p1gcit;
    volatile unsigned8  p2gcit;
    volatile unsigned8  p3gcit;
    volatile unsigned8  _res3[3];
    volatile unsigned8  pgcitsr;
    volatile unsigned8  _res4[3];
    volatile unsigned8  pdcsr;

    volatile unsigned16 p0psr;
    volatile unsigned16 p1psr;
    volatile unsigned16 p2psr;
    volatile unsigned16 p3psr;
    volatile unsigned16 pasr;
    volatile unsigned8  _res5;
    volatile unsigned8  plcr;
    volatile unsigned16 _res6;
    volatile unsigned16 pdrqr;
    volatile unsigned16 p0sdr;
    volatile unsigned16 p1sdr;
    volatile unsigned16 p2sdr;
    volatile unsigned16 p3sdr;
    volatile unsigned16 _res7;
    volatile unsigned16 pcsr;
} plic_regs_t;

typedef struct {
    volatile unsigned32 ecr;
    volatile unsigned32 eir;
    volatile unsigned32 eimr;
    volatile unsigned32 ivsr;
    volatile unsigned32 rdar;
    volatile unsigned32 tdar;
    volatile unsigned32 _res0[10];
    volatile unsigned32 mmfr;
    volatile unsigned32 mscr;
    volatile unsigned32 _res1[17];
    volatile unsigned32 frbr;
    volatile unsigned32 frsr;
    volatile unsigned32 _res2[4];
    volatile unsigned32 tfwr;
    volatile unsigned32 _res3[1];
    volatile unsigned32 tfsr;
    volatile unsigned32 _res4[21];
    volatile unsigned32 rcr;
    volatile unsigned32 mflr;
    volatile unsigned32 _res5[14];
    volatile unsigned32 tcr;
    volatile unsigned32 _res6[158];
    volatile unsigned32 malr;
    volatile unsigned32 maur;
    volatile unsigned32 htur;
    volatile unsigned32 htlr;
    volatile unsigned32 erdsr;
    volatile unsigned32 etdsr;
    volatile unsigned32 emrbr;
/*    volatile unsigned8  fifo[448]; */
} enet_regs_t;

typedef struct {
    volatile unsigned16 _res0;
    volatile unsigned16 fnr;
    volatile unsigned16 _res1;
    volatile unsigned16 fnmr;
    volatile unsigned16 _res2;
    volatile unsigned16 rfmr;
    volatile unsigned16 _res3;
    volatile unsigned16 rfmmr;
    volatile unsigned8  _res4[3]; 
    volatile unsigned8  far;
    volatile unsigned32 asr;
    volatile unsigned32 drr1;
    volatile unsigned32 drr2;
    volatile unsigned16 _res5; 
    volatile unsigned16 specr;
    volatile unsigned16 _res6;
    volatile unsigned16 ep0sr;

    volatile unsigned32 iep0cfg;
    volatile unsigned32 oep0cfg;
    volatile unsigned32 ep1cfg;
    volatile unsigned32 ep2cfg;
    volatile unsigned32 ep3cfg;
    volatile unsigned32 ep4cfg;
    volatile unsigned32 ep5cfg;
    volatile unsigned32 ep6cfg;
    volatile unsigned32 ep7cfg;
    volatile unsigned32 ep0ctl;

    volatile unsigned16 _res7;
    volatile unsigned16 ep1ctl;
    volatile unsigned16 _res8;
    volatile unsigned16 ep2ctl;
    volatile unsigned16 _res9;
    volatile unsigned16 ep3ctl;
    volatile unsigned16 _res10;
    volatile unsigned16 ep4ctl;
    volatile unsigned16 _res11;
    volatile unsigned16 ep5ctl;
    volatile unsigned16 _res12;
    volatile unsigned16 ep6ctl;
    volatile unsigned16 _res13;
    volatile unsigned16 ep7ctl;

    volatile unsigned32 ep0isr;

    volatile unsigned16 _res14;
    volatile unsigned16 ep1isr;
    volatile unsigned16 _res15;
    volatile unsigned16 ep2isr;
    volatile unsigned16 _res16;
    volatile unsigned16 ep3isr;
    volatile unsigned16 _res17;
    volatile unsigned16 ep4isr;
    volatile unsigned16 _res18;
    volatile unsigned16 ep5isr;
    volatile unsigned16 _res19;
    volatile unsigned16 ep6isr;
    volatile unsigned16 _res20;
    volatile unsigned16 ep7isr;

    volatile unsigned32 ep0imr;

    volatile unsigned16 _res21;
    volatile unsigned16 ep1imr;
    volatile unsigned16 _res22;
    volatile unsigned16 ep2imr;
    volatile unsigned16 _res23;
    volatile unsigned16 ep3imr;
    volatile unsigned16 _res24;
    volatile unsigned16 ep4imr;
    volatile unsigned16 _res25;
    volatile unsigned16 ep5imr;
    volatile unsigned16 _res26;
    volatile unsigned16 ep6imr;
    volatile unsigned16 _res27;
    volatile unsigned16 ep7imr;

    volatile unsigned32 ep0dr;
    volatile unsigned32 ep1dr;
    volatile unsigned32 ep2dr;
    volatile unsigned32 ep3dr;
    volatile unsigned32 ep4dr;
    volatile unsigned32 ep5dr;
    volatile unsigned32 ep6dr;
    volatile unsigned32 ep7dr;

    volatile unsigned16 _res28;
    volatile unsigned16 ep0dpr;
    volatile unsigned16 _res29;
    volatile unsigned16 ep1dpr;
    volatile unsigned16 _res30;
    volatile unsigned16 ep2dpr;
    volatile unsigned16 _res31;
    volatile unsigned16 ep3dpr;
    volatile unsigned16 _res32;
    volatile unsigned16 ep4dpr;
    volatile unsigned16 _res33;
    volatile unsigned16 ep5dpr;
    volatile unsigned16 _res34;
    volatile unsigned16 ep6dpr;
    volatile unsigned16 _res35;
    volatile unsigned16 ep7dpr;
/*    unsigned8  ram[1024]; */
} usb_regs_t;

extern intctrl_regs_t *g_intctrl_regs;
extern chipsel_regs_t *g_chipsel_regs;
extern gpio_regs_t *g_gpio_regs;
extern qspi_regs_t *g_qspi_regs;
extern pwm_regs_t *g_pwm_regs;
extern dma_regs_t *g_dma_regs;
extern uart_regs_t *g_uart0_regs;
extern uart_regs_t *g_uart1_regs;
extern timer_regs_t *g_timer_regs;
extern plic_regs_t *g_plic_regs;
extern enet_regs_t *g_enet_regs;
extern usb_regs_t *g_usb_regs;

#endif /* ASM */

#endif /* __MCF5272_H__ */
