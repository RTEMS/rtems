/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libmisc_dumpbuf Print Memory Buffer
 *
 * @brief Dump Buffer
 */

/*
 *  COPYRIGHT (c) 1997-2015.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <rtems/dumpbuf.h>
#include <rtems/bspIo.h>

#define HEX_FMT_LENGTH 3   /* Length of the formatted hex string. */
#define ASCII_FMT_LENGTH 1 /* Length of the formatted ASCII string. */
#define BYTES_PER_ROW 16    /* Amount of bytes from buffer shown in each row. */
#define BARS 2             /* Amount of bars in each row. */
/* Max length of each row string. */
#define ROW_LENGTH (BYTES_PER_ROW * (HEX_FMT_LENGTH + ASCII_FMT_LENGTH) + BARS)

/*
 *  Put the body below rtems_print_buffer so it won't get inlined.
 */

static void Dump_Line(const unsigned char *buffer, const unsigned int length);

/**
 * @brief Print \p length bytes from \p buffer, both in hex and ASCII.
 * Printing will be done in rows, each showing BYTES_PER_ROW bytes.
 * @details Non-printable chars will appear as dots.
 *
 * @param buffer The buffer we'll print.
 * @param length Amount of bytes from \p buffer we'll print. This can't be
 * unsigned because we don't have a way to check if we're erroneously getting
 * a negative \p length.
 */
void rtems_print_buffer(const unsigned char *buffer, const int length)
{
  unsigned int i, mod, max;

  if (length > 0) {
    mod = length % BYTES_PER_ROW;

    max = length - mod;

    /* Print length / BYTES_PER_ROW rows. */
    for (i = 0; i < max; i += BYTES_PER_ROW) {
      Dump_Line(&buffer[i], BYTES_PER_ROW);
    }

    /* Print another row with the remaining bytes. */
    if (mod > 0) {
      Dump_Line(&buffer[max], mod);
    }
  } else {
    printk("Error: length must be greater than zero.");
  }
}

static char const hexlist[] = "0123456789abcdef";

/**
 * @brief Print \p length bytes from \p buffer, both in hex and ASCII.
 * @details Non-printable chars will appear as dots.
 *
 * @param buffer The buffer we'll print.
 * @param length Amount of bytes from \p buffer we'll print.
 */
static void Dump_Line(const unsigned char *buffer, const unsigned int length)
{
  unsigned int i;

  /* Output the hex value of each byte. */
  for (i = 0; i < length; ++i) {
    unsigned char c = buffer[i];

    rtems_putc(hexlist[(c >> 4) & 0xf]);
    rtems_putc(hexlist[c & 0xf]);
    rtems_putc(' ');
  }

  /* Fill the remaining space with whitespace (if necessary). */
  for (; i < BYTES_PER_ROW; ++i) {
    rtems_putc(' ');
    rtems_putc(' ');
    rtems_putc(' ');
  }

  /* Append a bar. */
  rtems_putc('|');

  /* Now output the ASCII glyphs of printable chars. */
  for (i = 0; i < length; ++i) {
    unsigned char c = buffer[i];

    rtems_putc(isprint(c) ? c : '.');
  }

  /* Fill the remaining space with whitespace (if necessary). */
  for(; i < BYTES_PER_ROW; i++) {
    rtems_putc(' ');
  }

  /* Append another bar and print the resulting string. */
  rtems_putc('|');
  rtems_putc('\n');
}
