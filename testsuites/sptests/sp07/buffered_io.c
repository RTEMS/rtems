/*
 *  COPYRIGHT (c) 2003-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include "system.h"

#define RINGBUF_QUEUE_LENGTH 512

#include <rtems/ringbuf.h>

Ring_buffer_t Buffer;

void buffered_io_initialize( void )
{
   Ring_buffer_Initialize( &Buffer );
}

void buffered_io_flush(void)
{
  char ch;

  while ( !Ring_buffer_Is_empty(&Buffer) ) {
     Ring_buffer_Remove_character( &Buffer, ch );
     fprintf( stderr, "%c", ch );
  }
  FLUSH_OUTPUT();
}

void buffered_io_add_string( char *s )
{
  char *p = s;

  while ( *p ) {
    Ring_buffer_Add_character( &Buffer, *p++ );
  }
}
