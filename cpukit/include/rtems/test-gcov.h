/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplGcov
 *
 * @brief This header file provides the interfaces of the @ref RTEMSImplGcov.
 */

/*
 * Copyright (C) 2013, 2014 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_TEST_GCOV_H
#define _RTEMS_TEST_GCOV_H

#include <gcov.h>

#include <rtems/linkersets.h>
#include <rtems/dev/io.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSImplGcov Gcov Support
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief This group contains the gocv support.
 *
 * @{
 */

RTEMS_LINKER_ROSET_DECLARE( gcov_info, const struct gcov_info * );

/**
 * @brief Dumps the gcov information as a binary gcfn and gcda data
 *   stream using the put character handler.
 *
 * @param put_char is the put character handler used to output the data stream.
 *
 * @param arg is the argument passed to the put character handler.
 */
void _Gcov_Dump_info( IO_Put_char put_char, void *arg );

/**
 * @brief Dumps the gcov information as a base64 encoded gcfn and gcda data
 *   stream using the put character handler.
 *
 * @param put_char is the put character handler used to output the data stream.
 *
 * @param arg is the argument passed to the put character handler.
 */
void _Gcov_Dump_info_base64( IO_Put_char put_char, void *arg );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_TEST_GCOV_H */
