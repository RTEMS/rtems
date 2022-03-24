/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Interface to the POSIX utsname() Service
 *
 * This include file defines the interface to the POSIX utsname() service.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#ifndef __POSIX_SYS_UTSNAME_h
#define __POSIX_SYS_UTSNAME_h

/**
 * @defgroup UTSNAME utsname Service
 * 
 * @ingroup POSIXAPI
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  4.4.1 Get System Name (Table 4-1), P1003.1b-1993, p. 90
 *
 *  NOTE:  The lengths of the strings in this structure are
 *         just long enough to reliably contain the RTEMS information.
 *         For example, the fields are not long enough to support
 *         Internet hostnames.
 */

#ifdef _KERNEL
#define SYS_NMLN        48              /* uname(2) for the FreeBSD 1.1 ABI. */
#endif

#ifndef SYS_NMLN
#define SYS_NMLN        48		/* User can override. */
#endif

struct utsname {
  char sysname[SYS_NMLN];  /* Name of this implementation of the */
                           /*   operating system */
  char nodename[SYS_NMLN]; /* Name of this node within an implementation */
                           /*   specified communication network */
  char release[SYS_NMLN];  /* Current release level of this implementation */
  char version[SYS_NMLN];  /* Current version level of this release */
  char machine[SYS_NMLN];  /* Name of the hardware type on which the system */
                           /*   is running */
};

/**
 * @brief Get system name.
 * 
 * 4.4.1 Get System Name, P1003.1b-1993, p. 90
 */
int uname(
  struct utsname *name
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
