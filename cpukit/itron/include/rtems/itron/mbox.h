/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __RTEMS_ITRON_MBOX_h_
#define __RTEMS_ITRON_MBOX_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/itron/object.h>

/*
 *  The following defines the control block used to manage each mailbox.
 */

typedef struct {
  ITRON_Objects_Control   Object;
  unsigned32              XXX_more_stuff_goes_here;
}   ITRON_Mailbox_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

ITRON_EXTERN Objects_Information  _ITRON_Mailbox_Information;

/*
 *  _ITRON_Mailbox_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _ITRON_Mailbox_Manager_initialization(
  unsigned32 maximum_mailboxes
);

/*
 *  XXX insert private stuff here
 */

#include <rtems/itron/mbox.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

