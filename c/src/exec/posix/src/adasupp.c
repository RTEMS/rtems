/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif
 
#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/posix/pthread.h>

/*PAGE
 *
 *  _ada_pthread_minimum_stack_size
 *
 *  This routine returns the minimum stack size so the GNAT RTS can
 *  allocate enough stack for Ada tasks.
 */

size_t _ada_pthread_minimum_stack_size( void ) 
{
  /*
   *  Eventually this may need to include a per cpu family calculation
   *  but for now, this will do.
   */ 

  return PTHREAD_MINIMUM_STACK_SIZE * 2;
}
