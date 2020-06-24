/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines the parts of the Signal Manager API.
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

/* Generated from spec:/rtems/signal/if/header-2 */

#ifndef _RTEMS_RTEMS_ASR_H
#define _RTEMS_RTEMS_ASR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/signal/if/asr */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This type defines the return type of routines which are used to
 *   process asynchronous signals.
 *
 * @par Notes
 * This type can be used to document asynchronous signal routines in the source
 * code.
 */
typedef void rtems_asr;

/* Generated from spec:/rtems/signal/if/set */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This integer type represents a bit field which can hold exactly 32
 *   individual signals.
 */
typedef uint32_t rtems_signal_set;

/* Generated from spec:/rtems/signal/if/asr-entry */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This type defines the prototype of routines which are used to process
 *   asynchronous signals.
 */
typedef rtems_asr ( *rtems_asr_entry )(
  rtems_signal_set
);

/* Generated from spec:/rtems/signal/if/signal-00 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 0.
 */
#define RTEMS_SIGNAL_0 0x00000001

/* Generated from spec:/rtems/signal/if/signal-01 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 1.
 */
#define RTEMS_SIGNAL_1 0x00000002

/* Generated from spec:/rtems/signal/if/signal-02 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 2.
 */
#define RTEMS_SIGNAL_2 0x00000004

/* Generated from spec:/rtems/signal/if/signal-03 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 3.
 */
#define RTEMS_SIGNAL_3 0x00000008

/* Generated from spec:/rtems/signal/if/signal-04 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 4.
 */
#define RTEMS_SIGNAL_4 0x00000010

/* Generated from spec:/rtems/signal/if/signal-05 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 5.
 */
#define RTEMS_SIGNAL_5 0x00000020

/* Generated from spec:/rtems/signal/if/signal-06 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 6.
 */
#define RTEMS_SIGNAL_6 0x00000040

/* Generated from spec:/rtems/signal/if/signal-07 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 7.
 */
#define RTEMS_SIGNAL_7 0x00000080

/* Generated from spec:/rtems/signal/if/signal-08 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 8.
 */
#define RTEMS_SIGNAL_8 0x00000100

/* Generated from spec:/rtems/signal/if/signal-09 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 9.
 */
#define RTEMS_SIGNAL_9 0x00000200

/* Generated from spec:/rtems/signal/if/signal-10 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 10.
 */
#define RTEMS_SIGNAL_10 0x00000400

/* Generated from spec:/rtems/signal/if/signal-11 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 11.
 */
#define RTEMS_SIGNAL_11 0x00000800

/* Generated from spec:/rtems/signal/if/signal-12 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 12.
 */
#define RTEMS_SIGNAL_12 0x00001000

/* Generated from spec:/rtems/signal/if/signal-13 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 13.
 */
#define RTEMS_SIGNAL_13 0x00002000

/* Generated from spec:/rtems/signal/if/signal-14 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 14.
 */
#define RTEMS_SIGNAL_14 0x00004000

/* Generated from spec:/rtems/signal/if/signal-15 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 15.
 */
#define RTEMS_SIGNAL_15 0x00008000

/* Generated from spec:/rtems/signal/if/signal-16 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 16.
 */
#define RTEMS_SIGNAL_16 0x00010000

/* Generated from spec:/rtems/signal/if/signal-17 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 17.
 */
#define RTEMS_SIGNAL_17 0x00020000

/* Generated from spec:/rtems/signal/if/signal-18 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 18.
 */
#define RTEMS_SIGNAL_18 0x00040000

/* Generated from spec:/rtems/signal/if/signal-19 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 19.
 */
#define RTEMS_SIGNAL_19 0x00080000

/* Generated from spec:/rtems/signal/if/signal-20 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 20.
 */
#define RTEMS_SIGNAL_20 0x00100000

/* Generated from spec:/rtems/signal/if/signal-21 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 21.
 */
#define RTEMS_SIGNAL_21 0x00200000

/* Generated from spec:/rtems/signal/if/signal-22 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 22.
 */
#define RTEMS_SIGNAL_22 0x00400000

/* Generated from spec:/rtems/signal/if/signal-23 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 23.
 */
#define RTEMS_SIGNAL_23 0x00800000

/* Generated from spec:/rtems/signal/if/signal-24 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 24.
 */
#define RTEMS_SIGNAL_24 0x01000000

/* Generated from spec:/rtems/signal/if/signal-25 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 25.
 */
#define RTEMS_SIGNAL_25 0x02000000

/* Generated from spec:/rtems/signal/if/signal-26 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 26.
 */
#define RTEMS_SIGNAL_26 0x04000000

/* Generated from spec:/rtems/signal/if/signal-27 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 27.
 */
#define RTEMS_SIGNAL_27 0x08000000

/* Generated from spec:/rtems/signal/if/signal-28 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 28.
 */
#define RTEMS_SIGNAL_28 0x10000000

/* Generated from spec:/rtems/signal/if/signal-29 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 29.
 */
#define RTEMS_SIGNAL_29 0x20000000

/* Generated from spec:/rtems/signal/if/signal-30 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 30.
 */
#define RTEMS_SIGNAL_30 0x40000000

/* Generated from spec:/rtems/signal/if/signal-31 */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief This signal set constant represents the bit in the signal set
 *   associated with signal 31.
 */
#define RTEMS_SIGNAL_31 0x80000000

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_ASR_H */
