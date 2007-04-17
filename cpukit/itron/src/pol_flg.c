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

#include <rtems/itron/eventflags.h>

/*
 *  pol_flg - Wait for Eventflag(Polling)
 */

ER pol_flg(
  UINT *p_flgptn,
  ID    flgid,
  UINT  waiptn,
  UINT  wfmode
)
{
  return E_OK;
}
