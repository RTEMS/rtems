/*
 *  ISR Enable/Disable for SMP Configurations
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/smp.h>

void _ISR_SMP_Initialize(void)
{
}

ISR_Level _ISR_SMP_Disable(void)
{
  ISR_Level level;

  _ISR_Disable_on_this_core( level );
  return level;
}

void _ISR_SMP_Enable(ISR_Level level)
{
  _ISR_Enable_on_this_core( level );
}

void _ISR_SMP_Flash(ISR_Level level)
{
  ISR_Level ignored;

  _ISR_SMP_Enable( level );
  ignored = _ISR_SMP_Disable();
}

int _ISR_SMP_Enter(void)
{
  uint32_t isr_nest_level;
  ISR_Level level;

  _ISR_Disable_on_this_core( level );

  isr_nest_level = _ISR_Nest_level++;

  _Thread_Disable_dispatch();

  return isr_nest_level;
}

/*
 *  Return values:
 *     0 - simple return
 *     1 - dispatching return
 */
int _ISR_SMP_Exit(void)
{
  ISR_Level level;
  int       retval;

  retval = 0;

  _ISR_Disable_on_this_core( level );

  _ISR_Nest_level--;

  if ( _ISR_Nest_level == 0 ) {
    if ( _Thread_Dispatch_necessary ) {
      if ( _Thread_Dispatch_get_disable_level() == 1 ) {
        retval = 1;
      }
    } 
  }

  /*
   *  SPARC has special support to avoid some nasty recursive type behaviour.
   *  When dispatching in a thread and we want to return to it then it needs
   *  to finish.
   */
  #if defined(__sparc__)
    if ( _CPU_ISR_Dispatch_disable )
      retval = 0;
  #endif

  _ISR_Enable_on_this_core( level );

  _Thread_Dispatch_decrement_disable_level();

   if ( retval == 0 )
    _SMP_Request_other_cores_to_dispatch();

  return retval;
}
