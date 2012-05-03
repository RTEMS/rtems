/*===============================================================*\
| Project: RTEMS generic MPC83xx BSP                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file integrates the IPIC irq controller                    |
\*===============================================================*/


#include <mpc83xx/mpc83xx.h>

#include <rtems.h>

#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#define MPC83XX_IPIC_VECTOR_NUMBER 92

#define MPC83XX_IPIC_IS_VALID_VECTOR( vector) ((vector) >= 0 && (vector) < MPC83XX_IPIC_VECTOR_NUMBER)

#define MPC83XX_IPIC_INVALID_MASK_POSITION 255

typedef struct {
	volatile uint32_t *pend_reg;
	volatile uint32_t *mask_reg;
	const uint32_t bit_num;
} BSP_isrc_rsc_t;

/*
 * data structure to handle all mask registers in the IPIC
 *
 * Mask positions:
 *   simsr [0] :  0 .. 31
 *   simsr [1] : 32 .. 63
 *   semsr     : 64 .. 95
 *   sermr     : 96 .. 127
 */
typedef struct {
	uint32_t simsr_mask [2];
	uint32_t semsr_mask;
	uint32_t sermr_mask;
} mpc83xx_ipic_mask_t;

static const BSP_isrc_rsc_t mpc83xx_ipic_isrc_rsc [MPC83XX_IPIC_VECTOR_NUMBER] = {
	/* vector 0 */
	{&mpc83xx.ipic.sersr, &mpc83xx.ipic.sermr, 31},
	{NULL, NULL, 0},
	{NULL, NULL, 0},
	{NULL, NULL, 0},
	{NULL, NULL, 0},
	{NULL, NULL, 0},
	{NULL, NULL, 0},
	{NULL, NULL, 0},
	/* vector  8 */
	{NULL, NULL, 0},	/* reserved vector  8 */
	/* vector  9: UART1 SIxxR_H, Bit 24 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 24},
	/* vector 10: UART2 SIxxR_H, Bit 25 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 25},
	/* vector 11: SEC   SIxxR_H, Bit 26 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 26},
	{NULL, NULL, 0},	/* reserved vector 12 */
	{NULL, NULL, 0},	/* reserved vector 13 */
	/* vector 14: I2C1 SIxxR_H, Bit 29 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 29},
	/* vector 15: I2C2 SIxxR_H, Bit 30 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 30},
	/* vector 16: SPI  SIxxR_H, Bit 31 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 31},
	/* vector 17: IRQ1 SExxR  , Bit  1 */
	{&mpc83xx.ipic.sepnr, &mpc83xx.ipic.semsr, 1},
	/* vector 18: IRQ2 SExxR  , Bit  2 */
	{&mpc83xx.ipic.sepnr, &mpc83xx.ipic.semsr, 2},
	/* vector 19: IRQ3 SExxR  , Bit  3 */
	{&mpc83xx.ipic.sepnr, &mpc83xx.ipic.semsr, 3},
	/* vector 20: IRQ4 SExxR  , Bit  4 */
	{&mpc83xx.ipic.sepnr, &mpc83xx.ipic.semsr, 4},
	/* vector 21: IRQ5 SExxR  , Bit  5 */
	{&mpc83xx.ipic.sepnr, &mpc83xx.ipic.semsr, 5},
	/* vector 22: IRQ6 SExxR  , Bit  6 */
	{&mpc83xx.ipic.sepnr, &mpc83xx.ipic.semsr, 6},
	/* vector 23: IRQ7 SExxR  , Bit  7 */
	{&mpc83xx.ipic.sepnr, &mpc83xx.ipic.semsr, 7},
	{NULL, NULL, 0},	/* reserved vector 24 */
	{NULL, NULL, 0},	/* reserved vector 25 */
	{NULL, NULL, 0},	/* reserved vector 26 */
	{NULL, NULL, 0},	/* reserved vector 27 */
	{NULL, NULL, 0},	/* reserved vector 28 */
	{NULL, NULL, 0},	/* reserved vector 29 */
	{NULL, NULL, 0},	/* reserved vector 30 */
	{NULL, NULL, 0},	/* reserved vector 31 */
	/* vector 32: TSEC1 Tx  SIxxR_H  , Bit  0 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 0},
	/* vector 33: TSEC1 Rx  SIxxR_H  , Bit  1 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 1},
	/* vector 34: TSEC1 Err SIxxR_H  , Bit  2 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 2},
	/* vector 35: TSEC2 Tx  SIxxR_H  , Bit  3 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 3},
	/* vector 36: TSEC2 Rx  SIxxR_H  , Bit  4 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 4},
	/* vector 37: TSEC2 Err SIxxR_H  , Bit  5 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 5},
	/* vector 38: USB DR    SIxxR_H  , Bit  6 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 6},
	/* vector 39: USB MPH   SIxxR_H  , Bit  7 */
	{&mpc83xx.ipic.sipnr [0], &mpc83xx.ipic.simsr [0], 7},
	{NULL, NULL, 0},	/* reserved vector 40 */
	{NULL, NULL, 0},	/* reserved vector 41 */
	{NULL, NULL, 0},	/* reserved vector 42 */
	{NULL, NULL, 0},	/* reserved vector 43 */
	{NULL, NULL, 0},	/* reserved vector 44 */
	{NULL, NULL, 0},	/* reserved vector 45 */
	{NULL, NULL, 0},	/* reserved vector 46 */
	{NULL, NULL, 0},	/* reserved vector 47 */
	/* vector 48: IRQ0 SExxR  , Bit  0 */
	{&mpc83xx.ipic.sepnr, &mpc83xx.ipic.semsr, 0},
	{NULL, NULL, 0},	/* reserved vector 49 */
	{NULL, NULL, 0},	/* reserved vector 50 */
	{NULL, NULL, 0},	/* reserved vector 51 */
	{NULL, NULL, 0},	/* reserved vector 52 */
	{NULL, NULL, 0},	/* reserved vector 53 */
	{NULL, NULL, 0},	/* reserved vector 54 */
	{NULL, NULL, 0},	/* reserved vector 55 */
	{NULL, NULL, 0},	/* reserved vector 56 */
	{NULL, NULL, 0},	/* reserved vector 57 */
	{NULL, NULL, 0},	/* reserved vector 58 */
	{NULL, NULL, 0},	/* reserved vector 59 */
	{NULL, NULL, 0},	/* reserved vector 60 */
	{NULL, NULL, 0},	/* reserved vector 61 */
	{NULL, NULL, 0},	/* reserved vector 62 */
	{NULL, NULL, 0},	/* reserved vector 63 */
	/* vector 64: RTC SEC   SIxxR_L  , Bit  0 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 0},
	/* vector 65: PIT       SIxxR_L  , Bit  1 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 1},
	/* vector 66: PCI1      SIxxR_L  , Bit  2 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 2},
	/* vector 67: PCI2      SIxxR_L  , Bit  3 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 3},
	/* vector 68: RTC ALR   SIxxR_L  , Bit  4 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 4},
	/* vector 69: MU        SIxxR_L  , Bit  5 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 5},
	/* vector 70: SBA       SIxxR_L  , Bit  6 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 6},
	/* vector 71: DMA       SIxxR_L  , Bit  7 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 7},
	/* vector 72: GTM4      SIxxR_L  , Bit  8 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 8},
	/* vector 73: GTM8      SIxxR_L  , Bit  9 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 9},
	/* vector 74: GPIO1     SIxxR_L  , Bit 10 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 10},
	/* vector 75: GPIO2     SIxxR_L  , Bit 11 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 11},
	/* vector 76: DDR       SIxxR_L  , Bit 12 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 12},
	/* vector 77: LBC       SIxxR_L  , Bit 13 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 13},
	/* vector 78: GTM2      SIxxR_L  , Bit 14 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 14},
	/* vector 79: GTM6      SIxxR_L  , Bit 15 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 15},
	/* vector 80: PMC       SIxxR_L  , Bit 16 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 16},
	{NULL, NULL, 0},	/* reserved vector 81 */
	{NULL, NULL, 0},	/* reserved vector 82 */
	{NULL, NULL, 0},	/* reserved vector 63 */
	/* vector 84: GTM3      SIxxR_L  , Bit 20 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 20},
	/* vector 85: GTM7      SIxxR_L  , Bit 21 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 21},
	{NULL, NULL, 0},	/* reserved vector 81 */
	{NULL, NULL, 0},	/* reserved vector 82 */
	{NULL, NULL, 0},	/* reserved vector 63 */
	{NULL, NULL, 0},	/* reserved vector 63 */
	/* vector 90: GTM1      SIxxR_L  , Bit 26 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 26},
	/* vector 91: GTM5      SIxxR_L  , Bit 27 */
	{&mpc83xx.ipic.sipnr [1], &mpc83xx.ipic.simsr [1], 27}
};

static const uint8_t mpc83xx_ipic_mask_position_table [MPC83XX_IPIC_VECTOR_NUMBER] = {
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	7,
	6,
	5,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	2,
	1,
	0,
	94,
	93,
	92,
	91,
	90,
	89,
	88,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	31,
	30,
	29,
	28,
	27,
	26,
	25,
	24,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	95,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	63,
	62,
	61,
	60,
	59,
	58,
	57,
	56,
	55,
	54,
	53,
	52,
	51,
	50,
	49,
	48,
	47,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	43,
	42,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	MPC83XX_IPIC_INVALID_MASK_POSITION,
	37,
	36
};

/*
 * this array will be filled with mask values needed
 * to temporarily disable all IRQ soures with lower or same
 * priority of the current source (whose vector is the array index)
 */
static mpc83xx_ipic_mask_t mpc83xx_ipic_prio2mask [MPC83XX_IPIC_VECTOR_NUMBER];

rtems_status_code mpc83xx_ipic_set_mask( rtems_vector_number vector, rtems_vector_number mask_vector, bool mask)
{
	uint8_t pos = 0;
	mpc83xx_ipic_mask_t *mask_entry;
	uint32_t *mask_reg;
	rtems_interrupt_level level;

	/* Parameter check */
	if (!MPC83XX_IPIC_IS_VALID_VECTOR( vector) || !MPC83XX_IPIC_IS_VALID_VECTOR( mask_vector)) {
		return RTEMS_INVALID_NUMBER;
	} else if (vector == mask_vector) {
		return RTEMS_RESOURCE_IN_USE;
	}

	/* Position and mask entry */
	pos = mpc83xx_ipic_mask_position_table [mask_vector];
	mask_entry = &mpc83xx_ipic_prio2mask [vector];

	/* Mask register and position */
	if (pos < 32) {
		mask_reg = &mask_entry->simsr_mask [0];
	} else if (pos < 64) {
		pos -= 32;
		mask_reg = &mask_entry->simsr_mask [1];
	} else if (pos < 96) {
		pos -= 64;
		mask_reg = &mask_entry->semsr_mask;
	} else if (pos < 128) {
		pos -= 96;
		mask_reg = &mask_entry->sermr_mask;
	} else {
		return RTEMS_NOT_IMPLEMENTED;
	}

	/* Mask or unmask */
	if (mask) {
		rtems_interrupt_disable( level);
		*mask_reg &= ~(1 << pos);
		rtems_interrupt_enable( level);
	} else {
		rtems_interrupt_disable( level);
		*mask_reg |= 1 << pos;
		rtems_interrupt_enable( level);
	}

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_ipic_set_highest_priority_interrupt( rtems_vector_number vector, int type)
{
	rtems_interrupt_level level;
	uint32_t reg = 0;

	if (!MPC83XX_IPIC_IS_VALID_VECTOR( vector)) {
		return RTEMS_INVALID_NUMBER;
	} else if (type < 0 || type > MPC83XX_IPIC_INTERRUPT_CRITICAL) {
		return RTEMS_INVALID_NUMBER;
	}

	rtems_interrupt_disable( level);
	reg = mpc83xx.ipic.sicfr;
	mpc83xx.ipic.sicfr = (reg & ~0x7f000300) | (vector << 24) | (type << 8);
	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

/*
 * functions to enable/disable a source at the ipic
 */
rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number irqnum)
{
	rtems_vector_number vecnum = irqnum - BSP_IPIC_IRQ_LOWEST_OFFSET;
	const BSP_isrc_rsc_t *rsc_ptr;

	if (MPC83XX_IPIC_IS_VALID_VECTOR( vecnum)) {
		rsc_ptr = &mpc83xx_ipic_isrc_rsc [vecnum];
		if (rsc_ptr->mask_reg != NULL) {
			uint32_t bit = 1U << (31 - rsc_ptr->bit_num);
			rtems_interrupt_level level;

			rtems_interrupt_disable(level);
			*(rsc_ptr->mask_reg) |= bit;
			rtems_interrupt_enable(level);
		}
	}

	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number irqnum)
{
	rtems_vector_number vecnum = irqnum - BSP_IPIC_IRQ_LOWEST_OFFSET;
	const BSP_isrc_rsc_t *rsc_ptr;

	if (MPC83XX_IPIC_IS_VALID_VECTOR( vecnum)) {
		rsc_ptr = &mpc83xx_ipic_isrc_rsc [vecnum];
		if (rsc_ptr->mask_reg != NULL) {
			uint32_t bit = 1U << (31 - rsc_ptr->bit_num);
			rtems_interrupt_level level;

			rtems_interrupt_disable(level);
			*(rsc_ptr->mask_reg) &= ~bit;
			rtems_interrupt_enable(level);
		}
	}

	return RTEMS_SUCCESSFUL;
}

/*
 *  IRQ Handler: this is called from the primary exception dispatcher
 */
static int BSP_irq_handle_at_ipic( unsigned excNum)
{
	int32_t vecnum;
	mpc83xx_ipic_mask_t mask_save;
	const mpc83xx_ipic_mask_t *mask_ptr;
	uint32_t msr = 0;
	rtems_interrupt_level level;

	/* Get vector number */
	switch (excNum) {
		case ASM_EXT_VECTOR:
			vecnum = MPC83xx_VCR_TO_VEC( mpc83xx.ipic.sivcr);
			break;
		case ASM_E300_SYSMGMT_VECTOR:
			vecnum = MPC83xx_VCR_TO_VEC( mpc83xx.ipic.smvcr);
			break;
		case ASM_E300_CRIT_VECTOR:
			vecnum = MPC83xx_VCR_TO_VEC( mpc83xx.ipic.scvcr);
			break;
		default:
			return 1;
	}

	/*
	 * Check the vector number, mask lower priority interrupts, enable
	 * exceptions and dispatch the handler.
	 */
	if (MPC83XX_IPIC_IS_VALID_VECTOR( vecnum)) {
#ifdef GEN83XX_ENABLE_INTERRUPT_NESTING
		mask_ptr = &mpc83xx_ipic_prio2mask [vecnum];

		rtems_interrupt_disable( level);

		/* Save current mask registers */
		mask_save.simsr_mask [0] = mpc83xx.ipic.simsr [0];
		mask_save.simsr_mask [1] = mpc83xx.ipic.simsr [1];
		mask_save.semsr_mask = mpc83xx.ipic.semsr;
		mask_save.sermr_mask = mpc83xx.ipic.sermr;

		/* Mask all lower priority interrupts */
		mpc83xx.ipic.simsr [0] &= mask_ptr->simsr_mask [0];
		mpc83xx.ipic.simsr [1] &= mask_ptr->simsr_mask [1];
		mpc83xx.ipic.semsr &= mask_ptr->semsr_mask;
		mpc83xx.ipic.sermr &= mask_ptr->sermr_mask;

		rtems_interrupt_enable( level);

		/* Enable all interrupts */
		if (excNum != ASM_E300_CRIT_VECTOR) {
			msr = ppc_external_exceptions_enable();
		}
#endif /* GEN83XX_ENABLE_INTERRUPT_NESTING */

		/* Dispatch interrupt handlers */
		bsp_interrupt_handler_dispatch( vecnum + BSP_IPIC_IRQ_LOWEST_OFFSET);

#ifdef GEN83XX_ENABLE_INTERRUPT_NESTING
		/* Restore machine state */
		if (excNum != ASM_E300_CRIT_VECTOR) {
			ppc_external_exceptions_disable( msr);
		}

		/* Restore initial masks */
		rtems_interrupt_disable( level);
		mpc83xx.ipic.simsr [0] = mask_save.simsr_mask [0];
		mpc83xx.ipic.simsr [1] = mask_save.simsr_mask [1];
		mpc83xx.ipic.semsr = mask_save.semsr_mask;
		mpc83xx.ipic.sermr = mask_save.sermr_mask;
		rtems_interrupt_enable( level);
#endif /* GEN83XX_ENABLE_INTERRUPT_NESTING */
	} else {
		bsp_interrupt_handler_default( vecnum);
	}

	return 0;
}

/*
 * Fill the array mpc83xx_ipic_prio2mask to allow masking of lower prio sources
 * to implement nested interrupts.
 */
rtems_status_code mpc83xx_ipic_calc_prio2mask( void)
{
	rtems_status_code rc = RTEMS_SUCCESSFUL;

	/*
	 * FIXME: fill the array
	 */
	return rc;
}

/*
 * Activate the interrupt controller
 */
rtems_status_code mpc83xx_ipic_initialize( void)
{
	/*
	 * mask off all interrupts
	 */
	mpc83xx.ipic.simsr [0] = 0;
	mpc83xx.ipic.simsr [1] = 0;
	mpc83xx.ipic.semsr = 0;
	mpc83xx.ipic.sermr = 0;
	/*
	 * set desired configuration as defined in bspopts.h
	 * normally, the default values should be fine
	 */
#if defined( BSP_SICFR_VAL)	/* defined in bspopts.h ? */
	mpc83xx.ipic.sicfr = BSP_SICFR_VAL;
#endif

	/*
	 * set desired priorities as defined in bspopts.h
	 * normally, the default values should be fine
	 */
#if defined( BSP_SIPRR0_VAL)	/* defined in bspopts.h ? */
	mpc83xx.ipic.siprr [0] = BSP_SIPRR0_VAL;
#endif

#if defined( BSP_SIPRR1_VAL)	/* defined in bspopts.h ? */
	mpc83xx.ipic.siprr [1] = BSP_SIPRR1_VAL;
#endif

#if defined( BSP_SIPRR2_VAL)	/* defined in bspopts.h ? */
	mpc83xx.ipic.siprr [2] = BSP_SIPRR2_VAL;
#endif

#if defined( BSP_SIPRR3_VAL)	/* defined in bspopts.h ? */
	mpc83xx.ipic.siprr [3] = BSP_SIPRR3_VAL;
#endif

#if defined( BSP_SMPRR0_VAL)	/* defined in bspopts.h ? */
	mpc83xx.ipic.smprr [0] = BSP_SMPRR0_VAL;
#endif

#if defined( BSP_SMPRR1_VAL)	/* defined in bspopts.h ? */
	mpc83xx.ipic.smprr [1] = BSP_SMPRR1_VAL;
#endif

#if defined( BSP_SECNR_VAL)	/* defined in bspopts.h ? */
	mpc83xx.ipic.secnr = BSP_SECNR_VAL;
#endif

	/*
	 * calculate priority masks
	 */
	return mpc83xx_ipic_calc_prio2mask();
}

int mpc83xx_exception_handler( BSP_Exception_frame *frame, unsigned exception_number)
{
	return BSP_irq_handle_at_ipic( exception_number);
}

rtems_status_code bsp_interrupt_facility_initialize()
{
	/* Install exception handler */
	if (ppc_exc_set_handler( ASM_EXT_VECTOR, mpc83xx_exception_handler)) {
		return RTEMS_IO_ERROR;
	}
	if (ppc_exc_set_handler( ASM_E300_SYSMGMT_VECTOR, mpc83xx_exception_handler)) {
		return RTEMS_IO_ERROR;
	}
	if (ppc_exc_set_handler( ASM_E300_CRIT_VECTOR, mpc83xx_exception_handler)) {
		return RTEMS_IO_ERROR;
	}

	/* Initialize the interrupt controller */
	return mpc83xx_ipic_initialize();
}
