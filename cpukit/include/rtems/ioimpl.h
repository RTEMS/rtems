/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIO
 *
 * @brief This header file provides interfaces used by the I/O Manager
 *   implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_IOIMPL_H
#define _RTEMS_IOIMPL_H

#include <rtems/io.h>
#include <rtems/score/isrlock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSImplClassicIO I/O Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the I/O Manager implementation.
 *
 * @{
 */

extern const size_t _IO_Number_of_drivers;

extern rtems_driver_address_table _IO_Driver_address_table[];

extern bool _IO_All_drivers_initialized;

/**
 * @brief Initialization of all device drivers.
 *
 * Initializes all device drivers.
 */
void _IO_Initialize_all_drivers( void );

ISR_LOCK_DECLARE( extern, _IO_Driver_registration_lock )

static inline void _IO_Driver_registration_acquire(
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_ISR_disable_and_acquire(
    &_IO_Driver_registration_lock,
    lock_context
  );
}

static inline void _IO_Driver_registration_release(
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release_and_ISR_enable(
    &_IO_Driver_registration_lock,
    lock_context
  );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_IOIMPL_H */
