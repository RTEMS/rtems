/*
 * This file contains the ARM BSP startup package. It includes application,
 * board, and monitor specific initialization and configuration. The generic CPU
 * dependent initialization has been performed before this routine is invoked.
 */

/*
 *  Copyright (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <rtems/bspIo.h>
#include <s3c24xx.h>

/*
 *  BSP Specific Initialization in C
 */
static void bsp_start_default( void )
{
  uint32_t cr;
  uint32_t pend,last;
  uint32_t REFCNT;
  int i;

  /* stop RTC */
  #ifdef CPU_S3C2400
    rTICINT = 0x0;
  #else
    rTICNT = 0x0;
  #endif
  /* stop watchdog,ADC and timers */
  rWTCON = 0x0;
  rTCON = 0x0;
  rADCCON = 0x0;

  /* disable interrupts */
  rINTMOD = 0x0;
  rINTMSK = BIT_ALLMSK; /* unmasked by drivers */

  last = 0;
  for(i=0; i<4; i++) {
    pend = rSRCPND;
    if(pend == 0 || pend == last)
      break;
    rSRCPND = pend;
    rINTPND = pend;
    last    = pend;
  }

  /* setup clocks */
  rCLKDIVN = M_CLKDIVN;
  rMPLLCON = ((M_MDIV<<12)+(M_PDIV<<4)+M_SDIV);
  /* setup rREFRESH
   * period = 15.6 us, HCLK=66Mhz, (2048+1-15.6*66)
   */
  REFCNT   = 2048+1-(15.6*get_HCLK()/1000000);
  rREFRESH = ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Trc<<18)+(Tchr<<16)+REFCNT);

  /* set prescaler for timers 2,3,4 to 16(15+1) */
  cr = rTCFG0 & 0xFFFF00FF;
  rTCFG0 = (cr | (15<<8));

  /* set prescaler for timers 0,1 to 1(0+1) */
  cr = rTCFG0 & 0xFFFFFF00;
  rTCFG0 = (cr | (0<<0));

  /*
   * Init rtems interrupt management
   */
  bsp_interrupt_initialize();
}

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */
void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
