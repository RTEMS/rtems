/*  bspstart.c
 *
 *  This set of routines starts the application. It includes application,
 *  board, and monitor specific initialization and configuration. The generic
 *  CPU dependent initialization has been performed before any of these are
 *  invoked.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Modifications of respective RTEMS files:
 *  Copyright (c) 1998, National Research Council of Canada
 *
 *  $Id$
 */
 

#include <bsp.h>
#include <page_table.h>
#include <fatal.h>
#include <rtems/libio.h>
 
#include <libcsupport.h>
 
#include <string.h>
 

/*
 *  The original table from the application (in ROM) and our copy of it with
 *  some changes. Configuration is defined in <confdefs.h>. Make sure that
 *  our configuration tables are uninitialized so that they get allocated in
 *  the .bss section (RAM).
 */
extern rtems_configuration_table Configuration;
rtems_configuration_table BSP_Configuration;
rtems_extensions_table user_extension_table;

rtems_cpu_table Cpu_table;

/*
 *  Use the shared implementations of the following routines.
 *  Look in rtems/c/src/lib/libbsp/shared/bsppost.c and
 *  rtems/c/src/lib/libbsp/shared/bsplibc.c.
 */
void bsp_postdriver_hook( void );
void bsp_libc_init( void *, unsigned32, int );


/*
 *  bsp_pretasking_hook
 *
 *  Called when RTEMS initialization is complete but before interrupts and
 *  tasking are enabled. Used to setup libc and install any BSP extensions.
 *
 *  Must not use libc (to do io) from here, since drivers are not yet
 *  initialized.
 *
 *  Installed in the rtems_cpu_table defined in 
 *  rtems/c/src/exec/score/cpu/m68k/cpu.h in main() below. Called from
 *  rtems_initialize_executive() defined in rtems/c/src/exec/sapi/src/init.c
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
void bsp_pretasking_hook( void )
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
  extern unsigned char _HeapStart, _HeapEnd;

  bsp_libc_init(&_HeapStart, &_HeapEnd - &_HeapStart, 0);

#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}


/*
 *  bsp_start()
 *
 *  Board-specific initialization code. Called from the generic boot_card()
 *  function defined in rtems/c/src/lib/libbsp/shared/main.c. That function
 *  does some of the board independent initialization. It is called from the
 *  generic MC680x0 entry point _start() defined in
 *  rtems/c/src/lib/start/m68k/start.s
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
 *  ASSUMES THAT 167BUG IS PRESENT TO CATCH ANY EXCEPTIONS DURING
 *  INITIALIZATION.
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
void bsp_start( void )
{
  extern void *_WorkspaceBase;
  extern m68k_isr_entry M68Kvec[];
  
  m68k_isr_entry *rom_monitor_vector_table;
  int index;

  /*
   *  167Bug Vectors are at 0xFFE00000
   */
  rom_monitor_vector_table = (m68k_isr_entry *)0xFFE00000;
  m68k_set_vbr( rom_monitor_vector_table );


  /* 
   *  Copy 167Bug Bus Error handler into our exception vector. All 167Bug
   *  exception vectors are the same and point to the generalized exception
   *  handler. The bus error handler is the one that Motorola says to copy
   *  (p. 2-13, Debugging Package for Motorola 68K CISC CPUs User's Manual
   *  68KBUG/D1A3, October 1993).
   */
  for ( index=2 ; index<=255 ; index++ )
    M68Kvec[ index ] = rom_monitor_vector_table[ 2 ];

  /* Any exceptions during initialization should be trapped by 167Bug */
  m68k_set_vbr( &M68Kvec );

  /*
   *  You may wish to make the VME arbitration round-robin here, currently
   *  we leave it as it is.
   */

  /* Set the Interrupt Base Vectors */
  lcsr->vector_base = (VBR0 << 28) | (VBR1 << 24);

  /* 
   *  Initialize address translation
   *  May need to pass the multiprocessor configuration table.
   */
  page_table_init( &Configuration );

  /* We only use a hook to get the C library initialized. */
  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.interrupt_vector_table = (m68k_isr_entry *) &M68Kvec;
  Cpu_table.interrupt_stack_size = 4096; /* Must match value in start.s */
  
  /* 
   *  If the application has not overriden the default User_extension_table,
   *  supply one with our own fatal error handler that returns control to
   *  167Bug.
   */
  if ( BSP_Configuration.User_extension_table == NULL ) {
    user_extension_table.fatal = bsp_fatal_error_occurred;
    BSP_Configuration.User_extension_table = &user_extension_table;
  }
  
  /*
   *  Need to "allocate" the memory for the RTEMS Workspace and
   *  tell the RTEMS configuration where it is.  This memory is
   *  not malloc'ed.  It is just "pulled from the air".
   */
  BSP_Configuration.work_space_start = (void *)&_WorkspaceBase;

  /*
   *  Increase the number of semaphores that can be created on this node. The
   *  termios package requires one semaphore to protect the list of termios-
   *  capable terminals, and up to four semaphores per termios-capable
   *  terminal (add calls here as required). The maximum number of semaphores
   *  must be set before returning to boot_card(), which will call
   *  rtems_initialize_executive_early(). This latter function eventually
   *  calls _RTEMS_API_Initialize(), which in turn calls
   *  _Semaphore_Manager_initialization(), which allocates the space for the
   *  maximum number of semaphores in the object table. These calls occur
   *  before the call to the predriver hook and the calls to the device
   *  initialization callbacks. Hence, we must do this here.
   */
  console_reserve_resources( &BSP_Configuration );
}
