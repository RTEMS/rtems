/*  Screen12
 *
 *  This routine generates error screen 12 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "system.h"

void Screen12()
{
  void              *segment_address_1;
  void              *segment_address_2;
  void              *segment_address_3;
  rtems_unsigned32   offset;
  rtems_unsigned32   good_front_flag;
  rtems_unsigned32   good_back_flag;
  rtems_status_code  status;

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

#if defined(_C3x) || defined(_C4x)
  puts( "TA1 - rtems_region_create - RTEMS_INVALID_ADDRESS - SKIPPED" );
#else
  status = rtems_region_create(
    Region_name[ 1 ],
    Region_bad_area,
    0x40,
    32,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_create with illegal address"
  );
  puts( "TA1 - rtems_region_create - RTEMS_INVALID_ADDRESS" );
#endif

#if defined(_C3x) || defined(_C4x)
  puts( "TA1 - rtems_region_create - RTEMS_INVALID_SIZE - SKIPPED" );
#else
  status = rtems_region_create(
    Region_name[ 1 ],
    Region_good_area,
    0x40,
    34,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_SIZE,
    "rtems_region_create with illegal size"
  );
  puts( "TA1 - rtems_region_create - RTEMS_INVALID_SIZE" );
#endif

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

  status = rtems_region_delete( 0x10100 );
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
    "rtems_region_get_segment with illegal size"
  );
  puts( "TA1 - rtems_region_get_segment - RTEMS_INVALID_SIZE" );

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
    3 * TICKS_PER_SECOND,
    &segment_address_3
  );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_region_get_segment timeout"
  );
  puts( "TA1 - rtems_region_get_segment - woke up with RTEMS_TIMEOUT" );

  status = rtems_region_delete( Region_id[ 1 ] );
  fatal_directive_status(
    status,
    RTEMS_RESOURCE_IN_USE,
    "rtems_region_delete with buffers in use"
  );
  puts( "TA1 - rtems_region_delete - RTEMS_RESOURCE_IN_USE" );

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

#if 0

  offset = (segment_address_1 - (void *)Region_good_area) / 4;

/* bad FRONT_FLAG error */

  good_front_flag = Region_good_area[ offset - 1 ];
  Region_good_area[ offset - 1 ] = good_front_flag + 2;

  status = rtems_region_return_segment( Region_id[ 1 ], segment_address_1 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_return_segment with back_flag != front_flag"
  );
  puts( "TA1 - rtems_region_return_segment - RTEMS_INVALID_ADDRESS" );

  Region_good_area[ offset - 1 ] = good_front_flag;

/* bad FRONT_FLAG error */

  good_back_flag = Region_good_area[ offset - 2 ];
  Region_good_area[ offset - 2 ] = 1024;

  status = rtems_region_return_segment( Region_id[ 1 ], segment_address_1 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_region_return_segment with back_flag != front_flag"
  );
  puts( "TA1 - rtems_region_return_segment - RTEMS_INVALID_ADDRESS" );

  Region_good_area[ offset - 2 ] = good_back_flag;

#else
  offset = 0;
  good_front_flag = 0;
  good_back_flag = 0;
  puts( "TA1 - rtems_region_return_segment - RTEMS_INVALID_ADDRESS - SKIPPED" );
  puts( "TA1 - rtems_region_return_segment - RTEMS_INVALID_ADDRESS - SKIPPED" );
#endif

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

  status = rtems_region_extend(
    Region_id[ 1 ],
    Region_bad_area,
    128
  );
  fatal_directive_status(
    status,
    RTEMS_NOT_IMPLEMENTED,
    "rtems_region_extend with unsupported starting address"
  );
  puts(
    "TA1 - rtems_region_extend - non-contiguous lower - RTEMS_NOT_IMPLEMENTED"
  );

  status = rtems_region_extend(
    Region_id[ 1 ],
    &Region_good_area[ REGION_START_OFFSET - REGION_LENGTH ],
    128
  );
  fatal_directive_status(
    status,
    RTEMS_NOT_IMPLEMENTED,
    "rtems_region_extend with unsupported starting address"
  );
  puts(
    "TA1 - rtems_region_extend - contiguous lower - RTEMS_NOT_IMPLEMENTED"
  );

  status = rtems_region_extend(
    Region_id[ 1 ],
    &Region_good_area[ REGION_START_OFFSET + REGION_LENGTH + 16 ],
    128
  );
  fatal_directive_status(
    status,
    RTEMS_NOT_IMPLEMENTED,
    "rtems_region_extend with unsupported starting address"
  );
  puts(
    "TA1 - rtems_region_extend - non-contiguous higher - RTEMS_NOT_IMPLEMENTED"
  );

}
