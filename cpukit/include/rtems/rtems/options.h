/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file provides the Classic API directive options.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1989, 2008 On-Line Applications Research Corporation (OAR)
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

/* Generated from spec:/rtems/option/if/header */

#ifndef _RTEMS_RTEMS_OPTIONS_H
#define _RTEMS_RTEMS_OPTIONS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/option/if/group */

/**
 * @defgroup RTEMSAPIClassicOptions Directive Options
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief This group contains the Classic API directive options.
 */

/* Generated from spec:/rtems/option/if/default */

/**
 * @ingroup RTEMSAPIClassicOptions
 *
 * @brief This option constant represents the default option set.
 */
#define RTEMS_DEFAULT_OPTIONS 0x00000000

/* Generated from spec:/rtems/option/if/event-all */

/**
 * @ingroup RTEMSAPIClassicOptions
 *
 * @brief This option constant indicates that the task wishes to wait until all
 *   events of interest are available in rtems_event_receive() and
 *   rtems_event_system_receive().
 */
#define RTEMS_EVENT_ALL 0x00000000

/* Generated from spec:/rtems/option/if/event-any */

/**
 * @ingroup RTEMSAPIClassicOptions
 *
 * @brief This option constant indicates that the task wishes to wait until at
 *   least one of the events of interest is available in rtems_event_receive()
 *   and rtems_event_system_receive().
 */
#define RTEMS_EVENT_ANY 0x00000002

/* Generated from spec:/rtems/option/if/no-wait */

/**
 * @ingroup RTEMSAPIClassicOptions
 *
 * @brief This option constant indicates that the task does not want to wait on
 *   the resource.
 *
 * If the resource is not available, then the directives shall return
 * immediately with a status to indicate that the request is unsatisfied.
 */
#define RTEMS_NO_WAIT 0x00000001

/* Generated from spec:/rtems/option/if/option */

/**
 * @ingroup RTEMSAPIClassicOptions
 *
 * @brief This type represents a Classic API directive option set.
 */
typedef uint32_t rtems_option;

/* Generated from spec:/rtems/option/if/wait */

/**
 * @ingroup RTEMSAPIClassicOptions
 *
 * @brief This option constant indicates that the task wants to wait on the
 *   resource.
 *
 * If the resource is not available, then the task shall block and wait for
 * request completion.
 */
#define RTEMS_WAIT 0x00000000

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_OPTIONS_H */
