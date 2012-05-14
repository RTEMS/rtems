/*
 *  Motorola MC68xxx Dependent Idle Body Source
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/system.h>
#include <rtems/score/thread.h>


/*PAGE
 *
 *  _CPU_Thread_Idle_body
 *
 *  This kernel routine is the idle thread.  The idle thread runs any time
 *  no other thread is ready to run.  This thread loops forever with
 *  interrupts enabled.
 *
 *  Input parameters:
 *    ignored - this parameter is ignored
 *
 *  Output parameters:  NONE
 */

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{
#if defined(mcf5272)
  for( ; ; ) {
    __asm__ volatile( "nop" );
    __asm__ volatile( "nop" );
  }
#else
  for( ; ; )
    __asm__ volatile( "stop #0x3000":::"cc" );  /* supervisor mode, all interrupts on */
#endif
}
