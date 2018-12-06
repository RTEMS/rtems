/**
 *  @file
 *  @brief nftw() API Conformance Test
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

#define _XOPEN_SOURCE 500
#include <ftw.h>

int test( void );

static int fn(
  const char        *str,
  const struct stat *ptr,
  int                flag,
  struct FTW        *ftw
)
{
  return 1;
}

int test( void )
{
  char *path = ".";
  int return_value;

  return_value = nftw(path, fn, 1, FTW_MOUNT);
  return (return_value != -1);
}
