/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <itron.h>

#include <rtems/itron/mbox.h>

/*    
 *  _ITRON_Mailbox_Manager_initialization
 *  
 *  This routine initializes all mailboxes manager related data structures.
 *
 *  Input parameters:
 *    maximum_mailboxes - maximum configured mailboxes
 *
 *  Output parameters:  NONE
 */

void _ITRON_Mailbox_Manager_initialization(
  unsigned32 maximum_mailboxes
) 
{
  _Objects_Initialize_information(
    &_ITRON_Mailbox_Information,     /* object information table */
    OBJECTS_ITRON_MAILBOXES,         /* object class */
    FALSE,                           /* TRUE if this is a global */
                                     /*   object class */
    maximum_mailboxes,               /* maximum objects of this class */
    sizeof( ITRON_Mailbox_Control ), /* size of this object's control block */
    FALSE,                           /* TRUE if names for this object */
                                     /*   are strings */
    RTEMS_MAXIMUM_NAME_LENGTH,       /* maximum length of each object's */
                                     /*   name */
    FALSE                            /* TRUE if this class is threads */
  );
    
  /*
   *  Register the MP Process Packet routine.
   *
   *  NOTE: No MP Support YET in RTEMS ITRON implementation.
   */
 
} 

/*
 *  cre_mbx - Create Mailbox
 */

ER cre_mbx(
  ID      mbxid,
  T_CMBX *pk_cmbx
)
{
  return E_OK;
}

/*
 *  del_mbx - Delete Mailbox
 */

ER del_mbx(
  ID mbxid
)
{
  return E_OK;
}

/*
 *  snd_msg - Send Message to Mailbox
 */

ER snd_msg(
  ID     mbxid,
  T_MSG *pk_msg
)
{
  return E_OK;
}

/*
 *  rcv_msg - Receive Message from Mailbox 
 */

ER rcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid
)
{
  return E_OK;
}

/*
 *  prcv_msg - Poll and Receive Message from Mailbox
 */

ER prcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid
)
{
  return E_OK;
}

/*
 *  trcv_msg - Receive Message from Mailbox with Timeout
 */

ER trcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid,
  TMO     tmout
)
{
  return E_OK;
}

/*
 *  ref_mbx - Reference Mailbox Status
 */

ER ref_mbx(
  T_RMBX *pk_rmbx,
  ID      mbxid
)
{
  return E_OK;
}

