/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief BSP startup code.
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
 */

#include <mpc55xx/mpc55xx.h>
#include <mpc55xx/regs.h>
#include <mpc55xx/edma.h>

#include <rtems.h>

#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

#define DEBUG_DONE() RTEMS_DEBUG_PRINT( "Done\n")

#define MPC55XX_INTERRUPT_STACK_SIZE 0x1000

/* Symbols defined in linker command file */
LINKER_SYMBOL(bsp_ram_start);
LINKER_SYMBOL(bsp_ram_end);
LINKER_SYMBOL(bsp_external_ram_start);
LINKER_SYMBOL(bsp_external_ram_size);
LINKER_SYMBOL(bsp_section_bss_end);

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

void bsp_predriver_hook()
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	RTEMS_DEBUG_PRINT( "Initialize eDMA ...\n");
	sc = mpc55xx_edma_init();
	if (sc != RTEMS_SUCCESSFUL) {
		BSP_panic( "Cannot initialize eDMA");
	} else {
		DEBUG_DONE();
	}
}

static void mpc55xx_ebi_init()
{
	struct EBI_CS_tag cs = { .BR = MPC55XX_ZERO_FLAGS, .OR = MPC55XX_ZERO_FLAGS };
	union SIU_PCR_tag pcr = MPC55XX_ZERO_FLAGS;
	struct MMU_tag mmu = MMU_DEFAULT;
	int i = 0;

	/* ADDR [8 : 31] */
	for (i = 4; i < 4 + 24; ++i) {
		SIU.PCR [i].R = 0x440;
	}

	/* DATA [0 : 15] */
	for (i = 28; i < 28 + 16; ++i) {
		SIU.PCR [i].R = 0x440;
	}

	/* RD_!WR */
	SIU.PCR [62].R = 0x443;

	/* !BDIP */
	SIU.PCR [63].R = 0x443;

	/* !WE [0 : 3] */
	for (i = 64; i < 64 + 4; ++i) {
		SIU.PCR [i].R = 0x443;
	}

	/* !OE */
	SIU.PCR [68].R = 0x443;

	/* !TS */
	SIU.PCR [69].R = 0x443;

	/* External SRAM (2 wait states, 512kB, 4 word burst) */

	cs.BR.B.BA = 0;
	cs.BR.B.PS = 1;
	cs.BR.B.BL = 1;
	cs.BR.B.WEBS = 0;
	cs.BR.B.TBDIP = 0;
	cs.BR.B.BI = 1; /* TODO: Enable burst */
	cs.BR.B.V = 1;

	cs.OR.B.AM = 0x1fff0;
	cs.OR.B.SCY = 0;
	cs.OR.B.BSCY = 0;

	EBI.CS [0] = cs;

	/* !CS [0] */
	SIU.PCR [0].R = 0x443;

	/* External Ethernet Controller (3 wait states, 64kB) */

	mmu.MAS0.B.ESEL = 5;
	mmu.MAS1.B.VALID = 1;
	mmu.MAS1.B.IPROT = 1;
	mmu.MAS1.B.TSIZ = 1;
	mmu.MAS2.B.EPN = 0x3fff8;
	mmu.MAS2.B.I = 1;
	mmu.MAS2.B.G = 1;
	mmu.MAS3.B.RPN = 0x3fff8;
	mmu.MAS3.B.UW = 1;
	mmu.MAS3.B.SW = 1;
	mmu.MAS3.B.UR = 1;
	mmu.MAS3.B.SR = 1;

	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS0, mmu.MAS0.R);
	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS1, mmu.MAS1.R);
	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS2, mmu.MAS2.R);
	PPC_SET_SPECIAL_PURPOSE_REGISTER( FREESCALE_EIS_MAS3, mmu.MAS3.R);

	asm volatile ("tlbwe");

	cs.BR.B.BA = 0x7fff;
	cs.BR.B.PS = 1;
	cs.BR.B.BL = 0;
	cs.BR.B.WEBS = 0;
	cs.BR.B.TBDIP = 0;
	cs.BR.B.BI = 1;
	cs.BR.B.V = 1;

	cs.OR.B.AM = 0x1ffff;
	cs.OR.B.SCY = 1;
	cs.OR.B.BSCY = 0;

	EBI.CS [3] = cs;

	/* !CS [3] */
	SIU.PCR [3].R = 0x443;
}

/**
 * @brief Start BSP.
 */
void bsp_start(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	ppc_cpu_id_t myCpu;
	ppc_cpu_revision_t myCpuRevision;

	uint32_t interrupt_stack_start = bsp_ram_end - 2 * MPC55XX_INTERRUPT_STACK_SIZE;
	uint32_t interrupt_stack_size = MPC55XX_INTERRUPT_STACK_SIZE;

	/* ESCI pad configuration */
	SIU.PCR [89].R = 0x400;
	SIU.PCR [90].R = 0x400;

	RTEMS_DEBUG_PRINT( "BSP start ...\n");

	RTEMS_DEBUG_PRINT( "System clock          : %i\n", mpc55xx_get_system_clock());
	RTEMS_DEBUG_PRINT( "Memory start          : 0x%08x\n", bsp_ram_start);
	RTEMS_DEBUG_PRINT( "Memory end            : 0x%08x\n", bsp_ram_end);
	RTEMS_DEBUG_PRINT( "Memory size           : 0x%08x\n", bsp_ram_end - bsp_ram_start);
	RTEMS_DEBUG_PRINT( "Interrupt stack start : 0x%08x\n", interrupt_stack_start);
	RTEMS_DEBUG_PRINT( "Interrupt stack end   : 0x%08x\n", interrupt_stack_start + interrupt_stack_size);
	RTEMS_DEBUG_PRINT( "Interrupt stack size  : 0x%08x\n", interrupt_stack_size);
	
	/*
	 * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
	 * function store the result in global variables so that it can be used
	 * latter...
	 */
	myCpu = get_ppc_cpu_type();
	myCpuRevision = get_ppc_cpu_revision();
	
	/* Time reference value */
	bsp_clicks_per_usec = bsp_clock_speed / 1000000;

	/* Initialize External Bus Interface */
	mpc55xx_ebi_init();
	
	/* Initialize exceptions */
	RTEMS_DEBUG_PRINT( "Initialize exceptions ...\n");
	sc = ppc_exc_initialize(
		PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
		interrupt_stack_start,
		interrupt_stack_size
	);
	if (sc != RTEMS_SUCCESSFUL) {
		BSP_panic( "Cannot initialize exceptions");
	} else {
		DEBUG_DONE();
	}

	/* Initialize interrupts */
	RTEMS_DEBUG_PRINT( "Initialize interrupts ...\n");
	sc = bsp_interrupt_initialize();
	if (sc != RTEMS_SUCCESSFUL) {
		BSP_panic( "Cannot initialize interrupts");
	} else {
		DEBUG_DONE();
	}
	
	/* Initialize eMIOS */
	mpc55xx_emios_initialize( 1);

	return;

	/* TODO */
	/*
	* Enable instruction and data caches. Do not force writethrough mode.
	*/
#if INSTRUCTION_CACHE_ENABLE
	rtems_cache_enable_instruction();
#endif
#if DATA_CACHE_ENABLE
	rtems_cache_enable_data();
#endif
}
