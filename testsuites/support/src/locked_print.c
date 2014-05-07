/* 
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "test_support.h"
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
    RTEMS_BINARY_SEMAPHORE |
    RTEMS_PRIORITY_CEILING |
    RTEMS_PRIORITY,
    1,
    &locked_print_semaphore
  );
  directive_failed( sc, "rtems_semaphore_create" );
}

int locked_vprintf(const char *fmt, va_list ap)
{
  int rv;
  rtems_status_code sc;

  locked_print_initialize();

  /* Lock semaphore without releasing the cpu */
  do {
    sc = rtems_semaphore_obtain( locked_print_semaphore, RTEMS_NO_WAIT, 0 );
  } while (sc != RTEMS_SUCCESSFUL );

  rv = vprintf(fmt, ap);

  /* Release the semaphore  */
  rtems_semaphore_release( locked_print_semaphore );

  return rv;
}

int locked_printf_plugin(void *context, const char *fmt, ...)
{
  int rv;
  va_list ap;

  (void) context;

  va_start(ap, fmt);
  rv = locked_vprintf(fmt, ap);
  va_end(ap);

  return rv;
}

int locked_printf(const char *fmt, ...)
{
  int               rv;
  va_list           ap;       /* points to each unnamed argument in turn */

  va_start(ap, fmt); /* make ap point to 1st unnamed arg */
  rv = locked_vprintf(fmt, ap);
  va_end(ap);        /* clean up when done */

  return rv;
}

void locked_printk(const char *fmt, ...)
{
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
  rtems_semaphore_release( locked_print_semaphore );
}
