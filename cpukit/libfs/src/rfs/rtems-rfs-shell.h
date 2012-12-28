/**
 * @file
 *
 * 
 * @ingroup rtems-rfs
 * @brief RTEMS File Systems Shell Commands
 *
 * RTEMS File Systems Shell commands provide a CLI interface to support and
 * development of the RFS file system.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if !defined (_RTEMS_RFS_SHELL_H_)
#define _RTEMS_RFS_SHELL_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * The shell command for the RFS debugger.
 *
 * @param argc The argument count.
 * @param argv The argument variables.
 * @return int The exit code for the command. A 0 is no error.
 */
int rtems_shell_debugrfs (int argc, char *argv[]);

/**
 * The shell command for formatting an RFS file system.
 *
 * @param argc The argument count.
 * @param argv The argument variables.
 * @return int The exit code for the command. A 0 is no error.
 */
int rtems_shell_rfs_format (int argc, char* argv[]);

#endif
