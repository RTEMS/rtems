/**
 *  @file
 *  @brief thrd_equal() API Conformance Test
 */

/*
 *  COPYRIGHT (c) 2018.
 *  Himanshu Sekhar Nayak
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

#include <threads.h>

int test( void );
int th_func( void *arg );

int test( void )
{
  thrd_t lhs;
  thrd_t rhs;
  int ln = 1;
  int rn = 1;
  int result;

  thrd_create( &lhs, th_func, &ln );
  thrd_create( &rhs, th_func, &rn );
  result = thrd_equal( lhs, rhs );

  return ( result != -1 );
}

int th_func( void *arg )
{
    ++*(int*)arg;
    return 0;
}
