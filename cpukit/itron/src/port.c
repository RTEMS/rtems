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

#include <rtems/itron/port.h>

/*    
 *  _ITRON_Port_Manager_initialization
 *  
 *  This routine initializes all ports manager related data structures.
 *
 *  Input parameters:
 *    maximum_ports - maximum configured ports
 *
 *  Output parameters:  NONE
 */

void _ITRON_Port_Manager_initialization(
  unsigned32 maximum_ports
) 
{
  _Objects_Initialize_information(
    &_ITRON_Port_Information,     /* object information table */
    OBJECTS_ITRON_PORTS,          /* object class */
    FALSE,                        /* TRUE if this is a global object class */
    maximum_ports,                /* maximum objects of this class */
    sizeof( ITRON_Port_Control ), /* size of this object's control block */
    FALSE,                        /* TRUE if names for this object */
                                  /*   are strings */
    ITRON_MAXIMUM_NAME_LENGTH,    /* maximum length of each object's name */
    FALSE                         /* TRUE if this class is threads */
  );
    
  /*
   *  Register the MP Process Packet routine.
   *
   *  NOTE: No MP Support YET in RTEMS ITRON implementation.
   */
 
} 

/*
 *  cre_por - Create Port for Rendezvous
 */

ER cre_por(
  ID      porid,
  T_CPOR *pk_cpor
)
{
  return E_OK;
}

/*
 *  del_por - Delete Port for Rendezvous
 */

ER del_por(
  ID porid
)
{
  return E_OK;
}

/*
 *  cal_por - Call Port for Rendezvous Poll 
 */

ER cal_por(
  VP    msg,
  INT  *p_rmsgsz,
  ID    porid,
  UINT  calptn,
  INT   cmsgsz
)
{
  return E_OK;
}

/*
 *  pcal_por - Poll and Call Port for Rendezvous
 */

ER pcal_por(
  VP   msg,
  INT *p_rmsgsz,
  ID   porid,
  UINT calptn,
  INT  cmsgsz
)
{
  return E_OK;
}

/*
 *  tcal_por - Call Port for Rendezvous with Timeout
 */

ER tcal_por(
  VP    msg,
  INT  *p_rmsgsz,
  ID    porid,
  UINT  calptn,
  INT   cmsgsz,
  TMO   tmout
)
{
  return E_OK;
}

/*
 *  acp_por - Accept Port for Rendezvous Poll
 */

ER acp_por(
  RNO  *p_rdvno,
  VP    msg,
  INT  *p_cmsgsz,
  ID    porid,
  UINT  acpptn
)
{
  return E_OK;
}

/*
 *  pacp_por - Poll and Accept Port for Rendezvous
 */

ER pacp_por(
  RNO  *p_rdvno,
  VP    msg,
  INT  *p_cmsgsz,
  ID    porid,
  UINT  acpptn
)
{
  return E_OK;
}

/*
 *  tacp_por - Accept Port for Rendezvous with Timeout
 */

ER tacp_por(
  RNO  *p_rdvno,
  VP    msg,
  INT  *p_cmsgsz,
  ID    porid,
  UINT  acpptn,
  TMO   tmout
)
{
  return E_OK;
}

/*
 *  fwd_por - Forward Rendezvous to Other Port
 */

ER fwd_por(
  ID    porid,
  UINT  calptn,
  RNO   rdvno,
  VP    msg,
  INT   cmsgsz
)
{
  return E_OK;
}

/*
 *  rpl_rdv - Reply Rendezvous
 */

ER rpl_rdv(
  RNO  rdvno,
  VP   msg,
  INT rmsgsz
)
{
  return E_OK;
}

/*
 *  ref_por - Reference Port Status
 */

ER ref_por(
  T_RPOR *pk_rpor,
  ID      porid
)
{
  return E_OK;
}

