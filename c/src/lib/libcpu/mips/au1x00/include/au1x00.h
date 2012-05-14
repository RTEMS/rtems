/**
 *  @file
 *  
 *  AMD AU1X00 specific information
 */

/*
 *  Copyright (c) 2005 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __AU1X00_H__
#define __AU1X00_H__

#define bit(x)         (1 << (x))

/* Au1x00 CP0 registers
 */
#define CP0_Index		$0
#define CP0_Random		$1
#define CP0_EntryLo0	$2
#define CP0_EntryLo1	$3
#define CP0_Context		$4
#define CP0_PageMask	$5
#define CP0_Wired		$6
#define CP0_BadVAddr	$8
#define CP0_Count		$9
#define CP0_EntryHi		$10
#define CP0_Compare		$11
#define CP0_Status		$12
#define CP0_Cause		$13
#define CP0_EPC			$14
#define CP0_PRId		$15
#define CP0_Config		$16
#define CP0_Config0		$16
#define CP0_Config1		$16,1
#define CP0_LLAddr		$17
#define CP0_WatchLo		$18
#define CP0_IWatchLo	$18,1
#define CP0_WatchHi		$19
#define CP0_IWatchHi	$19,1
#define CP0_Scratch		$22
#define CP0_Debug		$23
#define CP0_DEPC		$24
#define CP0_PerfCnt		$25
#define CP0_PerfCtrl	$25,1
#define CP0_DTag		$28
#define CP0_DData		$28,1
#define CP0_ITag		$29
#define CP0_IData		$29,1
#define CP0_ErrorEPC	$30
#define CP0_DESave		$31

/* Addresses common to all AU1x00 CPUs */
#define AU1X00_MEM_ADDR		0xB4000000
#define AU1X00_AC97_ADDR	0xB0000000
#define AU1X00_USBH_ADDR	0xB0100000
#define AU1X00_USBD_ADDR	0xB0200000
#define AU1X00_MACDMA0_ADDR	0xB4004000
#define AU1X00_MACDMA1_ADDR	0xB4004200
#define AU1X00_UART0_ADDR	0xB1100000
#define AU1X00_UART3_ADDR	0xB1400000
#define AU1X00_SYS_ADDR	        0xB1900000
#define AU1X00_GPIO2_ADDR	0xB1700000
#define AU1X00_IC0_ADDR	        0xB0400000
#define AU1X00_IC1_ADDR	        0xB1800000

/* Au1100 base addresses (in KSEG1 region) */
#define AU1100_MAC0_ADDR	0xB0500000
#define AU1100_MACEN_ADDR	0xB0520000

/* Au1500 base addresses (in KSEG1 region) */
#define AU1500_MAC0_ADDR	0xB1500000
#define AU1500_MAC1_ADDR	0xB1510000
#define AU1500_MACEN_ADDR	0xB1520000
#define AU1500_PCI_ADDR	        0xB4005000

/* Au1x00 gpio2 register offsets
 */
#define gpio2_dir		0x0000
#define gpio2_output	0x0008
#define gpio2_pinstate	0x000c
#define gpio2_inten		0x0010
#define gpio2_enable	0x0014

/* Au1x00 memory controller register offsets
 */
#define mem_sdmode0		0x0000
#define mem_sdmode1		0x0004
#define mem_sdmode2		0x0008
#define mem_sdaddr0		0x000C
#define mem_sdaddr1		0x0010
#define mem_sdaddr2		0x0014
#define mem_sdrefcfg	0x0018
#define mem_sdprecmd	0x001C
#define mem_sdautoref	0x0020
#define mem_sdwrmd0		0x0024
#define mem_sdwrmd1		0x0028
#define mem_sdwrmd2		0x002C
#define mem_sdsleep		0x0030
#define mem_sdsmcke		0x0034

#define mem_stcfg0		0x1000
#define mem_sttime0		0x1004
#define mem_staddr0		0x1008
#define mem_stcfg1		0x1010
#define mem_sttime1		0x1014
#define mem_staddr1		0x1018
#define mem_stcfg2		0x1020
#define mem_sttime2		0x1024
#define mem_staddr2		0x1028
#define mem_stcfg3		0x1030
#define mem_sttime3		0x1034
#define mem_staddr3		0x1038

/*
 * Au1x00 peripheral register offsets
 */
#define ac97_enable		0x0010
#define usbh_enable		0x0007FFFC
#define usbd_enable		0x0058
#define irda_enable		0x0040
#define macen_mac0		0x0000
#define macen_mac1		0x0004
#define i2s_enable		0x0008
#define uart_enable		0x0100
#define ssi_enable		0x0100

#define sys_scratch0	0x0018
#define sys_scratch1	0x001c
#define sys_cntctrl		0x0014
#define sys_freqctrl0	0x0020
#define sys_freqctrl1	0x0024
#define sys_clksrc		0x0028
#define sys_pinfunc		0x002C
#define sys_powerctrl	0x003C
#define sys_endian		0x0038
#define sys_wakesrc		0x005C
#define sys_cpupll		0x0060
#define sys_auxpll		0x0064
#define sys_pininputen	0x0110

#define pci_cmem			0x0000
#define pci_config			0x0004
#define pci_b2bmask_cch		0x0008
#define pci_b2bbase0_venid	0x000C
#define pci_b2bbase1_id		0x0010
#define pci_mwmask_dev		0x0014
#define pci_mwbase_rev_ccl	0x0018
#define pci_err_addr		0x001C
#define pci_spec_intack		0x0020
#define pci_id				0x0100
#define pci_statcmd			0x0104
#define pci_classrev		0x0108
#define pci_hdrtype			0x010C
#define pci_mbar			0x0110

/*
 * CSB250-specific values
 */

#define SYS_CPUPLL		33
#define SYS_POWERCTRL	1
#define SYS_AUXPLL		8
#define SYS_CNTCTRL		256

/* RCE0: */
#define MEM_STCFG0	0x00000203
#define MEM_STTIME0	0x22080b20
#define MEM_STADDR0	0x11f03fc0

/* RCE1: */
#define MEM_STCFG1	0x00000203
#define MEM_STTIME1	0x22080b20
#define MEM_STADDR1	0x11e03fc0

/* RCE2: */
#define MEM_STCFG2	0x00000244
#define MEM_STTIME2	0x22080a20
#define MEM_STADDR2	0x11803f00

/* RCE3: */
#define MEM_STCFG3	0x00000201
#define MEM_STTIME3	0x22080b20
#define MEM_STADDR3	0x11003f00

/*
 * SDCS0 -
 * SDCS1 -
 * SDCS2 -
 */
#define MEM_SDMODE0		0x00552229
#define MEM_SDMODE1		0x00552229
#define MEM_SDMODE2		0x00552229

#define MEM_SDADDR0		0x001003F8
#define MEM_SDADDR1		0x001023F8
#define MEM_SDADDR2		0x001043F8

#define MEM_SDREFCFG_D	0x74000c30	/* disable */
#define MEM_SDREFCFG_E	0x76000c30	/* enable */
#define MEM_SDWRMD0		0x00000023
#define MEM_SDWRMD1		0x00000023
#define MEM_SDWRMD2		0x00000023

#define MEM_1MS			((396000000/1000000) * 1000)

#define AU1X00_IC_CFG0RD(x)       (*(volatile uint32_t*)(x + 0x40))
#define AU1X00_IC_CFG0SET(x)      (*(volatile uint32_t*)(x + 0x40))
#define AU1X00_IC_CFG0CLR(x)      (*(volatile uint32_t*)(x + 0x44))
#define AU1X00_IC_CFG1RD(x)       (*(volatile uint32_t*)(x + 0x48))
#define AU1X00_IC_CFG1SET(x)      (*(volatile uint32_t*)(x + 0x48))
#define AU1X00_IC_CFG1CLR(x)      (*(volatile uint32_t*)(x + 0x4c))
#define AU1X00_IC_CFG2RD(x)       (*(volatile uint32_t*)(x + 0x50))
#define AU1X00_IC_CFG2SET(x)      (*(volatile uint32_t*)(x + 0x50))
#define AU1X00_IC_CFG2CLR(x)      (*(volatile uint32_t*)(x + 0x54))
#define AU1X00_IC_REQ0INT(x)      (*(volatile uint32_t*)(x + 0x54))
#define AU1X00_IC_SRCRD(x)        (*(volatile uint32_t*)(x + 0x58))
#define AU1X00_IC_SRCSET(x)       (*(volatile uint32_t*)(x + 0x58))
#define AU1X00_IC_SRCCLR(x)       (*(volatile uint32_t*)(x + 0x5c))
#define AU1X00_IC_REQ1INT(x)      (*(volatile uint32_t*)(x + 0x5c))
#define AU1X00_IC_ASSIGNRD(x)     (*(volatile uint32_t*)(x + 0x60))
#define AU1X00_IC_ASSIGNSET(x)    (*(volatile uint32_t*)(x + 0x60))
#define AU1X00_IC_ASSIGNCLR(x)    (*(volatile uint32_t*)(x + 0x64))
#define AU1X00_IC_WAKERD(x)       (*(volatile uint32_t*)(x + 0x68))
#define AU1X00_IC_WAKESET(x)      (*(volatile uint32_t*)(x + 0x68))
#define AU1X00_IC_WAKECLR(x)      (*(volatile uint32_t*)(x + 0x6c))
#define AU1X00_IC_MASKRD(x)       (*(volatile uint32_t*)(x + 0x70))
#define AU1X00_IC_MASKSET(x)      (*(volatile uint32_t*)(x + 0x70))
#define AU1X00_IC_MASKCLR(x)      (*(volatile uint32_t*)(x + 0x74))
#define AU1X00_IC_RISINGRD(x)     (*(volatile uint32_t*)(x + 0x78))
#define AU1X00_IC_RISINGCLR(x)    (*(volatile uint32_t*)(x + 0x78))
#define AU1X00_IC_FALLINGRD(x)    (*(volatile uint32_t*)(x + 0x7c))
#define AU1X00_IC_FALLINGCLR(x)   (*(volatile uint32_t*)(x + 0x7c))
#define AU1X00_IC_TESTBIT(x)      (*(volatile uint32_t*)(x + 0x80))
#define AU1X00_IC_IRQ_MAC0        (bit(28))
#define AU1X00_IC_IRQ_MAC1        (bit(29))
#define AU1X00_IC_IRQ_TOY_MATCH0  (bit(15))
#define AU1X00_IC_IRQ_TOY_MATCH1  (bit(16))
#define AU1X00_IC_IRQ_TOY_MATCH2  (bit(17))



#define AU1X00_SYS_TOYTRIM(x)    (*(volatile uint32_t*)(x + 0x00))
#define AU1X00_SYS_TOYWRITE(x)   (*(volatile uint32_t*)(x + 0x04))
#define AU1X00_SYS_TOYMATCH0(x)  (*(volatile uint32_t*)(x + 0x08))
#define AU1X00_SYS_TOYMATCH1(x)  (*(volatile uint32_t*)(x + 0x0c))
#define AU1X00_SYS_TOYMATCH2(x)  (*(volatile uint32_t*)(x + 0x10))
#define AU1X00_SYS_CNTCTRL(x)    (*(volatile uint32_t*)(x + 0x14))
#define AU1X00_SYS_SCRATCH0(x)   (*(volatile uint32_t*)(x + 0x18))
#define AU1X00_SYS_SCRATCH1(x)   (*(volatile uint32_t*)(x + 0x1c))
#define AU1X00_SYS_WAKEMSK(x)   (*(volatile uint32_t*)(x + 0x34))
#define AU1X00_SYS_ENDIAN(x)     (*(volatile uint32_t*)(x + 0x38))
#define AU1X00_SYS_POWERCTRL(x)  (*(volatile uint32_t*)(x + 0x3c))
#define AU1X00_SYS_TOYREAD(x)    (*(volatile uint32_t*)(x + 0x40))
#define AU1X00_SYS_RTCTRIM(x)    (*(volatile uint32_t*)(x + 0x44))
#define AU1X00_SYS_RTCWRITE(x)   (*(volatile uint32_t*)(x + 0x48))
#define AU1X00_SYS_RTCMATCH0(x)  (*(volatile uint32_t*)(x + 0x4c))
#define AU1X00_SYS_RTCMATCH1(x)  (*(volatile uint32_t*)(x + 0x50))
#define AU1X00_SYS_RTCMATCH2(x)  (*(volatile uint32_t*)(x + 0x54))
#define AU1X00_SYS_RTCREAD(x)    (*(volatile uint32_t*)(x + 0x58))
#define AU1X00_SYS_WAKESRC(x)    (*(volatile uint32_t*)(x + 0x5c))
#define AU1X00_SYS_SLPPWR(x)     (*(volatile uint32_t*)(x + 0x78))
#define AU1X00_SYS_SLEEP(x)      (*(volatile uint32_t*)(x + 0x7c))

#define AU1X00_SYS_CNTCTRL_ERS   (bit(23))
#define AU1X00_SYS_CNTCTRL_RTS   (bit(20))
#define AU1X00_SYS_CNTCTRL_RM2   (bit(19))
#define AU1X00_SYS_CNTCTRL_RM1   (bit(18))
#define AU1X00_SYS_CNTCTRL_RM0   (bit(17))
#define AU1X00_SYS_CNTCTRL_RS    (bit(16))
#define AU1X00_SYS_CNTCTRL_BP    (bit(14))
#define AU1X00_SYS_CNTCTRL_REN   (bit(13))
#define AU1X00_SYS_CNTCTRL_BRT   (bit(12))
#define AU1X00_SYS_CNTCTRL_TEN   (bit(11))
#define AU1X00_SYS_CNTCTRL_BTT   (bit(10))
#define AU1X00_SYS_CNTCTRL_E0    (bit(8))
#define AU1X00_SYS_CNTCTRL_ETS   (bit(7))
#define AU1X00_SYS_CNTCTRL_32S   (bit(5))
#define AU1X00_SYS_CNTCTRL_TTS   (bit(4))
#define AU1X00_SYS_CNTCTRL_TM2   (bit(3))
#define AU1X00_SYS_CNTCTRL_TM1   (bit(2))
#define AU1X00_SYS_CNTCTRL_TM0   (bit(1))
#define AU1X00_SYS_CNTCTRL_TS    (bit(0))
#define AU1X00_SYS_WAKEMSK_M20   (bit(8))

#define AU1X00_MAC_CONTROL(x)         (*(volatile uint32_t*)(x + 0x00))
#define AU1X00_MAC_ADDRHIGH(x)        (*(volatile uint32_t*)(x + 0x04))
#define AU1X00_MAC_ADDRLOW(x)         (*(volatile uint32_t*)(x + 0x08))
#define AU1X00_MAC_HASHHIGH(x)        (*(volatile uint32_t*)(x + 0x0c))
#define AU1X00_MAC_HASHLOW(x)         (*(volatile uint32_t*)(x + 0x10))
#define AU1X00_MAC_MIICTRL(x)         (*(volatile uint32_t*)(x + 0x14))
#define AU1X00_MAC_MIIDATA(x)         (*(volatile uint32_t*)(x + 0x18))
#define AU1X00_MAC_FLOWCTRL(x)        (*(volatile uint32_t*)(x + 0x1c))
#define AU1X00_MAC_VLAN1(x)           (*(volatile uint32_t*)(x + 0x20))
#define AU1X00_MAC_VLAN2(x)           (*(volatile uint32_t*)(x + 0x24))
#define AU1X00_MAC_EN0                (*(volatile uint32_t*)(AU1X00_MACEN_ADDR + 0x0))
#define AU1X00_MAC_EN1                (*(volatile uint32_t*)(AU1X00_MACEN_ADDR + 0x4))
#define AU1X00_MAC_DMA_TX0_ADDR(x)    (*(volatile uint32_t*)(x + 0x000))
#define AU1X00_MAC_DMA_TX1_ADDR(x)    (*(volatile uint32_t*)(x + 0x010))
#define AU1X00_MAC_DMA_TX2_ADDR(x)    (*(volatile uint32_t*)(x + 0x020))
#define AU1X00_MAC_DMA_TX3_ADDR(x)    (*(volatile uint32_t*)(x + 0x030))
#define AU1X00_MAC_DMA_RX0_ADDR(x)    (*(volatile uint32_t*)(x + 0x100))
#define AU1X00_MAC_DMA_RX1_ADDR(x)    (*(volatile uint32_t*)(x + 0x110))
#define AU1X00_MAC_DMA_RX2_ADDR(x)    (*(volatile uint32_t*)(x + 0x120))
#define AU1X00_MAC_DMA_RX3_ADDR(x)    (*(volatile uint32_t*)(x + 0x130))

typedef struct {
    volatile uint32_t stat;
    volatile uint32_t addr;
    uint32_t _rsv0;
    uint32_t _rsv1;
} au1x00_macdma_rx_t;


typedef struct {
    volatile uint32_t stat;
    volatile uint32_t addr;
    volatile uint32_t len;
    uint32_t _rsv0;
} au1x00_macdma_tx_t;

#define AU1X00_MAC_CTRL_RA                (bit(31))
#define AU1X00_MAC_CTRL_EM                (bit(30))
#define AU1X00_MAC_CTRL_DO                (bit(23))
#define AU1X00_MAC_CTRL_LM(x)             ((x) << 21)
#define AU1X00_MAC_CTRL_LM_NORMAL         ((0) << 21)
#define AU1X00_MAC_CTRL_LM_INTERNAL       ((1) << 21)
#define AU1X00_MAC_CTRL_LM_EXTERNAL       ((2) << 21)
#define AU1X00_MAC_CTRL_F                 (bit(20))
#define AU1X00_MAC_CTRL_PM                (bit(19))
#define AU1X00_MAC_CTRL_PR                (bit(18))
#define AU1X00_MAC_CTRL_IF                (bit(17))
#define AU1X00_MAC_CTRL_PB                (bit(16))
#define AU1X00_MAC_CTRL_HO                (bit(15))
#define AU1X00_MAC_CTRL_HP                (bit(13))
#define AU1X00_MAC_CTRL_LC                (bit(12))
#define AU1X00_MAC_CTRL_DB                (bit(11))
#define AU1X00_MAC_CTRL_DR                (bit(10))
#define AU1X00_MAC_CTRL_AP                (bit(8))
#define AU1X00_MAC_CTRL_BL(x)             ((x) << 6)
#define AU1X00_MAC_CTRL_DC                (bit(5))
#define AU1X00_MAC_CTRL_TE                (bit(3))
#define AU1X00_MAC_CTRL_RE                (bit(2))

#define AU1X00_MAC_EN_JP                  (bit(6))
#define AU1X00_MAC_EN_E2                  (bit(5))
#define AU1X00_MAC_EN_E1                  (bit(4))
#define AU1X00_MAC_EN_C                   (bit(3))
#define AU1X00_MAC_EN_TS                  (bit(2))
#define AU1X00_MAC_EN_E0                  (bit(1))
#define AU1X00_MAC_EN_CE                  (bit(0))

#define AU1X00_MAC_ADDRHIGH_MASK          (0xffff)_
#define AU1X00_MAC_MIICTRL_PHYADDR(x)     ((x & 0x1f) << 11)
#define AU1X00_MAC_MIICTRL_MIIREG(x)      ((x & 0x1f) << 6)
#define AU1X00_MAC_MIICTRL_MW             (bit(1))
#define AU1X00_MAC_MIICTRL_MB             (bit(0))
#define AU1X00_MAC_MIIDATA_MASK           (0xffff)
#define AU1X00_MAC_FLOWCTRL_PT(x)         (((x) & 0xffff) << 16)
#define AU1X00_MAC_FLOWCTRL_PC            (bit(2))
#define AU1X00_MAC_FLOWCTRL_FE            (bit(1))
#define AU1X00_MAC_FLOWCTRL_FB            (bit(0))

#define AU1X00_MAC_DMA_RXSTAT_MI          (bit(31))
#define AU1X00_MAC_DMA_RXSTAT_PF          (bit(30))
#define AU1X00_MAC_DMA_RXSTAT_FF          (bit(29))
#define AU1X00_MAC_DMA_RXSTAT_BF          (bit(28))
#define AU1X00_MAC_DMA_RXSTAT_MF          (bit(27))
#define AU1X00_MAC_DMA_RXSTAT_UC          (bit(26))
#define AU1X00_MAC_DMA_RXSTAT_CF          (bit(25))
#define AU1X00_MAC_DMA_RXSTAT_LE          (bit(24))
#define AU1X00_MAC_DMA_RXSTAT_V2          (bit(23))
#define AU1X00_MAC_DMA_RXSTAT_V1          (bit(22))
#define AU1X00_MAC_DMA_RXSTAT_CR          (bit(21))
#define AU1X00_MAC_DMA_RXSTAT_DB          (bit(20))
#define AU1X00_MAC_DMA_RXSTAT_ME          (bit(19))
#define AU1X00_MAC_DMA_RXSTAT_FT          (bit(18))
#define AU1X00_MAC_DMA_RXSTAT_CS          (bit(17))
#define AU1X00_MAC_DMA_RXSTAT_FL          (bit(16))
#define AU1X00_MAC_DMA_RXSTAT_RF          (bit(15))
#define AU1X00_MAC_DMA_RXSTAT_WT          (bit(14))
#define AU1X00_MAC_DMA_RXSTAT_LEN(x)      ((x) & 0x3fff)
#define AU1X00_MAC_DMA_RXADDR_ADDR(x)     ((x) & ~0x1f)
#define AU1X00_MAC_DMA_RXADDR_CB_MASK     (0x3 << 0x2)
#define AU1X00_MAC_DMA_RXADDR_DN          (bit(1))
#define AU1X00_MAC_DMA_RXADDR_EN          (bit(0))


#define AU1X00_MAC_DMA_TXSTAT_PR          (bit(31))
#define AU1X00_MAC_DMA_TXSTAT_CC_MASK     (0xf << 10)
#define AU1X00_MAC_DMA_TXSTAT_LO          (bit(9))
#define AU1X00_MAC_DMA_TXSTAT_DF          (bit(8))
#define AU1X00_MAC_DMA_TXSTAT_UR          (bit(7))
#define AU1X00_MAC_DMA_TXSTAT_EC          (bit(6))
#define AU1X00_MAC_DMA_TXSTAT_LC          (bit(5))
#define AU1X00_MAC_DMA_TXSTAT_ED          (bit(4))
#define AU1X00_MAC_DMA_TXSTAT_LS          (bit(3))
#define AU1X00_MAC_DMA_TXSTAT_NC          (bit(2))
#define AU1X00_MAC_DMA_TXSTAT_JT          (bit(1))
#define AU1X00_MAC_DMA_TXSTAT_FA          (bit(0))
#define AU1X00_MAC_DMA_TXADDR_ADDR(x)     ((x) & ~0x1f)
#define AU1X00_MAC_DMA_TXADDR_CB_MASK     (0x3 << 0x2)
#define AU1X00_MAC_DMA_TXADDR_DN          (bit(1))
#define AU1X00_MAC_DMA_TXADDR_EN          (bit(0))



typedef struct {
    volatile uint32_t rxdata;
    volatile uint32_t txdata;
    volatile uint32_t inten;
    volatile uint32_t intcause;
    volatile uint32_t fifoctrl;
    volatile uint32_t linectrl;
    volatile uint32_t mdmctrl;
    volatile uint32_t linestat;
    volatile uint32_t mdmstat;
    volatile uint32_t clkdiv;
    volatile uint32_t _resv[54];
    volatile uint32_t enable;
} au1x00_uart_t;

extern au1x00_uart_t *uart0;
extern au1x00_uart_t *uart3;

void static inline au_sync(void)
{
	__asm__ volatile ("sync");
}


extern void mips_default_isr( int vector );

/* Generate a software interrupt */
extern int assert_sw_irq(uint32_t irqnum);

/* Clear a software interrupt */
extern int negate_sw_irq(uint32_t irqnum);

#endif
