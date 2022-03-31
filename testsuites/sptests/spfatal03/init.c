/* SPDX-License-Identifier: BSD-2-Clause */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 *  Semaphore Obtain in Critical Section
 *
 *  COPYRIGHT (c) 1989-2011.
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

#include <rtems/bspIo.h>

#define CONFIGURE_MAXIMUM_SEMAPHORES 10

#define FATAL_ERROR_TEST_NAME            "3"
#define FATAL_ERROR_DESCRIPTION          "Core Mutex obtain in critical section"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       \
          INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL

static void force_error(void)
{

  rtems_status_code status;
   rtems_id    mutex;

  status = rtems_semaphore_create(
    rtems_build_name( 'S','0',' ',' '),
    0,
    RTEMS_LOCAL|
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &mutex
  );
  directive_failed( status, "rtems_semaphore_create of S0");
  printk("Create semaphore S0\n");

  printk("Obtain semaphore in dispatching critical section\n");
  _Thread_Dispatch_disable();
  status = rtems_semaphore_obtain( mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  /* !!! SHOULD NOT RETURN FROM THE ABOVE CALL */

  rtems_test_assert( 0 );
  /* we will not run this far */
}

#include "../spfatal_support/spfatalimpl.h"
