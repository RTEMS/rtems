/**
 * @file
 *
 * @brief Dump Buffer
 * @ingroup libmisc_dumpbuf Print Memory Buffer
 */

/*
 *  COPYRIGHT (c) 1997-2015.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
