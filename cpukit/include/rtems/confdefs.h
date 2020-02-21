/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate Configuration Options
 *
 * This header file includes a couple of header files which evaluate the
 * configuration options specified by the application.  The macros and defines
 * used to configure the system are documented in the Configuring a System
 * chapter of the Classic API User's Guide.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1989, 2000 On-Line Applications Research Corporation (OAR)
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

#ifndef __CONFIGURATION_TEMPLATE_h
#define __CONFIGURATION_TEMPLATE_h

/**
 * @defgroup RTEMSApplicationConfiguration Application Configuration
 *
 * @ingroup RTEMSInternal
 *
 * @brief Evaluation of Application Configuration Options
 *
 * This group contains header files which evaluate the configuration options
 * specified by the application.
 *
 * @{
 */

/*
 * This header file must be included first.  For example, configuration options
 * which have been renamed are mapped to the new define.
 */
#include <rtems/confdefs/obsolete.h>

#include <rtems/confdefs/bdbuf.h>
#include <rtems/confdefs/clock.h>
#include <rtems/confdefs/console.h>
#include <rtems/confdefs/extensions.h>
#include <rtems/confdefs/inittask.h>
#include <rtems/confdefs/initthread.h>
#include <rtems/confdefs/iodrivers.h>
#include <rtems/confdefs/libio.h>
#include <rtems/confdefs/libpci.h>
#include <rtems/confdefs/malloc.h>
#include <rtems/confdefs/mpci.h>
#include <rtems/confdefs/newlib.h>
#include <rtems/confdefs/objectsclassic.h>
#include <rtems/confdefs/objectsposix.h>
#include <rtems/confdefs/percpu.h>
#include <rtems/confdefs/scheduler.h>
#include <rtems/confdefs/threads.h>
#include <rtems/confdefs/wkspace.h>

/** @} */

#endif
/* end of include file */
