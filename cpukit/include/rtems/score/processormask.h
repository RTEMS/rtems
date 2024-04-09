/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreProcessorMask
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreProcessorMask.
 */

/*
 * Copyright (C) 2016, 2017 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_PROCESSORMASK_H
#define _RTEMS_SCORE_PROCESSORMASK_H

#include <rtems/score/cpu.h>

#include <sys/_bitset.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Recent Newlib versions provide the bitset defines in the system reserved
 * namespace.
 */
#ifndef __BIT_AND2
#define __BIT_AND2 BIT_AND2
#endif
#ifndef __BIT_CLR
#define __BIT_CLR BIT_CLR
#endif
#ifndef __BIT_CMP
#define __BIT_CMP BIT_CMP
#endif
#ifndef __BIT_COPY
#define __BIT_COPY BIT_COPY
#endif
#ifndef __BIT_COUNT
#define __BIT_COUNT BIT_COUNT
#endif
#ifndef __BITSET_DEFINE
#define __BITSET_DEFINE BITSET_DEFINE
#endif
#ifndef __BIT_EMPTY
#define __BIT_EMPTY BIT_EMPTY
#endif
#ifndef __BIT_FILL
#define __BIT_FILL BIT_FILL
#endif
#ifndef __BIT_FLS
#define __BIT_FLS BIT_FLS
#endif
#ifndef __BIT_ISSET
#define __BIT_ISSET BIT_ISSET
#endif
#ifndef __BIT_OR2
#define __BIT_OR2 BIT_OR2
#endif
#ifndef __BIT_OVERLAP
#define __BIT_OVERLAP BIT_OVERLAP
#endif
#ifndef __BIT_SET
#define __BIT_SET BIT_SET
#endif
#ifndef __BIT_SETOF
#define __BIT_SETOF BIT_SETOF
#endif
#ifndef __BIT_SUBSET
#define __BIT_SUBSET BIT_SUBSET
#endif
#ifndef __BIT_XOR2
#define __BIT_XOR2 BIT_XOR2
#endif
#ifndef __BIT_ZERO
#define __BIT_ZERO BIT_ZERO
#endif

/**
 * @defgroup RTEMSScoreProcessorMask Processor Mask
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the implementation to support processor masks.
 *
 * The processor mask provides a bit map large enough to provide one bit for
 * each processor in the system.  It is a fixed size internal data type
 * provided for efficiency in addition to the API level cpu_set_t.
 *
 * @{
 */

/**
 * @brief A bit map which is large enough to provide one bit for each processor
 * in the system.
 */
typedef __BITSET_DEFINE( Processor_mask, CPU_MAXIMUM_PROCESSORS ) Processor_mask;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_PROCESSORMASK_H */
