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
	m68k_isr_entry *vbr;
	extern void _CopyDataClearBSSAndStart (void);

#if (defined (m68040) || defined (m68lc040) || defined (m68ec040))
	/*
	 *******************************************
	 * Motorola 68040 and companion-mode 68360 *
	 *******************************************
	 */

	/*
	 * Step 6: Is this a power-up reset?
	 * For now we just ignore this and do *all* the steps
	 * Someday we might want to:
	 *	if (Hard, Loss of Clock, Power-up)
	 *		Do all steps
	 *	else if (Double bus fault, watchdog or soft reset)
	 *		Skip to step 12
	 *	else (must be a reset command)
	 *		Skip to step 14
	 */

	/*
	 * Step 7: Deal with clock synthesizer
	 * HARDWARE:
	 *	Change if you're not using an external 25 MHz oscillator.
	 */
	m360.clkocr = 0x83;	/* No more writes, full-power CLKO2 */
	m360.pllcr = 0xD000;	/* PLL, no writes, no prescale,
				   no LPSTOP slowdown, PLL X1 */
	m360.cdvcr = 0x8000;	/* No more writes, no clock division */

	/*
	 * Step 8: Initialize system protection
	 *	Enable watchdog
	 *	Watchdog causes system reset
	 *	Next-to-slowest watchdog timeout (21 seconds with 25 MHz oscillator)
	 *	Enable double bus fault monitor
	 *	Enable bus monitor for external cycles
	 *	1024 clocks for external timeout
	 */
	m360.sypcr = 0xEC;

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
	 *	SINTOUT standard M68000 family interrupt level encoding
	 *	CF1MODE=10 (BCLRO* output)
	 *	No RAS1* double drive
	 *	A31 - A28
	 *	AMUX output
	 *	CAS2* - CAS3*
	 *	CAS0* - CAS1*
	 *	CS7*
	 *	AVEC*
	 */
	m360.pepar = 0x3440;

	/*
	 * Step 11: Remap Chip Select 0 (CS0*), set up GMR
	 */
	/*
	 * 512 addresses per DRAM page (256K DRAM chips)
	 * 70 nsec DRAM
	 * 180 nsec ROM (3 wait states)
	 */
	m360.gmr = M360_GMR_RCNT(23) | M360_GMR_RFEN | 
				M360_GMR_RCYC(0) | M360_GMR_PGS(1) |
				M360_GMR_DPS_32BIT | M360_GMR_NCS |
				M360_GMR_TSS40;
	m360.memc[0].br = (unsigned long)&_RomBase | M360_MEMC_BR_WP |
							M360_MEMC_BR_V;
	m360.memc[0].or = M360_MEMC_OR_WAITS(3) | M360_MEMC_OR_1MB |
						M360_MEMC_OR_32BIT;

	/*
	 * Step 12: Initialize the system RAM
	 */
	/*
	 *	Set up option/base registers
	 *		1M DRAM
	 *		70 nsec DRAM
	 *	Enable burst mode
	 *	No parity checking
	 *	Wait for chips to power up
	 *	Perform 8 read cycles
	 */
	m360.memc[1].or = M360_MEMC_OR_TCYC(0) |
					M360_MEMC_OR_1MB |
					M360_MEMC_OR_DRAM;
	m360.memc[1].br = (unsigned long)&_RamBase |
					M360_MEMC_BR_BACK40 |
					M360_MEMC_BR_V;
	for (i = 0; i < 50000; i++)
		continue;
	for (i = 0; i < 8; ++i)
		*((volatile unsigned long *)(unsigned long)&_RamBase);

	/*
	 * Step 13: Copy  the exception vector table to system RAM
	 */
	m68k_get_vbr (vbr);
	for (i = 0; i < 256; ++i)
		M68Kvec[i] = vbr[i];
	m68k_set_vbr (M68Kvec);
	
	/*
	 * Step 14: More system initialization
	 * SDCR (Serial DMA configuration register)
	 *	Enable SDMA during FREEZE
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
	 *	Bus request MC68040 Arbitration ID 3
	 *	Bus asynchronous timing mode (work around bug in Rev. B)
	 *	Arbitration asynchronous timing mode
	 *	Disable timers during FREEZE
	 *	Disable bus monitor during FREEZE
	 *	BCLRO* arbitration level 3
	 *	No show cycles
	 *	User/supervisor access
	 *	Bus clear in arbitration ID level  3
	 *	SIM60 interrupt sources higher priority than CPM
	 */
	m360.mcr = 0x6000EC3F;

#elif (defined (M68360_ATLAS_HSB))
	/*
	 ******************************************
	 * Standalone Motorola 68360 -- ATLAS HSB *
	 ******************************************
	 */

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
	m360.pllcr = 0xD000;	/* PLL, no writes, no prescale,
				   no LPSTOP slowdown, PLL X1 */
	m360.cdvcr = 0x8000;	/* No more writes, no clock division */

	/*
	 * Step 8: Initialize system protection
	 *	Enable watchdog
	 *	Watchdog causes system reset
	 *	Next-to-slowest watchdog timeout (21 seconds with 25 MHz oscillator)
	 *	Enable double bus fault monitor
	 *	Enable bus monitor for external cycles
	 *	1024 clocks for external timeout
	 */
	m360.sypcr = 0xEC;

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
	 *	WE0* - WE3*
	 *	OE* output
	 *	CAS2* - CAS3*
	 *	CAS0* - CAS1*
	 *	CS7*
	 *	AVEC*
	 * HARDWARE:
	 *	Change if you are using a different memory configuration
	 *	(static RAM, external address multiplexing, etc).
	 */
	m360.pepar = 0x0180;

	/*
	 * Step 11: Remap Chip Select 0 (CS0*), set up GMR
	 */
	m360.gmr = M360_GMR_RCNT(12) | M360_GMR_RFEN | 
				M360_GMR_RCYC(0) | M360_GMR_PGS(1) | 
				M360_GMR_DPS_32BIT | M360_GMR_DWQ |
				M360_GMR_GAMX;
	m360.memc[0].br = (unsigned long)&_RomBase | M360_MEMC_BR_WP |
								M360_MEMC_BR_V;
	m360.memc[0].or = M360_MEMC_OR_WAITS(3) | M360_MEMC_OR_1MB |
							M360_MEMC_OR_8BIT;

	/*
	 * Step 12: Initialize the system RAM
	 */
	/* first bank 1MByte DRAM */
	m360.memc[1].or = M360_MEMC_OR_TCYC(2) | M360_MEMC_OR_1MB |
					M360_MEMC_OR_PGME | M360_MEMC_OR_DRAM;
	m360.memc[1].br = (unsigned long)&_RamBase | M360_MEMC_BR_V;

	/* second bank 1MByte DRAM */
	m360.memc[2].or = M360_MEMC_OR_TCYC(2) | M360_MEMC_OR_1MB |
					M360_MEMC_OR_PGME | M360_MEMC_OR_DRAM;
	m360.memc[2].br = ((unsigned long)&_RamBase + 0x100000) |
					M360_MEMC_BR_V;

	/* flash rom socket U6 on CS5 */
	m360.memc[5].br = (unsigned long)ATLASHSB_ROM_U6 | M360_MEMC_BR_WP |
								M360_MEMC_BR_V;
	m360.memc[5].or = M360_MEMC_OR_WAITS(2) | M360_MEMC_OR_512KB |
								M360_MEMC_OR_8BIT;

	/* CSRs on CS7 */
	m360.memc[7].or = M360_MEMC_OR_TCYC(4) | M360_MEMC_OR_64KB |
					M360_MEMC_OR_8BIT;
	m360.memc[7].br = ATLASHSB_ESR | 0x01;
	for (i = 0; i < 50000; i++)
		continue;
	for (i = 0; i < 8; ++i)
		*((volatile unsigned long *)(unsigned long)&_RamBase);

	/*
	 * Step 13: Copy  the exception vector table to system RAM
	 */
	m68k_get_vbr (vbr);
	for (i = 0; i < 256; ++i)
		M68Kvec[i] = vbr[i];
	m68k_set_vbr (M68Kvec);
	
	/*
	 * Step 14: More system initialization
	 * SDCR (Serial DMA configuration register)
	 *	Enable SDMA during FREEZE
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
	 *	Bus clear interrupt service level 7
	 *	SIM60 interrupt sources higher priority than CPM
	 */
	m360.mcr = 0x4C7F;

#else
	/*
	 ***************************************************
	 * Generic Standalone Motorola 68360               *
	 *           As described in MC68360 User's Manual *
	 *           Atlas ACE360                          *
	 ***************************************************
	 */

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
	m360.pllcr = 0xD000;	/* PLL, no writes, no prescale,
				   no LPSTOP slowdown, PLL X1 */
	m360.cdvcr = 0x8000;	/* No more writes, no clock division */

	/*
	 * Step 8: Initialize system protection
	 *	Enable watchdog
	 *	Watchdog causes system reset
	 *	Next-to-slowest watchdog timeout (21 seconds with 25 MHz oscillator)
	 *	Enable double bus fault monitor
	 *	Enable bus monitor for external cycles
	 *	1024 clocks for external timeout
	 */
	m360.sypcr = 0xEC;

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
	 *	WE0* - WE3*
	 *	OE* output
	 *	CAS2* - CAS3*
	 *	CAS0* - CAS1*
	 *	CS7*
	 *	AVEC*
	 * HARDWARE:
	 *	Change if you are using a different memory configuration
	 *	(static RAM, external address multiplexing, etc).
	 */
	m360.pepar = 0x0180;

	/*
	 * Step 11: Remap Chip Select 0 (CS0*), set up GMR
	 */
	/*
	 * 1024/2048/4096 addresses per DRAM page (1M/4M/16M DRAM chips)
	 * 60 nsec DRAM
	 * 180 nsec ROM (3 wait states)
	 */
	switch ((unsigned long)&_RamSize) {
	default:
	case 4*1024*1024:
		m360.gmr = M360_GMR_RCNT(23) | M360_GMR_RFEN | 
					M360_GMR_RCYC(0) | M360_GMR_PGS(3) |
					M360_GMR_DPS_32BIT | M360_GMR_NCS | 
					M360_GMR_GAMX;
		break;

	case 16*1024*1024:
		m360.gmr = M360_GMR_RCNT(23) | M360_GMR_RFEN | 
					M360_GMR_RCYC(0) | M360_GMR_PGS(5) |
					M360_GMR_DPS_32BIT | M360_GMR_NCS | 
					M360_GMR_GAMX;
		break;

	case 64*1024*1024:
		m360.gmr = M360_GMR_RCNT(23) | M360_GMR_RFEN | 
					M360_GMR_RCYC(0) | M360_GMR_PGS(7) |
					M360_GMR_DPS_32BIT | M360_GMR_NCS | 
					M360_GMR_GAMX;
		break;
	}
	m360.memc[0].br = (unsigned long)&_RomBase | M360_MEMC_BR_WP |
								M360_MEMC_BR_V;
	m360.memc[0].or = M360_MEMC_OR_WAITS(3) | M360_MEMC_OR_1MB |
							M360_MEMC_OR_8BIT;

	/*
	 * Step 12: Initialize the system RAM
	 */
	/*
	 *	Set up option/base registers
	 *		4M/16M/64M DRAM
	 *		60 nsec DRAM
	 *	Wait for chips to power up
	 *	Perform 8 read cycles
	 *	Set all parity bits to correct state
	 *	Enable parity checking
	 */
	switch ((unsigned long)&_RamSize) {
	default:
	case 4*1024*1024:
		m360.memc[1].or = M360_MEMC_OR_TCYC(0) |
					M360_MEMC_OR_4MB |
					M360_MEMC_OR_DRAM;
		break;

	case 16*1024*1024:
		m360.memc[1].or = M360_MEMC_OR_TCYC(0) |
					M360_MEMC_OR_16MB |
					M360_MEMC_OR_DRAM;
		break;

	case 64*1024*1024:
		m360.memc[1].or = M360_MEMC_OR_TCYC(0) |
					M360_MEMC_OR_64MB |
					M360_MEMC_OR_DRAM;
		break;
	}
	m360.memc[1].br = (unsigned long)&_RamBase | M360_MEMC_BR_V;
	for (i = 0; i < 50000; i++)
		continue;
	for (i = 0; i < 8; ++i)
		*((volatile unsigned long *)(unsigned long)&_RamBase);
	for (i = 0 ; i < (unsigned long)&_RamSize ; i += sizeof (unsigned long)) {
		volatile unsigned long *lp;
		lp = (unsigned long *)((unsigned char *)&_RamBase + i);
		*lp = *lp;
	}
	m360.memc[1].br = (unsigned long)&_RamBase |
				M360_MEMC_BR_PAREN | M360_MEMC_BR_V;

	/*
	 * Step 13: Copy  the exception vector table to system RAM
	 */
	m68k_get_vbr (vbr);
	for (i = 0; i < 256; ++i)
		M68Kvec[i] = vbr[i];
	m68k_set_vbr (M68Kvec);
	
	/*
	 * Step 14: More system initialization
	 * SDCR (Serial DMA configuration register)
	 *	Enable SDMA during FREEZE
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
	 *	Bus clear interrupt service level 7
	 *	SIM60 interrupt sources higher priority than CPM
	 */
	m360.mcr = 0x4C7F;
#endif

	/*
	 * Copy data, clear BSS, switch stacks and call main()
	 */
	_CopyDataClearBSSAndStart ();
}
