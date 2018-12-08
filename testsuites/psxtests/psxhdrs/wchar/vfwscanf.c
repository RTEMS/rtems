/**
 *  @file
 *  @brief vfwscanf() API Conformance Test
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

#include <wchar.h>
#include <stdarg.h>
/* 
 Corresponds to ticket #3642, please remove the line
 above and this comment when fixed 
*/

int test(char *fmt, ...);

int test(char *fmt, ...)
{
  FILE *file;
  const wchar_t *wc;
  va_list va;

  wc = 0;
  file = (void *)0;
  va_start(va, fmt);
  return vfwscanf(file, wc, va);
}
