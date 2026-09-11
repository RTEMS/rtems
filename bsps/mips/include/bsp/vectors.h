/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMIPSShared
 *
 * @brief This header file provides the exception handler registration.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_MIPS_SHARED_VECTORS_H
#define LIBBSP_MIPS_SHARED_VECTORS_H

#include <rtems.h>
#include <rtems/score/cpuimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSBSPsMIPSSharedException MIPS Exception Support
 *
 * @ingroup RTEMSBSPsMIPSShared
 *
 * @brief This group contains the exception handler registration of the MIPS
 *   port.
 *
 * The cause register of coprocessor 0 names the exception with a code of
 * five bits.  A handler occupies the entry of its code.  The registration is
 * separate from the interrupt manager, so that the handler takes the
 * exception frame as an argument.
 *
 * @{
 */

/**
 * @brief This constant defines the number of exception codes of the
 *   architecture.
 */
#define MIPS_EXCEPTION_COUNT 32

/**
 * @brief This type defines the exception handler.
 *
 * @param frame is the frame of the exception.  A handler may change it.  The
 *   vector restores the frame before it returns to the interrupted context.
 */
typedef void ( *mips_exc_handler_t )( CPU_Exception_frame *frame );

/**
 * @brief Installs the handler of the exception code.
 *
 * @param vector is the exception code.  It shall be less than
 *   #MIPS_EXCEPTION_COUNT.
 *
 * @param handler is the new handler.  A NULL pointer restores the default
 *   handler, which terminates the system.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID The exception code was out of range.
 */
rtems_status_code mips_exc_set_handler(
  unsigned int       vector,
  mips_exc_handler_t handler
);

/**
 * @brief Gets the handler of the exception code.
 *
 * @param vector is the exception code.
 *
 * @return Returns the handler of the exception code, or NULL where the code
 *   is out of range or carries the default handler.
 */
mips_exc_handler_t mips_exc_get_handler( unsigned int vector );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_MIPS_SHARED_VECTORS_H */
