/**
 * @file rtems/itron/port.h
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_ITRON_PORT_H
#define _RTEMS_ITRON_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/itron/object.h>

/*
 *  The following defines the control block used to manage each port.
 */

typedef struct {
  ITRON_Objects_Control   Object;
  uint32_t                XXX_more_stuff_goes_here;
}   ITRON_Port_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

ITRON_EXTERN Objects_Information  _ITRON_Port_Information;

/*
 *  _ITRON_Port_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _ITRON_Port_Manager_initialization(void);

/*
 *  XXX insert private stuff here
 */

#include <rtems/itron/port.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
