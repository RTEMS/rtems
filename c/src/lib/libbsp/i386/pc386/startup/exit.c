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
| *  COPYRIGHT (c) 1989-1998.
| *  On-Line Applications Research Corporation (OAR).
| *  Copyright assigned to U.S. Government, 1994. 
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.OARcorp.com/rtems/license.html.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/


#include <stdio.h>
#include <bsp.h>
#include <pc386uart.h>

/*-------------------------------------------------------------------------+
 | Which console is in use: either (-1) which means normal console or 
 | uart id if uart was used
 +-------------------------------------------------------------------------*/
extern int PC386ConsolePort;

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern rtems_boolean _IBMPC_scankey(char *);  /* defined in 'inch.c' */

/*-------------------------------------------------------------------------+
|         Function: _exit
|      Description: Shutdown the PC. Called from libc's 'exit'. 
| Global Variables: None.
|        Arguments: status - exit status (ignored).
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void _exit(int status)
{
  unsigned char ch, *cp;
  static   char line[]="EXECUTIVE SHUTDOWN! Any key to reboot...";

  if(PC386ConsolePort == PC386_CONSOLE_PORT_CONSOLE)
    {

      printk("\n");
      printk(line);
      while(!_IBMPC_scankey(&ch))
	;
      printk("\n\n");
    }
  else
    {
      PC386_uart_intr_ctrl(PC386ConsolePort, PC386_UART_INTR_CTRL_DISABLE);
      
      PC386_uart_polled_write(PC386ConsolePort, '\r');
      PC386_uart_polled_write(PC386ConsolePort, '\n');
      
      for(cp=line; *cp != 0; cp++)
	{
	  PC386_uart_polled_write(PC386ConsolePort, *cp);
	}

      PC386_uart_polled_read(PC386ConsolePort);

      PC386_uart_polled_write(PC386ConsolePort, '\r');
      PC386_uart_polled_write(PC386ConsolePort, '\n');
      PC386_uart_polled_write(PC386ConsolePort, '\r');
      PC386_uart_polled_write(PC386ConsolePort, '\n');
    }

  rtemsReboot();
} /* _exit */







