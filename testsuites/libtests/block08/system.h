/* SPDX-License-Identifier: BSD-2-Clause */

/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  Copyright (C) 2010 OKTET Labs, St.-Petersburg, Russia
 *  Author: Oleg Kravtsov <Oleg.Kravtsov@oktetlabs.ru>
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

#include <rtems.h>
#include <rtems/blkdev.h>
#include <rtems/test-info.h>

#include <bsp.h>

rtems_task Init(
  rtems_task_argument argument
);

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS               28
#define CONFIGURE_MAXIMUM_TIMERS              10
#define CONFIGURE_MAXIMUM_SEMAPHORES          20
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES      10
#define CONFIGURE_MAXIMUM_PARTITIONS          1
#define CONFIGURE_MAXIMUM_REGIONS             1
#define CONFIGURE_MAXIMUM_PERIODS             1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     0
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS 2
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE   (512)
#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE     (512)
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE     (512)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE      (4 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_EXTRA_TASK_STACKS         (13 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* end of include file */
