/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains functions required by GCC if code and
 *   branch coverage instrumentation (gcov) is enabled.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>

#include <errno.h>

#include <rtems/seterr.h>

int __gcov_execl( const char *, char *, ... );

int __gcov_execl( const char *path, char *arg, ... )
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

int __gcov_execlp( const char *, char *, ... );

int __gcov_execlp( const char *path, char *arg, ... )
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

int __gcov_execle( const char *, char *, ... );

int __gcov_execle( const char *path, char *arg, ... )
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

int __gcov_execv( const char *, char *const[] );

int __gcov_execv( const char *path, char *const argv[] )
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

int __gcov_execvp( const char *, char *const[] );

int __gcov_execvp( const char *path, char *const argv[] )
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

int __gcov_execve( const char *, char *const[], char *const[] );

int __gcov_execve( const char *path, char *const argv[], char *const envp[] )
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

pid_t __gcov_fork( void );

pid_t __gcov_fork( void )
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
