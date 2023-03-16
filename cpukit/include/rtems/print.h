/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSPrintSupport
 *
 * @brief User print interface to the bspIO print plug in.
 *
 * This include file defines the user interface to kernel print methods.
 */

/*
 *  Copyright (c) 2016 Chris Johns <chrisj@rtems.org>
 *  All rights reserved.
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

#ifndef _RTEMS_PRINT_H
#define _RTEMS_PRINT_H

#include <rtems/score/basedefs.h>

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtems_printer rtems_printer;

/**
 * @defgroup RTEMSPrintSupport RTEMS Print Support
 *
 * @ingroup RTEMSAPIPrintSupport
 *
 * This module contains all methods and support related to providing the user
 * with an interface to the kernel level print support.
 *
 * @{
 */

/**
 * @brief Print to the kernel plugin handler. This has to be a macro because
 * there is no vprint version of the plug in handlers.
 *
 * @param[in] printer Pointer to the printer structure.
 * @param[in] fmt Print format string.
 * @param[in] ... Print variable argument list.
 *
 * @return int Number of characters printed.
 */
extern int rtems_printf(const rtems_printer *printer,
                        const char          *format,
                        ...) RTEMS_PRINTFLIKE(2, 3);

/**
 * @brief Print to the kernel plugin handler. This has to be a macro because
 * there is no vprint version of the plug in handlers.
 *
 * @param[in] printer Pointer to the printer structure.
 * @param[in] fmt Print format string.
 * @param[in] ap Print variable argument list pointer.
 *
 * @return int Number of characters printed.
 */
extern int rtems_vprintf(const rtems_printer *printer,
                         const char          *format,
                         va_list              ap);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_PRINT_H */
