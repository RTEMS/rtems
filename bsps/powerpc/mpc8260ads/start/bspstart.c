/*
 *  This routine does the bulk of the system initialization.
 */

/*
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
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

/*
#include <mmu.h>
*/

#include <mpc8260.h>
#include <rtems/score/thread.h>
#include <rtems/powerpc/powerpc.h>

#include <rtems/bspIo.h>
#include <rtems/counter.h>
#include <bsp/irq.h>
#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>

#include <string.h>

#define UART1_E	0x02000002		/* bit 6 of BCSR1 */
#define UART2_E	0x01000001		/* bit 7 of BCSR1 */

#define GP0_LED 0x02000002    /*  bit 6 of BCSR0 */
#define GP1_LED 0x01000001    /*  bit 7 of BCSR0 */

SPR_RW(SPRG1)

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clock_speed;
uint32_t   bsp_time_base_frequency;
uint32_t   bsp_clicks_per_usec;
uint32_t   bsp_serial_per_sec;	       /* Serial clocks per second */
bool       bsp_serial_external_clock;
bool       bsp_serial_xon_xoff;
bool       bsp_serial_cts_rts;
uint32_t   bsp_serial_rate;

/* leave in case needed in future */
#if 0
static void _BSP_GPLED0_on(void)
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr0 &=  ~GP0_LED;		/* Turn on GP0 LED */
}
#endif

static void _BSP_GPLED0_off(void)
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr0 |=  GP0_LED;		/* Turn off GP0 LED */
}

/* leave in case needed in future */
#if 0
static void _BSP_GPLED1_on(void)
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr0 &=  ~GP1_LED;		/* Turn on GP1 LED */
}
#endif

static void _BSP_GPLED1_off(void)
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr0 |=  GP1_LED;		/* Turn off GP1 LED */
}

static void _BSP_Uart1_enable(void)
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr1 &= ~UART1_E;		/* Enable Uart1 */
}

static void _BSP_Uart2_enable(void)
{
  BCSR *csr;
  csr = (BCSR *)(m8260.memc[1].br & 0xFFFF8000);
  csr->bcsr1 &= ~UART2_E;		/* Enable Uart2 */
}

uint32_t _CPU_Counter_frequency(void)
{
  return bsp_clock_speed;
}

void bsp_start(void)
{
  /* Set MPC8260ADS board LEDS and Uart enable lines */
  _BSP_GPLED0_off();
  _BSP_GPLED1_off();
  _BSP_Uart1_enable();
  _BSP_Uart2_enable();

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function stores the result in global variables so that it can be used
   * later...
   */
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  cpu_init();

/*
  mmu_init();
*/

  ppc_exc_initialize();
  bsp_interrupt_initialize();

/*
  mmu_init();
*/

  /*
   * Enable instruction and data caches. Do not force writethrough mode.
   */
#if BSP_INSTRUCTION_CACHE_ENABLED
  rtems_cache_enable_instruction();
#endif
#if BSP_DATA_CACHE_ENABLED
  rtems_cache_enable_data();
#endif

  /*
   *  initialize the device driver parameters
   */
  bsp_time_base_frequency    = 10000000;
  bsp_clicks_per_usec 	     = 10;  /* for 40MHz extclk */
  bsp_serial_per_sec  	     = 40000000;
  bsp_serial_external_clock  = 0;
  bsp_serial_xon_xoff 	     = 0;
  bsp_serial_cts_rts 	     = 0;
  bsp_serial_rate 	     = 9600;
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

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif

}
