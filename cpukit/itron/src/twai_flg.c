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
 *  twai_flg - Wait on Eventflag with Timeout
 */

ER twai_flg(
  UINT *p_flgptn __attribute__((unused)),
  ID    flgid __attribute__((unused)),
  UINT  waiptn __attribute__((unused)),
  UINT  wfmode __attribute__((unused)),
  TMO   tmout __attribute__((unused))
)
{
  return E_OK;
}
