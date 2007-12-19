/*
 *  Process free requests deferred because they were from ISR
 *  or other critical section.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_NEWLIB
#include <stdlib.h>
#include <errno.h>

#include "malloc_p.h"

boolean malloc_is_system_state_OK(void)
{
  if ( _Thread_Dispatch_disable_level > 0 )
    return FALSE;

  if ( _ISR_Nest_level > 0 )
    return FALSE;

  return TRUE;
}

void malloc_process_deferred_frees(void)
{
  Chain_Node  *to_be_freed;

  /*
   *  If some free's have been deferred, then do them now.
   */
  while ((to_be_freed = Chain_Get(&RTEMS_Malloc_GC_list)) != NULL)
    free(to_be_freed);
}

void malloc_defer_free(
  void *pointer
)
{
  Chain_Append(&RTEMS_Malloc_GC_list, (Chain_Node *)pointer);
}
#endif
