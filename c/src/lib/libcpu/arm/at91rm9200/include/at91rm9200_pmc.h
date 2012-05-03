/*
 * AT91RM9200 Power Management and Clock definitions
 *
 * Copyright (c) 2002 by Cogent Computer Systems
 * Written by Mike Kelly <mike@cogcomp.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef __AT91RM9200_PMC_H__
#define __AT91RM9200_PMC_H__

#include <bits.h>

/***********************************************************************
 *       Power Management and Clock Control Register Offsets
 ***********************************************************************/
int at91rm9200_get_mainclk(void);
int at91rm9200_get_slck(void);
int at91rm9200_get_mck(void);


#define PMC_SCER  0x00    /* System Clock Enable Register */
#define PMC_SCDR  0x04    /* System Clock Disable Register */
#define PMC_SCSR  0x08    /* System Clock Status Register */
#define PMC_PCER  0x10    /* Peripheral Clock Enable Register */
#define PMC_PCDR  0x14    /* Peripheral Clock Disable Register */
#define PMC_PCSR  0x18    /* Peripheral Clock Status Register */
#define PMC_MOR   0x20    /* Main Oscillator Register */
#define PMC_MCFR  0x24    /* Main Clock  Frequency Register */
#define PMC_PLLAR       0x28    /* PLL A Register */
#define PMC_PLLBR       0x2C    /* PLL B Register */
#define PMC_MCKR  0x30    /* Master Clock Register */
#define PMC_PCKR0       0x40    /* Programmable Clock Register 0 */
#define PMC_PCKR1       0x44    /* Programmable Clock Register 1 */
#define PMC_PCKR2       0x48    /* Programmable Clock Register 2 */
#define PMC_PCKR3       0x4C    /* Programmable Clock Register 3 */
#define PMC_PCKR4       0x50    /* Programmable Clock Register 4 */
#define PMC_PCKR5       0x54    /* Programmable Clock Register 5 */
#define PMC_PCKR6       0x58    /* Programmable Clock Register 6 */
#define PMC_PCKR7       0x5C    /* Programmable Clock Register 7 */
#define PMC_IER   0x60    /* Interrupt Enable Register */
#define PMC_IDR   0x64    /* Interrupt Disable Register */
#define PMC_SR    0x68    /* Status Register */
#define PMC_IMR   0x6C    /* Interrupt Mask Register */

/* Bit Defines */

/* PMC_SCDR - System Clock Disable Register */
/* PMC_SCSR - System Clock Status Register */
/* PMC_SCER - System Clock Enable Register */
#define PMC_SCR_PCK7        BIT15
#define PMC_SCR_PCK6        BIT14
#define PMC_SCR_PCK5        BIT13
#define PMC_SCR_PCK4        BIT12
#define PMC_SCR_PCK3        BIT11
#define PMC_SCR_PCK2        BIT10
#define PMC_SCR_PCK1        BIT9
#define PMC_SCR_PCK0        BIT8
#define PMC_SCR_UHP           BIT4
#define PMC_SCR_MCKUDP      BIT2
#define PMC_SCR_UDP         BIT1
#define PMC_SCR_PCK         BIT0

/* PMC_PCER - Peripheral Clock Enable Register */
/* PMC_PCDR - Peripheral Clock Disable Register */
/* PMC_PCSR - Peripheral Clock Status Register */
#define PMC_PCR_PID_EMAC    BIT24     /* Ethernet Peripheral Clock */
#define PMC_PCR_PID_UHP     BIT23     /* USB Host Ports Peripheral Clock */
#define PMC_PCR_PID_TC5     BIT22     /* Timer/Counter 5 Peripheral Clock */
#define PMC_PCR_PID_TC4     BIT21     /* Timer/Counter 4 Peripheral Clock */
#define PMC_PCR_PID_TC3     BIT20     /* Timer/Counter 3 Peripheral Clock */
#define PMC_PCR_PID_TC2     BIT19     /* Timer/Counter 2 Peripheral Clock */
#define PMC_PCR_PID_TC1     BIT18     /* Timer/Counter 1 Peripheral Clock */
#define PMC_PCR_PID_TC0     BIT17     /* Timer/Counter 0 Peripheral Clock */
#define PMC_PCR_PID_SSC2    BIT16     /* Synchronous Serial 2 Peripheral Clock */
#define PMC_PCR_PID_SSC1    BIT15     /* Synchronous Serial 1 Peripheral Clock */
#define PMC_PCR_PID_SSC0    BIT14     /* Synchronous Serial 0 Peripheral Clock */
#define PMC_PCR_PID_SPI     BIT13     /* Serial Peripheral Interface Peripheral Clock */
#define PMC_PCR_PID_TWI     BIT12     /* Two-Wire Interface Peripheral Clock */
#define PMC_PCR_PID_UDP     BIT11     /* USB Device Port Peripheral Clock */
#define PMC_PCR_PID_MCI     BIT10     /* MMC/SD Card Peripheral Clock */
#define PMC_PCR_PID_US3     BIT9      /* USART 3 Peripheral Clock */
#define PMC_PCR_PID_US2     BIT8      /* USART 2 Peripheral Clock */
#define PMC_PCR_PID_US1     BIT7      /* USART 1 Peripheral Clock */
#define PMC_PCR_PID_US0     BIT6      /* USART 0 Peripheral Clock */
#define PMC_PCR_PID_PIOD    BIT5      /* Parallel I/O D Peripheral Clock */
#define PMC_PCR_PID_PIOC    BIT4      /* Parallel I/O C Peripheral Clock */
#define PMC_PCR_PID_PIOB    BIT3      /* Parallel I/O B Peripheral Clock */
#define PMC_PCR_PID_PIOA    BIT2      /* Parallel I/O A Peripheral Clock */

/* PMC_MOR - Main Oscillator Register */
#define PMC_MOR_MOSCEN      BIT0

/* PMC_MCFR - Main Clock  Frequency Register */
#define PMC_MCFR_MAINRDY    BIT16

/* PMC_PLLAR - PLL A Register */
#define PMC_PLLAR_MUST_SET        BIT29           /* This bit must be set according to the docs */
#define PMC_PLLAR_MUL(_x_)        ((_x_ & 0x7ff) << 16)   /* Multiplier    */
#define PMC_PLLAR_MUL_MASK        (0x7ff << 16)   /* Multiplier mask */

#define PMC_PLLAR_OUT_80_160    (0 << 14)             /* select when PLL frequency is 80-160 Mhz */
#define PMC_PLLAR_OUT_150_240   (2 << 14)             /* select when PLL frequency is 150-240 Mhz */
#define PMC_PLLAR_DIV(_x_)        ((_x_ & 0xff) << 0)       /* Divider */
#define PMC_PLLAR_DIV_MASK        (0xff)    /* Divider mask */

/* PMC_PLLBR - PLL B Register */
#define PMC_PLLBR_USB_96M         BIT28           /* Set when PLL is 96Mhz to divide it by 2 for USB */
#define PMC_PLLBR_MUL(_x_)        ((_x_ & 0x7ff) << 16)   /* Multiplier    */
#define PMC_PLLBR_MUL_MASK      (0x7ff << 16)   /* Multiplier mask */
#define PMC_PLLBR_OUT_80_160    (0 << 14)         /* select when PLL frequency is 80-160 Mhz */
#define PMC_PLLBR_OUT_150_240   (2 << 14)        /* select when PLL frequency is 150-240 Mhz */
#define PMC_PLLBR_DIV(_x_)      ((_x_ & 0xff) << 0)       /* Divider */
#define PMC_PLLBR_DIV_MASK      (0xff)    /* Divider mask */

/* PMC_MCKR - Master Clock Register */
#define PMC_MCKR_MDIV_MASK      (3 << 8)        /* for masking out the MDIV field */
#define PMC_MCKR_MDIV_1     (0 << 8)        /* MCK = Core/1 */
#define PMC_MCKR_MDIV_2     (1 << 8)        /* MCK = Core/2 */
#define PMC_MCKR_MDIV_3     (2 << 8)        /* MCK = Core/3 */
#define PMC_MCKR_MDIV_4     (3 << 8)        /* MCK = Core/4 */
#define PMC_MCKR_PRES_MASK        (7 << 2)        /* for masking out the PRES field */
#define PMC_MCKR_PRES_1     (0 << 2)        /* Core = CSS/1 */
#define PMC_MCKR_PRES_2     (1 << 2)        /* Core = CSS/2 */
#define PMC_MCKR_PRES_4     (2 << 2)        /* Core = CSS/4 */
#define PMC_MCKR_PRES_8     (3 << 2)        /* Core = CSS/8 */
#define PMC_MCKR_PRES_16    (4 << 2)        /* Core = CSS/16 */
#define PMC_MCKR_PRES_32    (5 << 2)        /* Core = CSS/32 */
#define PMC_MCKR_PRES_64    (6 << 2)        /* Core = CSS/64 */
#define PMC_MCKR_CSS_MASK         (3 << 0)        /* for masking out the CSS field */
#define PMC_MCKR_CSS_SLOW         (0 << 0)        /* Core Source = Slow Clock */
#define PMC_MCKR_CSS_MAIN         (1 << 0)        /* Core Source = Main Oscillator */
#define PMC_MCKR_CSS_PLLA         (2 << 0)        /* Core Source = PLL A */
#define PMC_MCKR_CSS_PLLB         (3 << 0)        /* Core Source = PLL B */

/* PMC_PCKR0 - 7 - Programmable Clock Register 0 */
#define PMC_PCKR_PRES_1     (0 << 2)        /* Peripheral Clock = CSS/1 */
#define PMC_PCKR_PRES_2     (1 << 2)        /* Peripheral Clock = CSS/2 */
#define PMC_PCKR_PRES_4     (2 << 2)        /* Peripheral Clock = CSS/4 */
#define PMC_PCKR_PRES_8     (3 << 2)        /* Peripheral Clock = CSS/8 */
#define PMC_PCKR_PRES_16    (4 << 2)        /* Peripheral Clock = CSS/16 */
#define PMC_PCKR_PRES_32    (5 << 2)        /* Peripheral Clock = CSS/32 */
#define PMC_PCKR_PRES_64    (6 << 2)        /* Peripheral Clock = CSS/64 */
#define PMC_PCKR_CSS_SLOW         (0 << 0)        /* Peripheral Clock Source = Slow Clock */
#define PMC_PCKR_CSS_MAIN         (1 << 0)        /* Peripheral Clock Source = Main Oscillator */
#define PMC_PCKR_CSS_PLLA         (2 << 0)        /* Peripheral Clock Source = PLL A */
#define PMC_PCKR_CSS_PLLB         (3 << 0)        /* Peripheral Clock Source = PLL B */

/* PMC_IER - Interrupt Enable Register */
/* PMC_IDR - Interrupt Disable Register */
/* PMC_SR - Status Register */
/* PMC_IMR - Interrupt Mask Register */
#define PMC_INT_PCK7_RDY    BIT15
#define PMC_INT_PCK6_RDY    BIT14
#define PMC_INT_PCK5_RDY    BIT13
#define PMC_INT_PCK4_RDY    BIT12
#define PMC_INT_PCK3_RDY    BIT11
#define PMC_INT_PCK2_RDY    BIT10
#define PMC_INT_PCK1_RDY    BIT9
#define PMC_INT_PCK0_RDY    BIT8
#define PMC_INT_MCK_RDY     BIT3
#define PMC_INT_LOCKB       BIT2
#define PMC_INT_LCKA        BIT1
#define PMC_INT_MOSCS       BIT0


#endif
