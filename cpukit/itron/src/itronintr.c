/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <itron.h>

#include <rtems/itron/intr.h>

/*
 *  def_int - Define Interrupt Handler
 */

ER def_int(
  UINT    dintno,
  T_DINT *pk_dint
)
{
  return E_OK;
}

/*
 *  ret_int - Return from Interrupt Handler
 */

void ret_int( void )
{
}

/*
 *  ret_wup - Return and Wakeup Task
 */

void ret_wup(
  ID tskid
)
{
}

/*
 *  loc_cpu - Lock CPU
 */

ER loc_cpu( void )
{
  return E_OK;
}

/*
 *  unl_cpu - Unlock CPU
 */

ER unl_cpu( void )
{
  return E_OK;
}

/*
 *  dis_int - Disable Interrupt
 */

ER dis_int(
  UINT eintno
)
{
  return E_OK;
}

/*
 *  ena_int - Enable Interrupt 
 */

ER ena_int(
  UINT eintno
)
{
  return E_OK;
}

/*
 *  chg_iXX - Change Interrupt Mask(Level or Priority)
 */

ER chg_iXX(
  UINT iXXXX
)
{
  return E_OK;
}

/*
 *  ref_iXX - Reference Interrupt Mask(Level or Priority)
 */

ER ref_iXX(
  UINT *p_iXXXX
)
{
  return E_OK;
}

