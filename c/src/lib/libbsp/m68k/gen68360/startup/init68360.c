/*
 * MC68360 support routines
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/m68k/m68360.h>

extern void _CopyDataClearBSSAndStart (unsigned long ramSize);
extern void *RamBase;
extern void *_RomBase;	/* From linkcmds */

/*
 * Declare the m360 structure here for the benefit of the debugger
 */

volatile m360_t m360;

/*
 * Send a command to the CPM RISC processer
 */

void M360ExecuteRISC(uint16_t         command)
{
	uint16_t         sr;

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
	rtems_isr_entry *vbr;
	unsigned long ramSize;
	volatile unsigned long *RamBase_p;

	RamBase_p = (volatile unsigned long *)&RamBase;

#if (defined (__mc68040__))
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
	ramSize = 1 * 1024 * 1024;
	m360.memc[1].or = M360_MEMC_OR_TCYC(0) |
					M360_MEMC_OR_1MB |
					M360_MEMC_OR_DRAM;
	m360.memc[1].br = (unsigned long)&RamBase |
					M360_MEMC_BR_BACK40 |
					M360_MEMC_BR_V;
	for (i = 0; i < 50000; i++)
		continue;
	for (i = 0; i < 8; ++i)
		*RamBase_p;

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
	ramSize = 2 * 1024 * 1024;
	/* first bank 1MByte DRAM */
	m360.memc[1].or = M360_MEMC_OR_TCYC(2) | M360_MEMC_OR_1MB |
					M360_MEMC_OR_PGME | M360_MEMC_OR_DRAM;
	m360.memc[1].br = (unsigned long)&RamBase | M360_MEMC_BR_V;

	/* second bank 1MByte DRAM */
	m360.memc[2].or = M360_MEMC_OR_TCYC(2) | M360_MEMC_OR_1MB |
					M360_MEMC_OR_PGME | M360_MEMC_OR_DRAM;
	m360.memc[2].br = ((unsigned long)&RamBase + 0x100000) |
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
		*((volatile unsigned long *)(unsigned long)&RamBase);

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
	 */

#elif defined(PGH360)
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
	m360.clkocr = 0x8e;	/* No more writes, CLKO1=1/3, CLKO2=off */
	/*
	 * adjust crystal to average between 4.19 MHz and 4.00 MHz
	 * reprogram pll
	 */
	m360.pllcr = 0xA000+(24576000/((4000000+4194304)/2/128))-1;
					/* LPSTOP slowdown, PLL /128*??? */
	m360.cdvcr = 0x8000;	/* No more writes, no clock division */

	/*
	 * Step 8: Initialize system protection
	 *	Enable watchdog
	 *	Watchdog causes system reset
	 *	128 sec. watchdog timeout
	 *	Enable double bus fault monitor
	 *	Enable bus monitor external
	 *	128 clocks for external timeout
	 */
	m360.sypcr = 0xEF;
	/*
	 * also initialize the SWP bit in PITR to 1
	 */
	m360.pitr |= 0x0200;
	/*
	 * and trigger SWSR twice to ensure, that interval starts right now
	 */
	m360.swsr = 0x55;
	m360.swsr = 0xAA;
	m360.swsr = 0x55;
	m360.swsr = 0xAA;
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
	 *	IPIPE1
	 *	WE0-3
	 *	OE* output
	 *	CAS2* / CAS3*
	 *	CAS0* / CAS1*
	 *	CS7*
	 *	AVEC*
	 * HARDWARE:
	 *	Change if you are using a different memory configuration
	 *	(static RAM, external address multiplexing, etc).
	 */
	m360.pepar = 0x0080;
	/*
	 * Step 11: Remap Chip Select 0 (CS0*), set up GMR
	 *	no DRAM support
	 * HARDWARE:
	 *	Change if you are using a different memory configuration
	 */
	m360.gmr = M360_GMR_RCNT(23) | M360_GMR_RFEN      | M360_GMR_RCYC(0) |
		   M360_GMR_PGS(6)   | M360_GMR_DPS_32BIT | M360_GMR_DWQ     |
	           M360_GMR_GAMX;

	m360.memc[0].br = (unsigned long)&_RomBase | M360_MEMC_BR_WP |
							M360_MEMC_BR_V;
	m360.memc[0].or = M360_MEMC_OR_WAITS(3) | M360_MEMC_OR_512KB |
							M360_MEMC_OR_8BIT;

	/*
	 * Step 12: Initialize the system RAM
	 *	Set up option/base registers
	 *		16 MB DRAM
	 *		1 wait state
	 * HARDWARE:
	 *	Change if you are using a different memory configuration
	 *      NOTE: no Page mode possible for EDO RAMs (?)
	 */
	ramSize = 16 * 1024 * 1024;
	m360.memc[7].or = M360_MEMC_OR_TCYC(1)  | M360_MEMC_OR_16MB |
			  M360_MEMC_OR_FCMC(0)  | /* M360_MEMC_OR_PGME | */
                          M360_MEMC_OR_32BIT    | M360_MEMC_OR_DRAM;
	m360.memc[7].br = (unsigned long)&RamBase | M360_MEMC_BR_V;

	/*
	 * FIXME: here we should wait for 8 refresh cycles...
	 */
	/*
	 * Step 12a: test the ram, if wanted
	 * FIXME: when do we call this?
	 * -> only during firmware execution
	 * -> perform intesive test only on request
	 * -> ensure, that results are stored properly
	 */
#if 0 /* FIXME: activate RAM tests again */
	{
	  void *ram_base, *ram_end, *code_loc;
	  extern char ramtest_start,ramtest_end;
	  ram_base = &ramtest_start;
	  ram_end  = &ramtest_end;
	  code_loc = (void *)ramtest_exec;
	  if ((ram_base < ram_end) &&
	    !((ram_base <= code_loc) && (code_loc < ram_end))) {
	    ramtest_exec(ram_base,ram_end);
	  }
	}
#endif
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
	 *	Disable SDMA during FREEZE
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

#elif (defined (GEN68360_WITH_SRAM))
   /*
    ***************************************************
    * Generic Standalone Motorola 68360               *
    *           As described in MC68360 User's Manual *
    * But uses SRAM instead of DRAM                   *
    *  CS0* - 512kx8 flash memory                     *
    *  CS1* - 512kx32 static RAM                      *
    *  CS2* - 512kx32 static RAM                      *
    ***************************************************
    */

   /*
    * Step 7: Deal with clock synthesizer
    * HARDWARE:
    * Change if you're not using an external oscillator which
    * oscillates at the system clock rate.
    */
   m360.clkocr = 0x8F;     /* No more writes, no clock outputs */
   m360.pllcr = 0xD000;    /* PLL, no writes, no prescale,
                              no LPSTOP slowdown, PLL X1 */
   m360.cdvcr = 0x8000;    /* No more writes, no clock division */

   /*
    * Step 8: Initialize system protection
    * Enable watchdog
    * Watchdog causes system reset
    * Next-to-slowest watchdog timeout (21 seconds with 25 MHz oscillator)
    * Enable double bus fault monitor
    * Enable bus monitor for external cycles
    * 1024 clocks for external timeout
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
    * SINTOUT not used (CPU32+ mode)
    * CF1MODE=00 (CONFIG1 input)
    * IPIPE1*
    * WE0* - WE3*
    * OE* output
    * CAS2* - CAS3*
    * CAS0* - CAS1*
    * CS7*
    * AVEC*
    * HARDWARE:
    * Change if you are using a different memory configuration
    * (static RAM, external address multiplexing, etc).
    */
   m360.pepar = 0x0080;

   /*
    * Step 11: Set up GMR
    *
    */
   m360.gmr = 0x0;

   /*
    * Step 11a: Remap 512Kx8 flash memory on CS0*
    * 2 wait states
    * Make it read-only for now
    */
   m360.memc[0].br = (unsigned long)&_RomBase | M360_MEMC_BR_WP |
                                                   M360_MEMC_BR_V;
   m360.memc[0].or = M360_MEMC_OR_WAITS(2) | M360_MEMC_OR_512KB |
                                                   M360_MEMC_OR_8BIT;
   /*
    * Step 12: Set up main memory
    * 512Kx32 SRAM on CS1*
    * 512Kx32 SRAM on CS2*
    * 0 wait states
    */
   ramSize = 4 * 1024 * 1024;
   m360.memc[1].br = (unsigned long)&RamBase | M360_MEMC_BR_V;
   m360.memc[1].or = M360_MEMC_OR_WAITS(0) | M360_MEMC_OR_2MB |
                                                   M360_MEMC_OR_32BIT;
   m360.memc[2].br = ((unsigned long)&RamBase + 0x200000) | M360_MEMC_BR_V;
   m360.memc[2].or = M360_MEMC_OR_WAITS(0) | M360_MEMC_OR_2MB |
                                                   M360_MEMC_OR_32BIT;
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
    * Enable SDMA during FREEZE
    * Give SDMA priority over all interrupt handlers
    * Set DMA arbiration level to 4
    * CICR (CPM interrupt configuration register):
    * SCC1 requests at SCCa position
    * SCC2 requests at SCCb position
    * SCC3 requests at SCCc position
    * SCC4 requests at SCCd position
    * Interrupt request level 4
    * Maintain original priority order
    * Vector base 128
    * SCCs priority grouped at top of table
    */
   m360.sdcr = M360_SDMA_SISM_7 | M360_SDMA_SAID_4;
   m360.cicr = (3 << 22) | (2 << 20) | (1 << 18) | (0 << 16) |
                  (4 << 13) | (0x1F << 8) | (128);

   /*
    * Step 15: Set module configuration register
    * Disable timers during FREEZE
    * Enable bus monitor during FREEZE
    * BCLRO* arbitration level 3
    * No show cycles
    * User/supervisor access
    * Bus clear interrupt service level 7
    * SIM60 interrupt sources higher priority than CPM
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
	 *	32-bit DRAM
	 *	Internal DRAM address multiplexing
	 *	60 nsec DRAM
	 *	180 nsec ROM (3 wait states)
	 *	15.36 usec DRAM refresh interval
	 *	The DRAM page size selection is not modified since this
	 *	startup code may be running in a bootstrap PROM or in
	 *	a program downloaded by the bootstrap PROM.
	 */
	m360.gmr = (m360.gmr & 0x001C0000) | M360_GMR_RCNT(23) |
					M360_GMR_RFEN | M360_GMR_RCYC(0) |
					M360_GMR_DPS_32BIT | M360_GMR_NCS |
					M360_GMR_GAMX;
	m360.memc[0].br = (unsigned long)&_RomBase | M360_MEMC_BR_WP |
								M360_MEMC_BR_V;
	m360.memc[0].or = M360_MEMC_OR_WAITS(3) | M360_MEMC_OR_1MB |
							M360_MEMC_OR_8BIT;

	/*
	 * Step 12: Initialize the system RAM
	 * Do this only if the DRAM has not already been set up
	 */
	if ((m360.memc[1].br & M360_MEMC_BR_V) == 0) {
		/*
		 * Set up GMR DRAM page size, option and  base registers
		 *	Assume 16Mbytes of DRAM
		 *	60 nsec DRAM
		 */
		m360.gmr = (m360.gmr & ~0x001C0000) | M360_GMR_PGS(5);
		m360.memc[1].or = M360_MEMC_OR_TCYC(0) |
						M360_MEMC_OR_16MB |
						M360_MEMC_OR_DRAM;
		m360.memc[1].br = (unsigned long)&RamBase | M360_MEMC_BR_V;

		/*
		 * Wait for chips to power up
		 *	Perform 8 read cycles
		 */
		for (i = 0; i < 50000; i++)
			continue;
		for (i = 0; i < 8; ++i)
			*RamBase_p;

		/*
		 * Determine memory size (1, 4, or 16 Mbytes)
		 * Set GMR DRAM page size appropriately.
		 * The OR is left at 16 Mbytes.  The bootstrap PROM places its
		 * .data and .bss segments at the top of the 16 Mbyte space.
		 * A 1 Mbyte or 4 Mbyte DRAM will show up several times in
		 * the memory map, but will work with the same bootstrap PROM.
		 */
		*(volatile char *)&RamBase = 0;
		*((volatile char *)&RamBase+0x00C01800) = 1;
		if (*(volatile char *)&RamBase) {
			m360.gmr = (m360.gmr & ~0x001C0000) | M360_GMR_PGS(1);
		}
		else {
			*((volatile char *)&RamBase+0x00801000) = 1;
			if (*(volatile char *)&RamBase) {
				m360.gmr = (m360.gmr & ~0x001C0000) | M360_GMR_PGS(3);
			}
		}

		/*
		 * Enable parity checking
		 */
		m360.memc[1].br |= M360_MEMC_BR_PAREN;
	}
	switch (m360.gmr & 0x001C0000) {
	default:		ramSize =  4 * 1024 * 1024;	break;
	case M360_GMR_PGS(1):	ramSize =  1 * 1024 * 1024;	break;
	case M360_GMR_PGS(3):	ramSize =  4 * 1024 * 1024;	break;
	case M360_GMR_PGS(5):	ramSize = 16 * 1024 * 1024;	break;
	}

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
	 * Must pass ramSize as argument since the data/bss segment
	 * may be overwritten.
	 */
	_CopyDataClearBSSAndStart (ramSize);
}
