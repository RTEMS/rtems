/*
 *  RTEMS Malloc Family Internal Header
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/libcsupport.h>
#include <rtems/score/protectedheap.h>
#include <rtems/malloc.h>

#ifdef RTEMS_NEWLIB
#include <sys/reent.h>
#endif

#include <stdint.h>
#include <rtems/chain.h>

/*
 *  Process deferred free operations
 */
bool malloc_is_system_state_OK(void);
void malloc_deferred_frees_process(void);
void malloc_deferred_free(void *);
