/**
 * @file
 *
 * @ingroup mpc83xx
 *
 * @brief Source for BSP startup code.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <string.h>

#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/score/thread.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/ppc_exc_bspsupp.h>

#ifdef HAS_UBOOT

/*
 * We want this in the data section, because the startup code clears the BSS
 * section after the initialization of the board info.
 */
bd_t mpc83xx_uboot_board_info = { .bi_baudrate = 123 };

/* Size in words */
const size_t mpc83xx_uboot_board_info_size = (sizeof( bd_t) + 3) / 4;

#endif /* HAS_UBOOT */

/* Configuration parameters for console driver, ... */
unsigned int BSP_bus_frequency;

/* Configuration parameters for clock driver, ... */
uint32_t bsp_clicks_per_usec;

/*
 *  Use the shared implementations of the following routines.
 *  Look in rtems/c/src/lib/libbsp/shared/bsplibc.c.
 */
extern void cpu_init( void);

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

void bsp_pretasking_hook( void)
{
	/* Do noting */
}

void bsp_get_work_area( void **work_area_start, size_t *work_area_size, void **heap_start, size_t *heap_size)
{
#ifdef HAS_UBOOT
	char *ram_end = (char *) mpc83xx_uboot_board_info.bi_memstart + mpc83xx_uboot_board_info.bi_memsize;
#else /* HAS_UBOOT */
	char *ram_end = bsp_ram_end;
#endif /* HAS_UBOOT */

	*work_area_start = bsp_work_area_start;
	*work_area_size = ram_end - bsp_work_area_start;
	*heap_start = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
	*heap_size = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;
}

void bsp_start( void)
{
	ppc_cpu_id_t myCpu;
	ppc_cpu_revision_t myCpuRevision;

	uint32_t interrupt_stack_start = (uint32_t) bsp_interrupt_stack_start;
	uint32_t interrupt_stack_size = (uint32_t) bsp_interrupt_stack_size;

	/*
	 * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
	 * store the result in global variables so that it can be used latter...
	 */
	myCpu = get_ppc_cpu_type();
	myCpuRevision = get_ppc_cpu_revision();

	/* Basic CPU initialization */
	cpu_init();

	/*
	 * Enable instruction and data caches. Do not force writethrough mode.
	 */

#if INSTRUCTION_CACHE_ENABLE
	rtems_cache_enable_instruction();
#endif

#if DATA_CACHE_ENABLE
	rtems_cache_enable_data();
#endif

	/*
	 * This is evaluated during runtime, so it should be ok to set it
	 * before we initialize the drivers.
	 */

	/* Initialize some device driver parameters */

#ifdef HAS_UBOOT
	BSP_bus_frequency = mpc83xx_uboot_board_info.bi_busfreq;
	bsp_clicks_per_usec = mpc83xx_uboot_board_info.bi_intfreq / 8000000;
#else /* HAS_UBOOT */
	BSP_bus_frequency = BSP_CLKIN_FRQ * BSP_SYSPLL_MF / BSP_SYSPLL_CKID;
	bsp_clicks_per_usec = BSP_bus_frequency / 1000000;
#endif /* HAS_UBOOT */

	/* Initialize exception handler */
	ppc_exc_initialize(
		PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
		interrupt_stack_start,
		interrupt_stack_size
	);

	/* Initalize interrupt support */
	if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
		BSP_panic("Cannot intitialize interrupt support\n");
	}

#ifdef SHOW_MORE_INIT_SETTINGS
	printk("Exit from bspstart\n");
#endif
}

/**
 * @brief Idle thread body.
 *
 * Replaces the one in c/src/exec/score/src/threadidlebody.c
 * The MSR[POW] bit is set to put the CPU into the low power mode
 * defined in HID0.  HID0 is set during starup in start.S.
 */
Thread _Thread_Idle_body( uint32_t ignored)
{

	while (1) {
		asm volatile (
			"mfmsr 3;"
			"oris 3, 3, 4;"
			"sync;"
			"mtmsr 3;"
			"isync;"
			"ori 3, 3, 0;"
			"ori 3, 3, 0"
		);
	}

	return NULL;
}
