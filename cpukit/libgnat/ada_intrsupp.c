/*
 *  COPYRIGHT (c) 1989-2010.
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
#include <rtems/bspIo.h>

rtems_id __gnat_binary_semaphore_create(void)
{
  rtems_status_code status;
  rtems_id          semaphore;

  status = rtems_semaphore_create(
    rtems_build_name( 'A', 'I', 'S', 'R' ),
    0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_FIFO,
    0,
    &semaphore
  );
  if ( status != RTEMS_SUCCESSFUL )
    printk( "__gnat_binary_semaphore_create failed %d\n", status );

  #if defined(GNAT_DEBUG)
    printk( "__gnat_binary_semaphore_create\n" );
  #endif
  return semaphore;
}

int __gnat_binary_semaphore_delete(
  rtems_id semaphore
)
{
  rtems_status_code status;

  #if defined(GNAT_DEBUG)
    printk( "__gnat_binary_semaphore_delete\n" );
  #endif

  status = rtems_semaphore_delete( semaphore );
  if ( status != RTEMS_SUCCESSFUL )
    printk( "__gnat_binary_semaphore_delete failed %d\n", status );

  return 0;
}

int __gnat_binary_semaphore_obtain(
  rtems_id semaphore
)
{
  rtems_status_code status;

  #if defined(GNAT_DEBUG)
    printk( "__gnat_binary_semaphore_obtain\n" );
  #endif

  status = rtems_semaphore_obtain( semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  if ( status != RTEMS_SUCCESSFUL )
    printk( "__gnat_binary_semaphore_obtain failed %d\n", status );

  return 0;
}

int __gnat_binary_semaphore_release(
  rtems_id semaphore
)
{
  rtems_status_code status;

  #if defined(GNAT_DEBUG)
    printk( "__gnat_binary_semaphore_release\n" );
  #endif

  status = rtems_semaphore_release( semaphore );
  if ( status != RTEMS_SUCCESSFUL )
    printk( "__gnat_binary_semaphore_release failed %d\n", status );

  return 0;
}

int __gnat_binary_semaphore_flush(
  rtems_id semaphore
)
{
  rtems_status_code status;

  printk( "__gnat_binary_semaphore_flush\n" );

  status = rtems_semaphore_flush( semaphore );
  if ( status != RTEMS_SUCCESSFUL )
    printk( "__gnat_binary_semaphore_flush failed %d\n", status );

  return 0;
}

typedef void (*ISRHandler)(void*);
  void *set_vector( void *, rtems_vector_number, int );

int __gnat_interrupt_connect(
  int         vector,
  ISRHandler  handler,
  void       *parameter
)
{
  printk( "__gnat_interrupt_connect( %d, %p, %p )\n", vector, handler, parameter  );
  set_vector( handler, vector, 1 );
  return 0;
}

int __gnat_interrupt_set(
  int         vector,
  ISRHandler  handler
)
{
  printk( "__gnat_interrupt_set( %d, %p )\n", vector, handler );

  set_vector( handler, vector, 1 );
  return 0;
}

ISRHandler __gnat_interrupt_get(
  int         vector
)
{
  printk( "__gnat_interrupt_get( %d )\n", vector );
  return 0;
}

int __gnat_interrupt_number_to_vector(
  int intNum
)
{
  printk( "__gnat_interrupt_number_to_vector( %d )\n", intNum );
  return intNum;
}

