/**
 *  @file
 *  
 *  MIPS Tx4938 specific information
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __TX4938_h
#define __TX4938_h

#define TX4938_REG_BASE	0xFF1F0000

/* PCI1 Registers */
#define TX4938_PCI1_PCIID       0x7000
#define TX4938_PCI1_PCISTATUS   0x7004
#define TX4938_PCI1_PCICFG1     0x700c
#define TX4938_PCI1_P2GM1PLBASE 0x7018
#define TX4938_PCI1_P2GCFG      0x7090
#define TX4938_PCI1_PBAREQPORT  0x7100
#define TX4938_PCI1_PBACFG      0x7104
#define TX4938_PCI1_G2PM0GBASE  0x7120
#define TX4938_PCI1_G2PIOGBASE  0x7138
#define TX4938_PCI1_G2PM0MASK   0x7140
#define TX4938_PCI1_G2PIOMASK   0x714c
#define TX4938_PCI1_G2PM0PBASE  0x7150
#define TX4938_PCI1_G2PIOPBASE  0x7168
#define TX4938_PCI1_PCICCFG     0x7170
#define TX4938_PCI1_PCICSTATUS  0x7174
#define TX4938_PCI1_P2GM1GBASE  0x7188
#define TX4938_PCI1_G2PCFGADRS  0x71a0
#define TX4938_PCI1_G2PCFGDATA  0x71a4

/*
 *  Configuration Registers
 */
#define TX4938_CFG_CCFG 0xE000		/* Chip Configuration Register */
#define TX4938_CFG_REVID 0xE008		/* Chip Revision ID Register */
#define TX4938_CFG_PCFG 0xE010		/* Pin Configuration Register */
#define TX4938_CFG_TOEA 0xE018		/* TimeOut Error Access Address Register */
#define TX4938_CFG_CLKCTR 0xE020		/* Clock Control Register */
#define TX4938_CFG_GARBC 0xE030		/* GBUS Arbiter Control Register */
#define TX4938_CFG_RAMP 0xE048		/* Register Address Mapping Register */

/* Pin Configuration register bits */
#define SELCHI	0x00100000
#define SELTMR0 0x00000200


/*
 *  Timer Registers
 */

#define TX4938_TIMER0_BASE 0xF000
#define TX4938_TIMER1_BASE 0xF100
#define TX4938_TIMER2_BASE 0xF200

#define TX4938_TIMER_TCR  0x00			/* Timer Control Register */
#define TX4938_TIMER_TISR 0x04			/* Timer Interrupt Status Register */
#define TX4938_TIMER_CPRA 0x08			/* Compare Register A */
#define TX4938_TIMER_CPRB 0x0C			/* Compare Register B */
#define TX4938_TIMER_ITMR 0x10			/* Interval Timer Mode Register */
#define TX4938_TIMER_CCDR 0x20			/* Divide Cycle Register */
#define TX4938_TIMER_PGMR 0x30			/* Pulse Generator Mode Register */
#define TX4938_TIMER_WTMR 0x40			/* Reserved Register */
#define TX4938_TIMER_TRR  0xF0			/* Timer Read Register */

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
#define TX4938_IRQCTL_DEN 0xF600		/* Interrupt Detection Enable Register */
#define TX4938_IRQCTL_DM0 0xF604		/* Interrupt Detection Mode Register 0 */
#define TX4938_IRQCTL_DM1 0xF608		/* Interrupt Detection Mode Register 1 */
#define TX4938_IRQCTL_LVL0 0xF610		/* Interrupt Level Register 0 */
#define TX4938_IRQCTL_LVL1 0xF614		/* Interrupt Level Register 1 */
#define TX4938_IRQCTL_LVL2 0xF618		/* Interrupt Level Register 2 */
#define TX4938_IRQCTL_LVL3 0xF61C		/* Interrupt Level Register 3 */
#define TX4938_IRQCTL_LVL4 0xF620		/* Interrupt Level Register 4 */
#define TX4938_IRQCTL_LVL5 0xF624		/* Interrupt Level Register 5 */
#define TX4938_IRQCTL_LVL6 0xF628		/* Interrupt Level Register 6 */
#define TX4938_IRQCTL_LVL7 0xF62C		/* Interrupt Level Register 7 */
#define TX4938_IRQCTL_MSK 0xF640		/* Interrupt Mask Register */
#define TX4938_IRQCTL_EDC 0xF660		/* Interrupt Edge Detection Clear Register */
#define TX4938_IRQCTL_PND 0xF680		/* Interrupt Pending Register */
#define TX4938_IRQCTL_CS 0xF6A0			/* Interrupt Current Status Register */
#define TX4938_IRQCTL_FLAG0 0xF510		/* Interrupt Request Flag Register 0 */
#define TX4938_IRQCTL_FLAG1 0xF514		/* Interrupt Request Flag Register 1 */
#define TX4938_IRQCTL_POL 0xF518		/* Interrupt Request Polarity Control Register */
#define TX4938_IRQCTL_RCNT 0xF51C		/* Interrupt Request Control Register */
#define TX4938_IRQCTL_MASKINT 0xF520	/* Interrupt Request Internal Interrupt Mask Register */
#define TX4938_IRQCTL_MASKEXT 0xF524	/* Interrupt Request External Interrupt Mask Register */

#define TX4938_REG_READ( _base, _register ) \
  *((volatile uint32_t *)((_base) + (_register)))

#define TX4938_REG_WRITE( _base, _register, _value ) \
  *((volatile uint32_t *)((_base) + (_register))) = (_value)

/************************************************************************
 *      TX49 Register field encodings
*************************************************************************/
/******** reg: CCFG ********/
/* field: PCIDIVMODE */
#define TX4938_CCFG_SYSSP_SHF  6
#define TX4938_CCFG_SYSSP_MSK  (MSK(2) << TX4938_CCFG_SYSSP_SHF)

/* field: PCI1DMD */
#define TX4938_CCFG_PCI1DMD_SHF  8
#define TX4938_CCFG_PCI1DMD_MSK  (MSK(1) << TX4938_CCFG_PCI1DMD_SHF)

/* field: PCIDIVMODE */
#define TX4938_CCFG_PCIDIVMODE_SHF  10
#define TX4938_CCFG_PCIDIVMODE_MSK  (MSK(3) << TX4938_CCFG_PCIDIVMODE_SHF)

/* field: PCI1-66 */
#define TX4938_CCFG_PCI166_SHF  21
#define TX4938_CCFG_PCI166_MSK  ((UINT64)MSK(1) << TX4938_CCFG_PCI166_SHF)

/* field: PCIMODE */
#define TX4938_CCFG_PCIMODE_SHF  22
#define TX4938_CCFG_PCIMODE_MSK  ((UINT64)MSK(1) << TX4938_CCFG_PCIMODE_SHF)

/* field: BRDTY */
#define TX4938_CCFG_BRDTY_SHF  36
#define TX4938_CCFG_RRDTY_MSK  ((UINT64)MSK(4) << TX4938_CCFG_BRDTY_SHF)

/* field: BRDRV */
#define TX4938_CCFG_BRDRV_SHF  32
#define TX4938_CCFG_BRDRV_MSK  ((UINT64)MSK(4) << TX4938_CCFG_BRDRV_SHF)

/******** reg: CLKCTR ********/
/* field: PCIC1RST */
#define TX4938_CLKCTR_PCIC1RST_SHF  11
#define TX4938_CLKCTR_PCIC1RST_MSK  (MSK(1) << TX4938_CLKCTR_PCIC1RST_SHF)

/******** reg: PCISTATUS ********/
/* field: MEMSP */
#define TX4938_PCI_PCISTATUS_MEMSP_SHF 1
#define TX4938_PCI_PCISTATUS_MEMSP_MSK (MSK(1) << TX4938_PCI_PCISTATUS_MEMSP_SHF)

/* field: BM */
#define TX4938_PCI_PCISTATUS_BM_SHF    2
#define TX4938_PCI_PCISTATUS_BM_MSK    (MSK(1) << TX4938_PCI_PCISTATUS_BM_SHF)

/******** reg: PBACFG ********/
/* field: RPBA */
#define TX4938_PCI_PBACFG_RPBA_SHF     2
#define TX4938_PCI_PBACFG_RPBA_MSK    (MSK(1) << TX4938_PCI_PBACFG_RPBA_SHF)

/* field: PBAEN */
#define TX4938_PCI_PBACFG_PBAEN_SHF    1
#define TX4938_PCI_PBACFG_PBAEN_MSK   (MSK(1) << TX4938_PCI_PBACFG_PBAEN_SHF)

/******** reg: PCICFG ********/
/* field: G2PM0EN */
#define TX4938_PCI_PCICFG_G2PM0EN_SHF  6
#define TX4938_PCI_PCICFG_G2PM0EN_MSK (MSK(1) << TX4938_PCI_PCICFG_G2PM0EN_SHF)

/* field: G2PIOEN */
#define TX4938_PCI_PCICFG_G2PIOEN_SHF  5
#define TX4938_PCI_PCICFG_G2PIOEN_MSK (MSK(1) << TX4938_PCI_PCICFG_G2PIOEN_SHF)

/* field: TCAR */
#define TX4938_PCI_PCICFG_TCAR_SHF  4
#define TX4938_PCI_PCICFG_TCAR_MSK (MSK(1) << TX4938_PCI_PCICFG_TCAR_SHF)


#endif
