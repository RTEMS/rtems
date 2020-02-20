/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate POSIX Initialization Thread Configuration Options
 *
 * This header file defines _CONFIGURE_POSIX_INIT_THREAD_STACK_EXTRA for use by
 * other configuration header files.
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

#ifndef _RTEMS_CONFDEFS_INITTHREAD_H
#define _RTEMS_CONFDEFS_INITTHREAD_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#ifdef CONFIGURE_POSIX_INIT_THREAD_TABLE

#include <rtems/confdefs/percpu.h>
#include <rtems/posix/pthread.h>
#include <rtems/sysinit.h>

#ifndef CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT
  #define CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT POSIX_Init
#endif

#ifndef CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
  #define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE \
    CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE
#endif

#if CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE \
  > CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE
  #define _CONFIGURE_POSIX_INIT_THREAD_STACK_EXTRA \
    ( CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE - \
      CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE )
#endif

#ifdef __cplusplus
extern "C" {
#endif

const posix_initialization_threads_table _POSIX_Threads_User_thread_table = {
  CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT,
  CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
};

RTEMS_SYSINIT_ITEM(
  _POSIX_Threads_Initialize_user_thread,
  RTEMS_SYSINIT_POSIX_USER_THREADS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURE_POSIX_INIT_THREAD_TABLE */

#ifndef _CONFIGURE_POSIX_INIT_THREAD_STACK_EXTRA
  #define _CONFIGURE_POSIX_INIT_THREAD_STACK_EXTRA 0
#endif

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_INITTHREAD_H */
