/**
 * @file
 *
 * @ingroup mpc55xx
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
 * http://www.rtems.com/license/LICENSE.
 */

#include <mpc55xx/mpc55xx.h>
#include <mpc55xx/regs.h>
#include <mpc55xx/edma.h>
#include <mpc55xx/emios.h>

#include <string.h>

#include <rtems.h>
#include <rtems/config.h>

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

void _BSP_Fatal_error(unsigned n)
{
	rtems_interrupt_level level;

	(void) level;
	rtems_interrupt_disable( level);

	while (true) {
		mpc55xx_wait_for_interrupt();
	}
}

void mpc55xx_fatal(mpc55xx_fatal_code code)
{
  rtems_fatal(RTEMS_FATAL_SOURCE_BSP_SPECIFIC, code);
}

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

void bsp_start(void)
{
	null_pointer_protection();

	/*
	 * make sure BSS/SBSS is cleared
	 */
	memset(&bsp_section_bss_begin [0], 0, (size_t) bsp_section_bss_size);

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

	/* Initialize exceptions */
	ppc_exc_initialize_with_vector_base(
		PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
		(uintptr_t) bsp_section_work_begin,
		rtems_configuration_get_interrupt_stack_size(),
		mpc55xx_exc_vector_base
	);
	#ifndef PPC_EXC_CONFIG_USE_FIXED_HANDLER
		ppc_exc_set_handler(ASM_ALIGN_VECTOR, ppc_exc_alignment_handler);
	#endif

	/* Initialize interrupts */
	bsp_interrupt_initialize();

	mpc55xx_edma_init();
	#ifdef MPC55XX_EMIOS_PRESCALER
		mpc55xx_emios_initialize(MPC55XX_EMIOS_PRESCALER);
	#endif
}
