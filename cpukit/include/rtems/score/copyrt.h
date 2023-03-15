/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCopyright
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreCopyright.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#ifndef _RTEMS_SCORE_COPYRT_H
#define _RTEMS_SCORE_COPYRT_H

/**
 * @defgroup RTEMSScoreCopyright Copyright Notice
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the implementation to provide a copyright notice
 *   and the RTEMS version.
 *
 * @{
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This is the copyright string for RTEMS.
 */
extern const char _Copyright_Notice[];

/**
 * @brief This constant provides the RTEMS version string.
 *
 * The constant name does not follow the naming conventions.  Do not change it
 * for backward compatibility reasons.
 *
 * @see rtems_get_version_string()
 */
extern const char _RTEMS_version[];

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
