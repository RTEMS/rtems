/*
 *  Routines to Access Internal RTEMS Resources
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
 *
 */

#include <rtems.h>

void MY_task_set_note(
  Thread_Control *the_thread,
  unsigned32      notepad,
  unsigned32      note
)
{
  RTEMS_API_Control    *api;
 
  api = the_thread->API_Extensions[ THREAD_API_RTEMS ];

  if ( api )
    api->Notepads[ notepad ] = note;
}


unsigned32 MY_task_get_note(
  Thread_Control *the_thread,
  unsigned32      notepad
)
{
  RTEMS_API_Control    *api;
 
  api = the_thread->API_Extensions[ THREAD_API_RTEMS ];

  return api->Notepads[ notepad ];
}

void *MY_CPU_Context_FP_start(
  void       *base,
  unsigned32  offset
)
{
  return _CPU_Context_Fp_start( base, offset );
}

