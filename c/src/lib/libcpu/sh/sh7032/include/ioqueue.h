/*
 * Defines for low level queue management
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, Ralf Corsepius, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 * 
 */

#ifndef _io_queue_h
#define _io_queue_h

#include <stdlib.h>	/* size_t */

/*
 * NOTE: size needs to be a power of 2
 */
#define IO_QUEUE(type,size,name) 			\
typedef struct {					\
  volatile type			queue[size] ;		\
  volatile unsigned short	tail ;			\
  volatile unsigned short	head ;			\
} name ;

#define IO_QUEUE_FULL(queue, size) \
  ((queue)->tail == (((queue)->head+1) & ((size)-1)))

#define IO_QUEUE_EMPTY(queue) \
  (((queue)->tail) == ((queue)->head))

#define IO_QUEUE_INIT(queue) \
  (queue)->tail = (queue)->head = 0

#define IO_QUEUE_ADD(queue,size) \
  (queue)->head = (((queue)->head + 1) & ((size)-1))
  
#define IO_QUEUE_SUB(queue,size) \
  (queue)->tail = (((queue)->tail + 1) & ((size)-1))

#define IO_QUEUE_PUT(_queue,item) \
{ \
    size_t i; \
    unsigned char* dest = (unsigned char*) ((_queue)->queue[(_queue)->head]); \
    unsigned char* src  = (unsigned char*) (item); \
    for( i = 0; i < sizeof(item); i++) \
    { \
	dest[i] = src[i]; \
    } \
}

#define IO_QUEUE_GET(_queue,item) \
{\
   size_t i; \
   unsigned char *src = (unsigned char*) (_queue)->queue[(_queue)->tail]; \
   unsigned char *dest = (unsigned char*) (item); \
   for( i=0; i< sizeof(item); i++)\
   {\
      dest[i] = src[i]; \
   }\
}

#endif
