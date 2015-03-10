/**
 * @file rtems/bspIo.h
 *
 * @brief Interface to Kernel Print Methods
 *
 * This include file defines the interface to kernel print methods.
 */

/*
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
 *  COPYRIGHT (c) 2011 On-Line Applications Research Corporation.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_BSPIO_H
#define _RTEMS_BSPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup BSPIO Kernel Print Support
 *
 * This module contains all methods and support related to providing
 * kernel level print support.
 *
 * The following variables below are declared as extern and
 * MUST be declared and initialized in each BSP. Using this indirect
 * function, the functionality in this group is tailored for the BSP.
 *
 *  - BSP_output_char
 *  - BSP_poll_char
 */

/**
 * This type defines the prototype for the BSP provided method to
 * print a single character. It is assumed to be polled.
 */
typedef void 	(*BSP_output_char_function_type) 	(char c);

/**
 * This type defines the prototype for the BSP provided method to
 * input a single character. It is assumed to be polled.
 */
typedef int 	(*BSP_polling_getchar_function_type) 	(void);

/**
 * This variable points to the BSP provided method to output a
 * character for the purposes of debug output.
 */
extern 	BSP_output_char_function_type 		BSP_output_char;

/**
 * This variable points to the BSP provided method to input a
 * character for the purposes of debug input.
 */
extern 	BSP_polling_getchar_function_type 	BSP_poll_char;

#include <stdarg.h>

/**
 * @brief Get Character (kernel I/O)
 *
 * This method polls for a key in the simplest possible fashion
 * from whatever the debug console device is.
 *
 * @return If a character is available, it is returned.  Otherwise
 *         this method returns -1.
 *
 * @note This method uses the BSP_poll_char pointer to a BSP
 *       provided method.
 */
extern int getchark(void);

/**
 * @brief Variable Argument printk()
 *
 * This method allows the user to access printk() functionality
 * with a va_list style argument.
 *
 * @param[in] fmt is a printf()-style format string
 * @param[in] ap is a va_list pointer to arguments
 */
extern void vprintk(const char *fmt, va_list ap);

/**
 * @brief Kernel Print
 *
 * This method allows the user to perform a debug printk().
 *
 * @param[in] fmt is a printf()-style format string
 */
extern void printk(const char *fmt, ...);

/**
 * @brief Kernel Put String
 *
 * This method allows the user to perform a debug puts().
 *
 * @param[in] s is the string to print
 */
extern void putk(const char *s);

/**
 * @brief Kernel Put Character
 *
 * This method allows the user to perform a debug putc().
 *
 * @param[in] c is the character to print
 */
extern void rtems_putc(char c);

/**
 * Type definition for function which can be plugged in to
 * certain reporting routines to redirect the output.
 *
 * Methods following this prototype may be passed into RTEMS reporting
 * functions that allow their output to be redirected.  In particular,
 * the cpu usage, period usage, and stack usage reporting
 * functions use this.
 *
 * If the user provides their own "printf plugin", then they may
 * redirect those reports as they see fit.
 */
typedef int (*rtems_printk_plugin_t)(void *, const char *format, ...);

/**
 * @brief Reporting Methods printk() Plugin
 *
 * @param[in] context points to a user defined context.
 * @param[in] fmt is a printf()-style format string
 *
 * @return The number of characters printed.
 */
extern int printk_plugin(void *context, const char *fmt, ...);

/**
 * @brief Reporting Methods printf() Plugin
 *
 * This is a standard plug-in to support using printf() for output
 * instead of printk().
 *
 * @param[in] context points to a user defined context.
 * @param[in] fmt is a printf()-style format string
 *
 * @return The number of characters printed.
 */
extern int rtems_printf_plugin(void *context, const char *fmt, ...);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
