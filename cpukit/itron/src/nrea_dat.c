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

#include <rtems/itron/network.h>

/*
 *  nrea_dat - Read Data from another Node
 */

ER nrea_dat(
  INT *p_reasz,
  VP   dstadr,
  NODE srcnode,
  VP   srcadr,
  INT  datsz
)
{
  return E_OK;
}
