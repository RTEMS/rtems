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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>

/*
#include <mmu.h>
*/

#include <mpc8260.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/score/thread.h>
#include <rtems/bspIo.h>
#include <libcpu/cpuIdent.h>

#include <string.h>

#ifdef STACK_CHECKER_ON
#include <stackchk.h>
#endif



/*
 *  The original table from the application (in ROM) and our copy of it with
 *  some changes. Configuration is defined in <confdefs.h>. Make sure that
 *  our configuration tables are uninitialized so that they get allocated in
 *  the .bss section (RAM).
 */
extern rtems_configuration_table Configuration;
extern unsigned long intrStackPtr;
rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;


/*
 *  Use the shared implementations of the following routines.
 *  Look in rtems/c/src/lib/libbsp/shared/bsppost.c and
 *  rtems/c/src/lib/libbsp/shared/bsplibc.c.
 */
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );


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






extern void m8260_console_reserve_resources(rtems_configuration_table *);


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


  
#ifdef STACK_CHECKER_ON
  /*
   *  Initialize the stack bounds checker
   *  We can either turn it on here or from the app.
   */
  
  Stack_check_Initialize();
#endif
  
#ifdef RTEMS_DEBUG
  rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}


void bsp_start(void)
{
  extern void *_WorkspaceBase;
  extern int _end;
  rtems_unsigned32  heap_start;
  rtems_unsigned32  ws_start;
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;
  register unsigned char* intrStack;
  register unsigned int intrNestingLevel = 0;


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

  intrStack = (((unsigned char*)&intrStackPtr) - CPU_MINIMUM_STACK_FRAME_SIZE);
  asm volatile ("mtspr	273, %0" : "=r" (intrStack) : "0" (intrStack));
  asm volatile ("mtspr	272, %0" : "=r" (intrNestingLevel) : "0" (intrNestingLevel));

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

  BSP_Configuration.work_space_start = (void *)&_WorkspaceBase;


/*
  BSP_Configuration.microseconds_per_tick  = 1000;
*/

  /*
   *  initialize the CPU table for this BSP
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  if( Cpu_table.interrupt_stack_size < 4*1024 )
    Cpu_table.interrupt_stack_size   = 4 * 1024;

  Cpu_table.clicks_per_usec 	   = 10;  /* for 40MHz extclk */
  Cpu_table.serial_per_sec  	   = 40000000;
  Cpu_table.serial_external_clock  = 0;
  Cpu_table.serial_xon_xoff 	   = 0;
  Cpu_table.serial_cts_rts 	   = 0;
  Cpu_table.serial_rate 	   = 9600;
  Cpu_table.timer_average_overhead = 3;
  Cpu_table.timer_least_valid 	   = 3;
  Cpu_table.clock_speed 	   = 40000000;




#ifdef REV_0_2
  /* set up some board specific registers */
  m8260.siumcr &= 0xF3FFFFFF;		/* set TBEN ** BUG FIX ** */
  m8260.siumcr |= 0x08000000;
#endif

  /* use BRG1 to generate 32kHz timebase */
/*
  m8260.brgc1 = M8260_BRG_EN + (unsigned32)(((unsigned16)((40016384)/(32768)) - 1) << 1) + 0;
*/


  /*
   * Initalize RTEMS IRQ system
   */
  BSP_rtems_irq_mng_init(0);


  /*
   * Call this in case we use TERMIOS for console I/O
   */

  m8xx_uart_reserve_resources(&BSP_Configuration);

/*
  rtems_termios_initialize();
*/
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
  unsigned32 ignored )
{

  for( ; ; )
  {
    asm volatile(
      "mfmsr 3; oris 3,3,4; sync; mtmsr 3; isync; ori 3,3,0; ori 3,3,0"
    );
  }
}

