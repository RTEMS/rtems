/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief MDUMP Shell Command Implementation
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

/*
 *  Reworked by Ric Claus at SLAC.Stanford.edu
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/stringto.h>
#include "internal.h"

static int args_parse(int argc, char* argv[], void** addr, int* max, int* sz);

static void mdumpB(void* addr, int m);
static void mdumpW(void* addr, int m);
static void mdumpL(void* addr, int m);
static void mdumpC(void* addr, int m);

int rtems_mdump(void* addr, int max, int sz);

static int rtems_shell_main_mdump(
  int   argc,
  char *argv[]
)
{
  void *addr;
  int   max;
  int   sz;

  if (args_parse(argc, argv, &addr, &max, &sz))
    return -1;
  return rtems_mdump(addr, max, sz);
}

static int rtems_shell_main_wdump(
  int   argc,
  char *argv[]
)
{
  void *addr;
  int   max;
  int   sz;

  if (args_parse(argc, argv, &addr, &max, &sz))
    return -1;
  return rtems_mdump(addr, max, 2);
}


static int rtems_shell_main_ldump(
  int   argc,
  char *argv[]
)
{
  void *addr;
  int   max;
  int   sz;

  if (args_parse(argc, argv, &addr, &max, &sz))
    return -1;
  return rtems_mdump(addr, max, 4);
}


int args_parse(int    argc,
               char*  argv[],
               void** addr,
               int*   max,
               int*   sz)
{
  *addr = NULL;
  *max  = 320;
  *sz   = 1;

  if (argc > 1) {
    if ( rtems_string_to_pointer(argv[1], addr, NULL) ) {
      printf( "Address argument (%s) is not a number\n", argv[1] );
      return -1;
    }

    if (argc > 2) {
      if ( rtems_string_to_int(argv[2], max, NULL, 0) ) {
        printf( "Length argument (%s) is not a number\n", argv[2] );
        return -1;
      }

      if (argc > 3) {
        if ( rtems_string_to_int(argv[3], sz, NULL, 0) ) {
          printf( "Size argument (%s) is not a valid number\n", argv[3] );
          return -1;
        }
      }
    }
  } else {
    // Handle missing address argument
    printf("Usage: %s [address [length [size]]]\n", argv[0]);
    return -1;
  }
  return 0;
}


int rtems_mdump(void* addr, int max, int sz)
{
  unsigned char  m;
  unsigned char *pb;
  int            res;
  int            cnt;

  if (!((sz == 1) || (sz == 2) || (sz == 4))) {
    printf( "Size argument (%d) is not one of 1 (bytes), "
              " 2 (words) or 4 (longwords)\n", sz);
    return -1;
  }

  if (max <= 0) {
    max = 1;           /* print 1 item if 0 or neg. */
    res = 0;
  } else {
    max--;
    res = max & 0xf;   /* num bytes in last row */
    max >>= 4;         /* div by 16 */
    max++;             /* num of rows to print */
    if (max > 64) {    /* limit to 64 lines */
      max = 64;
      res = 0xf;       /* 16 bytes print in last row */
    }
  }

  pb = addr;
  for (m=0; m<max; m++) {
    cnt = m==(max-1)?res:0xf;
    printf("%10p ", pb);
    if      (sz == 1)  mdumpB(pb, cnt);
    else if (sz == 2)  mdumpW(pb, cnt);
    else if (sz == 4)  mdumpL(pb, cnt);
    mdumpC(pb, cnt);
    printf("\n");
    pb += 16;
  }

  return 0;
}


void mdumpB(void* addr, int m)
{
  volatile unsigned char* pb = addr;
  int n;
  for (n=0;n<=m;n++)
    printf("%02X%c",*pb++,n==7?'-':' ');
  for (;n<=0xf;n++)
    printf("  %c",n==7?'-':' ');
}


void mdumpW(void* addr, int m)
{
  volatile unsigned short* pb = addr;
  int n;
  for (n=0;n<=m;n+=2)
    printf("%04X%c",*pb++,n==6?'-':' ');
  for (;n<=0xf;n+=2)
    printf("    %c", n==6?'-':' ');
}


void mdumpL(void* addr, int m)
{
  volatile unsigned int* pb = addr;
  int n;
  for (n=0;n<=m;n+=4)
    printf("%08X%c",*pb++,n==4?'-':' ');
  for (;n<=0xf;n+=4)
    printf("        %c", n==4?'-':' ');
}


void mdumpC(void* addr, int m)
{
  volatile unsigned char* pb = addr;
  int n;
  for (n=0;n<=m;n++)
    printf("%c", isprint(pb[n]) ? pb[n] : '.');
}

rtems_shell_cmd_t rtems_shell_MDUMP_Command = {
  .name = "mdump",
  .usage = "mdump [address [length [size]]]",
  .topic = "mem",
  .command = rtems_shell_main_mdump,
  .alias = NULL,
  .next = NULL
};

rtems_shell_cmd_t rtems_shell_WDUMP_Command = {
  .name = "wdump",
  .usage = "wdump [address [length]]",
  .topic = "mem",
  .command = rtems_shell_main_wdump,
  .alias = NULL,
  .next = NULL
};

rtems_shell_cmd_t rtems_shell_LDUMP_Command = {
  .name = "ldump",
  .usage = "ldump [address [length]]",
  .topic = "mem",
  .command = rtems_shell_main_ldump,
  .alias = NULL,
  .next = NULL
};

