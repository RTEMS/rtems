/**
 *  @file
 *  
 *  MIPS Tx4925 specific information
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __TX4925_h
#define __TX4925_h

#define TX4925_REG_BASE	0xFF1F0000


/*
 *  Configuration Registers
 */
#define TX4925_CFG_CCFG 0xE000		/* Chip Configuration Register */
#define TX4925_CFG_REVID 0xE004		/* Chip Revision ID Register */
#define TX4925_CFG_PCFG 0xE008		/* Pin Configuration Register */
#define TX4925_CFG_TOEA 0xE00C		/* TimeOut Error Access Address Register */
#define TX4925_CFG_PDNCTR 0xE010	/* Power Down Control Register */
#define TX4925_CFG_GARBP 0xE018		/* GBUS Arbiter Priority Register */
#define TX4925_CFG_TOCNT 0xE020		/* Timeout Count Register */
#define TX4925_CFG_DRQCTR 0xE024		/* DMA Request Control Register */
#define TX4925_CFG_CLKCTR 0xE028		/* Clock Control Register */
#define TX4925_CFG_GARBC 0xE02C		/* GBUS Arbiter Control Register */
#define TX4925_CFG_RAMP 0xE030		/* Register Address Mapping Register */

/* Pin Configuration register bits */
#define SELCHI	0x00100000
#define SELTMR0 0x00000200


/*
 *  Timer Registers
 */

#define TX4925_TIMER0_BASE 0xF000
#define TX4925_TIMER1_BASE 0xF100
#define TX4925_TIMER2_BASE 0xF200

#define TX4925_TIMER_TCR  0x00			/* Timer Control Register */
#define TX4925_TIMER_TISR 0x04			/* Timer Interrupt Status Register */
#define TX4925_TIMER_CPRA 0x08			/* Compare Register A */
#define TX4925_TIMER_CPRB 0x0C			/* Compare Register B */
#define TX4925_TIMER_ITMR 0x10			/* Interval Timer Mode Register */
#define TX4925_TIMER_CCDR 0x20			/* Divide Cycle Register */
#define TX4925_TIMER_PGMR 0x30			/* Pulse Generator Mode Register */
#define TX4925_TIMER_WTMR 0x40			/* Reserved Register */
#define TX4925_TIMER_TRR  0xF0			/* Timer Read Register */

/* ITMR register bits */
#define TIMER_CLEAR_ENABLE_MASK		0x1
#define TIMER_INT_ENABLE_MASK	0x8000

/* PGMR register bits */
#define FFI			0x1
#define TPIAE		0x4000
#define TPIBE		0x8000

/* TISR register bits */
#define TIIS	0x1
#define TPIAS	0x2
#define TPIBS	0x4
#define TWIS	0x8


/*
 *	Interrupt Controller Registers
 */
#define TX4925_IRQCTL_DEN 0xF600		/* Interrupt Detection Enable Register */
#define TX4925_IRQCTL_DM0 0xF604		/* Interrupt Detection Mode Register 0 */
#define TX4925_IRQCTL_DM1 0xF608		/* Interrupt Detection Mode Register 1 */
#define TX4925_IRQCTL_LVL0 0xF610		/* Interrupt Level Register 0 */
#define TX4925_IRQCTL_LVL1 0xF614		/* Interrupt Level Register 1 */
#define TX4925_IRQCTL_LVL2 0xF618		/* Interrupt Level Register 2 */
#define TX4925_IRQCTL_LVL3 0xF61C		/* Interrupt Level Register 3 */
#define TX4925_IRQCTL_LVL4 0xF620		/* Interrupt Level Register 4 */
#define TX4925_IRQCTL_LVL5 0xF624		/* Interrupt Level Register 5 */
#define TX4925_IRQCTL_LVL6 0xF628		/* Interrupt Level Register 6 */
#define TX4925_IRQCTL_LVL7 0xF62C		/* Interrupt Level Register 7 */
#define TX4925_IRQCTL_MSK 0xF640		/* Interrupt Mask Register */
#define TX4925_IRQCTL_EDC 0xF660		/* Interrupt Edge Detection Clear Register */
#define TX4925_IRQCTL_PND 0xF680		/* Interrupt Pending Register */
#define TX4925_IRQCTL_CS 0xF6A0			/* Interrupt Current Status Register */
#define TX4925_IRQCTL_FLAG0 0xF510		/* Interrupt Request Flag Register 0 */
#define TX4925_IRQCTL_FLAG1 0xF514		/* Interrupt Request Flag Register 1 */
#define TX4925_IRQCTL_POL 0xF518		/* Interrupt Request Polarity Control Register */
#define TX4925_IRQCTL_RCNT 0xF51C		/* Interrupt Request Control Register */
#define TX4925_IRQCTL_MASKINT 0xF520	/* Interrupt Request Internal Interrupt Mask Register */
#define TX4925_IRQCTL_MASKEXT 0xF524	/* Interrupt Request External Interrupt Mask Register */

#define TX4925_REG_READ( _base, _register ) \
  *((volatile uint32_t *)((_base) + (_register)))

#define TX4925_REG_WRITE( _base, _register, _value ) \
  *((volatile uint32_t *)((_base) + (_register))) = (_value)

#endif
