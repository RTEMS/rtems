/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief BSP startup code.
 */

/*
 * Copyright (C) 2008, 2013 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
	PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS1, mmu.MAS1.R);
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
