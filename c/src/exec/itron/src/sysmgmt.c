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

#include <rtems/itron/sysmgmt.h>

/*
 *  get_ver - Get Version Information
 */

ER get_ver(
  T_VER *pk_ver
)
{
  return E_OK;
}

/*
 *  ref_sys - Reference System Status
 */

ER ref_sys(
  T_RSYS *pk_rsys
)
{
  return E_OK;
}

/*
 *  ref_cfg - Reference Configuration Information
 */

ER ref_cfg(
  T_RCFG *pk_rcfg
)
{
  return E_OK;
}

/*
 *  def_svc - Define Extended SVC Handler
 */

ER def_svc(
  FN      s_fncd,
  T_DSVC *pk_dsvc
)
{
  return E_OK;
}

/*
 *  def_exc - Define Exception Handler
 */

ER def_exc(
  UINT    exckind,
  T_DEXC *pk_dexc
)
{
  return E_OK;
}

