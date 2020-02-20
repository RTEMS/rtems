/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate Console Driver Configuration Options
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

#ifndef _RTEMS_CONFDEFS_SIMPLECONSOLE_H
#define _RTEMS_CONFDEFS_SIMPLECONSOLE_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#if defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER) \
  && defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER)
  #error "CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER and CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER are mutually exclusive"
#endif

#if defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER) \
  || defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER)

#include <rtems/console.h>
#include <rtems/sysinit.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
  RTEMS_SYSINIT_ITEM(
    _Console_simple_Initialize,
    RTEMS_SYSINIT_DEVICE_DRIVERS,
    RTEMS_SYSINIT_ORDER_SECOND
  );
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER
  RTEMS_SYSINIT_ITEM(
    _Console_simple_task_Initialize,
    RTEMS_SYSINIT_DEVICE_DRIVERS,
    RTEMS_SYSINIT_ORDER_SECOND
  );
#endif

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
  || CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER */

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_SIMPLECONSOLE_H */
