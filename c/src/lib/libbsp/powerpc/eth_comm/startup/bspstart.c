/*  bsp_start()
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  The MPC860 specific stuff was written by Jay Monkman (jmonkman@frasca.com)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <info.h>
#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>
#include <rtems/bspIo.h>

boardinfo_t M860_binfo;

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */
extern rtems_configuration_table Configuration;
extern unsigned long intrStackPtr;
rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;

/*
 *  Use the shared implementations of the following routines
 */
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

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
 *  Function:   bsp_pretasking_hook
 *  Created:    95/03/10
 *
 *  Description:
 *      BSP pretasking hook.  Called just before drivers are initialized.
 *      Used to setup libc and install any BSP extensions.
 *
 *  NOTES:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 *
 */
 
void
bsp_pretasking_hook(void)
{
  extern int       _end;
  rtems_unsigned32  heap_start;

  /* 
   * Let's check to see if the size of M860_binfo is what
   * it should be. It might not be if the info.h files
   * for RTEMS and the bootloader define boardinfo_t
   * differently.
   */

  /* Oops. printf() won't work yet, since the console is not initialized.
     I should probably find some way of doing this though.
  if (M860_binfo.size != sizeof(boardinfo_t)) {
      printf("The size of the Board Info Block appears to be incorrect.\n");
      printf(" This could occur if the 'info.h' files for RTEMS and the\n");
      printf(" bootloader differ in their definition of boardinfo_t\n");
  }
  */
  heap_start = (rtems_unsigned32) &_end;

  /* Align the heap on a natural boundary (4 bytes?) */
  if (heap_start & (CPU_ALIGNMENT-1)) {
    heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);
  }
  /* set up a 256K heap */
  bsp_libc_init((void *) heap_start, 256 * 1024, 0);
  
#ifdef RTEMS_DEBUG
  rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}

SPR_RW(SPRG0)
SPR_RW(SPRG1)

void bsp_start(void)
{
  extern int _end;
  rtems_unsigned32  heap_start;
  rtems_unsigned32  ws_start;
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;
  register unsigned char* intrStack;
  extern void cpu_init(void);
   
  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  myCpu 	= get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  cpu_init();
  mmu_init();
  /*
   * Initialize some SPRG registers related to irq handling
   */
   
  intrStack = (((unsigned char*)&intrStackPtr) - CPU_MINIMUM_STACK_FRAME_SIZE);

  _write_SPRG1((unsigned int)intrStack);

  /* Signal them that this BSP has fixed PR288 - eventually, this should go away */
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
   */

  /*
   *  Need to "allocate" the memory for the RTEMS Workspace and
   *  tell the RTEMS configuration where it is.  This memory is
   *  not malloc'ed.  It is just "pulled from the air".
   */

  heap_start = (rtems_unsigned32) &_end;
  if (heap_start & (CPU_ALIGNMENT-1))
    heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);


  ws_start = heap_start + (256 * 1024);
  if (ws_start & ((512 * 1024) - 1)) {  /* align to 512K boundary */
    ws_start = (ws_start + (512 * 1024)) & ~((512 * 1024) - 1);
  }

  BSP_Configuration.work_space_start = (void *)ws_start;
  BSP_Configuration.work_space_size = 512 * 1024; 

  /*
   *  initialize the CPU table for this BSP
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

  Cpu_table.clicks_per_usec = 1;  /* for 4MHz extclk */
  Cpu_table.serial_per_sec = 10000000;
  Cpu_table.serial_external_clock = 1;
  Cpu_table.serial_xon_xoff = 0;
  Cpu_table.serial_cts_rts = 1;
  Cpu_table.serial_rate = 9600;
  Cpu_table.timer_average_overhead = 0;
  Cpu_table.timer_least_valid = 0;
  Cpu_table.clock_speed = 40000000;

  /*
   * Since we are currently autodetecting whether to use SCC1 or
   * the FEC for ethernet, we set up a register in the ethernet
   * transciever that is used for 10/100 Mbps ethernet now, so that 
   * we can attempt to read it later in rtems_enet_driver_attach()
  */
  m8xx.fec.mii_speed = 0x0a;
  m8xx.fec.mii_data = 0x680a0000;


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


