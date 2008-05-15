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
 *
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/bspIo.h>
#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>
#include <rtems/powerpc/powerpc.h>

SPR_RW(SPRG0)
SPR_RW(SPRG1)

extern unsigned long intrStackPtr;

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clicks_per_usec;
uint32_t   bsp_clock_speed;
uint32_t   bsp_serial_per_sec;	       /* Serial clocks per second */
boolean    bsp_serial_external_clock;
boolean    bsp_serial_xon_xoff;
boolean    bsp_serial_cts_rts;
uint32_t   bsp_serial_rate;
uint32_t   bsp_timer_average_overhead; /* Average overhead of timer in ticks */
uint32_t   bsp_timer_least_valid;      /* Least valid number from timer      */
boolean    bsp_timer_internal_clock;   /* TRUE, when timer runs with CPU clk */

/*
 *  Use the shared implementations of the following routines.
 *  Look in rtems/c/src/lib/libbsp/shared/bsppost.c and
 *  rtems/c/src/lib/libbsp/shared/bsplibc.c.
 */
void bsp_libc_init( void *, uint32_t, int );

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
 *  bsp_pretasking_hook
 *
 *  Called when RTEMS initialization is complete but before interrupts and
 *  tasking are enabled. Used to setup libc and install any BSP extensions.
 *
 *  Must not use libc (to do io) from here, since drivers are not yet
 *  initialized.
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
void bsp_pretasking_hook(void)
{
  /*
   *  These are assigned addresses in the linkcmds file for the BSP. This
   *  approach is better than having these defined as manifest constants and
   *  compiled into the kernel, but it is still not ideal when dealing with
   *  multiprocessor configuration in which each board as a different memory
   *  map. A better place for defining these symbols might be the makefiles.
   *  Consideration should also be given to developing an approach in which
   *  the kernel and the application can be linked and burned into ROM
   *  independently of each other.
   */
  extern unsigned char _HeapStart;
  extern unsigned char _HeapEnd;

  bsp_libc_init( &_HeapStart, &_HeapEnd - &_HeapStart, 0 );
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
  extern void *_WorkspaceBase;

  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;
  register unsigned char* intrStack;

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
#ifdef INSTRUCTION_CACHE_ENABLE
  rtems_cache_enable_instruction();
#endif
#ifdef DATA_CACHE_ENABLE
  rtems_cache_enable_data();
#endif
#endif
  /*
   * Initialize some SPRG registers related to irq handling
   */

  intrStack = (((unsigned char*)&intrStackPtr) - PPC_MINIMUM_STACK_FRAME_SIZE);
  _write_SPRG1((unsigned int)intrStack);
  /* signal them that we have fixed PR288 - eventually, this should go away */
  _write_SPRG0(PPC_BSP_HAS_FIXED_PR288);

  /*
   * Install our own set of exception vectors
   */
  initialize_exceptions();

  /*
   *  Allocate the memory for the RTEMS Work Space.  This can come from
   *  a variety of places: hard coded address, malloc'ed from outside
   *  RTEMS world (e.g. simulator or primitive memory manager), or (as
   *  typically done by stock BSPs) by subtracting the required amount
   *  of work space from the last physical address on the CPU board.
   *
   *  In this case, the memory is not malloc'ed.  It is just
   *  "pulled from the air".
   */
  Configuration.work_space_start = (void *)&_WorkspaceBase;

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
  bsp_serial_external_clock = 1;
  bsp_serial_xon_xoff = 0;
  bsp_serial_cts_rts = 1;
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
  /*
   * Initalize RTEMS IRQ system
   */
  BSP_rtems_irq_mng_init(0);
#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif

}
