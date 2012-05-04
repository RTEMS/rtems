/*
 *  Coldfire MCF5272 definitions.
 *  Contents of this file based on information provided in
 *  Motorola MCF5272 User's Manual.
 *
 *  Copyright (C) 2004 Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
    volatile uint32_t mbar;       /* READ ONLY!! */

    volatile uint16_t scr;
    volatile uint16_t _res0;

    volatile uint16_t _res1;
    volatile uint16_t spr;

    volatile uint32_t pmr;

    volatile uint16_t _res2;
    volatile uint16_t alpr;

    volatile uint32_t dir;
} sim_regs_t;

typedef struct {
    volatile uint32_t icr1;
    volatile uint32_t icr2;
    volatile uint32_t icr3;
    volatile uint32_t icr4;
    volatile uint32_t isr;
    volatile uint32_t pitr;
    volatile uint32_t piwr;
    volatile uint8_t  _res0[3];
    volatile uint8_t  pivr;
} intctrl_regs_t;

typedef struct {
    volatile uint32_t csbr0;
    volatile uint32_t csor0;
    volatile uint32_t csbr1;
    volatile uint32_t csor1;
    volatile uint32_t csbr2;
    volatile uint32_t csor2;
    volatile uint32_t csbr3;
    volatile uint32_t csor3;
    volatile uint32_t csbr4;
    volatile uint32_t csor4;
    volatile uint32_t csbr5;
    volatile uint32_t csor5;
    volatile uint32_t csbr6;
    volatile uint32_t csor6;
    volatile uint32_t csbr7;
    volatile uint32_t csor7;
} chipsel_regs_t;

typedef struct {
    volatile uint32_t pacnt;

    volatile uint16_t paddr;
    volatile uint16_t _res0;

    volatile uint16_t _res1;
    volatile uint16_t padat;

    volatile uint32_t pbcnt;

    volatile uint16_t pbddr;
    volatile uint16_t _res2;

    volatile uint16_t _res3;
    volatile uint16_t pbdat;

    volatile uint16_t pcddr;
    volatile uint16_t _res4;

    volatile uint16_t _res5;
    volatile uint16_t pcdat;

    volatile uint32_t pdcnt;
} gpio_regs_t;

typedef struct {
    volatile uint32_t qmr;
    volatile uint32_t qdlyr;
    volatile uint32_t qwr;
    volatile uint32_t qir;
    volatile uint32_t qar;
    volatile uint32_t qdr;
} qspi_regs_t;

typedef struct {
    volatile uint8_t pwcr1;
    volatile uint8_t _res0[3];

    volatile uint8_t pwcr2;
    volatile uint8_t _res1[3];

    volatile uint8_t pwcr3;
    volatile uint8_t _res2[3];

    volatile uint8_t pwwd1;
    volatile uint8_t _res3[3];

    volatile uint8_t pwwd2;
    volatile uint8_t _res4[3];

    volatile uint8_t pwwd3;
    volatile uint8_t _res5[3];
} pwm_regs_t;

typedef struct {
    volatile uint32_t dcmr;

    volatile uint16_t _res0;
    volatile uint16_t dcir;

    volatile uint32_t dbcr;

    volatile uint32_t dsar;

    volatile uint32_t ddar;
} dma_regs_t;

typedef struct {
    volatile uint8_t umr;              /* 0x000 */
    volatile uint8_t _res0[3];

    volatile uint8_t ucsr;             /* 0x004 */
    volatile uint8_t _res2[3];

    volatile uint8_t ucr;              /* 0x008 */
    volatile uint8_t _res3[3];

    volatile uint8_t udata;            /* 0x00c */
    volatile uint8_t _res4[3];

    volatile uint8_t uccr;             /* 0x010 */
    volatile uint8_t _res6[3];

    volatile uint8_t uisr;             /* 0x014 */
    volatile uint8_t _res8[3];

    volatile uint8_t ubg1;             /* 0x018 */
    volatile uint8_t _res10[3];

    volatile uint8_t ubg2;             /* 0x01c */
    volatile uint8_t _res11[3];

    volatile uint8_t uabr1;            /* 0x020 */
    volatile uint8_t _res12[3];

    volatile uint8_t uabr2;            /* 0x024 */
    volatile uint8_t _res13[3];

    volatile uint8_t utxfcsr;          /* 0x028 */
    volatile uint8_t _res14[3];

    volatile uint8_t urxfcsr;          /* 0x02c */
    volatile uint8_t _res15[3];

    volatile uint8_t ufpdn;            /* 0x030 */
    volatile uint8_t _res16[3];

    volatile uint8_t uip;              /* 0x034 */
    volatile uint8_t _res17[3];

    volatile uint8_t uop1;             /* 0x038 */
    volatile uint8_t _res18[3];

    volatile uint8_t uop0;             /* 0x03c */
    volatile uint8_t _res19[3];
} uart_regs_t;

typedef struct {
    volatile uint16_t tmr0;
    volatile uint16_t _res0;

    volatile uint16_t trr0;
    volatile uint16_t _res1;

    volatile uint16_t tcap0;
    volatile uint16_t _res2;

    volatile uint16_t tcn0;
    volatile uint16_t _res3;

    volatile uint16_t ter0;
    volatile uint16_t _res4;

    volatile uint8_t  _res40[12];

    volatile uint16_t tmr1;
    volatile uint16_t _res5;

    volatile uint16_t trr1;
    volatile uint16_t _res6;

    volatile uint16_t tcap1;
    volatile uint16_t _res7;

    volatile uint16_t tcn1;
    volatile uint16_t _res8;

    volatile uint16_t ter1;
    volatile uint16_t _res9;

    volatile uint8_t  _res91[12];

    volatile uint16_t tmr2;
    volatile uint16_t _res10;

    volatile uint16_t trr2;
    volatile uint16_t _res11;

    volatile uint16_t tcap2;
    volatile uint16_t _res12;

    volatile uint16_t tcn2;
    volatile uint16_t _res13;

    volatile uint16_t ter2;
    volatile uint16_t _res14;

    volatile uint8_t  _res140[12];

    volatile uint16_t tmr3;
    volatile uint16_t _res15;

    volatile uint16_t trr3;
    volatile uint16_t _res16;

    volatile uint16_t tcap3;
    volatile uint16_t _res17;

    volatile uint16_t tcn3;
    volatile uint16_t _res18;

    volatile uint16_t ter3;
    volatile uint16_t _res19;

    volatile uint8_t  _res190[12];

    volatile uint16_t wrrr;
    volatile uint16_t _res20;

    volatile uint16_t wirr;
    volatile uint16_t _res21;

    volatile uint16_t wcr;
    volatile uint16_t _res22;

    volatile uint16_t wer;
    volatile uint16_t _res23;
} timer_regs_t;

typedef struct {
    volatile uint32_t p0b1rr;
    volatile uint32_t p1b1rr;
    volatile uint32_t p2b1rr;
    volatile uint32_t p3b1rr;
    volatile uint32_t p0b2rr;
    volatile uint32_t p1b2rr;
    volatile uint32_t p2b2rr;
    volatile uint32_t p3b2rr;

    volatile uint8_t  p0drr;
    volatile uint8_t  p1drr;
    volatile uint8_t  p2drr;
    volatile uint8_t  p3drr;

    volatile uint32_t p0b1tr;
    volatile uint32_t p1b1tr;
    volatile uint32_t p2b1tr;
    volatile uint32_t p3b1tr;
    volatile uint32_t p0b2tr;
    volatile uint32_t p1b2tr;
    volatile uint32_t p2b2tr;
    volatile uint32_t p3b2tr;

    volatile uint8_t  p0dtr;
    volatile uint8_t  p1dtr;
    volatile uint8_t  p2dtr;
    volatile uint8_t  p3dtr;

    volatile uint16_t p0cr;
    volatile uint16_t p1cr;
    volatile uint16_t p2cr;
    volatile uint16_t p3cr;
    volatile uint16_t p0icr;
    volatile uint16_t p1icr;
    volatile uint16_t p2icr;
    volatile uint16_t p3icr;
    volatile uint16_t p0gmr;
    volatile uint16_t p1gmr;
    volatile uint16_t p2gmr;
    volatile uint16_t p3gmr;
    volatile uint16_t p0gmt;
    volatile uint16_t p1gmt;
    volatile uint16_t p2gmt;
    volatile uint16_t p3gmt;

    volatile uint8_t  _res0;
    volatile uint8_t  pgmts;
    volatile uint8_t  pgmta;
    volatile uint8_t  _res1;
    volatile uint8_t  p0gcir;
    volatile uint8_t  p1gcir;
    volatile uint8_t  p2gcir;
    volatile uint8_t  p3gcir;
    volatile uint8_t  p0gcit;
    volatile uint8_t  p1gcit;
    volatile uint8_t  p2gcit;
    volatile uint8_t  p3gcit;
    volatile uint8_t  _res3[3];
    volatile uint8_t  pgcitsr;
    volatile uint8_t  _res4[3];
    volatile uint8_t  pdcsr;

    volatile uint16_t p0psr;
    volatile uint16_t p1psr;
    volatile uint16_t p2psr;
    volatile uint16_t p3psr;
    volatile uint16_t pasr;
    volatile uint8_t  _res5;
    volatile uint8_t  plcr;
    volatile uint16_t _res6;
    volatile uint16_t pdrqr;
    volatile uint16_t p0sdr;
    volatile uint16_t p1sdr;
    volatile uint16_t p2sdr;
    volatile uint16_t p3sdr;
    volatile uint16_t _res7;
    volatile uint16_t pcsr;
} plic_regs_t;

typedef struct {
    volatile uint32_t ecr;
    volatile uint32_t eir;
    volatile uint32_t eimr;
    volatile uint32_t ivsr;
    volatile uint32_t rdar;
    volatile uint32_t tdar;
    volatile uint32_t _res0[10];
    volatile uint32_t mmfr;
    volatile uint32_t mscr;
    volatile uint32_t _res1[17];
    volatile uint32_t frbr;
    volatile uint32_t frsr;
    volatile uint32_t _res2[4];
    volatile uint32_t tfwr;
    volatile uint32_t _res3[1];
    volatile uint32_t tfsr;
    volatile uint32_t _res4[21];
    volatile uint32_t rcr;
    volatile uint32_t mflr;
    volatile uint32_t _res5[14];
    volatile uint32_t tcr;
    volatile uint32_t _res6[158];
    volatile uint32_t malr;
    volatile uint32_t maur;
    volatile uint32_t htur;
    volatile uint32_t htlr;
    volatile uint32_t erdsr;
    volatile uint32_t etdsr;
    volatile uint32_t emrbr;
/*    volatile uint8_t  fifo[448]; */
} enet_regs_t;

typedef struct {
    volatile uint16_t _res0;
    volatile uint16_t fnr;
    volatile uint16_t _res1;
    volatile uint16_t fnmr;
    volatile uint16_t _res2;
    volatile uint16_t rfmr;
    volatile uint16_t _res3;
    volatile uint16_t rfmmr;
    volatile uint8_t  _res4[3];
    volatile uint8_t  far;
    volatile uint32_t asr;
    volatile uint32_t drr1;
    volatile uint32_t drr2;
    volatile uint16_t _res5;
    volatile uint16_t specr;
    volatile uint16_t _res6;
    volatile uint16_t ep0sr;

    volatile uint32_t iep0cfg;
    volatile uint32_t oep0cfg;
    volatile uint32_t ep1cfg;
    volatile uint32_t ep2cfg;
    volatile uint32_t ep3cfg;
    volatile uint32_t ep4cfg;
    volatile uint32_t ep5cfg;
    volatile uint32_t ep6cfg;
    volatile uint32_t ep7cfg;
    volatile uint32_t ep0ctl;

    volatile uint16_t _res7;
    volatile uint16_t ep1ctl;
    volatile uint16_t _res8;
    volatile uint16_t ep2ctl;
    volatile uint16_t _res9;
    volatile uint16_t ep3ctl;
    volatile uint16_t _res10;
    volatile uint16_t ep4ctl;
    volatile uint16_t _res11;
    volatile uint16_t ep5ctl;
    volatile uint16_t _res12;
    volatile uint16_t ep6ctl;
    volatile uint16_t _res13;
    volatile uint16_t ep7ctl;

    volatile uint32_t ep0isr;

    volatile uint16_t _res14;
    volatile uint16_t ep1isr;
    volatile uint16_t _res15;
    volatile uint16_t ep2isr;
    volatile uint16_t _res16;
    volatile uint16_t ep3isr;
    volatile uint16_t _res17;
    volatile uint16_t ep4isr;
    volatile uint16_t _res18;
    volatile uint16_t ep5isr;
    volatile uint16_t _res19;
    volatile uint16_t ep6isr;
    volatile uint16_t _res20;
    volatile uint16_t ep7isr;

    volatile uint32_t ep0imr;

    volatile uint16_t _res21;
    volatile uint16_t ep1imr;
    volatile uint16_t _res22;
    volatile uint16_t ep2imr;
    volatile uint16_t _res23;
    volatile uint16_t ep3imr;
    volatile uint16_t _res24;
    volatile uint16_t ep4imr;
    volatile uint16_t _res25;
    volatile uint16_t ep5imr;
    volatile uint16_t _res26;
    volatile uint16_t ep6imr;
    volatile uint16_t _res27;
    volatile uint16_t ep7imr;

    volatile uint32_t ep0dr;
    volatile uint32_t ep1dr;
    volatile uint32_t ep2dr;
    volatile uint32_t ep3dr;
    volatile uint32_t ep4dr;
    volatile uint32_t ep5dr;
    volatile uint32_t ep6dr;
    volatile uint32_t ep7dr;

    volatile uint16_t _res28;
    volatile uint16_t ep0dpr;
    volatile uint16_t _res29;
    volatile uint16_t ep1dpr;
    volatile uint16_t _res30;
    volatile uint16_t ep2dpr;
    volatile uint16_t _res31;
    volatile uint16_t ep3dpr;
    volatile uint16_t _res32;
    volatile uint16_t ep4dpr;
    volatile uint16_t _res33;
    volatile uint16_t ep5dpr;
    volatile uint16_t _res34;
    volatile uint16_t ep6dpr;
    volatile uint16_t _res35;
    volatile uint16_t ep7dpr;
/*    uint8_t  ram[1024]; */
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
