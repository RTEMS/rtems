#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/posix/pthread.h>

/*
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

uint32_t _ada_microseconds_per_tick(void)
{
  return rtems_configuration_get_microseconds_per_tick();
}
