/*-------------------------------------------------------------------------+
| exit.c - ARM BSP 
+--------------------------------------------------------------------------+
| Routines to shutdown and reboot the BSP.
+--------------------------------------------------------------------------+
|
| Copyright (c) 2000 Canon Research Centre France SA.
| Emmanuel Raguet, mailto:raguet@crf.canon.fr
|
|   The license and distribution terms for this file may be
|   found in found in the file LICENSE in this distribution or at
|   http://www.rtems.com/license/LICENSE.
|
+--------------------------------------------------------------------------*/


#include <stdio.h>
#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>

void bsp_reset(void);

void bsp_cleanup(void)
{
  bsp_reset();
}


