/* SPDX-License-Identifier: BSD-2-Clause */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#define FATAL_ERROR_TEST_NAME            "29"
#define FATAL_ERROR_DESCRIPTION          "yield in interrupt context"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL

#define CONFIGURE_MAXIMUM_TIMERS 1

static void timer(rtems_id id, void *arg)
{
  rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
}

static void force_error(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_timer_create(
    rtems_build_name('T', 'I', 'M', 'E'),
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_fire_after(id, 1, timer, NULL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_exit();
}

#include "../spfatal_support/spfatalimpl.h"
