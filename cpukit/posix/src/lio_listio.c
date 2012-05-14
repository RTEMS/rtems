/*
 *  6.7.4 List Directed I/O, P1003.1b-1993, p. 158
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <aio.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/seterr.h>

int lio_listio(
  int                    mode __attribute__((unused)),
  struct aiocb  * const  list[] __attribute__((unused)),
  int                    nent __attribute__((unused)),
  struct sigevent       *sig __attribute__((unused))
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
