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

#include <rtems/itron/eventflags.h>

/*    
 *  _ITRON_Eventflags_Manager_initialization
 *  
 *  This routine initializes all event flags manager related data structures.
 *
 *  Input parameters:
 *    maximum_eventflags - maximum configured eventflags
 *
 *  Output parameters:  NONE
 */

void _ITRON_Eventflags_Manager_initialization(
  unsigned32 maximum_eventflags
) 
{
  _Objects_Initialize_information(
    &_ITRON_Eventflags_Information,     /* object information table */
    OBJECTS_ITRON_EVENTFLAGS,           /* object class */
    FALSE,                              /* TRUE if this is a global */
                                        /*   object class */
    maximum_eventflags,                 /* maximum objects of this class */
    sizeof( ITRON_Eventflags_Control ), /* size of this object's */
                                        /*   control block */
    FALSE,                              /* TRUE if names for this object */
                                        /*   are strings */
    ITRON_MAXIMUM_NAME_LENGTH,          /* maximum length of each object's */
                                        /*   name */
    FALSE                               /* TRUE if this class is threads */
  );
    
  /*
   *  Register the MP Process Packet routine.
   *
   *  NOTE: No MP Support YET in RTEMS ITRON implementation.
   */
 
} 

/*
 *  cre_flg - Create Eventflag
 */

ER cre_flg(
  ID      flgid,
  T_CFLG *pk_cflg
)
{
  return E_OK;
}

/*
 *  del_flg - Delete Eventflag
 */

ER del_flg(
  ID flgid
)
{
  return E_OK;
}

/*
 *  set_flg - Set Eventflag
 */

ER set_flg(
  ID   flgid,
  UINT setptn
)
{
  return E_OK;
}

/*
 *  clr_flg - Clear Eventflag
 */

ER clr_flg(
  ID   flgid,
  UINT clrptn
)
{
  return E_OK;
}

/*
 *  wai_flg - Wait on Eventflag
 */

ER wai_flg(
  UINT *p_flgptn,
  ID    flgid,
  UINT  waiptn,
  UINT  wfmode
)
{
  return E_OK;
}

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

/*
 *  twai_flg - Wait on Eventflag with Timeout
 */

ER twai_flg(
  UINT *p_flgptn,
  ID    flgid,
  UINT  waiptn,
  UINT  wfmode,
  TMO   tmout
)
{
  return E_OK;
}

/*
 *  ref_flg - Reference Eventflag Status
 */

ER ref_flg(
  T_RFLG *pk_rflg,
  ID      flgid
)
{
  return E_OK;
}

