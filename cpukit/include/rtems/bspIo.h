/* bspIo.h
 *
 *  This include file contains declaration of interface that
 *  will be provided by the file contained in this directory.
 *
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
#ifndef _RTEMS_BSPIO_H
#define _RTEMS_BSPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * All the functions declared as extern after this comment
 * MUST be implemented in each BSP. Using this function,
 * this directory contains shared code that export higher level
 * functionnality described after the next command.
 */
typedef void 	(*BSP_output_char_function_type) 	(char c);
typedef char 	(*BSP_polling_getchar_function_type) 	(void);

extern 	BSP_output_char_function_type 		BSP_output_char;
extern 	BSP_polling_getchar_function_type 	BSP_poll_char;

/*
 * All the function declared as extern after this comment
 * are available for each BSP by compiling and linking
 * the files contained in this directory PROVIDED definition
 * and initialisation of the previous variable are done.
 */
#include <stdarg.h>
extern void vprintk(char *fmt, va_list ap);
extern void printk(char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
