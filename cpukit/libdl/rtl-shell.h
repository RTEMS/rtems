/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker ELF Shell Support.
 */

#if !defined (_RTEMS_RTL_SHELL_H_)
#define _RTEMS_RTL_SHELL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

/**
 * The RTL single shell command contains sub-commands.
 *
 * @param argc The argument count.
 * @param argv Array of argument strings.
 * @retval 0 No error.
 * @return int The exit code.
 */
int rtems_rtl_shell_command (int argc, char* argv[]);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
