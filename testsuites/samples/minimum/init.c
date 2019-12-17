/**
 *  @file
 *
 *  Minimum Size Application Initialization
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
#define CONFIGURE_MINIMUM_TASK_STACK_SIZE 512

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
 *  This configures RTEMS to use a single memory pool for the RTEMS Workspace
 *  and C Program Heap.  If not defined, there will be separate memory pools
 *  for the RTEMS Workspace and C Program Heap.  Having separate pools
 *  does haved some advantages in the event a task blows a stack or writes
 *  outside its memory area. However, in low memory systems the overhead of
 *  the two pools plus the potential for unused memory in either pool is
 *  very undesirable.
 *
 *  In high memory environments, this is desirable when you want to use
 *  the RTEMS "unlimited" objects option.  You will be able to create objects
 *  until you run out of memory.
 */
#define CONFIGURE_UNIFIED_WORK_AREAS

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
