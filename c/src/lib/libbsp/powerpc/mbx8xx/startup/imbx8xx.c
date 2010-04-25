/*
 *  imbx8xx.c
 *
 *  MBX860/MBX821 initialization routines.
 *
 *  Copyright (c) 1999, National Research Council of Canada
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/mbx.h>

/*
 *  EPPCBug rev 1.1 is stupid. It clears the interrupt mask register
 *  in the SIU when it takes control, but does not restore it before
 *  returning control to the program. We thus keep a copy of the
 *  register, and restore it from gdb using the hook facilities.
 *
 *  We arrange for simask_copy to be initialized to zero so that
 *  it resides in the .data section. This avoids having gdb set
 *  the mask to crud before we get to initialize explicitly. Of
 *  course, the code will not be safely restartable, but then again,
 *  a lot of the library code isn't either, so there!
 */
uint32_t   simask_copy = 0;

/*
 *  The memory controller's UPMA Ram array values.
 *  The values in table 2-6 and 2-7 in the "MBX Series Embedded
 *  Controller Programmer's Reference Guide", part number MBXA/PG2,
 *  differ from the ones in the older MBX Programmer's Guide, part
 *  number MBXA/PG1. We are assuming that the values in MBXA/PG1
 *  are for the older MBX boards whose part number does not have
 *  the "B" suffix, but we have discovered that the values from
 *  MBXA/PG2 work better, even for the older boards.
 *
 *  THESE VALUES HAVE ONLY BEEN VERIFIED FOR THE MBX821-001 and
 *  MBX860-002. USE WITH CARE!
 *
 *  NOTE: The MBXA/PG2 manual lists the clock speed of the MBX821_001B
 *  as being 50 MHz, while the MBXA/IH2.1 manual lists it as 40 MHz.
 *  We think the MBX821_001B is an entry level board and thus is 50 MHz,
 */
static uint32_t   upmaTable[64] = {

#if ( defined(mbx860_001b) || \
        defined(mbx821_001b) || \
        defined(mbx821_001) )

        /* 50 MHz MBX */
        /*
         * Note: For the mbx821_001, the following values (from the
         * MBXA/PG2 manual) work better than, but are different
         * from those published in the original MBXA/PG1 manual and
         * initialized by EPPCBug 1.1. In particular, the original
         * burst-write values do not work! Also, the following values
         * facilitate higher performance.
         */
	/* DRAM 60ns - single read. (offset 0x00 in UPM RAM) */
	0xCFAFC004, 0x0FAFC404, 0x0CAF8C04, 0x10AF0C04,
	0xF0AF0C00, 0xF3BF4805, 0xFFFFC005, 0xFFFFC005,

	/*  DRAM 60ns - burst read. (offset 0x08 in UPM RAM) */
	0xCFAFC004, 0x0FAFC404, 0x0CAF8C04, 0x00AF0C04,
	0x07AF0C08, 0x0CAF0C04, 0x01AF0C04, 0x0FAF0C08,
 	0x0CAF0C04, 0x01AF0C04, 0x0FAF0C08, 0x0CAF0C04,
	0x10AF0C04, 0xF0AFC000, 0xF3BF4805, 0xFFFFC005,

	/*  DRAM 60ns - single write. (offset 0x18 in UPM RAM) */
	0xCFFF0004, 0x0FFF0404, 0x0CFF0C00, 0x13FF4804,
	0xFFFFC004, 0xFFFFC005, 0xFFFFC005, 0xFFFFC005,

	/*  DRAM 60ns - burst write. (offset 0x20 in UPM RAM) */
	0xCFFF0004, 0x0FFF0404, 0x0CFF0C00, 0x03FF0C0C,
	0x0CFF0C00, 0x03FF0C0C, 0x0CFF0C00, 0x03FF0C0C,
	0x0CFF0C00, 0x13FF4804, 0xFFFFC004, 0xFFFFC005,
	0xFFFFC005, 0xFFFFC005, 0xFFFFC005, 0xFFFFC005,

	/*  Refresh 60ns. (offset 0x30 in UPM RAM) */
	0xFCFFC004, 0xC0FFC004, 0x01FFC004, 0x0FFFC004,
	0x1FFFC004, 0xFFFFC004, 0xFFFFC005, 0xFFFFC005,
	0xFFFFC005, 0xFFFFC005, 0xFFFFC005, 0xFFFFC005,

	/*  Exception. (offset 0x3c in UPM RAM) */
	0xFFFFC007, 0xFFFFC007, 0xFFFFC007, 0xFFFFC007

#elif ( defined(mbx860_002b) || \
        defined(mbx860_003b) || \
        defined(mbx860_004b) || \
        defined(mbx860_005b) || \
        defined(mbx860_006b) || \
        defined(mbx821_002b) || \
        defined(mbx821_003b) || \
        defined(mbx821_004b) || \
        defined(mbx821_005b) || \
        defined(mbx821_006b) || \
        defined(mbx860_001) || \
        defined(mbx860_002) || \
        defined(mbx860_003) || \
        defined(mbx860_004) || \
        defined(mbx860_005) || \
        defined(mbx821_002) || \
        defined(mbx821_003) || \
        defined(mbx821_004) || \
        defined(mbx821_005) )

        /* 40 MHz MBX */
        /*
         * Note: For the older MBX models (i.e. without the "b"
         * suffix, e.g. mbx860_001), the following values (from the
         * MBXA/PG2 manual) work better than, but are different
         * from those published in the original MBXA/PG1 manual and
         * initialized by EPPCBug 1.1. In particular, the following
         * burst-read and burst-write values facilitate higher
         * performance.
         */
	/* DRAM 60ns - single read. (offset 0x00 in UPM RAM) */
	0xCFAFC004, 0x0FAFC404, 0x0CAF0C04, 0x30AF0C00,
	0xF1BF4805, 0xFFFFC005, 0xFFFFC005, 0xFFFFC005,

	/*  DRAM 60ns - burst read. (offset 0x08 in UPM RAM) */
	0xCFAFC004, 0x0FAFC404, 0x0CAF0C04, 0x03AF0C08,
	0x0CAF0C04, 0x03AF0C08, 0x0CAF0C04, 0x03AF0C08,
 	0x0CAF0C04, 0x30AF0C00, 0xF3BF4805, 0xFFFFC005,
	0xFFFFC005, 0xFFFFC005, 0xFFFFC005, 0xFFFFC005,

	/*  DRAM 60ns - single write. (offset 0x18 in UPM RAM) */
	0xCFFF0004, 0x0FFF4004, 0x0CFF0C00, 0x33FF4804,
	0xFFFFC005, 0xFFFFC005, 0xFFFFC005, 0xFFFFC005,

	/*  DRAM 60ns - burst write. (offset 0x20 in UPM RAM) */
	0xCFFF0004, 0x0FFF4004, 0x0CFF0C00, 0x03FF0C0C,
	0x0CFF0C00, 0x03FF0C0C, 0x0CFF0C00, 0x03FF0C0C,
	0x0CFF0C00, 0x33FF4804, 0xFFFFC005, 0xFFFFC005,
	0xFFFFC005, 0xFFFFC005, 0xFFFFC005, 0xFFFFC005,

	/*  Refresh 60ns. (offset 0x30 in UPM RAM) */
	0xFCFFC004, 0xC0FFC004, 0x01FFC004, 0x0FFFC004,
	0x3FFFC004, 0xFFFFC005, 0xFFFFC005, 0xFFFFC005,
	0xFFFFC005, 0xFFFFC005, 0xFFFFC005, 0xFFFFC005,

	/*  Exception. (offset 0x3c in UPM RAM) */
	0xFFFFC007, 0xFFFFC007, 0xFFFFC007, 0xFFFFC007
#else
#error "MBX board model not specified."
#endif
};

#if ( !defined(EPPCBUG_VECTORS) )
  extern uint32_t   simask_copy;
#endif

/*
 *  Initialize MBX8xx
 */
void _InitMBX8xx (void)
{
#if ( !defined(EPPCBUG_VECTORS) )
  uint32_t i;
#endif
  register uint32_t   r1;

  /*
   *  Initialize the Debug Enable Register (DER) to an appropriate
   *  value for EPPCBug debugging.
   *  (This value should also work for BDM debugging.)
   */
  r1 = 0x70C67C07;	/* All except EXTIE, ALIE, DECIE */
  _mtspr( M8xx_DER, r1 );

  /*
   * Initialize the Instruction Support Control Register (ICTRL) to a
   * an appropriate value for normal operation. A different value,
   * such as 0x0, may be more appropriate for debugging.
   */
  r1 = 0x00000007;
  _mtspr( M8xx_ICTRL, r1 );

  /*
   * Disable and invalidate the instruction and data caches.
   */
  r1 = M8xx_CACHE_CMD_DISABLE;
  _mtspr( M8xx_IC_CST, r1 );
  _isync;
  r1 = M8xx_CACHE_CMD_UNLOCKALL;
  _mtspr( M8xx_IC_CST, r1 );
  _isync;
  r1 = M8xx_CACHE_CMD_INVALIDATE;	/* invalidate all */
  _mtspr( M8xx_IC_CST, r1 );
  _isync;

  r1 = M8xx_CACHE_CMD_DISABLE;
  _mtspr( M8xx_DC_CST, r1 );
  _isync;
  r1 = M8xx_CACHE_CMD_UNLOCKALL;
  _mtspr( M8xx_DC_CST, r1 );
  _isync;
  r1 = M8xx_CACHE_CMD_INVALIDATE;	/* invalidate all */
  _mtspr( M8xx_DC_CST, r1 );
  _isync;

  /*
   *  Initialize the Internal Memory Map Register (IMMR)
   *
   *  Use the value in MBXA/PG2, which is also the value that EPPC-Bug
   *  programmed into our boards. The alternative is the value in
   *  MBXA/PG1: 0xFFA00000. This value might well depend on the revision
   *  of the firmware.
   *
   *  THIS VALUE IS ALSO DECLARED IN THE linkcmds FILE and mmutlbtab.c!
   */
  r1 = 0xFA200000;
  _mtspr( M8xx_IMMR, r1 );

  /*
   *  Get the SIU interrupt mask.
   *  imd: accessing m8xx.* should not occure before setting up the immr !
   */
   simask_copy = m8xx.simask;

  /*
   * Initialize the SIU Module Configuration Register (SIUMCR)
   * m8xx.siumcr = 0x00602900, the default MBX and firmware value.
   */
  m8xx.siumcr = M8xx_SIUMCR_EARP0 | M8xx_SIUMCR_DBGC3 | M8xx_SIUMCR_DBPC0 |
		M8xx_SIUMCR_DPC | M8xx_SIUMCR_MLRC2 | M8xx_SIUMCR_SEME;

  /*
   * Initialize the System Protection Control Register (SYPCR).
   * The SYPCR can only be written once after Reset.
   *	- Enable bus monitor
   *	- Disable software watchdog timer
   * m8xx.sypcr = 0xFFFFFF88, the default MBX and firmware value.
   */
  m8xx.sypcr = M8xx_SYPCR_SWTC(0xFFFF) | M8xx_SYPCR_BMT(0xFF) |
		M8xx_SYPCR_BME | M8xx_SYPCR_SWF;

  /* Initialize the SIU Interrupt Edge Level Mask Register (SIEL) */
  m8xx.siel = 0xAAAA0000;         	/* Default MBX and firmware value. */

  /* Initialize the Transfer Error Status Register (TESR) */
  m8xx.tesr = 0xFFFFFFFF;         	/* Default firmware value. */

  /* Initialize the SDMA Configuration Register (SDCR) */
  m8xx.sdcr = 0x00000001;         	/* Default firmware value. */

  /*
   * Initialize the Timebase Status and Control Register (TBSCR)
   * m8xx.tbscr = 0x00C3, default MBX and firmware value.
   */
  m8xx.tbscrk = M8xx_UNLOCK_KEY;	/* unlock TBSCR */
  m8xx.tbscr = M8xx_TBSCR_REFA | M8xx_TBSCR_REFB |
  		M8xx_TBSCR_TBF | M8xx_TBSCR_TBE;

  /* Initialize the Real-Time Clock Status and Control Register (RTCSC) */
  m8xx.rtcsk = M8xx_UNLOCK_KEY;		/* unlock RTCSC */
  m8xx.rtcsc = 0x00C3;			/* Default MBX and firmware value. */

  /* Unlock other Real-Time Clock registers */
  m8xx.rtck = M8xx_UNLOCK_KEY;		/* unlock RTC */
  m8xx.rtseck = M8xx_UNLOCK_KEY;	/* unlock RTSEC */
  m8xx.rtcalk = M8xx_UNLOCK_KEY;	/* unlock RTCAL */

  /* Initialize the Periodic Interrupt Status and Control Register (PISCR) */
  m8xx.piscrk = M8xx_UNLOCK_KEY;	/* unlock PISCR */
  m8xx.piscr = 0x0083; 			/* Default MBX and firmware value. */

  /* Initialize the System Clock and Reset Control Register (SCCR)
   * Set the clock sources and division factors:
   *   Timebase Source is GCLK2 / 16
   *   Real-Time Clock Select is EXTCLK (4.192MHz)
   *   Real-Time Clock Divide is /4
   */
  m8xx.sccrk = M8xx_UNLOCK_KEY;		/* unlock SCCR */
  m8xx.sccr = 0x02800000;		/* for MBX860/MBX821 */

#if 0 /* IMD hack: do not init PLL after EPPCbug load */
  /* Initialize the PLL, Low-Power, and Reset Control Register (PLPRCR) */
  /* - set the clock speed and set normal power mode */
  m8xx.plprck = M8xx_UNLOCK_KEY;	/* unlock PLPRCR */
#if ( defined(mbx821_001) || defined(mbx821_001b) || defined(mbx860_001b) )
  m8xx.plprcr = 0x5F500000;
#elif ( defined(mbx860_005b) || \
        defined(mbx860_002b) || \
        defined(mbx860_003b) || \
        defined(mbx860_004b) || \
        defined(mbx860_006b) || \
        defined(mbx821_002b) || \
        defined(mbx821_003b) || \
        defined(mbx821_004b) || \
        defined(mbx821_005b) || \
        defined(mbx821_006b) )
  /* Set the multiplication factor to 0 and clear the timer interrupt status*/
  m8xx.plprcr = 0x00005000;
#elif ( defined(mbx860_001) || \
        defined(mbx860_002) || \
        defined(mbx860_003) || \
        defined(mbx860_004) || \
        defined(mbx860_005) || \
        defined(mbx821_002) || \
        defined(mbx821_003) || \
        defined(mbx821_004) || \
        defined(mbx821_005))
  m8xx.plprcr = 0x4C400000;
#else
#error "MBX board not defined"
#endif
#endif
  /* Unlock the timebase and decrementer registers. */
  m8xx.tbk = M8xx_UNLOCK_KEY;
  /*
   * Initialize decrementer register to a large value to
   * guarantee that a decrementer interrupt will not be
   * generated before the kernel is fully initialized.
   */
  r1 = 0x7FFFFFFF;
  _mtspr( M8xx_DEC, r1 );

  /* Initialize the timebase register (TB is 64 bits) */
  r1 = 0x00000000;
  _mtspr( M8xx_TBU_WR, r1 );
  _mtspr( M8xx_TBL_WR, r1 );

#if 0 /* IMD hack: do not init UPMs after EPPCbug load */
  /*
   * Memory Controller Initialization
   */

  /*
   * User Programmable Machine A (UPMA) Initialization
   *
   * If this initialization code is running from DRAM, it is very
   * dangerous to change the value of any UPMA Ram array word from
   * what the firmware (EPPCBug) initialized it to. Thus we don't
   * initialize UPMA if EPPCBUG_VECTORS is defined; we assume EPPCBug
   * has done the appropriate initialization.
   *
   * An exception to our rule, is that, for the older MBX boards
   * (those without the "B" suffix, e.g. MBX821-001 and MBX860-002),
   * we do re-initialize the burst-read and burst-write values with
   * values that are more efficient. Also, in the MBX821 case,
   * the burst-write original values set by EPPCBug do not work!
   * This change can be done safely because the caches have not yet
   * been activated.
   *
   * The RAM array of UPMA is initialized by writing to each of
   * its 64 32-bit RAM locations.
   * Note: UPM register initialization should occur before
   * initialization of the corresponding BRx and ORx registers.
   */
#if ( !defined(EPPCBUG_VECTORS) )
  for( i = 0; i < 64; ++i ) {
    m8xx.mdr = upmaTable[i];
    m8xx.mcr = M8xx_MEMC_MCR_WRITE | M8xx_MEMC_MCR_UPMA | M8xx_MEMC_MCR_MAD(i);
  }
#elif ( defined(mbx860_001) || \
        defined(mbx860_002) || \
        defined(mbx860_003) || \
        defined(mbx860_004) || \
        defined(mbx860_005) || \
        defined(mbx821_001) || \
        defined(mbx821_002) || \
        defined(mbx821_003) || \
        defined(mbx821_004) || \
        defined(mbx821_005) )
  /* Replace the burst-read and burst-write values with better ones. */
  /* burst-read values */
  for( i = 8; i < 24; ++i ) {
    m8xx.mdr = upmaTable[i];
    m8xx.mcr = M8xx_MEMC_MCR_WRITE | M8xx_MEMC_MCR_UPMA | M8xx_MEMC_MCR_MAD(i);
  }
  /* burst-write values */
  for( i = 32; i < 48; ++i ) {
    m8xx.mdr = upmaTable[i];
    m8xx.mcr = M8xx_MEMC_MCR_WRITE | M8xx_MEMC_MCR_UPMA | M8xx_MEMC_MCR_MAD(i);
  }
#endif

#if ( !defined(EPPCBUG_VECTORS) )
  /*
   *  Initialize the memory periodic timer.
   *	Memory Periodic Timer Prescaler Register (MPTPR: 16-bit register)
   *  m8xx.mptpr = 0x0200;
   */
  m8xx.mptpr = M8xx_MPTPR_PTP(0x2);

  /*
   *  Initialize the Machine A Mode Register (MAMR)
   *
   *  ASSUMES THAT DIMMs ARE NOT INSTALLED!
   *
   *  Without DIMMs:
   *  m8xx.mamr = 0x13821000 (40 MHz) or 0x18821000 (50 MHz).
   *
   *  With DIMMs:
   *  m8xx.mamr = 0x06821000 (40 MHz) or 0x08821000 (50 MHz).
   */
#if ( defined(mbx821_001) || defined(mbx821_001b) || defined(mbx860_001b) )
  m8xx.mamr = M8xx_MEMC_MMR_PTP(0x18) | M8xx_MEMC_MMR_PTE |
	M8xx_MEMC_MMR_DSP(0x1) | M8xx_MEMC_MMR_G0CL(0) | M8xx_MEMC_MMR_UPWAIT;
#else
  m8xx.mamr = M8xx_MEMC_MMR_PTP(0x13) | M8xx_MEMC_MMR_PTE |
	M8xx_MEMC_MMR_DSP(0x1) | M8xx_MEMC_MMR_G0CL(0) | M8xx_MEMC_MMR_UPWAIT;
#endif
#endif /* ! defined(EPPCBUG_VECTORS) */

  /*
   *  Initialize the Base and Option Registers (BR0-BR7 and OR0-OR7)
   *  Note: For all chip selects, ORx should be programmed before BRx,
   *  except when programming the boot chip select (CS0) after hardware
   *  reset, in which case, BR0 should be programmed before OR0.
   *
   *  MPC860/MPX821 Memory Map Summary:
   *	S-ADDR    E-ADDR    CS  PS  PE  WP  MS    BI  V  DESCRIPTION
   *	FE000000  FE7FFFFF  0   32  N   N   GPCM  Y   Y  Soldered FLASH Memory
   *	00000000  00zFFFFF  1   32  N   N   UPMA  N   Y  Local DRAM Memory
   *	00X00000  0XXXXXXX  2    0  N   N   UPMA  N   N  DIMM Memory - Bank #0
   *	00X00000  0XXXXXXX  3    0  N   N   UPMA  N   N  DIMM Memory - Bank #1
   *	FA000000  FA1FFFFF  4    8  N   N   GPCM  Y   Y  NVRAM & BCSR
   *	80000000  DFFFFFFF  5   32  N   N   GPCM  Y   Y  PCI/ISA I/O & Memory
   *	FA210000  FA21FFFF  6   32  N   N   GPCM  Y   Y  QSpan Registers
   *	FC000000  FC7FFFFF  7    8  N   N   GPCM  Y   Y  Socketed FLASH Memory
   *
   *  z = 3 for 4MB installed on the motherboard, z = F for 16M
   *
   *  NOTE: The devices selected by CS0 and CS7 can be selected with jumper J4.
   *  This table assumes that the 32-bit soldered flash device is the boot ROM.
   */

  /*
   *  CS0 : Soldered (32-bit) Flash Memory at 0xFE000000
   *
   *  CHANGE THIS CODE IF YOU CHANGE JUMPER J4 FROM ITS FACTORY DEFAULT SETTING!
   *  (or better yet, don't reprogram BR0 and OR0; just program BR7 and OR7 to
   *  access whatever flash device is not selected during hard reset.)
   *
   *  MBXA/PG2 appears to lie in note 14 for table 2-4. The manual states that
   *  "EPPCBUG configures the reset flash device at the lower address, and the
   *  nonreset flash device at the higher address." If we take reset flash device
   *  to mean the boot flash memory, then the statement must mean that BR0 must
   *  point to the device at the lower address, i.e. 0xFC000000, while BR7 must
   *  point to the device at the highest address, i.e. 0xFE000000.
   *
   *  THIS IS NOT THE CASE!
   *
   *  The boot flash is always configured to start at 0xFE000000, and the other
   *  one to start at 0xFC000000. Changing jumper J4 only changes the width of
   *  the memory ports into these two region.
   *
   * BR0 = 0xFE000001
   *	Base addr [0-16]	0b11111110000000000 = 0xFE000000
   *	Address type [17-19]	0b000
   *	Port size [20-21]	0b00 = 32 bits
   *	Parity enable [22]	0b0 = disabled
   *	Write protect [23]	0b0 = r/w
   *	Machine select [24-25]	0b00 = GPCM
   *	Reserved [26-30]	0b00000
   *	Valid Bit [31]		0b1 = this bank is valid
   * OR0 = 0xFF800930 @ 40 MHz, 0xFF800940 @ 50 MHz
   *	Address mask [0-16]	0b11111111100000000 = 0xFF800000
   *	Addr type mask [17-19]	0b000 = no address-type protection
   *	CS negation time [20]	0b1
   *	ACS [21-22]		0b00 = CS output at same time as address lines
   *	Burst inhibit [23]	0b1 = bank does not support burst accesses
   *	Cycle length [24-27]	0b0011/0b0100 = 3/4 clock cycle wait states
   *	SETA [28]		0b0 = TA generated internally
   *	Timing relaxed [29]	0b0 = not relaxed
   *	Extended hold time [30]	0b0 = not extended
   *	Reserved [31]		0b0
   *
   * m8xx.memc[0]._or = 0xFF800930 (40 MHz)
   * m8xx.memc[0]._or = 0xFF800940 (50 MHz)
   * m8xx.memc[0]._br = 0xFE000001
   */
#if ( defined(mbx821_001) || defined(mbx821_001b) || defined(mbx860_001b) )
  m8xx.memc[0]._or = M8xx_MEMC_OR_8M | M8xx_MEMC_OR_ATM(0) | M8xx_MEMC_OR_CSNT |
		M8xx_MEMC_OR_ACS_NORM | M8xx_MEMC_OR_BI | M8xx_MEMC_OR_SCY(4);
#else
  m8xx.memc[0]._or = M8xx_MEMC_OR_8M | M8xx_MEMC_OR_ATM(0) | M8xx_MEMC_OR_CSNT |
		M8xx_MEMC_OR_ACS_NORM | M8xx_MEMC_OR_BI | M8xx_MEMC_OR_SCY(3);
#endif
  m8xx.memc[0]._br = M8xx_BR_BA(0xFE000000) | M8xx_BR_AT(0) | M8xx_BR_PS32 |
		M8xx_BR_MS_GPCM | M8xx_MEMC_BR_V;

  /*
   * CS1 : Local DRAM Memory at 0x00000000
   * m8xx.memc[1]._or = 0xFFC00400;
   * m8xx.memc[1]._br = 0x00000081;
   */
#if ( defined(mbx860_001b) )
    m8xx.memc[1]._or = M8xx_MEMC_OR_2M | M8xx_MEMC_OR_ATM(0) |
    M8xx_MEMC_OR_ACS_QRTR | M8xx_MEMC_OR_SCY(0);
#elif ( defined(mbx860_002b) || \
         defined(mbx860_003b) || \
         defined(mbx821_001b) || \
         defined(mbx821_002b) || \
         defined(mbx821_003b) || \
         defined(mbx860_001)  || \
         defined(mbx860_002)  || \
         defined(mbx860_003)  || \
         defined(mbx821_001)  || \
         defined(mbx821_002)  || \
         defined(mbx821_003) )
    m8xx.memc[1]._or = M8xx_MEMC_OR_4M | M8xx_MEMC_OR_ATM(0) |
    M8xx_MEMC_OR_ACS_QRTR | M8xx_MEMC_OR_SCY(0);
#elif ( defined(mbx860_004) || \
        defined(mbx860_005) || \
        defined(mbx860_004b) || \
        defined(mbx860_005b) || \
        defined(mbx860_006b) || \
        defined(mbx821_004) || \
        defined(mbx821_005) || \
        defined(mbx821_004b) || \
        defined(mbx821_005b) || \
        defined(mbx821_006b) )
    m8xx.memc[1]._or = M8xx_MEMC_OR_16M | M8xx_MEMC_OR_ATM(0) |
      M8xx_MEMC_OR_ACS_QRTR | M8xx_MEMC_OR_SCY(0);
#else
#error "MBX board not defined"
#endif
  m8xx.memc[1]._br = M8xx_BR_BA(0x00000000) | M8xx_BR_AT(0) | M8xx_BR_PS32 |
		M8xx_BR_MS_UPMA | M8xx_MEMC_BR_V;

  /*
   * CS2 : DIMM Memory - Bank #0, not present
   * m8xx.memc[2]._or = 0x00000400;
   * m8xx.memc[2]._br = 0x00000080;
   */
  m8xx.memc[2]._or = M8xx_MEMC_OR_ATM(0) |
		M8xx_MEMC_OR_ACS_QRTR | M8xx_MEMC_OR_SCY(0);
  m8xx.memc[2]._br = M8xx_BR_AT(0) | M8xx_BR_PS32 |
		M8xx_BR_MS_UPMA;	/* ! M8xx_MEMC_BR_V */

  /*
   * CS3 : DIMM Memory - Bank #1, not present
   * m8xx.memc[3]._or = 0x00000400;
   * m8xx.memc[3]._br = 0x00000080;
   */
  m8xx.memc[3]._or = M8xx_MEMC_OR_ATM(0) |
		M8xx_MEMC_OR_ACS_QRTR | M8xx_MEMC_OR_SCY(0);
  m8xx.memc[3]._br = M8xx_BR_AT(0) | M8xx_BR_PS32 |
		M8xx_BR_MS_UPMA;	/* ! M8xx_MEMC_BR_V */

  /*
   * CS4 : Battery-Backed SRAM at 0xFA000000
   * m8xx.memc[4]._or = 0xFFE00920@ 40 MHz, 0xFFE00930 @ 50 MHz
   * m8xx.memc[4]._br = 0xFA000401;
   */
#if ( defined(mbx821_001) || defined(mbx821_001b) || defined(mbx860_001b) )
  m8xx.memc[4]._or = M8xx_MEMC_OR_2M | M8xx_MEMC_OR_ATM(0) | M8xx_MEMC_OR_CSNT |
		M8xx_MEMC_OR_ACS_NORM | M8xx_MEMC_OR_BI | M8xx_MEMC_OR_SCY(3);
#else
  m8xx.memc[4]._or = M8xx_MEMC_OR_2M | M8xx_MEMC_OR_ATM(0) | M8xx_MEMC_OR_CSNT |
		M8xx_MEMC_OR_ACS_NORM | M8xx_MEMC_OR_BI | M8xx_MEMC_OR_SCY(2);
#endif
  m8xx.memc[4]._br = M8xx_BR_BA(0xFA000000) | M8xx_BR_AT(0) | M8xx_BR_PS8 |
		M8xx_BR_MS_GPCM | M8xx_MEMC_BR_V;

  /*
   * CS5 : PCI I/O and Memory at 0x80000000
   * m8xx.memc[5]._or = 0xA0000108;
   * m8xx.memc[5]._br = 0x80000001;
   */
  m8xx.memc[5]._or = 0xA0000000 | M8xx_MEMC_OR_ATM(0) | M8xx_MEMC_OR_ACS_NORM |
		M8xx_MEMC_OR_BI | M8xx_MEMC_OR_SCY(0) | M8xx_MEMC_OR_SETA;
  m8xx.memc[5]._br = M8xx_BR_BA(0x80000000) | M8xx_BR_AT(0) | M8xx_BR_PS32 |
		M8xx_BR_MS_GPCM | M8xx_MEMC_BR_V;

  /*
   * CS6 : QSPAN Registers at 0xFA210000
   * m8xx.memc[6]._or = 0xFFFF0108;
   * m8xx.memc[6]._br = 0xFA210001;
   */
  m8xx.memc[6]._or = M8xx_MEMC_OR_64K | M8xx_MEMC_OR_ATM(0) | M8xx_MEMC_OR_ACS_NORM |
		M8xx_MEMC_OR_BI | M8xx_MEMC_OR_SCY(0) | M8xx_MEMC_OR_SETA;
  m8xx.memc[6]._br = M8xx_BR_BA(0xFA210000) | M8xx_BR_AT(0) | M8xx_BR_PS32 |
		M8xx_BR_MS_GPCM | M8xx_MEMC_BR_V;

  /*
   * CS7 : Socketed (8-bit) Flash at 0xFC000000
   * m8xx.memc[7]._or = 0xFF800930 @ 40 MHz, 0xFF800940 @ 50 MHz
   * m8xx.memc[7]._br = 0xFC000401;
   */
#if ( defined(mbx821_001) || defined(mbx821_001b) || defined(mbx860_001b) )
  m8xx.memc[7]._or = M8xx_MEMC_OR_8M | M8xx_MEMC_OR_ATM(0) | M8xx_MEMC_OR_CSNT |
		M8xx_MEMC_OR_ACS_NORM | M8xx_MEMC_OR_BI | M8xx_MEMC_OR_SCY(4);
#else
   m8xx.memc[7]._or = M8xx_MEMC_OR_8M | M8xx_MEMC_OR_ATM(0) | M8xx_MEMC_OR_CSNT |
		M8xx_MEMC_OR_ACS_NORM | M8xx_MEMC_OR_BI | M8xx_MEMC_OR_SCY(3);
#endif
  m8xx.memc[7]._br = M8xx_BR_BA(0xFC000000) | M8xx_BR_AT(0) | M8xx_BR_PS8 |
		M8xx_BR_MS_GPCM | M8xx_MEMC_BR_V;
#endif /* IMD hack */
  /*
   * PCMCIA initialization
   */
  /*
   * PCMCIA region 0: common memory
   */
  m8xx.pbr0 = PCMCIA_MEM_ADDR;
  m8xx.por0 = (M8xx_PCMCIA_POR_BSIZE_64MB
	       | M8xx_PCMCIA_POR_PSHT(15) | M8xx_PCMCIA_POR_PSST(15)
	       | M8xx_PCMCIA_POR_PSL(32)
	       | M8xx_PCMCIA_POR_PPS_16   | M8xx_PCMCIA_POR_PRS_MEM
	       |M8xx_PCMCIA_POR_PSLOT_A   |  M8xx_PCMCIA_POR_VALID);
  /*
   * PCMCIA region 1: dma memory
   */
  m8xx.pbr1 = PCMCIA_DMA_ADDR;
  m8xx.por1 = (M8xx_PCMCIA_POR_BSIZE_64MB
	       | M8xx_PCMCIA_POR_PSHT(15) | M8xx_PCMCIA_POR_PSST(15)
	       | M8xx_PCMCIA_POR_PSL(32)
	       | M8xx_PCMCIA_POR_PPS_16   | M8xx_PCMCIA_POR_PRS_DMA
	       |M8xx_PCMCIA_POR_PSLOT_A   |  M8xx_PCMCIA_POR_VALID);
  /*
   * PCMCIA region 2: attribute memory
   */
  m8xx.pbr2 = PCMCIA_ATTRB_ADDR;
  m8xx.por2 = (M8xx_PCMCIA_POR_BSIZE_64MB
	       | M8xx_PCMCIA_POR_PSHT(15) | M8xx_PCMCIA_POR_PSST(15)
	       | M8xx_PCMCIA_POR_PSL(32)
	       | M8xx_PCMCIA_POR_PPS_16   | M8xx_PCMCIA_POR_PRS_ATT
	       |M8xx_PCMCIA_POR_PSLOT_A   |  M8xx_PCMCIA_POR_VALID);
  /*
   * PCMCIA region 3: I/O access
   */
  m8xx.pbr3 = PCMCIA_IO_ADDR;
  m8xx.por3 = (M8xx_PCMCIA_POR_BSIZE_64MB
	       | M8xx_PCMCIA_POR_PSHT(15) | M8xx_PCMCIA_POR_PSST(15)
	       | M8xx_PCMCIA_POR_PSL(32)
	       | M8xx_PCMCIA_POR_PPS_16   | M8xx_PCMCIA_POR_PRS_IO
	       |M8xx_PCMCIA_POR_PSLOT_A   |  M8xx_PCMCIA_POR_VALID);

  /*
   * PCMCIA interface general control reg
   */
  m8xx.pgcra = 0; /* no special options set */
  /*
   * PCMCIA interface enable reg
   */
  m8xx.per   =0; /* no interrupts enabled now */
}
