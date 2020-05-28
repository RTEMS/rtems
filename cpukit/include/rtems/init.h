/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines the Initialization Manager API.
 */

/*
 * Copyright (C) 2015, 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

/* Generated from spec:/rtems/init/if/header */

#ifndef _RTEMS_INIT_H
#define _RTEMS_INIT_H

#include <stdint.h>
#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/init/if/group */

/**
 * @defgroup RTEMSAPIClassicInit Initialization Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Initialization Manager is responsible for initializing the
 *   system.
 *
 * The system initialization includes the initialization of the Board Support
 * Package, RTEMS, device drivers, the root filesystem, and the application.
 * The @ref RTEMSAPIClassicFatal is responsible for the system shutdown.
 */

/* Generated from spec:/rtems/init/if/initialize-executive */

/**
 * @ingroup RTEMSAPIClassicInit
 *
 * @brief Initializes the system and starts multitasking.
 *
 * Iterates through the system initialization linker set and invokes the
 * registered handlers.  The final step is to start multitasking.
 *
 * @par Notes
 * Errors in the initialization sequence are usually fatal and lead to a system
 * termination.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive should be called by boot_card() only.
 *
 * * The directive will not return to the caller.
 * @endparblock
 */
RTEMS_NO_RETURN void rtems_initialize_executive( void );

/* Generated from spec:/rtems/fatal/if/shutdown-executive */

/**
 * @ingroup RTEMSAPIClassicFatal
 *
 * @brief Invokes the fatal error handler.
 *
 * @param fatal_code is the fatal code.
 *
 * This directive processes fatal errors.  The fatal source is set to
 * RTEMS_FATAL_SOURCE_EXIT.  The fatal code is set to the value of the
 * ``fatal_code`` parameter.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not return to the caller.
 *
 * * The directive invokes the fatal error extensions in extension forward
 *   order.
 *
 * * The directive does not invoke handlers registered by atexit() or
 *   on_exit().
 *
 * * The directive may terminate the system.
 * @endparblock
 */
RTEMS_NO_RETURN void rtems_shutdown_executive( uint32_t fatal_code );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_INIT_H */
