/**
 * @file
 *
 * @brief User print interface to the bspIO print plug in.
 *
 * This include file defines the user interface to kernel print methods.
 */

/*
 *  Copyright (c) 2016 Chris Johns <chrisj@rtems.org>
 *  All rights reserved.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
