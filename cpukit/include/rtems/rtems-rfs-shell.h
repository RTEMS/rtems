/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @ingroup rtems_rfs
 * @brief RTEMS File Systems Shell Commands
 *
 * RTEMS File Systems Shell commands provide a CLI interface to support and
 * development of the RFS file system.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if !defined (_RTEMS_RFS_SHELL_H_)
#define _RTEMS_RFS_SHELL_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * The shell command for the RFS debugger.
 *
 * @param[in] argc is the argument count.
 * @param[in] argv is a pointer to the argument variables.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_shell_debugrfs (int argc, char *argv[]);

/**
 * The shell command for formatting an RFS file system.
 *
 * @param[in] argc is the argument count.
 * @param[in] argv is a pointer to the argument variables.
 *
 * @retval 0 Successful operation.
 * @retval 1 An error occurred.
 */
int rtems_shell_rfs_format (int argc, char* argv[]);

#endif
