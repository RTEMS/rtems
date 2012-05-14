/*  Screen12
 *
 *  This routine generates error screen 12 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

void Screen12()
{
  void                   *segment_address_1;
  void                   *segment_address_2;
  void                   *segment_address_3;
  uintptr_t               segment_size;
  rtems_status_code       status;
  Heap_Information_block  the_info;

  /* Check invalid name error case */
  status = rtems_region_create(
    0,
    Region_good_area,
    0x40,
    32,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_region_create with illegal name"
  );
  puts( "TA1 - rtems_region_create - RTEMS_INVALID_NAME" );

  /* Check NULL starting address error case */
  status = rtems_region_create(
    Region_name[ 1 ],
    NULL,
    0x40,
    32,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_create with NULL address"
  );
  puts( "TA1 - rtems_region_create - RTEMS_INVALID_ADDRESS" );

  /* Invalid size */
  status = rtems_region_create(
    Region_name[ 1 ],
    Region_good_area,
    0,
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_SIZE,
    "rtems_region_create with illegal size"
  );
  puts( "TA1 - rtems_region_create - RTEMS_INVALID_SIZE" );

  /* Check NULL id error case */
  status = rtems_region_create(
    Region_name[ 1 ],
    Region_good_area,
    REGION_LENGTH,
    0x40,
    RTEMS_DEFAULT_ATTRIBUTES,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_create with NULL id"
  );
  puts( "TA1 - rtems_region_create - RTEMS_INVALID_ADDRESS" );

  status = rtems_region_create(
    Region_name[ 1 ],
    &Region_good_area[ REGION_START_OFFSET ],
    REGION_LENGTH,
    0x40,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Region_id[ 1 ]
  );
  directive_failed( status, "rtems_region_create" );
  puts( "TA1 - rtems_region_create - RTEMS_SUCCESSFUL" );

  /* extend NULL address */
  status = rtems_region_extend(
    Region_id[ 1 ],
    NULL,
    REGION_LENGTH - 1
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_extend with NULL"
  );
  puts( "TA1 - rtems_region_extend - NULL address - RTEMS_INVALID_ADDRESS" );

  /* extend within heap */
  status = rtems_region_extend(
    Region_id[ 1 ],
    &Region_good_area[ REGION_START_OFFSET ],
    REGION_LENGTH - 1
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_extend with address in heap"
  );
  puts( "TA1 - rtems_region_extend - address within - RTEMS_INVALID_ADDRESS" );

  status = rtems_region_create(
    Region_name[ 1 ],
    Region_good_area,
    REGION_LENGTH,
    0x40,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_region_create of too many"
  );
  puts( "TA1 - rtems_region_create - RTEMS_TOO_MANY" );

  status = rtems_region_delete( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_region_delete with illegal id"
  );
  puts( "TA1 - rtems_region_delete - unknown RTEMS_INVALID_ID" );

  status = rtems_region_delete( rtems_build_id( 1, 1, 1, 256 ) );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_region_delete with illegal id"
  );
  puts( "TA1 - rtems_region_delete - local RTEMS_INVALID_ID" );

  status = rtems_region_ident( 0, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_region_ident with illegal name"
  );
  puts( "TA1 - rtems_region_ident - RTEMS_INVALID_NAME" );

  /* Check get_information errors */
  status = rtems_region_get_information( Region_id[ 1 ], NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_get_information with NULL information"
  );
  puts( "TA1 - rtems_region_get_information - RTEMS_INVALID_ADDRESS" );

  status = rtems_region_get_information( 100, &the_info );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_region_get_information with illegal id"
  );
  puts( "TA1 - rtems_region_get_information - unknown RTEMS_INVALID_ID" );

  /* Check get_free_information errors */
  status = rtems_region_get_free_information( Region_id[ 1 ], NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_get_free_information with NULL information"
  );
  puts( "TA1 - rtems_region_get_free_information - RTEMS_INVALID_ADDRESS" );

  status = rtems_region_get_free_information( 100, &the_info );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_region_get_free_information with illegal id"
  );
  puts( "TA1 - rtems_region_get_free_information - unknown RTEMS_INVALID_ID" );

  /* get segment illegal id */
  status = rtems_region_get_segment(
    100,
    0x40,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &segment_address_1
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_region_get_segment with illegal id"
  );
  puts( "TA1 - rtems_region_get_segment - RTEMS_INVALID_ID" );

  /* get_segment with NULL param */
  status = rtems_region_get_segment(
     Region_id[ 1 ],
     2,
     RTEMS_DEFAULT_OPTIONS,
     RTEMS_NO_TIMEOUT,
     NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_get_segment with NULL param"
  );
  puts( "TA1 - rtems_region_get_segment - RTEMS_INVALID_ADDRESS" );

  /* get_segment with illegal 0 size */
  status = rtems_region_get_segment(
     Region_id[ 1 ],
     0,
     RTEMS_DEFAULT_OPTIONS,
     RTEMS_NO_TIMEOUT,
     &segment_address_1
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_SIZE,
    "rtems_region_get_segment with 0 size"
  );
  puts( "TA1 - rtems_region_get_segment - 0 size - RTEMS_INVALID_SIZE" );

  /* get_segment with illegal big size */
  status = rtems_region_get_segment(
     Region_id[ 1 ],
     sizeof( Region_good_area ) * 2,
     RTEMS_DEFAULT_OPTIONS,
     RTEMS_NO_TIMEOUT,
     &segment_address_1
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_SIZE,
    "rtems_region_get_segment with big size"
  );
  puts( "TA1 - rtems_region_get_segment - too big - RTEMS_INVALID_SIZE" );

  status = rtems_region_get_segment(
     Region_id[ 1 ],
     384,
     RTEMS_DEFAULT_OPTIONS,
     RTEMS_NO_TIMEOUT,
     &segment_address_1
  );
  directive_failed( status, "rtems_region_get_segment" );
  puts( "TA1 - rtems_region_get_segment - RTEMS_SUCCESSFUL" );

  status = rtems_region_get_segment(
     Region_id[ 1 ],
     REGION_LENGTH / 2,
     RTEMS_NO_WAIT,
     RTEMS_NO_TIMEOUT,
     &segment_address_2
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_region_get_segment unsatisfied"
  );
  puts( "TA1 - rtems_region_get_segment - RTEMS_UNSATISFIED" );

  puts( "TA1 - rtems_region_get_segment - timeout in 3 seconds" );
  status = rtems_region_get_segment(
    Region_id[ 1 ],
    128,
    RTEMS_DEFAULT_OPTIONS,
    3 * rtems_clock_get_ticks_per_second(),
    &segment_address_3
  );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_region_get_segment timeout"
  );
  puts( "TA1 - rtems_region_get_segment - woke up with RTEMS_TIMEOUT" );

  /* Check get_segment_size errors */
  status = rtems_region_get_segment_size( Region_id[ 1 ], NULL, &segment_size );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_get_segment_size with NULL segment"
  );
  puts( "TA1 - rtems_region_get_segment_size - RTEMS_INVALID_ADDRESS" );

  status = rtems_region_get_segment_size(
    Region_id[ 1 ], segment_address_1, NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_get_segment_size with NULL size"
  );
  puts( "TA1 - rtems_region_get_segment_size - RTEMS_INVALID_ADDRESS" );

  status = rtems_region_get_segment_size(
    100, segment_address_1, &segment_size
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_region_get_segment_size with illegal id"
  );
  puts( "TA1 - rtems_region_get_segment_size - unknown RTEMS_INVALID_ID" );

  status = rtems_region_delete( Region_id[ 1 ] );
  fatal_directive_status(
    status,
    RTEMS_RESOURCE_IN_USE,
    "rtems_region_delete with buffers in use"
  );
  puts( "TA1 - rtems_region_delete - RTEMS_RESOURCE_IN_USE" );

  /* Check resize_segment errors */
  status = rtems_region_resize_segment(
    Region_id[ 1 ], segment_address_3, 256, NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_resize_segment with NULL old size"
  );
  puts( "TA1 - rtems_region_resize_segment - RTEMS_INVALID_ADDRESS" );

  status = rtems_region_resize_segment(
    Region_id[ 1 ], NULL, 256, &segment_size
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_resize_segment with NULL segment"
  );
  puts( "TA1 - rtems_region_resize_segment - RTEMS_INVALID_ADDRESS" );

  status = rtems_region_resize_segment(
    100, segment_address_3, 256, &segment_size
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_region_resize_segment with illegal id"
  );
  puts( "TA1 - rtems_region_resize_segment - RTEMS_INVALID_ID" );

  /* Check return_segment errors */
  status = rtems_region_return_segment( 100, segment_address_1 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_region_return_segment with illegal id"
  );
  puts( "TA1 - rtems_region_return_segment - RTEMS_INVALID_ID" );

  status = rtems_region_return_segment( Region_id[ 1 ], Region_good_area );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_return_segment with illegal segment"
  );
  puts( "TA1 - rtems_region_return_segment - RTEMS_INVALID_ADDRESS" );

/*
 *  The following generate internal heap errors.  Thus this code
 *  is subject to change if the heap code changes.
 */

  puts( "TA1 - rtems_debug_disable - RTEMS_DEBUG_REGION" );
  rtems_debug_disable( RTEMS_DEBUG_REGION );

  puts( "TA1 - rtems_debug_enable - RTEMS_DEBUG_REGION" );
  rtems_debug_enable( RTEMS_DEBUG_REGION );

  status = rtems_region_extend(
    100,
    Region_good_area,
    128
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_region_extend with illegal id"
  );
  puts( "TA1 - rtems_region_extend - RTEMS_INVALID_ID" );

  status = rtems_region_extend(
    Region_id[ 1 ],
    &Region_good_area[ REGION_START_OFFSET + 16 ],
    128
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_extend with illegal starting address"
  );
  puts( "TA1 - rtems_region_extend - within heap - RTEMS_INVALID_ADDRESS" );
}
