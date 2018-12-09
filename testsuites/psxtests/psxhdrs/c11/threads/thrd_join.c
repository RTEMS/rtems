/**
 *  @file
 *  @brief thrd_join() API Conformance Test
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
  thrd_t th;
  int n = 1;
  int res = 1;
  int result;

  thrd_create( &th, th_func, &n );
  result = thrd_join( th, &res );

  return ( result != -1 );
}

int th_func( void *arg )
{
    ++*(int*)arg;
    return 0;
}
