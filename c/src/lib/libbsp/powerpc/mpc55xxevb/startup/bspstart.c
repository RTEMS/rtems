/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief BSP startup code.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

extern Heap_Control *RTEMS_Malloc_Heap;

/* Symbols defined in linker command file */
LINKER_SYMBOL(mpc55xx_exc_vector_base);

unsigned int bsp_clock_speed = 0;

uint32_t bsp_clicks_per_usec = 0;

void BSP_panic( char *s)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable( level);

	printk( "%s PANIC %s\n", _RTEMS_version, s);

	while (1) {
		/* Do nothing */
	}
}

void _BSP_Fatal_error( unsigned n)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable( level);

	printk( "%s PANIC ERROR %u\n", _RTEMS_version, n);

	while (1) {
		/* Do nothing */
	}
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
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	ppc_cpu_id_t myCpu;
	ppc_cpu_revision_t myCpuRevision;

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
	myCpu = get_ppc_cpu_type();
	myCpuRevision = get_ppc_cpu_revision();

	/*
	 * determine clock speed
	 */
	bsp_clock_speed = mpc55xx_get_system_clock() / MPC55XX_SYSTEM_CLOCK_DIVIDER;

	/* Time reference value */
	bsp_clicks_per_usec = bsp_clock_speed / 1000000;

	/* Initialize exceptions */
	ppc_exc_vector_base = (uint32_t) mpc55xx_exc_vector_base;
	sc = ppc_exc_initialize(
		PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
                (uintptr_t) bsp_section_work_begin,
                Configuration.interrupt_stack_size
	);
	if (sc != RTEMS_SUCCESSFUL) {
		BSP_panic( "Cannot initialize exceptions");
	}
	ppc_exc_set_handler(ASM_ALIGN_VECTOR, ppc_exc_alignment_handler);

	/* Initialize interrupts */
	sc = bsp_interrupt_initialize();
	if (sc != RTEMS_SUCCESSFUL) {
		BSP_panic( "Cannot initialize interrupts");
	}

	mpc55xx_edma_init();
	#ifdef MPC55XX_EMIOS_PRESCALER
		mpc55xx_emios_initialize(MPC55XX_EMIOS_PRESCALER);
	#endif
}

void bsp_pretasking_hook(void)
{
	#if MPC55XX_CHIP_TYPE / 10 == 564
		_Heap_Extend(
			RTEMS_Malloc_Heap,
			bsp_section_rwextra_end,
			(uintptr_t) bsp_ram_end
				- (uintptr_t) bsp_section_rwextra_end,
			NULL
		);
	#endif
}
