/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief FTP Server Information
 */

/*
 * Copyright (C) 1999 Jake Janovetz <jvanovetz@uiuc.edu>
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

#ifndef _RTEMS_FTPD_H
#define _RTEMS_FTPD_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FTPD_CONTROL_PORT   21

/* Various buffer sizes */
enum {
  FTPD_BUFSIZE  = 256,       /* Size for temporary buffers */
  FTPD_DATASIZE = 4 * 1024,      /* Size for file transfer buffers */
  FTPD_STACKSIZE = RTEMS_MINIMUM_STACK_SIZE + FTPD_DATASIZE /* Tasks stack size */
};

/* FTPD access control flags */
enum
{
  FTPD_NO_WRITE = 0x1,
  FTPD_NO_READ  = 0x2,
  FTPD_NO_RW    = FTPD_NO_WRITE | FTPD_NO_READ
};

typedef int (*rtems_ftpd_hookfunction)(char *, size_t);

#include <rtems/shell.h>

struct rtems_ftpd_hook
{
   char                    *filename;
   rtems_ftpd_hookfunction hook_function;
};

struct rtems_ftpd_configuration
{
   rtems_task_priority     priority;           /* FTPD task priority  */
   unsigned long           max_hook_filesize;  /* Maximum buffersize  */
                                               /*    for hooks        */
   int                     port;               /* Well-known port     */
   struct rtems_ftpd_hook  *hooks;             /* List of hooks       */
   char const              *root;              /* Root for FTPD or 0 for / */
   int                     tasks_count;        /* Max. connections    */
   int                     idle;               /* Idle timeout in seoconds
                                                  or 0 for no (inf) timeout */
   int                     access;             /* 0 - r/w, 1 - read-only,
                                                  2 - write-only,
                                                  3 - browse-only */
   rtems_shell_login_check_t login;            /* Login check or 0 to ignore
                                                  user/passwd. */
   bool                    verbose;            /* Say hello! */
};

rtems_status_code rtems_ftpd_start(
  const struct rtems_ftpd_configuration *config
);

extern struct rtems_ftpd_configuration rtems_ftpd_configuration;

rtems_status_code rtems_initialize_ftpd(void);

#ifdef __cplusplus
}
#endif

#endif  /* _RTEMS_FTPD_H */
