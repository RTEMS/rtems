/*
 *  $Id$
 */

#include <rtems.h>

#include "system.h"

#define RINGBUF_QUEUE_LENGTH 512

#include <ringbuf.h>

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

  while ( *s ) {
    Ring_buffer_Add_character( &Buffer, *s++ );
  }
}
