/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Error
 */

#if !defined (_RTEMS_RTL_ERROR_H_)
#define _RTEMS_RTL_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if __GNUC__
#define RTEMS_RTL_PRINTF_ATTR   __attribute__((__format__(__printf__,2,3)))
#else
#define RTEMS_RTL_PRINTF_ATTR
#endif

/**
 * Sets the error.
 *
 * Assumes the RTL has been locked.
 *
 * @param error The errno error number.
 * @param format The error format string.
 * @param ... The variable arguments that depend on the format string.
 */
void rtems_rtl_set_error (int error, const char* format, ...) RTEMS_RTL_PRINTF_ATTR;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
