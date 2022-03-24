/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BSPCommandLine
 *
 * @brief BSP Command Line Handler
 *
 * This include file contains all prototypes and specifications
 * related to the BSP Command Line String and associated helper
 * routines. The helpers are useful for locating command line
 * type arguments (e.g. --mode) and their associated right
 * hand side (e.g. FAST in --mode=FAST).
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef __BSP_COMMAND_LINE_h
#define __BSP_COMMAND_LINE_h

/**
 * @defgroup BSPCommandLine BSP Command Line Helpers
 *
 * @ingroup RTEMSAPIClassic
 *
 * The BSP Command Line Handler provides a set of routines which assist
 * in examining and decoding the Command Line String passed to the BSP
 * at boot time.
 */
/**@{*/

#include <stddef.h> /* for size_t */

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Obtain Pointer to BSP Boot Command String
 *
 * This method returns a pointer to the BSP Boot Command String. It
 * is as likely to be NULL as point to a string as most BSPs do not
 * have a start environment that provides a boot string.
 *
 * @retval This method returns the pointer to the BSP Boot Command String.
 */
const char *rtems_bsp_cmdline_get(void);

/**
 * @brief Obtain COPY of the Entire Matching Argument
 *
 * This method searches for the argument @a name in the BSP Boot Command
 * String and returns a copy of the entire string associated with it in
 * @a value up to a string of @a length. This will include the argument
 * and any right hand side portion of the string. For example, one might
 * be returned --mode=FAST if
 * searching for --mode.
 *
 * @param[in] name is the arugment to search for
 * @param[in] value points to where the contents will
 *            be placed if located.
 * @param[in] length is the maximum length to copy
 *
 * @return This method returns NULL if not found and
 *         @a value if found.
 */
const char *rtems_bsp_cmdline_get_param(
  const char *name,
  char       *value,
  size_t      length
);


/**
 * @brief Obtain COPY of the Right Hand Side of the Matching Argument
 *
 * This method searches for the argument @a name in
 * the BSP Boot Command String and returns the right hand side
 * associated with it in @a value up to a maximum string @a length.
 * This will NOT include the argument but only any right hand side
 * portion of the string. *  For example, one might be returned FAST if
 * searching for --mode.
 *
 * @param[in] name is the arugment to search for
 * @param[in] value points to where the contents will
 *            be placed if located.
 * @param[in] length is the maximum length to copy
 *
 * @retval This method returns NULL if not found and
 *         @a value if found.
 */
const char *rtems_bsp_cmdline_get_param_rhs(
  const char *name,
  char       *value,
  size_t      length
);

/**
 * @brief Obtain Pointer to the Entire Matching Argument
 *
 * This method searches for the argument @a name in
 * the BSP Boot Command String and returns a pointer to the
 * entire string associated with it. This will include the
 * argument and any right hand side portion of the string.
 * For example, one might be returned --mode=FAST if
 * searching for --mode.
 *
 * @param[in] name is the arugment to search for
 *
 * @retval This method returns NULL if not found and a pointer
 *         into the BSP Boot Command String if found.
 *
 * @note The pointer will be to the original BSP Command
 *       Line string. Exercise caution when using this.
 */
const char *rtems_bsp_cmdline_get_param_raw(
  const char *name
);

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
