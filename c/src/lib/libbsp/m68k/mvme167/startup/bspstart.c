/*  bspstart.c
 *
 *  This set of routines starts the application. It includes application,
 *  board, and monitor specific initialization and configuration. The generic
 *  CPU dependent initialization has been performed before any of these are
 *  invoked.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications of respective RTEMS files:
 *  Copyright (c) 1998, National Research Council of Canada
 *
 *  $Id$
 */
 

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <page_table.h>
#include <fatal.h>

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
void bsp_pretasking_hook(void);               /* m68k version */

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
  void M68KFPSPInstallExceptionHandlers (void);
  
  extern m68k_isr_entry  M68Kvec[];
  extern void           *_WorkspaceBase;
  extern void           *_RamSize;
  extern unsigned long   _M68k_Ramsize;

  m68k_isr_entry *rom_monitor_vector_table;
  int index;

  _M68k_Ramsize = (unsigned long)&_RamSize;		/* RAM size set in linker script */
  
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
  
  /* Install the 68040 FPSP here */
  M68KFPSPInstallExceptionHandlers();

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
  /* Must match value in start.s */
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;
  
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
}
