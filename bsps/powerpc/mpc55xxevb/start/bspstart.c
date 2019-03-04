/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief BSP startup code.
 */

/*
 * Copyright (c) 2008-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <mpc55xx/mpc55xx.h>
#include <mpc55xx/regs.h>
#include <mpc55xx/edma.h>
#include <mpc55xx/emios.h>

#include <string.h>

#include <rtems.h>
#include <rtems/config.h>
#include <rtems/counter.h>

#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>
#include <bsp/start.h>
#include <bsp/mpc55xx-config.h>

/* Symbols defined in linker command file */
LINKER_SYMBOL(mpc55xx_exc_vector_base);

unsigned int bsp_clock_speed = 0;

uint32_t bsp_clicks_per_usec = 0;

static void null_pointer_protection(void)
{
#ifdef MPC55XX_NULL_POINTER_PROTECTION
	struct MMU_tag mmu = { .MAS0 = { .B = { .TLBSEL = 1, .ESEL = 1 } } };

	PPC_SET_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS0, mmu.MAS0.R);
	__asm__ volatile ("tlbre");
	mmu.MAS1.R = PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS1);
	mmu.MAS1.B.VALID = 0;
	PPC_SET_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS1, mmu.MAS1.R);
	__asm__ volatile ("tlbwe");
#endif
}

uint32_t _CPU_Counter_frequency(void)
{
	return bsp_clock_speed;
}

void bsp_start(void)
{
	null_pointer_protection();

	/*
	 * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
	 * function store the result in global variables so that it can be used
	 * latter...
	 */
	get_ppc_cpu_type();
	get_ppc_cpu_revision();

	/*
	 * determine clock speed
	 */
	bsp_clock_speed = mpc55xx_get_system_clock() / MPC55XX_SYSTEM_CLOCK_DIVIDER;

	/* Time reference value */
	bsp_clicks_per_usec = bsp_clock_speed / 1000000;

	ppc_exc_initialize_with_vector_base(
		(uintptr_t) _ISR_Stack_area_begin,
		mpc55xx_exc_vector_base
	);
	bsp_interrupt_initialize();

	#if MPC55XX_CHIP_FAMILY != 566
		mpc55xx_edma_init();
	#endif

	#ifdef MPC55XX_EMIOS_PRESCALER
		mpc55xx_emios_initialize(MPC55XX_EMIOS_PRESCALER);
	#endif
}
