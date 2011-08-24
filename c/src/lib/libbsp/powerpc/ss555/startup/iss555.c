/*
 *  iss555.c
 *
 *  Intec SS555 initialization routines.
 *
 *  SS555 port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from c/src/lib/libbsp/powerpc/mbx8xx/startup/imbx8xx.c:
 *
 *  Copyright (c) 1999, National Research Council of Canada
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

SPR_RW(ICTRL);
SPR_RW(PPC_DEC);
SPR_RW(TBWU);
SPR_RW(TBWL);
SPR_RO(IMMR);
SPR_RW(MI_GRA);
SPR_RW(L2U_GRA);
SPR_RW(BBCMCR);

extern char int_ram_top[];		/* top of internal ram */

/*
 *  Initialize SS555
 */
void _InitSS555 (void)
{
  register uint32_t plprcr, msr;

  /*
   * Initialize the System Protection Control Register (SYPCR).
   * The SYPCR can only be written once after Reset.
   */
  usiu.sypcr =
      USIU_SYPCR_SWTC(WATCHDOG_TIMEOUT)	/* set watchdog timeout */
    | USIU_SYPCR_BMT(0xFF) 		/* set bus monitor timeout */
    | USIU_SYPCR_BME 			/* enable bus monitor */
    | USIU_SYPCR_SWF			/* watchdog halted in freeze */
#if WATCHDOG_TIMEOUT != 0xFFFF
    | USIU_SYPCR_SWE			/* enable watchdog */
#endif
    | USIU_SYPCR_SWRI			/* watchdog forces reset */
    | USIU_SYPCR_SWP;			/* prescale watchdog by 2048 */

  TICKLE_WATCHDOG();			/* restart watchdog timer */

  /*
   * Re-tune the PLL to the desired system clock frequency.
   */
  usiu.plprck = USIU_UNLOCK_KEY;	/* unlock PLPRCR */
  usiu.plprcr =
      USIU_PLPRCR_TEXPS			/* assert TEXP always */
    | USIU_PLPRCR_MF(BSP_CLOCK_HZ / BSP_CRYSTAL_HZ);
  					/* PLL multiplication factor */
  usiu.plprck = 0;			/* lock PLPRCR */

  while (((plprcr = usiu.plprcr) & USIU_PLPRCR_SPLS) == 0)
    ;					/* wait for PLL to re-lock */

  /*
   * Enable the timebase and decrementer, then initialize decrementer
   * register to a large value to guarantee that a decrementer interrupt
   * will not be generated before the kernel is fully initialized.
   * Initialize the timebase register to zero.
   */
  usiu.tbscrk = USIU_UNLOCK_KEY;
  usiu.tbscr |= USIU_TBSCR_TBE;		/* enable time base and decrementer */
  usiu.tbscrk = 0;

  usiu.tbk = USIU_UNLOCK_KEY;
  _write_PPC_DEC(0x7FFFFFFF);
  _write_TBWU(0x00000000 );
  _write_TBWL(0x00000000 );
  usiu.tbk = 0;

  /*
   * Run the Inter-Module Bus at full speed.
   */
  imb.uimb.umcr &= ~UIMB_UMCR_HSPEED;

  /*
   * Initialize Memory Controller for External RAM
   *
   * Initialize the Base and Option Registers (BR0-BR7 and OR0-OR7).  Note
   * that for all chip selects, ORx should be programmed before BRx.
   *
   * If booting from internal flash ROM, configure the external RAM to
   * extend the internal RAM.  If booting from external RAM, leave it at
   * zero but set it up appropriately.
   */
  usiu.memc[0]._or =
      USIU_MEMC_OR_512K			/* bank size */
    | USIU_MEMC_OR_SCY(0)		/* wait states in first beat of burst */
    | USIU_MEMC_OR_BSCY(0);		/* wait states in subsequent beats */

  usiu.memc[0]._br =
      USIU_MEMC_BR_BA(_read_IMMR() & IMMR_FLEN
        ? (uint32_t)int_ram_top : 0)	/* base address */
    | USIU_MEMC_BR_PS32			/* 32-bit data bus */
    | USIU_MEMC_BR_TBDIP		/* toggle bdip */
    | USIU_MEMC_BR_V;			/* base register valid */

  /*
   * Initialize Memory Controller for External CPLD
   *
   * The SS555 board includes a CPLD to control on-board features and
   * off-board devices.  (Configuration taken from Intec's hwhook.c)
   */
  usiu.memc[3]._or =
      USIU_MEMC_OR_16M			/* bank size */
    | USIU_MEMC_OR_CSNT			/* negate CS/WE early */
    | USIU_MEMC_OR_ACS_HALF		/* assert CS half cycle after address */
    | USIU_MEMC_OR_SCY(15)		/* wait states in first beat of burst */
    | USIU_MEMC_OR_TRLX;		/* relaxed timing */

  usiu.memc[3]._br =
      USIU_MEMC_BR_BA(&cpld)		/* base address */
    | USIU_MEMC_BR_PS16			/* 16-bit data bus */
    | USIU_MEMC_BR_BI			/* inhibit bursting */
    | USIU_MEMC_BR_V;			/* base register valid */

  /*
   * Disable show cycles and serialization so that burst accesses will work
   * properly.  A different value, such as 0x0, may be more appropriate for
   * debugging, but can be set with the debugger, if needed.
   */
  _write_ICTRL(0x00000007);

  /*
   * Set up Burst Buffer Controller (BBC)
   */
  _write_BBCMCR(
      BBCMCR_ETRE			/* enable exception relocation */
    | BBCMCR_BE);			/* enable burst accesses */
  _isync;

  _CPU_MSR_GET(msr);
  msr |= MSR_IP;		/* set prefix for exception relocation */
  _CPU_MSR_SET(msr);
}
