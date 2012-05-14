/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/gxx_wrappers.h>

#define FATAL_ERROR_TEST_NAME            "GXX KEY ADD FAILURE"
#define FATAL_ERROR_DESCRIPTION          "GXX KEY ADD FAILURE"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_IS_INTERNAL TRUE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_GXX_KEY_ADD_FAILED

void force_error()
{
  void *alloc_ptr = NULL;
  Heap_Information_block info;
  __gthread_key_t key = 0;
  bool ok = false;

  ok = rtems_workspace_get_information( &info );
  rtems_test_assert( ok );

  ok = rtems_workspace_allocate( info.Free.largest - 4, &alloc_ptr );
  rtems_test_assert( ok );

  rtems_gxx_getspecific( key );
}
