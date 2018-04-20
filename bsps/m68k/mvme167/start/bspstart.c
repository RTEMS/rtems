/**
 *  @file
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
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Modifications of respective RTEMS files:
 *  Copyright (c) 1998, National Research Council of Canada
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <page_table.h>

void M68KFPSPInstallExceptionHandlers (void);

void bsp_start( void )
{
  void **rom_monitor_vector_table;
  int index;

  /*
   *  167Bug Vectors are at 0xFFE00000
   */
  rom_monitor_vector_table = (void **)0xFFE00000;
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
   */
  page_table_init();
}
