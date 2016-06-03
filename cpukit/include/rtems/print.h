/**
 * @file rtems/print.h
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

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <rtems/bspIo.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS Print Support
 *
 * This module contains all methods and support related to providing the user
 * with an interface to the kernel level print support.
 */

/**
 * Type definition for the printer structure used to access the kernel print
 * support.
 */
typedef struct rtems_printer {
  void                *context;
  rtems_print_plugin_t printer;
} rtems_printer;

/**
 * @brief check if the printer is valid.
 *
 * @param[in] printer Pointer to the printer structure.
 *
 * @return true The printer is valid else false is returned.
 */
static inline bool rtems_print_printer_valid(const rtems_printer *printer)
{
  return printer != NULL && printer->printer != NULL;
}

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
                        ...) RTEMS_PRINTF_ATTRIBUTE(2, 3);

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

/**
 * @brief Intiialise the rtems_printer struct to empty.
 *
 * An empty printer prints nothing. You can use this to implement an enable and
 * disable type print implementation.
 *
 * @param[in] printer Pointer to the printer structure.
 */
static inline void rtems_print_printer_empty(rtems_printer *printer)
{
  printer->context = NULL;
  printer->printer = NULL;
}

/**
 * @brief Intiialise the rtems_printer struct to printk
 *
 * The printer will output to the kernel printk support.
 *
 * @param[in] printer Pointer to the printer structure.
 */
void rtems_print_printer_printk(rtems_printer *printer);

/**
 * @brief Intiialise the rtems_printer struct to printf
 *
 * The printer will output to the libc printf support.
 *
 * @param[in] printer Pointer to the printer structure.
 */
extern void rtems_print_printer_printf(rtems_printer *printer);

/**
 * @brief Intiialise the rtems_printer struct to a fprintf device.
 *
 * The printer will output to the libc fprintf file provided.
 *
 * @param[in] printer Pointer to the printer structure.
 */
extern void rtems_print_printer_fprintf(rtems_printer *printer, FILE *file);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
