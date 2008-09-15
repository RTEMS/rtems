/*  bsp_start()
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  The MPC860 specific stuff was written by Jay Monkman (jmonkman@frasca.com)
 *
 *  Modified for the MPC8260ADS board by Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited, 2001
 *  A 40MHz system clock is assumed.
 *  The PON. RST.CONF. Dip switches (DS1) are
 *  1 - Off
 *  2 - On
 *  3 - Off
 *  4 - On
 *  5 - Off
 *  6 - Off
 *  7 - Off
 *  8 - Off
 *  Dip switches on DS2 and DS3 are all set to ON
 *  The LEDs on the board are used to signal panic and fatal_error
 *  conditions.
 *  The mmu is unused at this time.
 *
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#warning The interrupt disable mask is now stored in SPRG0, please verify that this is compatible to this BSP (see also bootcard.c).

#include <bsp.h>

/*
#include <mmu.h>
*/

#include <mpc8260.h>
#include <rtems/score/thread.h>
#include <rtems/powerpc/powerpc.h>

#include <rtems/bspIo.h>
#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>

#include <string.h>

SPR_RW(SPRG1)

extern unsigned long intrStackPtr;

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clock_speed;
uint32_t   bsp_clicks_per_usec;
uint32_t   bsp_serial_per_sec;	       /* Serial clocks per second */
bool       bsp_serial_external_clock;
bool       bsp_serial_xon_xoff;
bool       bsp_serial_cts_rts;
uint32_t   bsp_serial_rate;
uint32_t   bsp_timer_average_overhead; /* Average overhead of timer in ticks */
uint32_t   bsp_timer_least_valid;      /* Least valid number from timer      */
bool       bsp_timer_internal_clock;   /* TRUE, when timer runs with CPU clk */

void  _BSP_GPLED1_on(void);
void  _BSP_GPLED0_on(void);
void  cpu_init(void);
void  initialize_exceptions(void);

void BSP_panic(char *s)
{
  _BSP_GPLED1_on();
  printk("%s PANIC %s\n",_RTEMS_version, s);
  __asm__ __volatile ("sc");
}

void _BSP_Fatal_error(unsigned int v)
{
  _BSP_GPLED0_on();
  _BSP_GPLED1_on();
  printk("%s PANIC ERROR %x\n",_RTEMS_version, v);
  __asm__ __volatile ("sc");
}

void _BSP_GPLED0_on()
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr0 &=  ~GP0_LED;		/* Turn on GP0 LED */
}

void _BSP_GPLED0_off()
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr0 |=  GP0_LED;		/* Turn off GP0 LED */
}

void _BSP_GPLED1_on()
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr0 &=  ~GP1_LED;		/* Turn on GP1 LED */
}

void _BSP_GPLED1_off()
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr0 |=  GP1_LED;		/* Turn off GP1 LED */
}

void _BSP_Uart1_enable()
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr1 &= ~UART1_E;		/* Enable Uart1 */
}

void _BSP_Uart1_disable()
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr1 |=  UART1_E;		/* Disable Uart1 */
}

void _BSP_Uart2_enable()
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr1 &= ~UART2_E;		/* Enable Uart2 */
}

void _BSP_Uart2_disable()
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr1 |=  UART2_E;		/* Disable Uart2 */

}

void bsp_start(void)
{
  extern void *_WorkspaceBase;
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;
  register unsigned char* intrStack;

  /* Set MPC8260ADS board LEDS and Uart enable lines */
  _BSP_GPLED0_off();
  _BSP_GPLED1_off();
  _BSP_Uart1_enable();
  _BSP_Uart2_enable();

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  myCpu 	= get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  cpu_init();

/*
  mmu_init();
*/
  /*
   * Initialize some SPRG registers related to irq handling
   */

  intrStack = (((unsigned char*)&intrStackPtr) - PPC_MINIMUM_STACK_FRAME_SIZE);
  _write_SPRG1((unsigned int)intrStack);

/*
  printk( "About to call initialize_exceptions\n" );
*/
   /*
    * Install our own set of exception vectors
    */

   initialize_exceptions();

/*
  mmu_init();
*/

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
   *  initialize the device driver parameters
   */
  bsp_clicks_per_usec 	   = 10;  /* for 40MHz extclk */
  bsp_serial_per_sec  	   = 40000000;
  bsp_serial_external_clock  = 0;
  bsp_serial_xon_xoff 	   = 0;
  bsp_serial_cts_rts 	   = 0;
  bsp_serial_rate 	   = 9600;
  bsp_timer_average_overhead = 3;
  bsp_timer_least_valid 	   = 3;
  bsp_clock_speed 	   = 40000000;

#ifdef REV_0_2
  /* set up some board specific registers */
  m8260.siumcr &= 0xF3FFFFFF;		/* set TBEN ** BUG FIX ** */
  m8260.siumcr |= 0x08000000;
#endif

  /* use BRG1 to generate 32kHz timebase */
/*
  m8260.brgc1 = M8260_BRG_EN + (uint32_t)(((uint16_t)((40016384)/(32768)) - 1) << 1) + 0;
*/

  /*
   * Initalize RTEMS IRQ system
   */
  BSP_rtems_irq_mng_init(0);

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif

}

/*
 *
 *  _Thread_Idle_body
 *
 *  Replaces the one in c/src/exec/score/src/threadidlebody.c
 *  The MSR[POW] bit is set to put the CPU into the low power mode
 *  defined in HID0.  HID0 is set during starup in start.S.
 *
 */
Thread _Thread_Idle_body(
  uint32_t   ignored )
{

  for( ; ; )
  {
    asm volatile(
      "mfmsr 3; oris 3,3,4; sync; mtmsr 3; isync; ori 3,3,0; ori 3,3,0"
    );
  }

  return 0; /* to remove warning */
}
