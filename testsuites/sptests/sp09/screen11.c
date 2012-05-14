/*  Screen11
 *
 *  This routine generates error screen 11 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2009.
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

void Screen11()
{
  void              *buffer_address_1;
  void              *buffer_address_2;
  void              *buffer_address_3;
  rtems_status_code  status;
  size_t             size;

  status = rtems_partition_create(
    0,
    Partition_good_area,
    128,
    40,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_partition_create with illegal name"
  );
  puts( "TA1 - rtems_partition_create - RTEMS_INVALID_NAME" );

  status = rtems_partition_create(
    Partition_name[ 1 ],
    Partition_good_area,
    0,
    71,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_SIZE,
    "rtems_partition_create with illegal length"
  );
  puts( "TA1 - rtems_partition_create - length - RTEMS_INVALID_SIZE" );

  status = rtems_partition_create(
    Partition_name[ 1 ],
    Partition_good_area,
    128,
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_SIZE,
    "rtems_partition_create with illegal buffer size"
  );
  puts( "TA1 - rtems_partition_create - buffer size - RTEMS_INVALID_SIZE" );

  status = rtems_partition_create(
    Partition_name[ 1 ],
    Partition_good_area,
    128,
    256,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_SIZE,
    "rtems_partition_create with buffer_size > length"
  );
  puts(
    "TA1 - rtems_partition_create - length < buffer size - RTEMS_INVALID_SIZE"
  );

  /*
   * Attempt to create a partition with a buffer size that is not large
   * enough to account for the overhead.
   */
  puts(
    "TA1 - rtems_partition_create - buffer size < overhead - RTEMS_INVALID_SIZE"
  );
#define SIZEOF_CHAIN_NODE 2 * sizeof(void *)
  for ( size=0 ; size < SIZEOF_CHAIN_NODE ; size++) {
    status = rtems_partition_create(
      Partition_name[ 1 ],
      Partition_good_area,
      size,
      256,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Junk_id
    );
    if ( status != RTEMS_INVALID_SIZE )
      printf( "ERROR when size == %zu\n", size );

    fatal_directive_status(
      status,
      RTEMS_INVALID_SIZE,
      "rtems_partition_create with buffer_size > length"
    );
  }

  /*
   *  The check for an object being global is only made if
   *  multiprocessing is enabled.
   */

#if defined(RTEMS_MULTIPROCESSING)
  status = rtems_partition_create(
    Partition_name[ 1 ],
    Partition_good_area,
    128,
    64,
    RTEMS_GLOBAL,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_MP_NOT_CONFIGURED,
    "rtems_partition_create of global"
  );
#endif
  puts( "TA1 - rtems_partition_create - RTEMS_MP_NOT_CONFIGURED" );

#if defined(_C3x) || defined(_C4x)
  puts( "TA1 - rtems_partition_create - RTEMS_INVALID_ADDRESS - SKIPPED" );
#else
  status = rtems_partition_create(
    Partition_name[ 1 ],
    Partition_bad_area,
    128,
    64,
    RTEMS_GLOBAL,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_partition_create with bad address"
  );
  puts( "TA1 - rtems_partition_create - RTEMS_INVALID_ADDRESS" );
#endif

#if defined(_C3x) || defined(_C4x)
  puts( "TA1 - rtems_partition_create - RTEMS_INVALID_SIZE - SKIPPED" );
#else
  status = rtems_partition_create(
    Partition_name[ 1 ],
    Partition_good_area,
    128,
    35,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_SIZE,
    "rtems_partition_create with unaligned buffer_size"
  );
  puts( "TA1 - rtems_partition_create - RTEMS_INVALID_SIZE" );
#endif

  status = rtems_partition_delete( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_partition_delete with illegal id"
  );
  puts( "TA1 - rtems_partition_delete - unknown RTEMS_INVALID_ID" );

  status = rtems_partition_delete( rtems_build_id( 1, 1, 1, 256 ) );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_partition_delete with illegal id"
  );
  puts( "TA1 - rtems_partition_delete - local RTEMS_INVALID_ID" );

  /* get bad address */
  status = rtems_partition_get_buffer( 100, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_partition_get_buffer with NULL param"
  );
  puts( "TA1 - rtems_partition_get_buffer - RTEMS_INVALID_ADDRESS" );

  /* get bad Id */
  status = rtems_partition_get_buffer( 100, &buffer_address_1 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_partition_get_buffer with illegal id"
  );
  puts( "TA1 - rtems_partition_get_buffer - RTEMS_INVALID_ID" );

  status = rtems_partition_ident( 0, RTEMS_SEARCH_ALL_NODES, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_partition_ident with illegal name"
  );
  puts( "TA1 - rtems_partition_ident - RTEMS_INVALID_NAME" );

  status = rtems_partition_return_buffer( 100, buffer_address_1 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_partition_return_buffer with illegal id"
  );
  puts( "TA1 - rtems_partition_return_buffer - RTEMS_INVALID_ID" );

  /* create bad area */
  status = rtems_partition_create(
    Partition_name[ 1 ],
    NULL,
    128,
    64,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_partition_return_buffer with NULL area"
  );
  puts( "TA1 - rtems_partition_create - RTEMS_INVALID_ADDRESS" );

  /* create OK */
  status = rtems_partition_create(
    Partition_name[ 1 ],
    Partition_good_area,
    128,
    64,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Partition_id[ 1 ]
  );
  directive_failed( status, "rtems_partition_create" );
  puts( "TA1 - rtems_partition_create - RTEMS_SUCCESSFUL" );

  status = rtems_partition_create(
    Partition_name[ 1 ],
    Partition_good_area,
    128,
    32,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_partition_create of too many"
  );
  puts( "TA1 - rtems_partition_create - RTEMS_TOO_MANY" );

  status = rtems_partition_get_buffer( Partition_id[ 1 ], &buffer_address_1 );
  directive_failed( status, "rtems_partition_get_buffer");
  puts( "TA1 - rtems_partition_get_buffer - RTEMS_SUCCESSFUL" );

  status = rtems_partition_get_buffer( Partition_id[ 1 ], &buffer_address_2 );
  directive_failed( status, "rtems_partition_get_buffer" );
  puts( "TA1 - rtems_partition_get_buffer - RTEMS_SUCCESSFUL" );

  status = rtems_partition_get_buffer( Partition_id[ 1 ], &buffer_address_3 );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_partition_get_buffer unsatisfied"
  );
  puts( "TA1 - rtems_partition_get_buffer - RTEMS_UNSATISFIED" );

  status = rtems_partition_delete( Partition_id[ 1 ] );
  fatal_directive_status(
    status,
    RTEMS_RESOURCE_IN_USE,
    "rtems_partition_delete with buffers in use"
  );
  puts( "TA1 - rtems_partition_delete - RTEMS_RESOURCE_IN_USE" );

  status = rtems_partition_return_buffer(
    Partition_id[ 1 ],
    Region_good_area                  /* NOTE: Region Memory */
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_partition_return_buffer with buffer address out of partition"
  );
  puts(
    "TA1 - rtems_partition_return_buffer - RTEMS_INVALID_ADDRESS - out of range"
  );

  status = rtems_partition_return_buffer(
    Partition_id[ 1 ],
    &Partition_good_area[ 7 ]
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_partition_return_buffer with buffer address not on boundary"
  );
  puts_nocr( "TA1 - rtems_partition_return_buffer - " );
  puts     ( "RTEMS_INVALID_ADDRESS - not on boundary");
}
