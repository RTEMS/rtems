/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsPartValPart
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsPartValPart spec:/rtems/part/val/part
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief This test case collection provides validation test cases for
 *   requirements of the @ref RTEMSAPIClassicPart.
 *
 * This test case performs the following actions:
 *
 * - Create a partition with a buffer area length which is greater than three
 *   times the buffer size and less than four times the buffer size.
 *
 *   - Check that exactly three buffers can be obtained from the partition for
 *     use in parallel.
 *
 *   - Return the three buffers in use to the partition and check that they can
 *     be obtained from the partition for use in parallel in FIFO order.
 *
 * - Assert that RTEMS_PARTITION_ALIGNMENT is a constant expression which
 *   evaluates to the expected value.
 *
 * @{
 */

/**
 * @brief Create a partition with a buffer area length which is greater than
 *   three times the buffer size and less than four times the buffer size.
 */
static void RtemsPartValPart_Action_0( void )
{
  RTEMS_ALIGNED( RTEMS_PARTITION_ALIGNMENT ) uint8_t
    buffers[ 4 ][ 2 * sizeof( void * ) ];
  void              *pointers[ RTEMS_ARRAY_SIZE( buffers ) ];
  void              *pointer;
  rtems_status_code  sc;
  rtems_id           id;

  id = 0xffffffff;
  sc = rtems_partition_create(
    rtems_build_name( 'N', 'A', 'M', 'E' ),
    buffers,
    sizeof( buffers ) - 1,
    sizeof( buffers[ 0 ] ),
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  T_step_rsc_success( 0, sc );

  /*
   * Check that exactly three buffers can be obtained from the partition for
   * use in parallel.
   */
  pointers[ 0 ] = NULL;
  sc = rtems_partition_get_buffer( id, &pointers[ 0 ] );
  T_step_rsc_success( 1, sc );
  T_step_not_null( 2, pointers[ 0 ] );

  pointers[ 1 ] = NULL;
  sc = rtems_partition_get_buffer( id, &pointers[ 1 ] );
  T_step_rsc_success( 3, sc );
  T_step_not_null( 4, pointers[ 1 ] );

  pointers[ 2 ] = NULL;
  sc = rtems_partition_get_buffer( id, &pointers[ 2 ] );
  T_step_rsc_success( 5, sc );
  T_step_not_null( 6, pointers[ 2 ] );

  pointers[ 3 ] = NULL;
  sc = rtems_partition_get_buffer( id, &pointers[ 3 ] );
  T_step_rsc( 7, sc, RTEMS_UNSATISFIED );
  T_step_null( 8, pointers[ 3 ] );

  /*
   * Return the three buffers in use to the partition and check that they can
   * be obtained from the partition for use in parallel in FIFO order.
   */
  sc = rtems_partition_return_buffer( id, pointers[ 1 ] );
  T_step_rsc_success( 9, sc );

  sc = rtems_partition_return_buffer( id, pointers[ 2 ] );
  T_step_rsc_success( 10, sc );

  sc = rtems_partition_return_buffer( id, pointers[ 0 ] );
  T_step_rsc_success( 11, sc );

  pointer = NULL;
  sc = rtems_partition_get_buffer( id, &pointer );
  T_step_rsc_success( 12, sc );
  T_step_eq_ptr( 13, pointer, pointers[ 1 ] );

  pointer = NULL;
  sc = rtems_partition_get_buffer( id, &pointer );
  T_step_rsc_success( 14, sc );
  T_step_eq_ptr( 15, pointer, pointers[ 2 ] );

  pointer = NULL;
  sc = rtems_partition_get_buffer( id, &pointer );
  T_step_rsc_success( 16, sc );
  T_step_eq_ptr( 17, pointer, pointers[ 0 ] );

  sc = rtems_partition_return_buffer( id, pointers[ 0 ] );
  T_step_rsc_success( 18, sc );

  sc = rtems_partition_return_buffer( id, pointers[ 1 ] );
  T_step_rsc_success( 19, sc );

  sc = rtems_partition_return_buffer( id, pointers[ 2 ] );
  T_step_rsc_success( 20, sc );

  sc = rtems_partition_delete( id );
  T_step_rsc_success( 21, sc );
}

/**
 * @brief Assert that RTEMS_PARTITION_ALIGNMENT is a constant expression which
 *   evaluates to the expected value.
 */
static void RtemsPartValPart_Action_1( void )
{
  RTEMS_STATIC_ASSERT(
    RTEMS_PARTITION_ALIGNMENT == CPU_SIZEOF_POINTER,
    ALIGNMENT
  );
}

/**
 * @fn void T_case_body_RtemsPartValPart( void )
 */
T_TEST_CASE( RtemsPartValPart )
{
  T_plan( 22 );

  RtemsPartValPart_Action_0();
  RtemsPartValPart_Action_1();
}

/** @} */
