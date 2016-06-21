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

#ifndef _RTEMS_PRINTER_H
#define _RTEMS_PRINTER_H

#include <rtems/print.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup XXX
 *
 * @{
 */

/**
 * @defgroup RTEMSPrintSupport
 */

/**
 * Type definition for function which can be plugged in to certain reporting
 * routines to redirect the output.
 *
 * Use the RTEMS Print interface to call these functions. Do not directly use
 * them.
 *
 * If the user provides their own printer, then they may redirect those reports
 * as they see fit.
 */
typedef int (*rtems_print_printer)(void *, const char *format, va_list ap);

/**
 * Type definition for the printer structure used to access the kernel print
 * support.
 */
struct rtems_printer {
  void                *context;
  rtems_print_printer  printer;
};

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
 * @brief Initializes the rtems_printer struct to empty.
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
 * @brief Initializes the rtems_printer struct to printk
 *
 * The printer will output to the kernel printk support.
 *
 * @param[in] printer Pointer to the printer structure.
 */
void rtems_print_printer_printk(rtems_printer *printer);

/**
 * @brief Initializes the rtems_printer struct to printf
 *
 * The printer will output to the libc printf support.
 *
 * @param[in] printer Pointer to the printer structure.
 */
extern void rtems_print_printer_printf(rtems_printer *printer);

/**
 * @brief Initializes the rtems_printer struct to a fprintf device.
 *
 * The printer will output to the libc fprintf file provided.
 *
 * @param[in] printer Pointer to the printer structure.
 */
extern void rtems_print_printer_fprintf(rtems_printer *printer, FILE *file);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_PRINTER_H */
