/**
 * @file
 *
 * @brief Simple Ring Buffer Functionality
 *
 * This file provides simple ring buffer functionality.
 */


#ifndef _RTEMS_RINGBUF_H
#define _RTEMS_RINGBUF_H

#include <rtems.h>

#ifndef RINGBUF_QUEUE_LENGTH
#define RINGBUF_QUEUE_LENGTH 128
#endif

typedef struct {
  uint8_t buffer[RINGBUF_QUEUE_LENGTH];
  volatile int  head;
  volatile int  tail;
  rtems_interrupt_lock lock;
} Ring_buffer_t;

#define Ring_buffer_Initialize( _buffer ) \
  do { \
    (_buffer)->head = (_buffer)->tail = 0; \
    rtems_interrupt_lock_initialize(&(_buffer)->lock, "ring buffer"); \
  } while ( 0 )

#define Ring_buffer_Destory( _buffer ) \
  do { \
    rtems_interrupt_lock_destroy(&(_buffer)->lock); \
  } while ( 0 )

#define Ring_buffer_Is_empty( _buffer ) \
   ( (_buffer)->head == (_buffer)->tail )

#define Ring_buffer_Is_full( _buffer ) \
   ( (_buffer)->head == ((_buffer)->tail + 1) % RINGBUF_QUEUE_LENGTH )

#define Ring_buffer_Add_character( _buffer, _ch ) \
  do { \
    rtems_interrupt_lock_context lock_context; \
    \
    rtems_interrupt_lock_acquire( &(_buffer)->lock, &lock_context ); \
      (_buffer)->tail = ((_buffer)->tail+1) % RINGBUF_QUEUE_LENGTH; \
      (_buffer)->buffer[ (_buffer)->tail ] = (_ch); \
    rtems_interrupt_lock_release( &(_buffer)->lock, &lock_context ); \
  } while ( 0 )

#define Ring_buffer_Remove_character( _buffer, _ch ) \
  do { \
    rtems_interrupt_lock_context lock_context; \
    \
    rtems_interrupt_lock_acquire( &(_buffer)->lock, &lock_context ); \
      (_buffer)->head = ((_buffer)->head+1) % RINGBUF_QUEUE_LENGTH; \
      (_ch) = (_buffer)->buffer[ (_buffer)->head ]; \
    rtems_interrupt_lock_release( &(_buffer)->lock, &lock_context ); \
  } while ( 0 )

#endif
