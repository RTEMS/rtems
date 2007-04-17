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
 *  nwri_dat - Write Data to another Node
 */

ER nwri_dat(
  INT  *p_wrisz,
  NODE  dstnode,
  VP    dstadr,
  VP    srcadr,
  INT   datsz
)
{
  return E_OK;
}

