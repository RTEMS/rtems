/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task5
 *
 *  This routine serves as a test task.  It competes with the other tasks
 *  for region resources.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Task5(
  rtems_task_argument argument
)
{
  void              *segment_address_1;
  void              *segment_address_2;
  rtems_status_code  status;

  puts( "TA5 - rtems_region_get_segment - wait on 1.5K segment from region 1" );
  status = rtems_region_get_segment(
    Region_id[ 1 ],
    1536,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &segment_address_1
  );
  directive_failed( status, "rtems_region_get_segment" );

  status = rtems_region_return_segment( Region_id[ 1 ], segment_address_1 );
  directive_failed( status, "rtems_region_return_segment" );
  puts_nocr( "TA5 - got and returned " );
  Put_address_from_area_1( segment_address_1 );
  new_line;

  status = rtems_task_wake_after( RTEMS_MICROSECONDS_TO_TICKS( 1000000 ) );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA5 - rtems_region_get_segment - wait on 3K segment from region 1" );
  status = rtems_region_get_segment(
    Region_id[ 1 ],
    3072,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &segment_address_2
  );
  puts_nocr( "TA5 - got segment from region 1 - " );
  Put_address_from_area_1( segment_address_2 );
  new_line;

  status = rtems_region_return_segment( Region_id[ 1 ], segment_address_2 );
  puts_nocr(
    "TA5 - rtems_region_return_segment - return segment to region 1 - "
  );
  Put_address_from_area_1( segment_address_2 );
  new_line;

  puts( "TA5 - rtems_task_exit" );
  rtems_task_exit();
}
