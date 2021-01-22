/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief This header file defines the RTEMS Classic API.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/if/header */

#ifndef _RTEMS_H
#define _RTEMS_H

#include <rtems/config.h>
#include <rtems/extension.h>
#include <rtems/fatal.h>
#include <rtems/init.h>
#include <rtems/io.h>
#include <rtems/rtems/barrier.h>
#include <rtems/rtems/cache.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/intr.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/part.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/rtems/region.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/signal.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/timer.h>
#include <rtems/rtems/types.h>

#if defined(RTEMS_MULTIPROCESSING)
  #include <rtems/rtems/mp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/if/group */

/**
 * @defgroup RTEMSAPIClassic Classic
 *
 * @ingroup RTEMSAPI
 *
 * @brief This group contains the Classic API groups.
 */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_H */
