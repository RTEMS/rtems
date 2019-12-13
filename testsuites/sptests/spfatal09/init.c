#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 *  Malloc Initialization Error
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/memory.h>
#include <rtems/sysinit.h>

#include <stdlib.h>

#define FATAL_ERROR_TEST_NAME            "9"
#define FATAL_ERROR_DESCRIPTION          "Bad heap address to malloc"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_NO_MEMORY_FOR_HEAP

static void force_error( void )
{
  void *p;

  /* we will not run this far */
  p = malloc( 1 );
  RTEMS_OBFUSCATE_VARIABLE( p );
}

static void consume_all_memory( void )
{
  const Memory_Information *mem;
  size_t                    i;

  mem = _Memory_Get();

  for ( i = 0; i < _Memory_Get_count( mem ); ++i ) {
    Memory_Area *area;

    area = _Memory_Get_area( mem, i );
    _Memory_Consume( area, _Memory_Get_free_size( area ) );
  }
}

RTEMS_SYSINIT_ITEM(
  consume_all_memory,
  RTEMS_SYSINIT_MALLOC,
  RTEMS_SYSINIT_ORDER_FIRST
);

#include "../spfatal_support/spfatalimpl.h"
