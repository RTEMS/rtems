/*
 *  This is a shared BSP pretasking hook which does nothing.
 *  If all the BSP needs to do is initialize the C library,
 *  then it can rely on bootcard.c and provide bsp_get_workarea().
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <fcntl.h>

void bsp_pretasking_hook(void)
{
}
