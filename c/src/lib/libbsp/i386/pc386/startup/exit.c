/*
 * Routines to shutdown and reboot the PC.
 *
 * (C) Copyright 1997 -
 * - NavIST Group - Real-Time Distributed Systems and Industrial Automation
 *   http://pandora.ist.utl.pt
 *   Instituto Superior Tecnico * Lisboa * PORTUGAL
 *
 *  Disclaimer:
 *
 * This file is provided "AS IS" without warranty of any kind, either
 * expressed or implied.
 * 
 *
 * This code is based on:
 *   exit.c,v 1.2 1995/12/19 20:07:36 joel Exp -  go32 BSP
 * With the following copyright notice:
 * **************************************************************************
 * *  COPYRIGHT (c) 1989-1999.
 * *  On-Line Applications Research Corporation (OAR).
 * *
 * *  The license and distribution terms for this file may be
 * *  found in found in the file LICENSE in this distribution or at
 * *  http://www.rtems.com/license/LICENSE.
 * **************************************************************************
 *
 * Joel: It has been modified many times since submission.
 *
 *  $Id$
 */

#include <stdio.h>
#include <bsp.h>
#include <rtems/libio.h>
#include <uart.h>

void bsp_cleanup(void)
{
  /*
   * At this point, the console driver is disconnected => we must
   * use polled output/input. This is exactly what printk
   * does.
   */
  printk( "\nEXECUTIVE SHUTDOWN! " );

  #if (BSP_PRESS_KEY_FOR_RESET)
    { 
      unsigned char ch;

      printk( "Any key to reboot..." );
      ch = BSP_poll_char();
    }
  #else
    printk( "resetting system\n" );
  #endif
  rtemsReboot();
}
