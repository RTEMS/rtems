/*
 * MC68360 support routines
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <m68360.h>

/*
 * Send a command to the CPM RISC processer
 */

void M360ExecuteRISC(rtems_unsigned16 command)
{
	rtems_unsigned16 sr;

	m68k_disable_interrupts (sr);
	while (m360.cr & M360_CR_FLG)
		continue;
	m360.cr = command | M360_CR_FLG;
	m68k_enable_interrupts (sr);
}

/*
 * Initialize MC68360
 */

void _Init68360 (void)
{
	int i;
	extern void *_RomBase, *_RamBase;
	m68k_isr_entry *vbr;
	extern void _CopyDataClearBSSAndStart (void);

	/*
	 * Step 6: Is this a power-up reset?
	 * For now we just ignore this and do *all* the steps
	 * Someday we might want to:
	 *	if (Hard, Loss of Clock, Power-up)
	 *		Do all steps
	 *	else if (Double bus fault, watchdog or soft reset)
	 *		Skip to step 12
	 *	else (must be a CPU32+ reset command)
	 *		Skip to step 14
	 */

	/*
	 * Step 7: Deal with clock synthesizer
	 * HARDWARE:
	 *	Change if you're not using an external 25 MHz oscillator.
	 */
	m360.clkocr = 0x8F;	/* No more writes, no clock outputs */
	m360.pllcr = 0xD000;	/* PLL, no writes, no prescale,  */
					/* no LPSTOP slowdown, PLL X1 */
	m360.cdvcr = 0x8000;	/* No more writes, no clock division */

	/*
	 * Step 8: Initialize system protection
	 *	Disable watchdog FIXME: Should use watchdog!!!!
	 *	Watchdog causes system reset
	 *	Fastest watchdog timeout
	 *	Enable double bus fault monitor
	 *	Enable bus monitor external
	 *	128 clocks for external timeout
	 */
	m360.sypcr = 0x4F;

	/*
	 * Step 9: Clear parameter RAM and reset communication processor module
	 */
	for (i = 0 ; i < 192  ; i += sizeof (long)) {
		*((long *)((char *)&m360 + 0xC00 + i)) = 0;
		*((long *)((char *)&m360 + 0xD00 + i)) = 0;
		*((long *)((char *)&m360 + 0xE00 + i)) = 0;
		*((long *)((char *)&m360 + 0xF00 + i)) = 0;
	}
	M360ExecuteRISC (M360_CR_RST);

	/*
	 * Step 10: Write PEPAR
	 *	SINTOUT not used (CPU32+ mode)
	 *	CF1MODE=00 (CONFIG1 input)
	 *	RAS1* double drive
	 *	A31-A28
	 *	OE* output
	 *	CAS2* / CAS3*
	 *	CAS0* / CAS1*
	 *	CS7*
	 *	AVEC*
	 * HARDWARE:
	 *	Change if you are using a different memory configuration
	 *	(static RAM, external address multiplexing, etc).
	 */
	m360.pepar = 0x0100;

	/*
	 * Step 11: Remap Chip Select 0 (CS0*), set up GMR
	 *	1024 addresses per DRAM page (1M DRAM chips)
	 *	60 nsec DRAM
	 *	180 nsec ROM (3 wait states)
	 * HARDWARE:
	 *	Change if you are using a different memory configuration
	 */
	m360.gmr = M360_GMR_RCNT(24) | M360_GMR_RFEN | M360_GMR_RCYC(0) |
			M360_GMR_PGS(3) | M360_GMR_DPS_32BIT | M360_GMR_NCS |
			M360_GMR_GAMX;
	m360.memc[0].br = (unsigned long)&_RomBase | M360_MEMC_BR_WP |
							M360_MEMC_BR_V;
	m360.memc[0].or = M360_MEMC_OR_WAITS(3) | M360_MEMC_OR_512KB |
							M360_MEMC_OR_8BIT;

	/*
	 * Step 12: Initialize the system RAM
	 *	Set up option/base registers
	 *		4 MB DRAM
	 *		60 nsec DRAM
	 *	Wait for chips to power up
	 *	Perform 8 read cycles
	 *	Set all parity bits to correct state
	 *	Enable parity checking
	 * HARDWARE:
	 *	Change if you are using a different memory configuration
	 */
	m360.memc[1].or = M360_MEMC_OR_TCYC(0) | M360_MEMC_OR_4MB |
				M360_MEMC_OR_DRAM;
	m360.memc[1].br = (unsigned long)&_RamBase | M360_MEMC_BR_V;
	for (i = 0; i < 50000; i++)
		continue;
	for (i = 0; i < 8; ++i)
		*((volatile unsigned long *)(unsigned long)&_RamBase);
	for (i = 0 ; i < 4*1024*1024 ; i += sizeof (unsigned long)) {
		volatile unsigned long *lp;
		lp = (unsigned long *)((unsigned char *)&_RamBase + i);
		*lp = *lp;
	}
	m360.memc[1].br = (unsigned long)&_RamBase | M360_MEMC_BR_PAREN |
								M360_MEMC_BR_V;

	/*
	 * Step 13: Copy  the exception vector table to system RAM
	 */
	m68k_get_vbr (vbr);
	for (i = 0; i < 256; ++i)
		M68Kvec[i] = vbr[i];
	m68k_set_vbr (M68Kvec);
	
	/*
	 * Step 14: More system initialization
	 * SDCR (Serial DMA configuratin register)
	 *	Give SDMA priority over all interrupt handlers
	 *	Set DMA arbiration level to 4
	 * CICR (CPM interrupt configuration register):
	 *	SCC1 requests at SCCa position
	 *	SCC2 requests at SCCb position
	 *	SCC3 requests at SCCc position
	 *	SCC4 requests at SCCd position
	 *	Interrupt request level 4
	 *	Maintain original priority order
	 *	Vector base 128
	 *	SCCs priority grouped at top of table
	 */
	m360.sdcr = M360_SDMA_SISM_7 | M360_SDMA_SAID_4;
	m360.cicr = (3 << 22) | (2 << 20) | (1 << 18) | (0 << 16) |
						(4 << 13) | (0x1F << 8) | (128);

	/*
	 * Step 15: Set module configuration register
	 *	Disable timers during FREEZE
	 *	Enable bus monitor during FREEZE
	 *	BCLRO* arbitration level 3
	 *	No show cycles
	 *	User/supervisor access
	 *	Bus clear interupt service level 7
	 *	SIM60 interrupt sources higher priority than CPM
	 */
	m360.mcr = 0x4C7F;

	/*
	 * Copy data, clear BSS, switch stacks and call main()
	 */
	_CopyDataClearBSSAndStart ();
}
