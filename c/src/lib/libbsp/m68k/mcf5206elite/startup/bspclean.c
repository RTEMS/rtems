/*
 *  MCF5206eLITE bsp_cleanup
 *
 *  This routine returns control from RTEMS to the dBUG monitor.
 *
 *  Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia.
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 * 
 *  http://www.OARcorp.com/rtems/license.html.
 * 
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

/* bsp_cleanup --
 *     This function called when RTEMS execution is finished. bsp_cleanup
 *     for MCF5206eLITE evaluation board throw execution to the dBUG
 *     monitor.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 */
void
bsp_cleanup(void)
{
}
