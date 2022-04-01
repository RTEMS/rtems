/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <rtems.h>
#include "test_driver.h"
#include <rtems/libio.h>
#include <rtems/devnull.h>
/* 
 * The test driver routines are mostly derived from the null driver routines.
 */
uint32_t   TEST_major;
static char initialized;

/*  testDriver_initialize
 *
 *  This routine is the test device driver init routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargp - pointer to parameter block
 *
 *  Output parameters:
 *    rval       - RTEMS_SUCCESSFUL
 */
rtems_device_driver testDriver_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *pargp RTEMS_UNUSED
)
{
  rtems_device_driver status;

  if ( !initialized ) {
    initialized = 1;

    status = rtems_io_register_name(
      "/dev/test",
      major,
      (rtems_device_minor_number) 0
    );

    if (status != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(status);

    TEST_major = major;
  }

  return RTEMS_SUCCESSFUL;
}

/*  testDriver_open
 *
 *  This routine is the test device driver open routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargb - pointer to open parameter block
 *
 *  Output parameters:
 *    rval       - RTEMS_SUCCESSFUL
 */
rtems_device_driver testDriver_open(
  rtems_device_major_number major RTEMS_UNUSED,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *pargp RTEMS_UNUSED
)
{
  return RTEMS_SUCCESSFUL;
}

/*  testDriver_close
 *
 *  This routine is the test device driver close routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargb - pointer to close parameter block
 *
 *  Output parameters:
 *    rval       - RTEMS_SUCCESSFUL
 */
rtems_device_driver testDriver_close(
  rtems_device_major_number major RTEMS_UNUSED,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *pargp RTEMS_UNUSED
)
{
  return RTEMS_SUCCESSFUL;
}

/*  testDriver_read
 *
 *  This routine is the test device driver read routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargp - pointer to read parameter block
 *
 *  Output parameters:
 *    rval       - RTEMS_SUCCESSFUL
 */
rtems_device_driver testDriver_read(
  rtems_device_major_number major RTEMS_UNUSED,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *pargp
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) pargp;

  if ( rw_args ) {
    if( rw_args->count == 5 )
      rw_args->bytes_moved = 0;
    else {
      rw_args->bytes_moved = 0;
      return RTEMS_NOT_IMPLEMENTED;
    }
  }
  
  return RTEMS_SUCCESSFUL;
}

/*  testDriver_write
 *
 *  This routine is the test device driver write routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargp - pointer to write parameter block
 *
 *  Output parameters:
 *    rval       - RTEMS_SUCCESSFUL
 */
rtems_device_driver testDriver_write(
  rtems_device_major_number major RTEMS_UNUSED,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *pargp
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) pargp;

  if ( rw_args ) {
    if( rw_args->count == 5 )
      return null_write( 0, 0, pargp );
    else {
      rw_args->bytes_moved = 0;
      return RTEMS_NOT_IMPLEMENTED;
    }
  }

  return RTEMS_SUCCESSFUL;
}

/*  testDriver_control
 *
 *  This routine is the test device driver control routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargp - pointer to cntrl parameter block
 *
 *  Output parameters:
 *    rval       - RTEMS_SUCCESSFUL
 */

rtems_device_driver testDriver_control(
  rtems_device_major_number major RTEMS_UNUSED,
  rtems_device_minor_number minor RTEMS_UNUSED,
  void *pargp RTEMS_UNUSED
)
{
  return RTEMS_NOT_IMPLEMENTED;
}
