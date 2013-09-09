/**
 *  @file
 *
 *  @brief Malloc Deferred Support
 *  @ingroup libcsupport
 */

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
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_NEWLIB
#include <stdlib.h>
#include <errno.h>

#include "malloc_p.h"

#include <rtems/score/threaddispatch.h>

RTEMS_CHAIN_DEFINE_EMPTY(RTEMS_Malloc_GC_list);

bool malloc_is_system_state_OK(void)
{
  if ( !_Thread_Dispatch_is_enabled() )
    return false;

  return true;
}

void malloc_deferred_frees_process(void)
{
  rtems_chain_node  *to_be_freed;

  /*
   *  If some free's have been deferred, then do them now.
   */
  while ((to_be_freed = rtems_chain_get(&RTEMS_Malloc_GC_list)) != NULL)
    free(to_be_freed);
}

void malloc_deferred_free(
  void *pointer
)
{
  rtems_chain_append(&RTEMS_Malloc_GC_list, (rtems_chain_node *)pointer);
}
#endif
