/*
 *  Routines to Access Internal RTEMS Resources
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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

