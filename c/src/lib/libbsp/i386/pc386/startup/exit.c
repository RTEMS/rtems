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
| * COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.                      *
| * On-Line Applications Research Corporation (OAR).                       *
| * All rights assigned to U.S. Government, 1994.                          *
| *                                                                        *
| * This material may be reproduced by or for the U.S. Government pursuant *
| * to the copyright license under the clause at DFARS 252.227-7013.  This *
| * notice must appear in all copies of this file and its derivatives.     *
| **************************************************************************
+--------------------------------------------------------------------------*/


#include <stdio.h>

#include <bsp.h>

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern rtems_boolean _IBMPC_scankey(char *);  /* define in 'inch.c' */


/*-------------------------------------------------------------------------+
|         Function: rtemsReboot
|      Description: Reboot the PC. 
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
inline void rtemsReboot(void)
{
  /* shutdown and reboot */
  outport_byte(0x64, 0xFE);        /* use keyboard controler to do the job... */
} /* rtemsReboot */


/*-------------------------------------------------------------------------+
|         Function: _exit
|      Description: Shutdown the PC. Called from libc's 'exit'. 
| Global Variables: None.
|        Arguments: status - exit status (ignored).
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void _exit(int status)
{
  unsigned char ch;
  puts("\nEXECUTIVE SHUTDOWN! Any key to reboot...");

  while(!_IBMPC_scankey(&ch))
    ;

  rtemsReboot();
} /* _exit */
