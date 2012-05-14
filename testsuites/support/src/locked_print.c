/* 
 *  COPYRIGHT (c) 1989-2011.
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
#include <rtems/system.h>
#include <sys/types.h>
#include <string.h>
#include <stdarg.h>

#include "tmacros.h"

static rtems_id locked_print_semaphore;      /* synchronisation semaphore */ 

void locked_print_initialize(void)
{
  rtems_status_code sc;
  static            bool initted = false;

  if ( initted )
    return;

  initted = true;

  /* Create/verify synchronisation semaphore */
  sc = rtems_semaphore_create(
    rtems_build_name ('S', 'E', 'M', '1'),
    1,                                             
    RTEMS_LOCAL                   |
    RTEMS_SIMPLE_BINARY_SEMAPHORE |
    RTEMS_PRIORITY,
    1,
    &locked_print_semaphore
  );
  directive_failed( sc, "rtems_semaphore_create" );
}

void locked_printf(const char *fmt, ...) {
  va_list           ap;       /* points to each unnamed argument in turn */
  rtems_status_code sc;

  locked_print_initialize();

  /* Lock semaphore without releasing the cpu */
  do {
    sc = rtems_semaphore_obtain( locked_print_semaphore, RTEMS_NO_WAIT, 0 );
  } while (sc != RTEMS_SUCCESSFUL );


  va_start(ap, fmt); /* make ap point to 1st unnamed arg */
  vprintf(fmt, ap);
  va_end(ap);        /* clean up when done */

  /* Release the semaphore  */
  sc = rtems_semaphore_release( locked_print_semaphore );
} 

void locked_printk(const char *fmt, ...) {
  va_list           ap;       /* points to each unnamed argument in turn */
  rtems_status_code sc;


  locked_print_initialize();

  /* Lock semaphore without releasing the cpu */
  do {
    sc = rtems_semaphore_obtain( locked_print_semaphore, RTEMS_NO_WAIT, 0 );
  } while (sc != RTEMS_SUCCESSFUL );

  va_start(ap, fmt); /* make ap point to 1st unnamed arg */
  vprintk(fmt, ap);
  va_end(ap);        /* clean up when done */

  /* Release the semaphore  */
  sc = rtems_semaphore_release( locked_print_semaphore );
} 
