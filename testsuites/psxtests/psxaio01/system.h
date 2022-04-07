/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright 2010, Alin Rus <alin.codejunkie@gmail.com>
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

/* functions */

#include <pmacros.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>

void *POSIX_Init (void *argument);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             20
#define CONFIGURE_MAXIMUM_SEMAPHORES        20
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES    20
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 20

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS        10
#define CONFIGURE_MAXIMUM_POSIX_KEYS           10

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_EXTRA_TASK_STACKS         (10 * RTEMS_MINIMUM_STACK_SIZE)
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE (10 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* global variables */
TEST_EXTERN pthread_t Init_id;

/* end of include file */
