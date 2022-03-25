/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
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
