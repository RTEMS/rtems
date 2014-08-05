/**
 * @file capture_buffer.h
 *
 * @brief Capture buffer
 *
 * This is a set of functions to control a variable length capture record buffer.
 */

/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 2014.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution.

  This software with is provided ``as is'' and with NO WARRANTY.

  ------------------------------------------------------------------------
*/

#ifndef __CAPTUREBUFFER_H_
#define __CAPTUREBUFFER_H_

#include <stdlib.h>


/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint8_t           *buffer;
  size_t            size;
  volatile uint32_t count;
  volatile uint32_t head;
  volatile uint32_t tail;
  volatile uint32_t end;
} rtems_capture_buffer_t;

static inline void rtems_capture_buffer_flush(  rtems_capture_buffer_t* buffer )
{
  buffer->end = buffer->size;
  buffer->head = buffer->tail =  0;
  buffer->count = 0;
}

static inline void rtems_capture_buffer_create( rtems_capture_buffer_t* buffer, size_t size )
{
  buffer->buffer = malloc(size);
  buffer->size = size;
  rtems_capture_buffer_flush( buffer );
}

static inline void rtems_capture_buffer_destroy( rtems_capture_buffer_t*  buffer )
{
  rtems_capture_buffer_flush( buffer );
  free( buffer->buffer);
  buffer->buffer = NULL;
}

static inline bool rtems_capture_buffer_is_empty(  rtems_capture_buffer_t* buffer )
{
   return( buffer->count == 0 );
}

static inline bool rtems_capture_buffer_is_full( rtems_capture_buffer_t* buffer )
{
   return (buffer->count == buffer->size);
}

static inline bool rtems_capture_buffer_has_wrapped( rtems_capture_buffer_t* buffer )
{
  if ( buffer->tail > buffer->head)
    return true;

  return false;
}

static inline void *rtems_capture_buffer_peek(  rtems_capture_buffer_t* buffer, size_t *size ) 
{
  if (rtems_capture_buffer_is_empty(buffer)) {
    *size = 0; 
    return NULL;
  }

  if ( buffer->tail > buffer->head)  
    *size = buffer->end - buffer->tail;
  else
    *size = buffer->head - buffer->tail;

  return &buffer->buffer[ buffer->tail ];
}

void *rtems_capture_buffer_allocate( rtems_capture_buffer_t* buffer, size_t size );

void *rtems_capture_buffer_free( rtems_capture_buffer_t* buffer, size_t size );

#ifdef __cplusplus
}
#endif

#endif
