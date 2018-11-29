/**
 *  @file
 *  @brief sem_destroy() API Conformance Test
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

  #include <fcntl.h>
  #include <sys/stat.h>
  #include <semaphore.h>

  int test( void );

  int test( void )
  {
    sem_t *sem1;
    int oflag = O_CREAT;
    int result;
    int value;

    value = 1;

    sem1 = sem_open( "sem", oflag, 0777, value );
    result = sem_destroy( sem1 );

    return result;
  }
