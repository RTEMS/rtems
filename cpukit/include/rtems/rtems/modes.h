/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file provides the task modes API of the Task Manager.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

/* Generated from spec:/rtems/mode/if/header */

#ifndef _RTEMS_RTEMS_MODES_H
#define _RTEMS_RTEMS_MODES_H

#include <stdint.h>
#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/mode/if/group */

/**
 * @defgroup RTEMSAPIClassicModes Task Modes
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief This group contains the Classic API task modes.
 */

/* Generated from spec:/rtems/mode/if/all-mode-masks */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant is a bit mask with all mode bits set.
 */
#define RTEMS_ALL_MODE_MASKS 0x0000ffff

/* Generated from spec:/rtems/mode/if/asr */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant indicates that signal processing is disabled.
 */
#define RTEMS_ASR 0x00000000

/* Generated from spec:/rtems/mode/if/asr-mask */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This mode constant corresponds to the signal enable/disable bit.
 */
#define RTEMS_ASR_MASK 0x00000400

/* Generated from spec:/rtems/mode/if/current-mode */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant indicates that the current task mode of the
 *   executing task shall be returned by rtems_task_mode().
 */
#define RTEMS_CURRENT_MODE 0

/* Generated from spec:/rtems/mode/if/default */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant represents the default mode set.
 */
#define RTEMS_DEFAULT_MODES 0x00000000

/* Generated from spec:/rtems/mode/if/interrupt-mask */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant corresponds to the interrupt enable/disable
 *   bits.
 */
#define RTEMS_INTERRUPT_MASK CPU_MODES_INTERRUPT_MASK

/* Generated from spec:/rtems/mode/if/interrupt-level */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief Maps the interrupt level to the associated processor-dependent task
 *   mode interrupt level.
 *
 * @param _interrupt_level is the interrupt level to map.
 *
 * @return Returns the processor-dependent task mode interrupt level associated
 *   with the interrupt level.
 *
 * @par Notes
 * The Classic API supports 256 interrupt levels using the least significant
 * eight bits of the mode set.  On any particular processor variant, fewer than
 * 256 levels may be supported.  At least level 0 (all interrupts enabled) and
 * level 1 (interrupts disabled, on most architectures) are supported.
 */
#define RTEMS_INTERRUPT_LEVEL( _interrupt_level ) \
  ( ( _interrupt_level ) & RTEMS_INTERRUPT_MASK )

/* Generated from spec:/rtems/mode/if/interrupt-mask-constant */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant has the same value as #RTEMS_INTERRUPT_MASK.
 *
 * @par Notes
 * This task mode constant is used by bindings from languages other than C and
 * C++.
 */
extern const uint32_t rtems_interrupt_mask;

/* Generated from spec:/rtems/mode/if/mode */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This type represents a Classic API task mode set.
 */
typedef uint32_t rtems_mode;

/* Generated from spec:/rtems/mode/if/interrupt-level-body */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief Maps the interrupt level to the associated processor-dependent task
 *   mode interrupt level.
 *
 * @param level is the interrupt level to map.
 *
 * @return Returns RTEMS_INTERRUPT_LEVEL() for the interrupt level.
 *
 * @par Notes
 * This function is used by bindings from languages other than C and C++.
 */
rtems_mode rtems_interrupt_level_body( uint32_t level );

/* Generated from spec:/rtems/mode/if/no-asr */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant indicates that signal processing is disabled.
 */
#define RTEMS_NO_ASR 0x00000400

/* Generated from spec:/rtems/mode/if/no-preempt */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant indicates that preemption is disabled.
 */
#define RTEMS_NO_PREEMPT 0x00000100

/* Generated from spec:/rtems/mode/if/no-timeslice */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant indicates that timeslicing is disabled.
 */
#define RTEMS_NO_TIMESLICE 0x00000000

/* Generated from spec:/rtems/mode/if/preempt */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant indicates that preemption is enabled.
 */
#define RTEMS_PREEMPT 0x00000000

/* Generated from spec:/rtems/mode/if/preempt-mask */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant corresponds to the preemption enable/disable
 *   bit.
 */
#define RTEMS_PREEMPT_MASK 0x00000100

/* Generated from spec:/rtems/mode/if/timeslice */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant indicates that timeslicing is enabled.
 */
#define RTEMS_TIMESLICE 0x00000200

/* Generated from spec:/rtems/mode/if/timeslice-mask */

/**
 * @ingroup RTEMSAPIClassicModes
 *
 * @brief This task mode constant corresponds to the timeslice enable/disable
 *   bit.
 */
#define RTEMS_TIMESLICE_MASK 0x00000200

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_MODES_H */
