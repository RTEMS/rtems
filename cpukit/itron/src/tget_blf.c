/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/itron.h>

#include <rtems/itron/fmempool.h>

/*
 *  tget_blf - Get Fixed-Size Memory Block with Timeout
 */

ER tget_blf(
  VP *p_blf __attribute__((unused)),
  ID  mpfid __attribute__((unused)),
  TMO tmout __attribute__((unused))
)
{
  return E_OK;
}
