/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  Minimum Size Application Initialization
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <rtems/score/thread.h>

static void *Init( uintptr_t ignored )
{
  /* initialize application */

  /* Real application would call idle loop functionality */

  /* but in this case, just return and fall into a fatal error */

  return NULL;
}

/* configuration information */

/*
 * This application has no device drivers.
 */
/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

/*
 *  This application has no filesytem and libio support.
 */
#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

/*
 *  This disables reentrancy support in the C Library.  It is usually
 *  not something an application wants to do unless the development
 *  team is committed to using C Library routines that are KNOWN to
 *  be reentrant.  Caveat Emptor!!
 */
#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

/*
 *  This test does not use any stdio.
 */
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

/*
 *  This may prevent us from running on every architecture but it
 *  demonstrates that the user can specify how small of a minimum
 *  stack they want.
 */
#ifdef RTEMS_GCOV_COVERAGE
#define CONFIGURE_MINIMUM_TASK_STACK_SIZE \
  (CPU_STACK_MINIMUM_SIZE - CPU_STACK_ALIGNMENT)
#else
#define CONFIGURE_MINIMUM_TASK_STACK_SIZE 512
#endif

/*
 * Keep the interrupt/initialization stack as is.  Otherwise, the test may fail
 * in the low level system initialization.
 */
#ifdef BSP_INTERRUPT_STACK_SIZE
  #define CONFIGURE_INTERRUPT_STACK_SIZE BSP_INTERRUPT_STACK_SIZE
#else
  #define CONFIGURE_INTERRUPT_STACK_SIZE CPU_STACK_MINIMUM_SIZE
#endif

/*
 *  This lowers the number of priorities that this test is able to
 *  use.  The Idle task will be running at the lowest priority.
 */
#define CONFIGURE_MAXIMUM_PRIORITY 15

/*
 *  In this application, the initialization task performs the system
 *  initialization and then transforms itself into the idle task.
 */
#define CONFIGURE_IDLE_TASK_BODY Init
#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

/*
 *  If you are debugging confdefs.h, define this
 */
/* #define CONFIGURE_CONFDEFS_DEBUG */

/*
 *  Instantiate the configuration tables.
 */
#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* global variables */
