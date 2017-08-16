/*
 *  COPYRIGHT (c) 2013-2017 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_fdt
 *
 * @brief RTEMS Flattened Device Tree Shell Command
 *
 * Support for loading, managing and accessing FDT blobs in RTEMS.
 */

#if !defined (_RTEMS_FDT_SHELL_H_)
#define _RTEMS_FDT_SHELL_H_

#include <rtems/rtems-fdt.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Add a shell command to access memory and registers associated with the DTF.
 */
void rtems_fdt_add_shell_command (void);

/**
 * Get a pointer to the handle. You can use this to load files or register
 * blobs and have the shell command access them.
 */
rtems_fdt_handle* rtems_fdt_get_shell_handle (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
