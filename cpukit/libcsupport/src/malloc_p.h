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
#include <rtems/score/protectedheap.h>
#include <rtems/malloc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
  MALLOC_SYSTEM_STATE_NORMAL,
  MALLOC_SYSTEM_STATE_NO_PROTECTION,
  MALLOC_SYSTEM_STATE_NO_ALLOCATION
} Malloc_System_state;

Malloc_System_state _Malloc_System_state( void );

void _Malloc_Deferred_free( void * );

void _Malloc_Process_deferred_frees( void );

#ifdef __cplusplus
}
#endif /* __cplusplus */
