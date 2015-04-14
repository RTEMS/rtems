/**
 * @file
 *
 * Default bsp_reset() for LM32 BSPs which can run on the GDB simulator
 */

/*
 *  COPYRIGHT (c) 1989-2015.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_reset( void )
{
  /*
   * If on the GDB simulator, trap to it and exit.
   */
  #if LM32_ON_SIMULATOR
     __asm__ volatile ( "mvi     r8, 1" );  /* 1 is SYS_exit */
     __asm__ volatile ( "scall" );
  #endif
}
