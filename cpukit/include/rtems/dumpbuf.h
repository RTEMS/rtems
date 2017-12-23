/**
 * @file
 *
 * @brief Print a Memory Buffer
 * 
 * This file defines the interface to the RTEMS methods to print a
 * memory buffer in a style similar to many ROM monitors and debuggers.
 */

/*
 *  COPYRIGHT (c) 1997-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __DUMP_BUFFER_h
#define __DUMP_BUFFER_h

/**
 *  @defgroup libmisc_dumpbuf Print Memory Buffer
 *
 *  @ingroup libmisc
 */
/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print memory buffer.
 *
 * This method prints @a length bytes beginning at @a buffer in
 * a nice format similar to what one would expect from a debugger
 * or ROM monitor.
 *
 * @param[in] buffer is the address of the buffer
 * @param[in] length is the length of the buffer
 */
void rtems_print_buffer(
  const unsigned char *buffer,
  int                  length
);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif
/* end of include file */
