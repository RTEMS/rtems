/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 2014.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution.

  This software with is provided ``as is'' and with NO WARRANTY.

  ------------------------------------------------------------------------

  RTEMS Performance Monitoring and Measurement Framework.

  This is the Target Interface Command Line Interface. You need
  start the RTEMS monitor.

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/score/assert.h>
#include "capture_buffer.h"

void * rtems_capture_buffer_allocate( rtems_capture_buffer_t* buffer, size_t size )
{
  static uint32_t   end;
  static void       *ptr;

  if ( rtems_capture_buffer_is_full( buffer ) )
    return NULL;

  if ( (buffer->count + size) > buffer->end )
    return NULL;

  /*
   *  Determine if the end of free space is marked with
   *  the end of buffer space, or the head of allocated
   *  space.
   *
   *  |...|head| freespace |tail| ...| end
   *
   *  tail|.....|head| freespace| end
   *
   */
  if (buffer->tail > buffer->head) {
    end = buffer->tail;
  } else {
    end = buffer->end;
  }

  /*
   *  Can we allocate it easily?
   */
  if ((buffer->head + size) <= end) {
    ptr = &buffer->buffer[ buffer->head ];
    buffer->head += size;
    buffer->count = buffer->count + size;
    return ptr;
  }

  /*
   * We have to consider wrapping around to the front of the buffer
   */

  /* If there is not room at the end of the buffer         */
  /* and we have we already wrapped then we can't allocate */
  if ( end == buffer->tail )
    return NULL;

  /* Is there no room at the front of the buffer */
  if ( (buffer->tail  < size ))
    return NULL;

  /* change the end pointer to the last used byte, so a read will wrap when out of data */
  buffer->end = buffer->head;

  /* now return the buffer */
  ptr = buffer->buffer;
  buffer->head = size;
  buffer->count = buffer->count + size;

  return ptr; 
}

void *rtems_capture_buffer_free( rtems_capture_buffer_t* buffer, size_t size ) 
{
    static void             *ptr;
    static uint32_t         next;
    size_t                  buff_size;

    if (size == 0)
      return NULL;

    ptr = rtems_capture_buffer_peek(buffer, &buff_size);
    next = buffer->tail + size;

   /* Check if we are freeing space past the end of the buffer */
    _Assert( ! rtems_capture_buffer_is_empty( buffer ) );
    _Assert( !((buffer->tail > buffer->head) && (next > buffer->end)) );
    _Assert( !((buffer->tail < buffer->head) && (next > buffer->head)) );

    buffer->count = buffer->count - size;

    if (next == buffer->end) {
      buffer->end = buffer->size;
      buffer->tail = 0;
    } else {
      buffer->tail = next;
    }
     
    return ptr;
}
