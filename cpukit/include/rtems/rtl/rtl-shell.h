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
 * @brief RTEMS Run-Time Linker ELF Shell Support.
 */

#if !defined (_RTEMS_RTL_SHELL_H_)
#define _RTEMS_RTL_SHELL_H_

#include <rtems/print.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * The RTL single shell command contains sub-commands.
 *
 * @param argc The argument count.
 * @param argv Array of argument strings.
 * @retval 0 No error.
 * @return int The exit code.
 */
int rtems_rtl_shell_command (int argc, char* argv[]);

/**
 * List object files.
 */
int rtems_rtl_shell_list (const rtems_printer* printer, int argc, char* argv[]);

/**
 * Symbols.
 */
int rtems_rtl_shell_sym (const rtems_printer* printer, int argc, char* argv[]);

/**
 * Object files.
 */
int rtems_rtl_shell_object (const rtems_printer* printer, int argc, char* argv[]);


/**
 * Archive files.
 */
int rtems_rtl_shell_archive (const rtems_printer* printer, int argc, char* argv[]);

/**
 * Call text symbol.
 */
int rtems_rtl_shell_call (const rtems_printer* printer, int argc, char* argv[]);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
