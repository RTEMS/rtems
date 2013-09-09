/*
 *  RTEMS Malloc Family Internal Header
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
 *  Malloc Statistics Structure
 */
extern rtems_malloc_statistics_t rtems_malloc_statistics;

#define MSBUMP(_f,_n)    rtems_malloc_statistics._f += (_n)

/*
 *  Process deferred free operations
 */
bool malloc_is_system_state_OK(void);
void malloc_deferred_frees_process(void);
void malloc_deferred_free(void *);
