/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplShell
 *
 * @brief This header file provides the internal interfaces of the shell.
 */

/*
 * Copyright (C) 2008, 2014 embedded brains GmbH & Co. KG
 * Copyright (C) 2007, 2013 On-Line Applications Research Corporation (OAR)
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

#ifndef _RTEMS_SHELL_INTERNAL_H
#define _RTEMS_SHELL_INTERNAL_H

#include <rtems/shell.h>

/**
 * @defgroup RTEMSImplShell Shell
 *
 * @ingroup RTEMSImpl
 *
 * @brief This group contains the shell implementation.
 *
 * @{
 */

/**
 * @defgroup RTEMSImplShellCommands Shell Commands
 *
 * @ingroup RTEMSImplShell
 *
 * @brief This group contains the shell commands.
 */

/**
 * @defgroup RTEMSImplShellUtilities Shell Utilities
 *
 * @ingroup RTEMSImplShell
 *
 * @brief This group contains the utility code which the shell commands use.
 */

extern rtems_shell_cmd_t   * rtems_shell_first_cmd;
extern rtems_shell_topic_t * rtems_shell_first_topic;

extern void rtems_shell_register_monitor_commands(void);

extern void rtems_shell_print_heap_info(
  const char             *c,
  const Heap_Information *h
);

extern void rtems_shell_print_heap_stats(
  const Heap_Statistics *s
);

extern void rtems_shell_print_unified_work_area_message(void);

#include <sys/types.h>

extern void strmode(mode_t mode, char *p);
extern const char *user_from_uid(uid_t uid, int nouser);
extern char *group_from_gid(gid_t gid, int nogroup);

/** @} */

#endif
