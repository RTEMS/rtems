/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate User Initialization Task Configuration Options
 *
 * This header file defines _CONFIGURE_INIT_TASK_STACK_EXTRA for use by other
 * configuration header files.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_CONFDEFS_INITTASK_H
#define _RTEMS_CONFDEFS_INITTASK_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#ifdef CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs/percpu.h>
#include <rtems/rtems/object.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/sysinit.h>

#ifndef CONFIGURE_INIT_TASK_ATTRIBUTES
  #define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#endif

#ifndef CONFIGURE_INIT_TASK_INITIAL_MODES
  #ifdef RTEMS_SMP
    #define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
  #else
    #define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_NO_PREEMPT
  #endif
#endif

#ifndef CONFIGURE_INIT_TASK_NAME
  #define CONFIGURE_INIT_TASK_NAME rtems_build_name( 'U', 'I', '1', ' ' )
#endif

#ifndef CONFIGURE_INIT_TASK_PRIORITY
  #define CONFIGURE_INIT_TASK_PRIORITY 1
#endif

#ifndef CONFIGURE_INIT_TASK_STACK_SIZE
  #define CONFIGURE_INIT_TASK_STACK_SIZE CONFIGURE_MINIMUM_TASK_STACK_SIZE
#endif

#if CONFIGURE_INIT_TASK_STACK_SIZE > CONFIGURE_MINIMUM_TASK_STACK_SIZE
  #define _CONFIGURE_INIT_TASK_STACK_EXTRA \
    ( CONFIGURE_INIT_TASK_STACK_SIZE - CONFIGURE_MINIMUM_TASK_STACK_SIZE )
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIGURE_INIT_TASK_ENTRY_POINT
  rtems_task Init( rtems_task_argument );
  #define CONFIGURE_INIT_TASK_ENTRY_POINT Init

  #ifndef CONFIGURE_INIT_TASK_ARGUMENTS
    extern const char *bsp_boot_cmdline;
    #define CONFIGURE_INIT_TASK_ARGUMENTS \
      ( (rtems_task_argument) &bsp_boot_cmdline )
  #endif
#endif

#ifndef CONFIGURE_INIT_TASK_ARGUMENTS
  #define CONFIGURE_INIT_TASK_ARGUMENTS 0
#endif

const rtems_initialization_tasks_table _RTEMS_tasks_User_task_table = {
  CONFIGURE_INIT_TASK_NAME,
  CONFIGURE_INIT_TASK_STACK_SIZE,
  CONFIGURE_INIT_TASK_PRIORITY,
  CONFIGURE_INIT_TASK_ATTRIBUTES,
  CONFIGURE_INIT_TASK_ENTRY_POINT,
  CONFIGURE_INIT_TASK_INITIAL_MODES,
  CONFIGURE_INIT_TASK_ARGUMENTS
};

RTEMS_SYSINIT_ITEM(
  _RTEMS_tasks_Initialize_user_task,
  RTEMS_SYSINIT_CLASSIC_USER_TASKS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURE_RTEMS_INIT_TASKS_TABLE */

#ifndef _CONFIGURE_INIT_TASK_STACK_EXTRA
  #define _CONFIGURE_INIT_TASK_STACK_EXTRA 0
#endif

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_INITTASK_H */
