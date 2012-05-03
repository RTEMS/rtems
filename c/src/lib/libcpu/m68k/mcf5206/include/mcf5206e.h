/*
 *  Coldfire MCF5206e on-chip peripherial definitions.
 *  Contents of this file based on information provided in
 *  Motorola MCF5206e User's Manual
 *
 *  Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __MCF5206E_H__
#define __MCF5206E_H__

#ifdef ASM
#define MCF5206E_REG8(base,ofs) (ofs+base)
#define MCF5206E_REG16(base,ofs) (ofs+base)
#define MCF5206E_REG32(base,ofs) (ofs+base)
#else
#define MCF5206E_REG8(base,ofs) \
    (volatile uint8_t*)((uint8_t*)(base) + (ofs))
#define MCF5206E_REG16(base,ofs) \
    (volatile uint16_t*)((uint8_t*)(base) + (ofs))
#define MCF5206E_REG32(base,ofs) \
    (volatile uint32_t*)((uint8_t*)(base) + (ofs))
#endif

/*** Instruction Cache -- MCF5206e User's Manual, Chapter 4 ***/

/* CACR - Cache Control Register */
#define MCF5206E_CACR_CENB  (0x80000000) /* Cache Enable */
#define MCF5206E_CACR_CPDI  (0x10000000) /* Disable CPUSHL Invalidation */
#define MCF5206E_CACR_CFRZ  (0x08000000) /* Cache Freeze */
#define MCF5206E_CACR_CINV  (0x01000000) /* Cache Invalidate */
#define MCF5206E_CACR_CEIB  (0x00000400) /* Cache Enable Noncacheable
                                            instruction bursting */
#define MCF5206E_CACR_DCM   (0x00000200) /* Default cache mode - noncacheable*/
#define MCF5206E_CACR_DBWE  (0x00000100) /* Default Buffered Write Enable */
#define MCF5206E_CACR_DWP   (0x00000020) /* Default Write Protection */
#define MCF5206E_CACR_CLNF  (0x00000003) /* Cache Line Fill */

/* ACR0, ACR1 - Access Control Registers */
#define MCF5206E_ACR_AB     (0xff000000) /* Address Base */
#define MCF5206E_ACR_AB_S   (24)
#define MCF5206E_ACR_AM     (0x00ff0000) /* Address Mask */
#define MCF5206E_ACR_AM_S   (16)
#define MCF5206E_ACR_EN     (0x00008000) /* Enable ACR */
#define MCF5206E_ACR_SM     (0x00006000) /* Supervisor Mode */
#define MCF5206E_ACR_SM_USR (0x00000000) /* Match if user mode */
#define MCF5206E_ACR_SM_SVR (0x00002000) /* Match if supervisor mode */
#define MCF5206E_ACR_SM_ANY (0x00004000) /* Match Always */
#define MCF5206E_ACR_CM     (0x00000040) /* Cache Mode (1 - noncacheable) */
#define MCF5206E_ACR_BUFW   (0x00000020) /* Buffered Write Enable */
#define MCF5206E_ACR_WP     (0x00000004) /* Write Protect */
#define MCF5206E_ACR_BASE(base) ((base) & MCF5206E_ACR_AB)
#define MCF5206E_ACR_MASK(mask) (((mask) >> 8) & MCF5206E_ACR_AM)

/*** SRAM -- MCF5206e User's Manual, Chapter 5 ***/

/* RAMBAR - SRAM Base Address Register */
#define MCF5206E_RAMBAR_BA  (0xffffe000) /* SRAM Base Address */
#define MCF5206E_RAMBAR_WP  (0x00000100) /* Write Protect */
#define MCF5206E_RAMBAR_CI  (0x00000020) /* CPU Space mask */
#define MCF5206E_RAMBAR_SC  (0x00000010) /* Supervisor Code Space Mask */
#define MCF5206E_RAMBAR_SD  (0x00000008) /* Supervisor Data Space Mask */
#define MCF5206E_RAMBAR_UC  (0x00000004) /* User Code Space Mask */
#define MCF5206E_RAMBAR_UD  (0x00000002) /* User Data Space Mask */
#define MCF5206E_RAMBAR_V   (0x00000001) /* Contents of RAMBAR are valid */

/*** DMA Controller Module -- MCF5206e User's Manual, Chapter 7 ***/

/* DMA Source Address Register */
#define MCF5206E_SAR(mbar,chn) MCF5206E_REG32(mbar,0x200 + ((chn) * 0x40))

/* DMA Destination Address Register */
#define MCF5206E_DAR(mbar,chn) MCF5206E_REG32(mbar,0x204 + ((chn) * 0x40))

/* DMA Byte Count Register */
#define MCF5206E_BCR(mbar,chn) MCF5206E_REG16(mbar,0x20C + ((chn) * 0x40))

/* DMA Control Register */
#define MCF5206E_DCR(mbar,chn) MCF5206E_REG16(mbar,0x208 + ((chn) * 0x40))
#define MCF5206E_DCR_INT    (0x8000) /* Interrupt on completion of transfer */
#define MCF5206E_DCR_EEXT   (0x4000) /* Enable External DMA Request */
#define MCF5206E_DCR_CS     (0x2000) /* Cycle Steal */
#define MCF5206E_DCR_AA     (0x1000) /* Auto Align */
#define MCF5206E_DCR_BWC    (0x0E00) /* Bandwidth Control: */
#define MCF5206E_DCR_BWC_DISABLE (0x0000) /* Bandwidth Control Disabled */
#define MCF5206E_DCR_BWC_512     (0x0200) /* 512 bytes */
#define MCF5206E_DCR_BWC_1024    (0x0400) /* 1024 bytes */
#define MCF5206E_DCR_BWC_2048    (0x0600) /* 2048 bytes */
#define MCF5206E_DCR_BWC_4096    (0x0800) /* 4096 bytes */
#define MCF5206E_DCR_BWC_8192    (0x0A00) /* 8192 bytes */
#define MCF5206E_DCR_BWC_16384   (0x0C00) /* 16384 bytes */
#define MCF5206E_DCR_BWC_32768   (0x0E00) /* 32768 bytes */
#define MCF5206E_DCR_SAA    (0x0100) /* Single Address Access */
#define MCF5206E_DCR_S_RW   (0x0080) /* Single Address Access Read/Write Val */
#define MCF5206E_DCR_SINC   (0x0040) /* Source Increment */
#define MCF5206E_DCR_SSIZE  (0x0030) /* Source Size: */
#define MCF5206E_DCR_SSIZE_LONG  (0x0000) /* Longword (4 bytes) */
#define MCF5206E_DCR_SSIZE_BYTE  (0x0010) /* Byte */
#define MCF5206E_DCR_SSIZE_WORD  (0x0020) /* Word (2 bytes) */
#define MCF5206E_DCR_SSIZE_LINE  (0x0030) /* Line (16 bytes) */
#define MCF5206E_DCR_DINC   (0x0008) /* Destination Increment */
#define MCF5206E_DCR_DSIZE  (0x0006) /* Destination Size: */
#define MCF5206E_DCR_DSIZE_LONG  (0x0000) /* Longword (4 bytes) */
#define MCF5206E_DCR_DSIZE_BYTE  (0x0002) /* Byte */
#define MCF5206E_DCR_DSIZE_WORD  (0x0004) /* Word (2 bytes) */
#define MCF5206E_DCR_DSIZE_LINE  (0x0006) /* Line (16 bytes) */
#define MCF5206E_DCR_START  (0x0001) /* Start Transfer */

/* DMA Status Register */
#define MCF5206E_DSR(mbar,chn) MCF5206E_REG8(mbar,0x210 + ((chn) * 0x40))
#define MCF5206E_DSR_CE     (0x40) /* Configuration Error has occured */
#define MCF5206E_DSR_BES    (0x20) /* Bus Error on Source */
#define MCF5206E_DSR_BED    (0x10) /* Bus Error on Destination */
#define MCF5206E_DSR_REQ    (0x04) /* Request */
#define MCF5206E_DSR_BSY    (0x02) /* Busy */
#define MCF5206E_DSR_DONE   (0x01) /* Transaction Done */

/* DMA Interrupt Vector Register */
#define MCF5206E_DIVR(mbar,chn) MCF5206E_REG8(mbar,0x214 + ((chn) * 0x40))


/*** System Integration Module -- MCF5206e User's Manual, Chapter 8 ***/

/* MBAR - Module Base Address Register */
#define MCF5206E_MBAR_BA  (0xFFFFFC00) /* Base Address */
#define MCF5206E_MBAR_SC  (0x00000010) /* Supervisor Code Space Mask */
#define MCF5206E_MBAR_SD  (0x00000008) /* Supervisor Data Space Mask */
#define MCF5206E_MBAR_UC  (0x00000004) /* User Code Space Mask */
#define MCF5206E_MBAR_UD  (0x00000002) /* User Data Space Mask */
#define MCF5206E_MBAR_V   (0x00000001) /* Contents of MBAR are valid */

/* SIM Configuration Register */
#define MCF5206E_SIMR(mbar) MCF5206E_REG8(mbar,0x003)
#define MCF5206E_SIMR_FRZ1 (0x80)  /* Disable Soft Wdog Timer when FREEZE */
#define MCF5206E_SIMR_FRZ0 (0x40)  /* Disable Bus Timeout monitor when FREEZE*/
#define MCF5206E_SIMR_BL   (0x01)  /* Bus Lock Enable */

/* Interrupt numbers assignment */
#define MCF5206E_INTR_EXT_IRQ1 (1)   /* External IRQ1 */
#define MCF5206E_INTR_EXT_IPL1 (1)   /* External IPL1 */
#define MCF5206E_INTR_EXT_IPL2 (2)   /* External IPL2 */
#define MCF5206E_INTR_EXT_IPL3 (3)   /* External IPL3 */
#define MCF5206E_INTR_EXT_IRQ4 (4)   /* External IRQ4 */
#define MCF5206E_INTR_EXT_IPL4 (4)   /* External IPL4 */
#define MCF5206E_INTR_EXT_IPL5 (5)   /* External IPL5 */
#define MCF5206E_INTR_EXT_IPL6 (6)   /* External IPL6 */
#define MCF5206E_INTR_EXT_IRQ7 (7)   /* External IRQ7 */
#define MCF5206E_INTR_EXT_IPL7 (7)   /* External IPL7 */
#define MCF5206E_INTR_SWT      (8)   /* Software Watchdog Timer */
#define MCF5206E_INTR_TIMER_1  (9)   /* Timer 1 interrupt */
#define MCF5206E_INTR_TIMER_2  (10)  /* Timer 2 interrupt */
#define MCF5206E_INTR_MBUS     (11)  /* MBUS interrupt */
#define MCF5206E_INTR_UART_1   (12)  /* UART 1 interrupt */
#define MCF5206E_INTR_UART_2   (13)  /* UART 2 interrupt */
#define MCF5206E_INTR_DMA_0    (14)  /* DMA channel 0 interrupt */
#define MCF5206E_INTR_DMA_1    (15)  /* DMA channel 1 interrupt */

#define MCF5206E_INTR_BIT(n) (1 << (n))

/* Interrupt Control Registers (ICR1 - ICR15) */
#define MCF5206E_ICR(mbar,n) MCF5206E_REG8(mbar,0x014 + (n) - 1)

#define MCF5206E_ICR_AVEC (0x80) /* Autovector Enable */
#define MCF5206E_ICR_IL   (0x1c) /* Interrupt Level */
#define MCF5206E_ICR_IL_S (2)
#define MCF5206E_ICR_IP   (0x03) /* Interrupt Priority */
#define MCF5206E_ICR_IP_S (0)

/* Interrupt Mask Register */
#define MCF5206E_IMR(mbar) MCF5206E_REG16(mbar,0x036)

/* Interrupt Pending Register */
#define MCF5206E_IPR(mbar) MCF5206E_REG16(mbar,0x03a)

/* Reset Status Register */
#define MCF5206E_RSR(mbar) MCF5206E_REG8(mbar,0x040)
#define MCF5206E_RSR_HRST  (0x80) /* Hard Reset or System Reset */
#define MCF5206E_RSR_SWTR  (0x20) /* Software Watchdog Timer Reset */

/* System Protection Control Register */
#define MCF5206E_SYPCR(mbar) MCF5206E_REG8(mbar,0x041)
#define MCF5206E_SYPCR_SWE   (0x80) /* Software Watchdog Enable */
#define MCF5206E_SYPCR_SWRI  (0x40) /* Software Watchdog Reset/Interrupt Sel.*/
#define MCF5206E_SYPCR_SWP   (0x20) /* Software Watchdog Prescaler */
#define MCF5206E_SYPCR_SWT   (0x18) /* Software Watchdog Timing: */
#define MCF5206E_SYPCR_SWT_S (3)
#define MCF5206E_SYPCR_SWT_9   (0x00) /* timeout = (1<<9)/sysfreq */
#define MCF5206E_SYPCR_SWT_11  (0x08) /* timeout = (1<<11)/sysfreq */
#define MCF5206E_SYPCR_SWT_13  (0x10) /* timeout = (1<<13)/sysfreq */
#define MCF5206E_SYPCR_SWT_15  (0x18) /* timeout = (1<<15)/sysfreq */
#define MCF5206E_SYPCR_SWT_18  (0x20) /* timeout = (1<<18)/sysfreq */
#define MCF5206E_SYPCR_SWT_20  (0x28) /* timeout = (1<<20)/sysfreq */
#define MCF5206E_SYPCR_SWT_22  (0x30) /* timeout = (1<<22)/sysfreq */
#define MCF5206E_SYPCR_SWT_24  (0x38) /* timeout = (1<<24)/sysfreq */
#define MCF5206E_SYPCR_BME   (0x04) /* Bus Timeout Monitor Enable */
#define MCF5206E_SYPCR_BMT   (0x03) /* Bus Monitor Timing: */
#define MCF5206E_SYPCR_BMT_1024 (0x00) /* timeout 1024 system clocks */
#define MCF5206E_SYPCR_BMT_512  (0x01) /* timeout 512 system clocks */
#define MCF5206E_SYPCR_BMT_256  (0x02) /* timeout 256 system clocks */
#define MCF5206E_SYPCR_BMT_128  (0x03) /* timeout 128 system clocks */

/* Software Watchdog Interrupt Vector Register */
#define MCF5206E_SWIVR(mbar) MCF5206E_REG8(mbar,0x042)

/* Software Watchdog Service Register */
#define MCF5206E_SWSR(mbar)  MCF5206E_REG8(mbar,0x043)
#define MCF5206E_SWSR_KEY1 (0x55)
#define MCF5206E_SWSR_KEY2 (0xAA)

/* Pin Assignment Register */
#define MCF5206E_PAR(mbar) MCF5206E_REG16(mbar,0x0CA)
#define MCF5206E_PAR_PAR9          (0x200)
#define MCF5206E_PAR_PAR9_TOUT     (0x000) /* Timer 0 output */
#define MCF5206E_PAR_PAR9_DREQ1    (0x200) /* DMA channel 1 request */
#define MCF5206E_PAR_PAR8          (0x100)
#define MCF5206E_PAR_PAR8_TIN0     (0x000) /* Timer 1 input */
#define MCF5206E_PAR_PAR8_DREQ0    (0x100) /* DMA channel 0 request */
#define MCF5206E_PAR_PAR7          (0x080)
#define MCF5206E_PAR_PAR7_RSTO     (0x000) /* Reset output */
#define MCF5206E_PAR_PAR7_UART2    (0x080) /* UART 2 RTS output */
#define MCF5206E_PAR_PAR6          (0x040)
#define MCF5206E_PAR_PAR6_IRQ      (0x000) /* IRQ7, IRQ4, IRQ1 */
#define MCF5206E_PAR_PAR6_IPL      (0x040) /* IPL2, IPL1, IPL0 */
#define MCF5206E_PAR_PAR5          (0x020)
#define MCF5206E_PAR_PAR5_GPIO     (0x000) /* General purpose I/O PP7-PP4 */
#define MCF5206E_PAR_PAR5_PST      (0x020) /* BDM signals PST3-PST0 */
#define MCF5206E_PAR_PAR4          (0x010)
#define MCF5206E_PAR_PAR4_GPIO     (0x000) /* General purpose I/O PP3-PP0 */
#define MCF5206E_PAR_PAR4_DDATA    (0x010) /* BDM signals DDATA3-DDATA0 */
#define MCF5206E_PAR_PAR3          (0x008)
#define MCF5206E_PAR_PAR2          (0x004)
#define MCF5206E_PAR_PAR1          (0x002)
#define MCF5206E_PAR_PAR0          (0x001)
#define MCF5206E_PAR_WE0_WE1_WE2_WE3 (0x000)
#define MCF5206E_PAR_WE0_WE1_CS5_CS4 (0x001)
#define MCF5206E_PAR_WE0_WE1_CS5_A24 (0x002)
#define MCF5206E_PAR_WE0_WE1_A25_A24 (0x003)
#define MCF5206E_PAR_WE0_CS6_CS5_CS4 (0x004)
#define MCF5206E_PAR_WE0_CS6_CS5_A24 (0x005)
#define MCF5206E_PAR_WE0_CS6_A25_A24 (0x006)
#define MCF5206E_PAR_WE0_A26_A25_A24 (0x007)
#define MCF5206E_PAR_CS7_CS6_CS5_CS4 (0x008)
#define MCF5206E_PAR_CS7_CS6_CS4_A24 (0x009)
#define MCF5206E_PAR_CS7_CS6_A25_A24 (0x00A)
#define MCF5206E_PAR_CS7_A26_A25_A24 (0x00B)
#define MCF5206E_PAR_A27_A26_A25_A24 (0x00C)

/* Bus Master Arbitration Control */
#define MCF5206E_MARB(mbar) MCF5206E_REG8(mbar,0x007)
#define MCF5206E_MARB_NOARB     (0x08) /* Arbiter operation disable */
#define MCF5206E_MARB_ARBCTRL   (0x04) /* Arb. order: Internal DMA, Coldfire */

/*** Chip Select Module -- MCF5206e User's Manual, Chapter 9 ***/

/* Chip Select Address Register */
#define MCF5206E_CSAR(mbar,bank) MCF5206E_REG16(mbar,0x064 + ((bank) * 12))

/* Chip Select Mask Register */
#define MCF5206E_CSMR(mbar,bank) MCF5206E_REG32(mbar,0x068 + ((bank) * 12))
#define MCF5206E_CSMR_BAM (0xffff0000) /* Base Address Mask */
#define MCF5206E_CSMR_BAM_S (16)
#define MCF5206E_CSMR_MASK_256M  (0x0FFF0000)
#define MCF5206E_CSMR_MASK_128M  (0x07FF0000)
#define MCF5206E_CSMR_MASK_64M   (0x03FF0000)
#define MCF5206E_CSMR_MASK_32M   (0x01FF0000)
#define MCF5206E_CSMR_MASK_16M   (0x00FF0000)
#define MCF5206E_CSMR_MASK_8M    (0x007F0000)
#define MCF5206E_CSMR_MASK_4M    (0x003F0000)
#define MCF5206E_CSMR_MASK_2M    (0x001F0000)
#define MCF5206E_CSMR_MASK_1M    (0x000F0000)
#define MCF5206E_CSMR_MASK_1024K (0x000F0000)
#define MCF5206E_CSMR_MASK_512K  (0x00070000)
#define MCF5206E_CSMR_MASK_256K  (0x00030000)
#define MCF5206E_CSMR_MASK_128K  (0x00010000)
#define MCF5206E_CSMR_MASK_64K   (0x00000000)
#define MCF5206E_CSMR_CI  (0x00000020) /* CPU Space Mask (CSMR1 only) */
#define MCF5206E_CSMR_SC  (0x00000010) /* Supervisor Code Space Mask */
#define MCF5206E_CSMR_SD  (0x00000008) /* Supervisor Data Space Mask */
#define MCF5206E_CSMR_UC  (0x00000004) /* User Code Space Mask */
#define MCF5206E_CSMR_UD  (0x00000002) /* User Data Space Mask */

/* Chip Select Control Register */
#define MCF5206E_CSCR(mbar,bank) MCF5206E_REG16(mbar,0x6E + ((bank) * 12))
#define MCF5206E_CSCR_WS    (0x3c00) /* Wait States */
#define MCF5206E_CSCR_WS_S  (10)
#define MCF5206E_CSCR_WS0   (0x0000) /* 0 Wait States */
#define MCF5206E_CSCR_WS1   (0x0400) /* 1 Wait States */
#define MCF5206E_CSCR_WS2   (0x0800) /* 2 Wait States */
#define MCF5206E_CSCR_WS3   (0x0C00) /* 3 Wait States */
#define MCF5206E_CSCR_WS4   (0x1000) /* 4 Wait States */
#define MCF5206E_CSCR_WS5   (0x1400) /* 5 Wait States */
#define MCF5206E_CSCR_WS6   (0x1800) /* 6 Wait States */
#define MCF5206E_CSCR_WS7   (0x1C00) /* 7 Wait States */
#define MCF5206E_CSCR_WS8   (0x2000) /* 8 Wait States */
#define MCF5206E_CSCR_WS9   (0x2400) /* 9 Wait States */
#define MCF5206E_CSCR_WS10  (0x2800) /* 10 Wait States */
#define MCF5206E_CSCR_WS11  (0x2C00) /* 11 Wait States */
#define MCF5206E_CSCR_WS12  (0x3000) /* 12 Wait States */
#define MCF5206E_CSCR_WS13  (0x3400) /* 13 Wait States */
#define MCF5206E_CSCR_WS14  (0x3800) /* 14 Wait States */
#define MCF5206E_CSCR_WS15  (0x3C00) /* 15 Wait States */
#define MCF5206E_CSCR_BRST  (0x0200) /* Burst Enable */
#define MCF5206E_CSCR_AA    (0x0100) /* Coldfire Core Auto Acknowledge
                                        Enable */
#define MCF5206E_CSCR_PS    (0x00C0) /* Port Size */
#define MCF5206E_CSCR_PS_S  (6)
#define MCF5206E_CSCR_PS_32 (0x0000) /* Port Size = 32 bits */
#define MCF5206E_CSCR_PS_8  (0x0040) /* Port Size = 8 bits */
#define MCF5206E_CSCR_PS_16 (0x0080) /* Port Size = 16 bits */
#define MCF5206E_CSCR_EMAA  (0x0020) /* External Master Automatic Acknowledge
                                        Enable */
#define MCF5206E_CSCR_ASET  (0x0010) /* Address Setup Enable */
#define MCF5206E_CSCR_WRAH  (0x0008) /* Write Address Hold Enable */
#define MCF5206E_CSCR_RDAH  (0x0004) /* Read Address Hold Enable */
#define MCF5206E_CSCR_WR    (0x0002) /* Write Enable */
#define MCF5206E_CSCR_RD    (0x0001) /* Read Enable */

/* Default Memory Control Register */
#define MCF5206E_DMCR(mbar) MCF5206E_REG16(mbar, 0x0C6)

/*** Parallel Port (GPIO) Module -- MCF5206e User's Manual, Chapter 10 ***/

/* Port A Data Direction Register */
#define MCF5206E_PPDDR(mbar) MCF5206E_REG8(mbar,0x1C5)

/* Port A Data Register */
#define MCF5206E_PPDAT(mbar) MCF5206E_REG8(mbar,0x1C9)

#define MCF5206E_PP_DAT0  (0x01)
#define MCF5206E_PP_DAT1  (0x02)
#define MCF5206E_PP_DAT2  (0x04)
#define MCF5206E_PP_DAT3  (0x08)
#define MCF5206E_PP_DAT4  (0x10)
#define MCF5206E_PP_DAT5  (0x20)
#define MCF5206E_PP_DAT6  (0x40)
#define MCF5206E_PP_DAT7  (0x80)

/*** DRAM Controller -- MCF5206e User's Manual, Chapter 11 ***/

/* DRAM Controller Refresh Register */
#define MCF5206E_DCRR(mbar) MCF5206E_REG16(mbar,0x046)

/* DRAM Controller Timing Register */
#define MCF5206E_DCTR(mbar) MCF5206E_REG16(mbar,0x04A)
#define MCF5206E_DCTR_DAEM  (0x8000) /* Drive Multiplexed Address During
                                        External Master DRAM Transfers */
#define MCF5206E_DCTR_EDO   (0x4000) /* Extended Data-Out Enable */
#define MCF5206E_DCTR_RCD   (0x1000) /* RAS-to-CAS Delay Time */
#define MCF5206E_DCTR_RSH   (0x0600) /* RAS Hold Time */
#define MCF5206E_DCTR_RSH_0 (0x0000) /* See User's Manual for details */
#define MCF5206E_DCTR_RSH_1 (0x0200)
#define MCF5206E_DCTR_RSH_2 (0x0400)
#define MCF5206E_DCTR_RP    (0x0060) /* RAS Precharge Time */
#define MCF5206E_DCTR_RP_15 (0x0000) /* RAS Precharges for 1.5 system clks */
#define MCF5206E_DCTR_RP_25 (0x0020) /* RAS Precharges for 2.5 system clks */
#define MCF5206E_DCTR_RP_35 (0x0040) /* RAS Precharges for 3.5 system clks */
#define MCF5206E_DCTR_CAS   (0x0008) /* Column Address Strobe Time */
#define MCF5206E_DCTR_CP    (0x0002) /* CAS Precharge Time */
#define MCF5206E_DCTR_CSR   (0x0001) /* CAS Setup Time for CAS before RAS
                                        refresh */

/* DRAM Controller Address Registers */
#define MCF5206E_DCAR(mbar,bank) MCF5206E_REG16(mbar,0x4C + ((bank) * 12))

/* DRAM Controller Mask Registers */
#define MCF5206E_DCMR(mbar,bank) MCF5206E_REG32(mbar,0x50 + ((bank) * 12))
#define MCF5206E_DCMR_BAM (0xffff0000) /* Base Address Mask */
#define MCF5206E_DCMR_BAM_S (16)
#define MCF5206E_DCMR_MASK_256M  (0x0FFE0000)
#define MCF5206E_DCMR_MASK_128M  (0x07FE0000)
#define MCF5206E_DCMR_MASK_64M   (0x03FE0000)
#define MCF5206E_DCMR_MASK_32M   (0x01FE0000)
#define MCF5206E_DCMR_MASK_16M   (0x00FE0000)
#define MCF5206E_DCMR_MASK_8M    (0x007E0000)
#define MCF5206E_DCMR_MASK_4M    (0x003E0000)
#define MCF5206E_DCMR_MASK_2M    (0x001E0000)
#define MCF5206E_DCMR_MASK_1M    (0x000E0000)
#define MCF5206E_DCMR_MASK_1024K (0x000E0000)
#define MCF5206E_DCMR_MASK_512K  (0x00060000)
#define MCF5206E_DCMR_MASK_256K  (0x00020000)
#define MCF5206E_DCMR_MASK_128K  (0x00000000)
#define MCF5206E_DCMR_SC  (0x00000010) /* Supervisor Code Space Mask */
#define MCF5206E_DCMR_SD  (0x00000008) /* Supervisor Data Space Mask */
#define MCF5206E_DCMR_UC  (0x00000004) /* User Code Space Mask */
#define MCF5206E_DCMR_UD  (0x00000002) /* User Data Space Mask */

/* DRAM Controller Control Register */
#define MCF5206E_DCCR(mbar,bank) MCF5206E_REG8(mbar, 0x57 + ((bank) * 12))
#define MCF5206E_DCCR_PS        (0xC0) /* Port Size */
#define MCF5206E_DCCR_PS_32     (0x00) /* 32 bit Port Size */
#define MCF5206E_DCCR_PS_8      (0x40) /* 8 bit Port Size */
#define MCF5206E_DCCR_PS_16     (0x80) /* 16 bit Port Size */
#define MCF5206E_DCCR_BPS       (0x30) /* Bank Page Size */
#define MCF5206E_DCCR_BPS_512   (0x00) /* 512 Byte Page Size */
#define MCF5206E_DCCR_BPS_1K    (0x10) /* 1 KByte Page Size */
#define MCF5206E_DCCR_BPS_2K    (0x20) /* 2 KByte Page Size */
#define MCF5206E_DCCR_PM        (0x0C) /* Page Mode Select */
#define MCF5206E_DCCR_PM_NORMAL (0x00) /* Normal Mode */
#define MCF5206E_DCCR_PM_BURSTP (0x04) /* Burst Page Mode */
#define MCF5206E_DCCR_PM_FASTP  (0x0C) /* Fast Page Mode */
#define MCF5206E_DCCR_WR        (0x02) /* Write Enable */
#define MCF5206E_DCCR_RD        (0x01) /* Read Enable */

/*** UART Module -- MCF5206e User's Manual, Chapter 12 ***/

#define MCF5206E_UART_CHANNELS (2)
/* UART Mode Register */
#define MCF5206E_UMR(mbar,n) MCF5206E_REG8(mbar,0x140 + (((n)-1) * 0x40))
#define MCF5206E_UMR1_RXRTS          (0x80) /* Receiver Request-to-Send
                                               Control */
#define MCF5206E_UMR1_RXIRQ          (0x40) /* Receiver Interrupt Select */
#define MCF5206E_UMR1_ERR            (0x20) /* Error Mode */
#define MCF5206E_UMR1_PM             (0x1C) /* Parity Mode, Parity Type */
#define MCF5206E_UMR1_PM_EVEN        (0x00) /* Even Parity */
#define MCF5206E_UMR1_PM_ODD         (0x04) /* Odd Parity */
#define MCF5206E_UMR1_PM_FORCE_LOW   (0x08) /* Force parity low */
#define MCF5206E_UMR1_PM_FORCE_HIGH  (0x0C) /* Force parity high */
#define MCF5206E_UMR1_PM_NO_PARITY   (0x10) /* No Parity */
#define MCF5206E_UMR1_PM_MULTI_DATA  (0x18) /* Multidrop mode - data char */
#define MCF5206E_UMR1_PM_MULTI_ADDR  (0x1C) /* Multidrop mode - addr char */
#define MCF5206E_UMR1_BC             (0x03) /* Bits per Character */
#define MCF5206E_UMR1_BC_5           (0x00) /* 5 bits per character */
#define MCF5206E_UMR1_BC_6           (0x01) /* 6 bits per character */
#define MCF5206E_UMR1_BC_7           (0x02) /* 7 bits per character */
#define MCF5206E_UMR1_BC_8           (0x03) /* 8 bits per character */

#define MCF5206E_UMR2_CM             (0xC0) /* Channel Mode */
#define MCF5206E_UMR2_CM_NORMAL      (0x00) /* Normal Mode */
#define MCF5206E_UMR2_CM_AUTO_ECHO   (0x40) /* Automatic Echo Mode */
#define MCF5206E_UMR2_CM_LOCAL_LOOP  (0x80) /* Local Loopback Mode */
#define MCF5206E_UMR2_CM_REMOTE_LOOP (0xC0) /* Remote Loopback Modde */
#define MCF5206E_UMR2_TXRTS          (0x20) /* Transmitter Ready-to-Send op */
#define MCF5206E_UMR2_TXCTS          (0x10) /* Transmitter Clear-to-Send op */
#define MCF5206E_UMR2_SB             (0x0F) /* Stop Bit Length */
#define MCF5206E_UMR2_SB_1           (0x07) /* 1 Stop Bit for 6-8 bits char */
#define MCF5206E_UMR2_SB_15          (0x08) /* 1.5 Stop Bits for 6-8 bits chr*/
#define MCF5206E_UMR2_SB_2           (0x0F) /* 2 Stop Bits for 6-8 bits char */
#define MCF5206E_UMR2_SB5_1          (0x00) /* 1 Stop Bits for 5 bit char */
#define MCF5206E_UMR2_SB5_15         (0x07) /* 1.5 Stop Bits for 5 bit char */
#define MCF5206E_UMR2_SB5_2          (0x0F) /* 2 Stop Bits for 5 bit char */

/* UART Status Register (read only) */
#define MCF5206E_USR(mbar,n) MCF5206E_REG8(mbar,0x144 + (((n)-1) * 0x40))
#define MCF5206E_USR_RB     (0x80) /* Received Break */
#define MCF5206E_USR_FE     (0x40) /* Framing Error */
#define MCF5206E_USR_PE     (0x20) /* Parity Error */
#define MCF5206E_USR_OE     (0x10) /* Overrun Error */
#define MCF5206E_USR_TXEMP  (0x08) /* Transmitter Empty */
#define MCF5206E_USR_TXRDY  (0x04) /* Transmitter Ready */
#define MCF5206E_USR_FFULL  (0x02) /* FIFO Full */
#define MCF5206E_USR_RXRDY  (0x01) /* Receiver Ready */

/* UART Clock Select Register (write only) */
#define MCF5206E_UCSR(mbar,n) MCF5206E_REG8(mbar,0x144 + (((n)-1) * 0x40))
#define MCF5206E_UCSR_RCS       (0xF0) /* Receiver Clock Select */
#define MCF5206E_UCSR_RCS_TIMER (0xD0) /* Timer */
#define MCF5206E_UCSR_RCS_EXT16 (0xE0) /* External clk x16 */
#define MCF5206E_UCSR_RCS_EXT   (0xF0) /* External clk x1 */
#define MCF5206E_UCSR_TCS       (0x0F) /* Transmitter Clock Select */
#define MCF5206E_UCSR_TCS_TIMER (0x0D) /* Timer */
#define MCF5206E_UCSR_TCS_EXT16 (0x0E) /* External clk x16 */
#define MCF5206E_UCSR_TCS_EXT   (0x0F) /* External clk x1 */

/* UART Command Register (write only) */
#define MCF5206E_UCR(mbar,n) MCF5206E_REG8(mbar,0x148 + (((n)-1) * 0x40))
#define MCF5206E_UCR_MISC            (0x70) /* Miscellaneous Commands: */
#define MCF5206E_UCR_MISC_NOP        (0x00) /* No Command */
#define MCF5206E_UCR_MISC_RESET_MR   (0x10) /* Reset Mode Register Ptr */
#define MCF5206E_UCR_MISC_RESET_RX   (0x20) /* Reset Receiver */
#define MCF5206E_UCR_MISC_RESET_TX   (0x30) /* Reset Transmitter */
#define MCF5206E_UCR_MISC_RESET_ERR  (0x40) /* Reset Error Status */
#define MCF5206E_UCR_MISC_RESET_BRK  (0x50) /* Reset Break-Change Interrupt */
#define MCF5206E_UCR_MISC_START_BRK  (0x60) /* Start Break */
#define MCF5206E_UCR_MISC_STOP_BRK   (0x70) /* Stop Break */
#define MCF5206E_UCR_TC              (0x0C) /* Transmitter Commands: */
#define MCF5206E_UCR_TC_NOP          (0x00) /* No Action Taken */
#define MCF5206E_UCR_TC_ENABLE       (0x04) /* Transmitter Enable */
#define MCF5206E_UCR_TC_DISABLE      (0x08) /* Transmitter Disable */
#define MCF5206E_UCR_RC              (0x03) /* Receiver Commands: */
#define MCF5206E_UCR_RC_NOP          (0x00) /* No Action Taken */
#define MCF5206E_UCR_RC_ENABLE       (0x01) /* Receiver Enable */
#define MCF5206E_UCR_RC_DISABLE      (0x02) /* Receiver Disable */

/* UART Receive Buffer (read only) */
#define MCF5206E_URB(mbar,n) MCF5206E_REG8(mbar,0x14C + (((n)-1) * 0x40))

/* UART Transmit Buffer (write only) */
#define MCF5206E_UTB(mbar,n) MCF5206E_REG8(mbar,0x14C + (((n)-1) * 0x40))

/* UART Input Port Change Register (read only) */
#define MCF5206E_UIPCR(mbar,n) MCF5206E_REG8(mbar,0x150 + (((n)-1) * 0x40))
#define MCF5206E_UIPCR_COS   (0x10) /* Change of State at CTS input */
#define MCF5206E_UIPCR_CTS   (0x01) /* Current State of CTS */

/* UART Auxiliary Control Register (write only) */
#define MCF5206E_UACR(mbar,n) MCF5206E_REG8(mbar,0x150 + (((n)-1) * 0x40))
#define MCF5206E_UACR_IEC (0x01) /* Input Enable Control - generate interrupt
                                    on CTS change */

/* UART Interrupt Status Register (read only) */
#define MCF5206E_UISR(mbar,n) MCF5206E_REG8(mbar,0x154 + (((n)-1) * 0x40))
#define MCF5206E_UISR_COS   (0x80) /* Change of State has occured at CTS */
#define MCF5206E_UISR_DB    (0x04) /* Delta Break */
#define MCF5206E_UISR_RXRDY (0x02) /* Receiver Ready or FIFO Full */
#define MCF5206E_UISR_TXRDY (0x01) /* Transmitter Ready */

/* UART Interrupt Mask Register (write only) */
#define MCF5206E_UIMR(mbar,n) MCF5206E_REG8(mbar,0x154 + (((n)-1) * 0x40))
#define MCF5206E_UIMR_COS   (0x80) /* Change of State interrupt enable */
#define MCF5206E_UIMR_DB    (0x04) /* Delta Break interrupt enable */
#define MCF5206E_UIMR_FFULL (0x02) /* FIFO Full interrupt enable */
#define MCF5206E_UIMR_TXRDY (0x01) /* Transmitter Ready Interrupt enable */

/* UART Baud Rate Generator Prescale MSB Register */
#define MCF5206E_UBG1(mbar,n) MCF5206E_REG8(mbar,0x158 + (((n)-1) * 0x40))

/* UART Baud Rate Generator Prescale LSB Register */
#define MCF5206E_UBG2(mbar,n) MCF5206E_REG8(mbar,0x15C + (((n)-1) * 0x40))

/* UART Interrupt Vector Register */
#define MCF5206E_UIVR(mbar,n) MCF5206E_REG8(mbar,0x170 + (((n)-1) * 0x40))

/* UART Input Port Register (read only) */
#define MCF5206E_UIP(mbar,n) MCF5206E_REG8(mbar,0x174 + (((n)-1) * 0x40))
#define MCF5206E_UIP_CTS  (0x01) /* Current state of CTS input */

/* UART Output Port Bit Set Command (address-triggered command, write) */
#define MCF5206E_UOP1(mbar,n) MCF5206E_REG8(mbar,0x178 + (((n)-1) * 0x40))

/* UART Output Port Bit Reset Command (address-triggered command, write */
#define MCF5206E_UOP0(mbar,n) MCF5206E_REG8(mbar,0x17C + (((n)-1) * 0x40))

/*** M-BUS (I2C) Module -- MCF5206e User's Manual, Chapter 13 ***/

/* M-Bus Address Register */
#define MCF5206E_MADR(mbar) MCF5206E_REG8(mbar, 0x1E0)

/* M-Bus Frequency Divider Register */
#define MCF5206E_MFDR(mbar) MCF5206E_REG8(mbar, 0x1E4)

/* M-Bus Control Register */
#define MCF5206E_MBCR(mbar) MCF5206E_REG8(mbar, 0x1E8)
#define MCF5206E_MBCR_MEN    (0x80) /* M-Bus Enable */
#define MCF5206E_MBCR_MIEN   (0x40) /* M-Bus Interrupt Enable */
#define MCF5206E_MBCR_MSTA   (0x20) /* Master Mode Selection */
#define MCF5206E_MBCR_MTX    (0x10) /* Transmit Mode Selection */
#define MCF5206E_MBCR_TXAK   (0x08) /* Transmit Acknowledge Enable */
#define MCF5206E_MBCR_RSTA   (0x04) /* Repeat Start */

/* M-Bus Status Register */
#define MCF5206E_MBSR(mbar) MCF5206E_REG8(mbar, 0x1EC)
#define MCF5206E_MBSR_MCF    (0x80) /* Data Transferring Bit */
#define MCF5206E_MBSR_MAAS   (0x40) /* Addressed as a Slave Bit */
#define MCF5206E_MBSR_MBB    (0x20) /* Bus Busy Bit */
#define MCF5206E_MBSR_MAL    (0x10) /* Arbitration Lost */
#define MCF5206E_MBSR_SRW    (0x04) /* Slave Read/Write */
#define MCF5206E_MBSR_MIF    (0x02) /* MBus Interrupt pending */
#define MCF5206E_MBSR_RXAK   (0x01) /* Received Acknowledge */

/* M-Bus Data I/O Register */
#define MCF5206E_MBDR(mbar) MCF5206E_REG8(mbar, 0x1F0)

/*** Timer Module -- MCF5206e User's Manual, Chapter 14 ***/

/* Timer Mode Register */
#define MCF5206E_TMR(mbar,n) MCF5206E_REG16(mbar, 0x100 + (((n)-1)*0x20))
#define MCF5206E_TMR_PS          (0xFF00) /* Prescaler Value */
#define MCF5206E_TMR_PS_S        (8)
#define MCF5206E_TMR_CE          (0x00C0) /* Capture Edge and Enable
                                             Interrupt */
#define MCF5206E_TMR_CE_ANY      (0x00C0) /* Capture on any edge */
#define MCF5206E_TMR_CE_FALL     (0x0080) /* Capture on falling edge only */
#define MCF5206E_TMR_CE_RISE     (0x0040) /* Capture on rising edge only */
#define MCF5206E_TMR_CE_NONE     (0x0000) /* Disable Interrupt on capture
                                             event */
#define MCF5206E_TMR_OM          (0x0020) /* Output Mode - Toggle output */
#define MCF5206E_TMR_ORI         (0x0010) /* Output Reference Interrupt
                                             Enable */
#define MCF5206E_TMR_FRR         (0x0008) /* Free Run/Restart */
#define MCF5206E_TMR_ICLK        (0x0006) /* Input Clock Source */
#define MCF5206E_TMR_ICLK_TIN    (0x0006) /* TIN pin (falling edge) */
#define MCF5206E_TMR_ICLK_DIV16  (0x0004) /* Master system clock divided
                                             by 16 */
#define MCF5206E_TMR_ICLK_MSCLK  (0x0002) /* Master System Clock */
#define MCF5206E_TMR_ICLK_STOP   (0x0000) /* Stops counter */
#define MCF5206E_TMR_RST         (0x0001) /* Reset/Enable Timer */

/* Timer Reference Register */
#define MCF5206E_TRR(mbar,n) MCF5206E_REG16(mbar, 0x104 + (((n)-1)*0x20))

/* Timer Capture Register */
#define MCF5206E_TCR(mbar,n) MCF5206E_REG16(mbar, 0x108 + (((n)-1)*0x20))

/* Timer Counter Register */
#define MCF5206E_TCN(mbar,n) MCF5206E_REG16(mbar, 0x10C + (((n)-1)*0x20))

/* Timer Event Register */
#define MCF5206E_TER(mbar,n) MCF5206E_REG8(mbar, 0x111 + (((n)-1)*0x20))
#define MCF5206E_TER_REF  (0x02) /* Output Reference Event */
#define MCF5206E_TER_CAP  (0x01) /* Capture Event */



#endif
