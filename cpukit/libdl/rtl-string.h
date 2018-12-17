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
 * @brief RTEMS Run-Time Linker String managment.
 */

#if !defined (_RTEMS_RTL_STRING_H_)
#define _RTEMS_RTL_STRING_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * A string duplicate that uses the RTL allocator.
 *
 * @param s1 The string to duplicate.
 * @return char* The copy of the string. NULL if there is no memory.
 */
char* rtems_rtl_strdup (const char *s1);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
