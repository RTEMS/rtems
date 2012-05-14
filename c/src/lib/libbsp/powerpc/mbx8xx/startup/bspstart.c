/*  bspstart.c
 *
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications for MBX860:
 *  Copyright (c) 1999, National Research Council of Canada
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>
#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>
#include <rtems/powerpc/powerpc.h>

SPR_RW(SPRG1)

int bsp_interrupt_initialize(void);

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clicks_per_usec;
uint32_t   bsp_clock_speed;
uint32_t   bsp_serial_per_sec;	       /* Serial clocks per second */
bool       bsp_serial_external_clock;
bool       bsp_serial_xon_xoff;
bool       bsp_serial_cts_rts;
uint32_t   bsp_serial_rate;
uint32_t   bsp_timer_average_overhead; /* Average overhead of timer in ticks */
uint32_t   bsp_timer_least_valid;      /* Least valid number from timer      */
bool       bsp_timer_internal_clock;   /* TRUE, when timer runs with CPU clk */

extern char IntrStack_start [];
extern char intrStack [];

void BSP_panic(char *s)
{
  printk("%s PANIC %s\n",_RTEMS_version, s);
  __asm__ __volatile ("sc");
}

void _BSP_Fatal_error(unsigned int v)
{
  printk("%s PANIC ERROR %x\n",_RTEMS_version, v);
  __asm__ __volatile ("sc");
}

/*
 *  bsp_start()
 *
 *  Board-specific initialization code. Called from the generic boot_card()
 *  function defined in rtems/c/src/lib/libbsp/shared/main.c. That function
 *  does some of the board independent initialization. It is called from the
 *  MBX8xx entry point _start() defined in
 *  rtems/c/src/lib/libbsp/powerpc/mbx8xx/startup/start.S
 *
 *  _start() has set up a stack, has zeroed the .bss section, has turned off
 *  interrupts, and placed the processor in the supervisor mode. boot_card()
 *  has left the processor in that state when bsp_start() was called.
 *
 *  RUNS WITH ADDRESS TRANSLATION AND CACHING TURNED OFF!
 *  ASSUMES THAT THE VIRTUAL ADDRESSES WILL BE IDENTICAL TO THE PHYSICAL
 *  ADDRESSES. Software-controlled address translation would be required
 *  otherwise.
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
void bsp_start(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  myCpu 	= get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  mmu_init();

  /*
   * Enable instruction and data caches. Do not force writethrough mode.
   */
#if NVRAM_CONFIGURE == 1
  if ( nvram->cache_mode & 0x02 )
    rtems_cache_enable_instruction();
  if ( nvram->cache_mode & 0x01 )
    rtems_cache_enable_data();
#else
#if BSP_INSTRUCTION_CACHE_ENABLED
  rtems_cache_enable_instruction();
#endif
#if BSP_DATA_CACHE_ENABLED
  rtems_cache_enable_data();
#endif
#endif

  /* Initialize exception handler */
  sc = ppc_exc_initialize(
    PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
    (uintptr_t) IntrStack_start,
    (uintptr_t) intrStack - (uintptr_t) IntrStack_start
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    BSP_panic( "cannot initialize exceptions" );
  }

  /* Initalize interrupt support */
  sc = bsp_interrupt_initialize();
  if ( sc != RTEMS_SUCCESSFUL ) {
    BSP_panic( "cannot initialize interrupts" );
  }

  /*
   *  initialize the device driver parameters
   */

#if    ( defined(mbx860_001b) || \
         defined(mbx860_002b) || \
         defined(mbx860_003b) || \
         defined(mbx860_003b) || \
         defined(mbx860_004b) || \
         defined(mbx860_005b) || \
         defined(mbx860_006b) || \
         defined(mbx821_001b) || \
         defined(mbx821_002b) || \
         defined(mbx821_003b) || \
         defined(mbx821_004b) || \
         defined(mbx821_005b) || \
         defined(mbx821_006b))
  bsp_clicks_per_usec = 0;  /* for 32768Hz extclk */
#else
  bsp_clicks_per_usec = 1;  /* for 4MHz extclk */
#endif

  bsp_serial_per_sec = 10000000;
  bsp_serial_external_clock = true;
  bsp_serial_xon_xoff = false;
  bsp_serial_cts_rts = true;
  bsp_serial_rate = 9600;
#if ( defined(mbx821_001) || defined(mbx821_001b) || defined(mbx860_001b) )
  bsp_clock_speed = 50000000;
  bsp_timer_average_overhead = 3;
  bsp_timer_least_valid = 3;
#else
  bsp_clock_speed = 40000000;
  bsp_timer_average_overhead = 3;
  bsp_timer_least_valid = 3;
#endif

  m8xx.scc2.sccm=0;
  m8xx.scc2p.rbase=0;
  m8xx.scc2p.tbase=0;
  m8xx_cp_execute_cmd( M8xx_CR_OP_STOP_TX | M8xx_CR_CHAN_SCC2 );

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif

}
