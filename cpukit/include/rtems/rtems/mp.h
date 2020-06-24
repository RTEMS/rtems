/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines the Multiprocessing Manager API.
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

/* Generated from spec:/rtems/mp/if/header */

#ifndef _RTEMS_RTEMS_MP_H
#define _RTEMS_RTEMS_MP_H

#include <rtems/score/mpci.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/mp/if/group */

/**
 * @defgroup RTEMSAPIClassicMP Multiprocessing Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Multiprocessing Manager provides support for heterogeneous
 *   multiprocessing systems based on message passing in a network of
 *   multiprocessing nodes.
 *
 * In multiprocessor real-time systems, new requirements, such as sharing data
 * and global resources between processors, are introduced.  This requires an
 * efficient and reliable communications vehicle which allows all processors to
 * communicate with each other as necessary.  In addition, the ramifications of
 * multiple processors affect each and every characteristic of a real-time
 * system, almost always making them more complicated.
 *
 * RTEMS addresses these issues by providing simple and flexible real-time
 * multiprocessing capabilities.  The executive easily lends itself to both
 * tightly-coupled and loosely-coupled configurations of the target system
 * hardware.  In addition, RTEMS supports systems composed of both homogeneous
 * and heterogeneous mixtures of processors and target boards.
 *
 * A major design goal of the RTEMS executive was to transcend the physical
 * boundaries of the target hardware configuration.  This goal is achieved by
 * presenting the application software with a logical view of the target system
 * where the boundaries between processor nodes are transparent.  As a result,
 * the application developer may designate objects such as tasks, queues,
 * events, signals, semaphores, and memory blocks as global objects.  These
 * global objects may then be accessed by any task regardless of the physical
 * location of the object and the accessing task.  RTEMS automatically
 * determines that the object being accessed resides on another processor and
 * performs the actions required to access the desired object.  Simply stated,
 * RTEMS allows the entire system, both hardware and software, to be viewed
 * logically as a single system.
 */

/* Generated from spec:/rtems/mp/if/announce */

/**
 * @ingroup RTEMSAPIClassicMP
 *
 * @brief Announces the arrival of a packet.
 *
 * This directive informs RTEMS that a multiprocessing communications packet
 * has arrived from another node.  This directive is called by the
 * user-provided MPCI, and is only used in multiprocessing configurations.
 *
 * @par Notes
 * @parblock
 * This directive is typically called from an ISR.
 *
 * This directive does not generate activity on remote nodes.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may unblock another task which may preempt the calling task.
 * @endparblock
 */
void rtems_multiprocessing_announce( void );

/* Generated from spec:/rtems/mp/if/minimum-hetero-conversion */

/**
 * @ingroup RTEMSAPIClassicMP
 *
 * @brief This constant defines the count of uint32_t numbers in a packet which
 *   must be converted to native format in a heterogeneous system.
 *
 * In packets longer than this value, some of the extra data may be a user
 * message buffer which is not automatically endian swapped.
 */
#define RTEMS_MINIMUN_HETERO_CONVERSION MP_PACKET_MINIMUN_HETERO_CONVERSION

/* Generated from spec:/rtems/mp/if/minimum-packet-size */

/**
 * @ingroup RTEMSAPIClassicMP
 *
 * @brief This constant defines the minimum packet size which must be supported
 *   by the MPCI.
 */
#define RTEMS_MINIMUM_PACKET_SIZE MP_PACKET_MINIMUM_PACKET_SIZE

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_MP_H */
