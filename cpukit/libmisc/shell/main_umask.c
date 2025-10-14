/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief UMASK Shell Command Implementation
 */

/*
 * Copyright (C) 2001 Fernando Ruiz Casas <fruizcasas@gmail.com>
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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/stringto.h>
#include "internal.h"

static int rtems_shell_main_umask(
  int   argc,
  char *argv[]
)
{
  unsigned long tmp;
  mode_t        msk = umask(0);

  if (argc == 2) {
    if ( rtems_string_to_unsigned_long(argv[1], &tmp, NULL, 0) ) {
      printf( "Mask argument (%s) is not a number\n", argv[1] );
      return -1;
    }
    msk = (mode_t) tmp;

  }
  umask(msk);

  msk = umask(0);
  printf("0%o\n", (unsigned int) msk);
  umask(msk);
  return 0;
}

rtems_shell_cmd_t rtems_shell_UMASK_Command = {
  .name = "umask",
  .usage = "umask [new_umask]",
  .topic = "misc",
  .command = rtems_shell_main_umask,
  .alias = NULL,
  .next = NULL
};
