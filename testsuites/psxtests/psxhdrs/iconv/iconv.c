/**
 *  @file
 *  @brief iconv() API Conformance Test
 */

/*
 *  COPYRIGHT (c) 2018.
 *  Jacob Shin
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

#include <iconv.h>

int test( void );

int test( void )
{
  iconv_t cd = iconv_open("ASCII", "UTF-8");
  char inbuf[10] = "string";
  char outbuf[10];
  char *inptr = inbuf;
  char *outptr = (char *)outbuf;
  size_t inbytesLeft = 7;
  size_t outbytesLeft = 10;
  size_t return_value;

  return_value = iconv(cd, &inptr, &inbytesLeft, &outptr, &outbytesLeft);
  return (return_value != (size_t)-1);
}
