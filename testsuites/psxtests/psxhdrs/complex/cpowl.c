/**
 *  @file
 *  @brief cpowl() API Conformance Test
 */

/*
 *  COPYRIGHT (c) 2018.
 *  Zenon (zehata).
 *
 *  Permission to use, copy, modify, and/or distribute this software
 *  for any purpose with or without fee is hereby granted.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *  WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 *  BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 *  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __CYGWIN__ 1 /* required until ticker 3635 fixed */
#include <complex.h>

int test(void);

int test(void)
{
  long double complex number;
  long double complex exponent;
  long double complex result;

  number = 1.0 + 1.0 * I;
  exponent = 1.0 + 1.0 * I;
  result = cpowl(number, exponent);
  return (result != 0);
}
