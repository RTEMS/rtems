/*
 *  Intel i386 Dependent Source
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 */


void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
  register unsigned16  fp_status asm ("ax");
  register void       *fp_context;

  _CPU_Table = *cpu_table;

  /*
   *  The following code saves a NULL i387 context which is given
   *  to each task at start and restart time.  The following code
   *  is based upon that provided in the i386 Programmer's
   *  Manual and should work on any coprocessor greater than
   *  the i80287.
   *
   *  NOTE: The NO RTEMS_WAIT form of the coprocessor instructions
   *        MUST be used in case there is not a coprocessor
   *        to wait for.
   */

  fp_status = 0xa5a5;
  asm volatile( "fninit" );
  asm volatile( "fnstsw %0" : "=a" (fp_status) : "0" (fp_status) );

  if ( fp_status ==  0 ) {

    fp_context = &_CPU_Null_fp_context;

    asm volatile( "fsave (%0)" : "=r" (fp_context)
                               : "0"  (fp_context)
                );
  }
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  unsigned32 level;
 
  i386_get_interrupt_level( level );
 
  return level;
}

void _CPU_Thread_Idle_body ()
{
  while(1){
    asm volatile ("hlt");
  }
}
     
