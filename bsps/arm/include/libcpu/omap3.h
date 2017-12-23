/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
 * Modified by Ben Gras <beng@shrike-systems.com> to add lots
 * of beagleboard/beaglebone definitions, delete lpc32xx specific
 * ones, and merge with some other header files.
 */

/* Interrupt controller memory map */
#define OMAP3_DM37XX_INTR_BASE 0x48200000 /* INTCPS physical address */

/* Interrupt controller memory map */
#define OMAP3_AM335X_INTR_BASE 0x48200000 /* INTCPS physical address */

/* Interrupt controller registers */
#define OMAP3_INTCPS_REVISION     0x000 /* IP revision code */
#define OMAP3_INTCPS_SYSCONFIG    0x010 /* Controls params */
#define OMAP3_INTCPS_SYSSTATUS    0x014 /* Status */
#define OMAP3_INTCPS_SIR_IRQ      0x040 /* Active IRQ number */
#define OMAP3_INTCPS_SIR_FIQ      0x044 /* Active FIQ number */
#define OMAP3_INTCPS_CONTROL      0x048 /* New int agreement bits */
#define OMAP3_INTCPS_PROTECTION   0x04C /* Protection for other regs */
#define OMAP3_INTCPS_IDLE         0x050 /* Clock auto-idle/gating */
#define OMAP3_INTCPS_IRQ_PRIORITY 0x060 /* Active IRQ priority level */
#define OMAP3_INTCPS_FIQ_PRIORITY 0x064 /* Active FIQ priority level */
#define OMAP3_INTCPS_THRESHOLD    0x068 /* Priority threshold */
#define OMAP3_INTCPS_ITR0         0x080 /* Raw pre-masking interrupt status */
#define OMAP3_INTCPS_MIR0         0x084 /* Interrupt mask */
#define OMAP3_INTCPS_MIR1         0x0A4 /* Interrupt mask */
#define OMAP3_INTCPS_MIR2         0x0C4 /* Interrupt mask */
#define OMAP3_INTCPS_MIR3         0x0E4 /* Interrupt mask */
#define OMAP3_INTCPS_MIR_CLEAR0   0x088 /* Clear interrupt mask bits */
#define OMAP3_INTCPS_MIR_SET0     0x08C /* Set interrupt mask bits */
#define OMAP3_INTCPS_ISR_SET0     0x090 /* Set software int bits */
#define OMAP3_INTCPS_ISR_CLEAR0   0x094 /* Clear software int bits */
#define OMAP3_INTCPS_PENDING_IRQ0 0x098 /* IRQ status post-masking */
#define OMAP3_INTCPS_PENDING_IRQ1 0x0b8 /* IRQ status post-masking */
#define OMAP3_INTCPS_PENDING_IRQ2 0x0d8 /* IRQ status post-masking */
#define OMAP3_INTCPS_PENDING_IRQ3 0x0f8 /* IRQ status post-masking */
#define OMAP3_INTCPS_PENDING_FIQ0 0x09C /* FIQ status post-masking */
#define OMAP3_INTCPS_ILR0         0x100 /* Priority for interrupts */

/* SYSCONFIG */
#define OMAP3_SYSCONFIG_AUTOIDLE	0x01	/* SYSCONFIG.AUTOIDLE bit */

#define OMAP3_INTR_ITR(base,n) \
    (base + OMAP3_INTCPS_ITR0 + 0x20 * (n))
#define OMAP3_INTR_MIR(base,n) \
    (base + OMAP3_INTCPS_MIR0 + 0x20 * (n))
#define OMAP3_INTR_MIR_CLEAR(base,n)	\
    (base + OMAP3_INTCPS_MIR_CLEAR0 + 0x20 * (n))
#define OMAP3_INTR_MIR_SET(base,n) \
    (base + OMAP3_INTCPS_MIR_SET0 + 0x20 * (n))
#define OMAP3_INTR_ISR_SET(base,n) \
    (base + OMAP3_INTCPS_ISR_SET0 + 0x20 * (n))
#define OMAP3_INTR_ISR_CLEAR(base,n) \
    (base + OMAP3_INTCPS_ISR_CLEAR0 + 0x20 * (n))
#define OMAP3_INTR_PENDING_IRQ(base,n) \
    (base + OMAP3_INTCPS_PENDING_IRQ0 + 0x20 * (n))
#define OMAP3_INTR_PENDING_FIQ(base,n) \
    (base + OMAP3_INTCPS_PENDING_FIQ0 + 0x20 * (n))
#define OMAP3_INTR_ILR(base,m) \
    (base + OMAP3_INTCPS_ILR0 + 0x4 * (m))

#define OMAP3_INTR_SPURIOUSIRQ_MASK (0x1FFFFFF << 7) /* Spurious IRQ mask for SIR_IRQ */
#define OMAP3_INTR_ACTIVEIRQ_MASK 0x7F /* Active IRQ mask for SIR_IRQ */
#define OMAP3_INTR_NEWIRQAGR      0x1  /* New IRQ Generation */

#define OMAP3_DM337X_NR_IRQ_VECTORS    96

/* Interrupt mappings */
#define OMAP3_MCBSP2_ST_IRQ  4  /* Sidestone McBSP2 overflow */
#define OMAP3_MCBSP3_ST_IRQ  5  /* Sidestone McBSP3 overflow */
#define OMAP3_SYS_NIRQ       7  /* External source (active low) */
#define OMAP3_SMX_DBG_IRQ    9  /* L3 interconnect error for debug */
#define OMAP3_SMX_APP_IRQ   10  /* L3 interconnect error for application */
#define OMAP3_PRCM_IRQ      11  /* PRCM module */
#define OMAP3_SDMA0_IRQ     12  /* System DMA request 0 */
#define OMAP3_SDMA1_IRQ     13  /* System DMA request 1 */
#define OMAP3_SDMA2_IRQ     14  /* System DMA request 2 */
#define OMAP3_SDMA3_IRQ     15  /* System DMA request 3 */
#define OMAP3_MCBSP1_IRQ    16  /* McBSP module 1 */
#define OMAP3_MCBSP2_IRQ    17  /* McBSP module 2 */
#define OMAP3_GPMC_IRQ      20  /* General-purpose memory controller */
#define OMAP3_SGX_IRQ       21  /* 2D/3D graphics module */
#define OMAP3_MCBSP3_IRQ    22  /* McBSP module 3 */
#define OMAP3_MCBSP4_IRQ    23  /* McBSP module 4 */
#define OMAP3_CAM0_IRQ      24  /* Camera interface request 0 */
#define OMAP3_DSS_IRQ       25  /* Display subsystem module */
#define OMAP3_MAIL_U0_IRQ   26  /* Mailbox user 0 request */
#define OMAP3_MCBSP5_IRQ    27  /* McBSP module 5 */
#define OMAP3_IVA2_MMU_IRQ  28  /* IVA2 MMU */
#define OMAP3_GPIO1_IRQ     29  /* GPIO module 1 */
#define OMAP3_GPIO2_IRQ     30  /* GPIO module 2 */
#define OMAP3_GPIO3_IRQ     31  /* GPIO module 3 */
#define OMAP3_GPIO4_IRQ     32  /* GPIO module 4 */
#define OMAP3_GPIO5_IRQ     33  /* GPIO module 5 */
#define OMAP3_GPIO6_IRQ     34  /* GPIO module 6 */
#define OMAP3_WDT3_IRQ      36  /* Watchdog timer module 3 overflow */
#define OMAP3_GPT1_IRQ      37  /* General-purpose timer module 1 */
#define OMAP3_GPT2_IRQ      38  /* General-purpose timer module 2 */
#define OMAP3_GPT3_IRQ      39  /* General-purpose timer module 3 */
#define OMAP3_GPT4_IRQ      40  /* General-purpose timer module 4 */
#define OMAP3_GPT5_IRQ      41  /* General-purpose timer module 5 */
#define OMAP3_GPT6_IRQ      42  /* General-purpose timer module 6 */
#define OMAP3_GPT7_IRQ      43  /* General-purpose timer module 7 */
#define OMAP3_GPT8_IRQ      44  /* General-purpose timer module 8 */
#define OMAP3_GPT9_IRQ      45  /* General-purpose timer module 9 */
#define OMAP3_GPT10_IRQ     46  /* General-purpose timer module 10 */
#define OMAP3_GPT11_IRQ     47  /* General-purpose timer module 11 */
#define OMAP3_SPI4_IRQ      48  /* McSPI module 4 */
#define OMAP3_MCBSP4_TX_IRQ 54  /* McBSP module 4 transmit */
#define OMAP3_MCBSP4_RX_IRQ 55  /* McBSP module 4 receive */
#define OMAP3_I2C1_IRQ      56  /* I2C module 1 */
#define OMAP3_I2C2_IRQ      57  /* I2C module 2 */
#define OMAP3_HDQ_IRQ       58  /* HDQ/1-Wire */
#define OMAP3_MCBSP1_TX_IRQ 59  /* McBSP module 1 transmit */
#define OMAP3_MCBSP1_RX_IRQ 60  /* McBSP module 1 receive */
#define OMAP3_I2C3_IRQ      61  /* I2C module 3 */
#define OMAP3_MCBSP2_TX_IRQ 62  /* McBSP module 2 transmit */
#define OMAP3_MCBSP2_RX_IRQ 63  /* McBSP module 2 receive */
#define OMAP3_SPI1_IRQ      65  /* McSPI module 1 */
#define OMAP3_SPI2_IRQ      66  /* McSPI module 2 */
#define OMAP3_UART1_IRQ     72  /* UART module 1 */
#define OMAP3_UART2_IRQ     73  /* UART module 2 */
#define OMAP3_UART3_IRQ     74  /* UART module 3 */
#define OMAP3_PBIAS_IRQ     75  /* Merged interrupt for PBIASlite 1/2 */
#define OMAP3_OHCI_IRQ      76  /* OHCI HSUSB MP Host Interrupt */
#define OMAP3_EHCI_IRQ      77  /* EHCI HSUSB MP Host Interrupt */
#define OMAP3_TLL_IRQ       78  /* HSUSB MP TLL Interrupt */
#define OMAP3_MCBSP5_TX_IRQ 81  /* McBSP module 5 transmit */
#define OMAP3_MCBSP5_RX_IRQ 82  /* McBSP module 5 receive */
#define OMAP3_MMC1_IRQ      83  /* MMC/SD module 1 */
#define OMAP3_MMC2_IRQ      86  /* MMC/SD module 2 */
#define OMAP3_ICR_IRQ       87  /* MPU ICR */
#define OMAP3_D2DFRINT_IRQ  88  /* 3G coproc (in stacked modem config) */
#define OMAP3_MCBSP3_TX_IRQ 89  /* McBSP module 3 transmit */
#define OMAP3_MCBSP3_RX_IRQ 90  /* McBSP module 3 receive */
#define OMAP3_SPI3_IRQ      91  /* McSPI module 3 */
#define OMAP3_HSUSB_MC_IRQ  92  /* High-speed USB OTG */
#define OMAP3_HSUSB_DMA_IRQ 93  /* High-speed USB OTG DMA */
#define OMAP3_MMC3_IRQ      94  /* MMC/SD module 3 */

/* General-purpose timer register map */
#define OMAP3_GPTIMER1_BASE  0x48318000
    /* GPTIMER1 physical address */
#define OMAP3_GPTIMER2_BASE  0x49032000
    /* GPTIMER2 physical address */
#define OMAP3_GPTIMER3_BASE  0x49034000
    /* GPTIMER3 physical address */
#define OMAP3_GPTIMER4_BASE  0x49036000
    /* GPTIMER4 physical address */
#define OMAP3_GPTIMER5_BASE  0x49038000
    /* GPTIMER5 physical address */
#define OMAP3_GPTIMER6_BASE  0x4903A000
    /* GPTIMER6 physical address */
#define OMAP3_GPTIMER7_BASE  0x4903C000
    /* GPTIMER7 physical address */
#define OMAP3_GPTIMER8_BASE  0x4903E000
    /* GPTIMER8 physical address */
#define OMAP3_GPTIMER9_BASE  0x49040000
    /* GPTIMER9 physical address */
#define OMAP3_GPTIMER10_BASE 0x48086000
    /* GPTIMER10 physical address */
#define OMAP3_GPTIMER11_BASE 0x48088000
    /* GPTIMER11 physical address */


/* General-purpose timer registers */
#define OMAP3_TIMER_TIDR      0x000
    /* IP revision code */
#define OMAP3_TIMER_TIOCP_CFG 0x010
    /* Controls params for GP timer L4 iface */
#define OMAP3_TIMER_TISTAT    0x014
    /* Status (excl. interrupt status) */
#define OMAP3_TIMER_TISR      0x018
    /* Pending interrupt status */
#define OMAP3_TIMER_TIER      0x01C
    /* Interrupt enable */
#define OMAP3_TIMER_TWER      0x020
    /* Wakeup enable */
#define OMAP3_TIMER_TCLR      0x024
    /* Controls optional features */
#define OMAP3_TIMER_TCRR      0x028
    /* Internal counter value */
#define OMAP3_TIMER_TLDR      0x02C
    /* Timer load value */
#define OMAP3_TIMER_TTGR      0x030
    /* Triggers counter reload */
#define OMAP3_TIMER_TWPS      0x034
    /* Indicates if Write-Posted pending */
#define OMAP3_TIMER_TMAR      0x038
    /* Value to be compared with counter */
#define OMAP3_TIMER_TCAR1     0x03C
    /* First captured value of counter reg */
#define OMAP3_TIMER_TSICR     0x040
    /* Control posted mode and functional SW rst */
#define OMAP3_TIMER_TCAR2     0x044
    /* Second captured value of counter register */
#define OMAP3_TIMER_TPIR      0x048
    /* Positive increment (1 ms tick) */
#define OMAP3_TIMER_TNIR      0x04C
    /* Negative increment (1 ms tick) */
#define OMAP3_TIMER_TCVR      0x050
    /* Defines TCRR is sub/over-period (1 ms tick) */
#define OMAP3_TIMER_TOCR      0x054
    /* Masks tick interrupt */
#define OMAP3_TIMER_TOWR      0x058
    /* Number of masked overflow interrupts */

/* Interrupt status register fields */
#define OMAP3_TISR_MAT_IT_FLAG  (1 << 0) /* Pending match interrupt status */
#define OMAP3_TISR_OVF_IT_FLAG  (1 << 1) /* Pending overflow interrupt status */
#define OMAP3_TISR_TCAR_IT_FLAG (1 << 2) /* Pending capture interrupt status */

/* Interrupt enable register fields */
#define OMAP3_TIER_MAT_IT_ENA  (1 << 0) /* Enable match interrupt */
#define OMAP3_TIER_OVF_IT_ENA  (1 << 1) /* Enable overflow interrupt */
#define OMAP3_TIER_TCAR_IT_ENA (1 << 2) /* Enable capture interrupt */

/* Timer control fields */
#define OMAP3_TCLR_ST       (1 << 0)  /* Start/stop timer */
#define OMAP3_TCLR_AR       (1 << 1)  /* Autoreload or one-shot mode */
#define OMAP3_TCLR_PRE      (1 << 5)  /* Prescaler on */
#define OMAP3_TCLR_PTV      (1 << 1)  /* looks like "bleed" from Minix */
#define OMAP3_TCLR_OVF_TRG  (1 << 10) /* Overflow trigger */


#define OMAP3_CM_CLKSEL_GFX		0x48004b40
#define OMAP3_CM_CLKEN_PLL		0x48004d00
#define OMAP3_CM_FCLKEN1_CORE	0x48004A00
#define OMAP3_CM_CLKSEL_CORE	0x48004A40 /* GPT10 src clock sel. */
#define OMAP3_CM_FCLKEN_PER		0x48005000
#define OMAP3_CM_CLKSEL_PER		0x48005040
#define OMAP3_CM_CLKSEL_WKUP    0x48004c40 /* GPT1 source clock selection */


#define CM_MODULEMODE_MASK        (0x3 << 0)
#define CM_MODULEMODE_ENABLE      (0x2 << 0)
#define CM_MODULEMODE_DISABLED    (0x0 << 0)

#define CM_CLKCTRL_IDLEST         (0x3 << 16)
#define CM_CLKCTRL_IDLEST_FUNC    (0x0 << 16)
#define CM_CLKCTRL_IDLEST_TRANS   (0x1 << 16)
#define CM_CLKCTRL_IDLEST_IDLE    (0x2 << 16)
#define CM_CLKCTRL_IDLEST_DISABLE (0x3 << 16)

#define CM_WKUP_BASE 0x44E00400 /* Clock Module Wakeup Registers */

#define CM_WKUP_TIMER1_CLKCTRL	(CM_WKUP_BASE + 0xC4)
    /* This register manages the TIMER1 clocks. [Memory Mapped] */

#define CM_PER_BASE 0x44E00000 /* Clock Module Peripheral Registers */
#define CM_PER_TIMER7_CLKCTRL	(CM_PER_BASE + 0x7C)
    /* This register manages the TIMER7 clocks. [Memory Mapped] */

/* CM_DPLL registers */

#define CM_DPLL_BASE 	0x44E00500 /* Clock Module PLL Registers */

#define CLKSEL_TIMER1MS_CLK (CM_DPLL_BASE + 0x28)

#define CLKSEL_TIMER1MS_CLK_SEL_MASK (0x7 << 0)
#define CLKSEL_TIMER1MS_CLK_SEL_SEL1 (0x0 << 0)
    /* Select CLK_M_OSC clock */
#define CLKSEL_TIMER1MS_CLK_SEL_SEL2 (0x1 << 0)
    /* Select CLK_32KHZ clock */
#define CLKSEL_TIMER1MS_CLK_SEL_SEL3 (0x2 << 0)
    /* Select TCLKIN clock */
#define CLKSEL_TIMER1MS_CLK_SEL_SEL4 (0x3 << 0)
    /* Select CLK_RC32K clock */
#define CLKSEL_TIMER1MS_CLK_SEL_SEL5 (0x4 << 0)
    /* Selects the CLK_32768 from 32KHz Crystal Osc */

#define CLKSEL_TIMER7_CLK   (CM_DPLL_BASE + 0x04)
#define CLKSEL_TIMER7_CLK_SEL_MASK (0x3 << 0)
#define CLKSEL_TIMER7_CLK_SEL_SEL1 (0x0 << 0) /* Select TCLKIN clock */
#define CLKSEL_TIMER7_CLK_SEL_SEL2 (0x1 << 0) /* Select CLK_M_OSC clock */
#define CLKSEL_TIMER7_CLK_SEL_SEL3 (0x2 << 0) /* Select CLK_32KHZ clock */
#define CLKSEL_TIMER7_CLK_SEL_SEL4 (0x3 << 0) /* Reserved */

/*RTC CLOCK BASE & Registers*/
#define CM_RTC_BASE            0x44E00800
#define CM_RTC_RTC_CLKCTRL     0x0
#define CM_RTC_CLKSTCTRL       0x4


#define OMAP3_CLKSEL_GPT1    (1 << 0)
#define OMAP3_CLKSEL_GPT10   (1 << 6)
#define OMAP3_CLKSEL_GPT11   (1 << 7)

#define OMAP34XX_CORE_L4_IO_BASE  0x48000000

#define ARM_TTBR_ADDR_MASK (0xffffc000)
#define ARM_TTBR_OUTER_NC    (0x0 << 3) /* Non-cacheable*/
#define ARM_TTBR_OUTER_WBWA  (0x1 << 3) /* Outer Write-Back */
#define ARM_TTBR_OUTER_WT    (0x2 << 3) /* Outer Write-Through */
#define ARM_TTBR_OUTER_WBNWA (0x3 << 3) /* Outer Write-Back */
#define ARM_TTBR_FLAGS_CACHED ARM_TTBR_OUTER_WBWA

/* cpu control flags */
/* CPU control register (CP15 register 1) */
#define CPU_CONTROL_MMU_ENABLE  0x00000001 /* M: MMU/Protection unit enable */
#define CPU_CONTROL_AFLT_ENABLE 0x00000002 /* A: Alignment fault enable */
#define CPU_CONTROL_DC_ENABLE   0x00000004 /* C: IDC/DC enable */
#define CPU_CONTROL_WBUF_ENABLE 0x00000008 /* W: Write buffer enable */
#define CPU_CONTROL_32BP_ENABLE 0x00000010 /* P: 32-bit exception handlers */
#define CPU_CONTROL_32BD_ENABLE 0x00000020 /* D: 32-bit addressing */
#define CPU_CONTROL_LABT_ENABLE 0x00000040 /* L: Late abort enable */
#define CPU_CONTROL_BEND_ENABLE 0x00000080 /* B: Big-endian mode */
#define CPU_CONTROL_SYST_ENABLE 0x00000100 /* S: System protection bit */
#define CPU_CONTROL_ROM_ENABLE  0x00000200 /* R: ROM protection bit */
#define CPU_CONTROL_CPCLK       0x00000400 /* F: Implementation defined */
#define CPU_CONTROL_SWP_ENABLE  0x00000400 /* SW: SWP{B} perform normally. */
#define CPU_CONTROL_BPRD_ENABLE 0x00000800 /* Z: Branch prediction enable */
#define CPU_CONTROL_IC_ENABLE   0x00001000 /* I: IC enable */
#define CPU_CONTROL_VECRELOC    0x00002000 /* V: Vector relocation */
#define CPU_CONTROL_ROUNDROBIN  0x00004000 /* RR: Predictable replacement */
#define CPU_CONTROL_V4COMPAT    0x00008000 /* L4: ARMv4 compat LDR R15 etc */
#define CPU_CONTROL_FI_ENABLE   0x00200000 /* FI: Low interrupt latency */
#define CPU_CONTROL_UNAL_ENABLE 0x00400000 /* U: unaligned data access */
#define CPU_CONTROL_XP_ENABLE   0x00800000 /* XP: extended page table */
#define CPU_CONTROL_V_ENABLE    0x01000000 /* VE: Interrupt vectors enable */
#define CPU_CONTROL_EX_BEND     0x02000000 /* EE: exception endianness */
#define CPU_CONTROL_NMFI        0x08000000 /* NMFI: Non maskable FIQ */
#define CPU_CONTROL_TR_ENABLE   0x10000000 /* TRE: */
#define CPU_CONTROL_AF_ENABLE   0x20000000 /* AFE: Access flag enable */
#define CPU_CONTROL_TE_ENABLE   0x40000000 /* TE: Thumb Exception enable */

#define CPU_CONTROL_IDC_ENABLE  CPU_CONTROL_DC_ENABLE

/* VM bits */

/* Big page (1MB section) specific flags. */
#define ARM_VM_SECTION                  (1 << 1)
    /* 1MB section */
#define ARM_VM_SECTION_PRESENT          (1 << 1)
    /* Section is present */
#define ARM_VM_SECTION_B                (1 << 2)
    /* B Bit */
#define ARM_VM_SECTION_C                (1 << 3)
    /* C Bit */
#define ARM_VM_SECTION_DOMAIN           (0xF << 5)
    /* Domain Number */
#define ARM_VM_SECTION_SUPER            (0x1 << 10)
    /* Super access only AP[1:0] */
#define ARM_VM_SECTION_USER             (0x3 << 10)
    /* Super/User access AP[1:0] */
#define ARM_VM_SECTION_TEX0             (1 << 12)
    /* TEX[0] */
#define ARM_VM_SECTION_TEX1             (1 << 13)
    /* TEX[1] */
#define ARM_VM_SECTION_TEX2             (1 << 14)
    /* TEX[2] */
#define ARM_VM_SECTION_RO               (1 << 15)
    /* Read only access AP[2] */
#define ARM_VM_SECTION_SHAREABLE        (1 << 16)
    /* Shareable */
#define ARM_VM_SECTION_NOTGLOBAL        (1 << 17)
    /* Not Global */

#define ARM_VM_SECTION_WB \
    (ARM_VM_SECTION_TEX2 | ARM_VM_SECTION_TEX0 | ARM_VM_SECTION_B )
/* inner and outer write-back, write-allocate */
#define ARM_VM_SECTION_WT \
    (ARM_VM_SECTION_TEX2 | ARM_VM_SECTION_TEX1 | ARM_VM_SECTION_C )
/* inner and outer write-through, no write-allocate */
#define ARM_VM_SECTION_WTWB \
    (ARM_VM_SECTION_TEX2 | ARM_VM_SECTION_TEX0 | ARM_VM_SECTION_C )
/* Inner , Write through, No Write Allocate Outer - Write Back, Write Allocate */

/* shareable device */
#define ARM_VM_SECTION_CACHED   ARM_VM_SECTION_WTWB
#define ARM_VM_SECTION_DEVICE   (ARM_VM_SECTION_B)
