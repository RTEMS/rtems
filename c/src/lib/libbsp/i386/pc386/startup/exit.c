/*-------------------------------------------------------------------------+
| exit.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| Routines to shutdown and reboot the PC.
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   exit.c,v 1.2 1995/12/19 20:07:36 joel Exp -  go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1999.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.rtems.com/license/LICENSE.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/


#include <stdio.h>
#include <bsp.h>
#include <rtems/libio.h>
#include <uart.h>

void bsp_cleanup(void)
{
  unsigned char ch;
  static   char line[]="EXECUTIVE SHUTDOWN! Any key to reboot...";
  /*
   * AT this point, the console driver is disconnected => we must
   * use polled output/input. This is exactly what printk
   * does.
   */
  printk("\n");
  printk(line);
  ch = BSP_poll_char();
  rtemsReboot();
}








