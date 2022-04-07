/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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

/* functions */

#include <pmacros.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <tmacros.h>

void *POSIX_Init(void *argument);
void *Task_1_through_3(void *argument);
void *Task_4(void *argument);
void *Task_5(void *argument);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS         5
#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES 10
#if defined(RTEMS_POSIX_API)
#define CONFIGURE_MAXIMUM_POSIX_TIMERS          4
#define CONFIGURE_MAXIMUM_TIMERS                4
#endif

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE \
        (RTEMS_MINIMUM_STACK_SIZE * 10)

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN pthread_t        Init_id;
TEST_EXTERN pthread_t        Task_id;

#define MSGSIZE   9
#define MAXMSG    4

typedef enum {
  RD_QUEUE,   /* Read only queue               */
  WR_QUEUE,   /* Write only queue              */
  RW_QUEUE,   /* Read Write non-blocking queue */
  BLOCKING,   /* Read Write blocking queue     */
  DEFAULT_RW, /* default must be last          */
  CLOSED,     /* Created and closed queue      */
  NUMBER_OF_TEST_QUEUES
} Test_Queue_Types;
extern int Priority_Order[MAXMSG+1];
void *Task_1 ( void *argument );
void *Task_2( void *argument );
void *Task_3( void *argument );
void *Task_4( void *argument );

/* end of include file */
