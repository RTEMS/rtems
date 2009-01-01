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

#include <rtems/itron/vmempool.h>

/*
 *  tget_blk - Get Variable-Size Memory Block with Timeout
 */

ER tget_blk(
  VP  *p_blk __attribute__((unused)),
  ID   mplid __attribute__((unused)),
  INT  blksz __attribute__((unused)),
  TMO  tmout __attribute__((unused))
)
{
  return E_OK;
}
