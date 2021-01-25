/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file provides Classic API directive attributes.
 */

/*
 * Copyright (C) 2014, 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 2008 On-Line Applications Research Corporation (OAR)
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

/* Generated from spec:/rtems/attr/if/header */

#ifndef _RTEMS_RTEMS_ATTR_H
#define _RTEMS_RTEMS_ATTR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/attr/if/group */

/**
 * @defgroup RTEMSAPIClassicAttr Directive Attributes
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief This group contains the Classic API directive attributes.
 */

/* Generated from spec:/rtems/attr/if/application-task */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API task created
 *   by rtems_task_create() or rtems_task_construct() shall be an application
 *   task.
 */
#define RTEMS_APPLICATION_TASK 0x00000000

/* Generated from spec:/rtems/attr/if/attribute */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This type represents Classic API attributes.
 *
 * @par Notes
 * Attributes are primarily used when creating objects.
 */
typedef uint32_t rtems_attribute;

/* Generated from spec:/rtems/attr/if/barrier-automatic-release */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API barrier
 *   created by rtems_barrier_create() shall use the automatic release
 *   protocol.
 */
#define RTEMS_BARRIER_AUTOMATIC_RELEASE 0x00000200

/* Generated from spec:/rtems/attr/if/barrier-manual-release */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API barrier
 *   created by rtems_barrier_create() shall use the manual release protocol.
 */
#define RTEMS_BARRIER_MANUAL_RELEASE 0x00000000

/* Generated from spec:/rtems/attr/if/binary-semaphore */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API semaphore
 *   created by rtems_semaphore_create() shall be a proper binary semaphore or
 *   mutex.
 */
#define RTEMS_BINARY_SEMAPHORE 0x00000010

/* Generated from spec:/rtems/attr/if/counting-semaphore */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API semaphore
 *   created by rtems_semaphore_create() shall be a counting semaphore.
 */
#define RTEMS_COUNTING_SEMAPHORE 0x00000000

/* Generated from spec:/rtems/attr/if/default */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant represents the default attribute set.
 */
#define RTEMS_DEFAULT_ATTRIBUTES 0x00000000

/* Generated from spec:/rtems/attr/if/fifo */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API object shall
 *   manage blocking tasks using the FIFO discipline.
 */
#define RTEMS_FIFO 0x00000000

/* Generated from spec:/rtems/attr/if/floating-point */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API task created
 *   by rtems_task_create() or rtems_task_construct() shall be able to use the
 *   floating point hardware.
 *
 * @par Notes
 * On some architectures, there will be a floating point context associated
 * with this task.
 */
#define RTEMS_FLOATING_POINT 0x00000001

/* Generated from spec:/rtems/attr/if/global */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API object shall
 *   be a global resource in a multiprocessing network.
 *
 * @par Notes
 * This attribute does not refer to SMP systems.
 */
#define RTEMS_GLOBAL 0x00000002

/* Generated from spec:/rtems/attr/if/inherit-priority */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API semaphore
 *   created by rtems_semaphore_create() shall use the Priority Inheritance
 *   Protocol.
 *
 * @par Notes
 * The semaphore shall be a binary semaphore (#RTEMS_BINARY_SEMAPHORE).
 */
#define RTEMS_INHERIT_PRIORITY 0x00000040

/* Generated from spec:/rtems/attr/if/local */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API object shall
 *   be a local resource in a multiprocessing network.
 *
 * @par Notes
 * This attribute does not refer to SMP systems.
 */
#define RTEMS_LOCAL 0x00000000

/* Generated from spec:/rtems/attr/if/multiprocessor-resource-sharing */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API semaphore
 *   created by rtems_semaphore_create() shall use the Multiprocessor Resource
 *   Sharing Protocol.
 *
 * @par Notes
 * The semaphore shall be a binary semaphore (#RTEMS_BINARY_SEMAPHORE).
 */
#define RTEMS_MULTIPROCESSOR_RESOURCE_SHARING 0x00000100

/* Generated from spec:/rtems/attr/if/no-floating-point */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API task created
 *   by rtems_task_create() or rtems_task_construct() will not use the floating
 *   point hardware.
 *
 * @par Notes
 * If the architecture permits it, then the FPU will be disabled when the task
 * is executing.
 */
#define RTEMS_NO_FLOATING_POINT 0x00000000

/* Generated from spec:/rtems/attr/if/no-inherit-priority */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API semaphore
 *   created by rtems_semaphore_create() will not use the Priority Inheritance
 *   Protocol.
 */
#define RTEMS_NO_INHERIT_PRIORITY 0x00000000

/* Generated from spec:/rtems/attr/if/no-multiprocessor-resource-sharing */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API semaphore
 *   created by rtems_semaphore_create() will not use the Multiprocessor
 *   Resource Sharing Protocol.
 */
#define RTEMS_NO_MULTIPROCESSOR_RESOURCE_SHARING 0x00000000

/* Generated from spec:/rtems/attr/if/no-priority-ceiling */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API semaphore
 *   created by rtems_semaphore_create() will not use the Priority Ceiling
 *   Protocol.
 */
#define RTEMS_NO_PRIORITY_CEILING 0x00000000

/* Generated from spec:/rtems/attr/if/priority */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API object shall
 *   manage blocking tasks using the task priority discipline.
 */
#define RTEMS_PRIORITY 0x00000004

/* Generated from spec:/rtems/attr/if/priority-ceiling */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API semaphore
 *   created by rtems_semaphore_create() shall use the Priority Ceiling
 *   Protocol.
 *
 * @par Notes
 * The semaphore shall be a binary semaphore (#RTEMS_BINARY_SEMAPHORE).
 */
#define RTEMS_PRIORITY_CEILING 0x00000080

/* Generated from spec:/rtems/attr/if/semaphore-class */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant represents the mask of bits associated with
 *   the Classic API semaphore classes #RTEMS_BINARY_SEMAPHORE,
 *   #RTEMS_COUNTING_SEMAPHORE, and #RTEMS_SIMPLE_BINARY_SEMAPHORE.
 */
#define RTEMS_SEMAPHORE_CLASS 0x00000030

/* Generated from spec:/rtems/attr/if/simple-binary-semaphore */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API semaphore
 *   created by rtems_semaphore_create() shall be a simple binary semaphore.
 */
#define RTEMS_SIMPLE_BINARY_SEMAPHORE 0x00000020

/* Generated from spec:/rtems/attr/if/system-task */

/**
 * @ingroup RTEMSAPIClassicAttr
 *
 * @brief This attribute constant indicates that the Classic API task created
 *   by rtems_task_create() or rtems_task_construct() shall be a system task.
 */
#define RTEMS_SYSTEM_TASK 0x00008000

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_ATTR_H */
