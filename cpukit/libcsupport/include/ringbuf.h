/*
 *  ringbuf.h 
 *
 *  This file provides simple ring buffer functionality.
 *
 *  $Id$
 */

#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#ifndef RINGBUF_QUEUE_LENGTH
#define RINGBUF_QUEUE_LENGTH 200
#endif

typedef struct {
  char buffer[RINGBUF_QUEUE_LENGTH];
  int  head;
  int  tail;
} Ring_buffer_t;

#define Ring_buffer_Initialize( _buffer ) \
  do { \
    (_buffer)->head = (_buffer)->tail = 0; \
  } while ( 0 ) 
  
#define Ring_buffer_Is_empty( _buffer ) \
   ( (_buffer)->head == (_buffer)->tail )

#define Ring_buffer_Add_character( _buffer, _ch ) \
  do { \
    (_buffer)->buffer[ (_buffer)->tail ] = (_ch); \
    (_buffer)->tail = ((_buffer)->tail+1) % RINGBUF_QUEUE_LENGTH; \
  } while ( 0 ) 

#define Ring_buffer_Remove_character( _buffer, _ch ) \
  do { \
    (_ch) = (_buffer)->buffer[ (_buffer)->head ]; \
    (_buffer)->head = ((_buffer)->head+1) % RINGBUF_QUEUE_LENGTH; \
  } while ( 0 ) 

#endif
