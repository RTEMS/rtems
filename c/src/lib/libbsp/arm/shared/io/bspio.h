/* bspIo.h
 *
 *  This include file contains declaration of interface that
 *  will be provided by the file contained in this directory.
 *
 *
 *  COPYRIGHT (c) 2000 Canon Research France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
#ifndef _LIBBSP_ARM_SHARED_IO_BSP_IO_H
#define _LIBBSP_ARM_SHARED_IO_BSP_IO_H

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
 * are available for each ix86 BSP by compiling and linking
 * the files contained in this directory PROVIDED definition
 * and initialisation of the previous variable are done.
 */
void printk(char *fmt, ...);

#endif
