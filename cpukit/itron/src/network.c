/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

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

/*
 *  nget_nod - Get Local Node Number
 */

ER nget_nod(
  NODE *p_node
)
{
  return E_OK;
}

/*
 *  nget_ver - Get Version Information of another Node
 */

ER nget_ver(
  T_VER *pk_ver,
  NODE   node
)
{
  return E_OK;
}

