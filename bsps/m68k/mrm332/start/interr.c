/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-1999.
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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>

/*
 * Ugly hack.... _CPU_Fatal_halt() disonnects the bdm. Without this
 * change, the_error is only known only to the cpu :).
 */

#define RAW_PUTS(str) \
  { register char *ptr = str; \
    while (*ptr) outbyte(*ptr++); \
  }

#define RAW_PUTI(n) { \
    register int i, j; \
    \
    RAW_PUTS("0x"); \
    for (i=28;i>=0;i -= 4) { \
      j = (n>>i) & 0xf; \
      outbyte( (j>9 ? j-10+'a' : j+'0') ); \
    } \
  }

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code the_error
)
{
  /* try to print error message to outbyte */
  RAW_PUTS("\r\nRTEMS: A fatal error has occured.\r\n");
  RAW_PUTS("RTEMS:    fatal error ");
  RAW_PUTI( the_error );
  RAW_PUTS(" (");
  outbyte( (char)((the_error>>24) & 0xff) );
  outbyte( (char)((the_error>>16) & 0xff) );
  outbyte( (char)((the_error>>8) & 0xff) );
  outbyte( (char)(the_error & 0xff) );
  RAW_PUTS(").\r\n");
}
