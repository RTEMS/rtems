/*
 *  cpuinit.c
 *
 *  TQM8xx initialization routines.
 * derived from MBX8xx BSP
 * adapted to TQM8xx by Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>
 *
 *  Copyright (c) 1999, National Research Council of Canada
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/tqm.h>


/*
 *  Initialize TQM8xx
 */
void _InitTQM8xx (void)
{
  register uint32_t   r1;

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
   * Initialize the SIU Module Configuration Register (SIUMCR)
   * m8xx.siumcr = 0x00602900, the default value.
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
   */
  m8xx.sccrk = M8xx_UNLOCK_KEY;		/* unlock SCCR */
  m8xx.sccr |= 0x02000000;

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
}
/*
 * further initialization (called from bsp_start)
 */
void cpu_init(void)
{
  /* mmu initialization */
  mmu_init();
}
