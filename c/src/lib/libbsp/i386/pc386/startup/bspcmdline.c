/*-------------------------------------------------------------------------+
| This file contains the PC386 BSP startup package. It includes application,
| board, and monitor specific initialization and configuration. The generic CPU
| dependent initialization has been performed before this routine is invoked.
+--------------------------------------------------------------------------+
| (C) Copyright 2009 RTEMS Project
|     Chris Johns (chrisj@rtems.org)
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   common sense
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-2008.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in the file LICENSE in this distribution or at
| *  http://www.rtems.com/license/LICENSE.
| **************************************************************************
+--------------------------------------------------------------------------*/

#include <bsp.h>
#include <rtems/pci.h>
#include <libcpu/cpuModel.h>

/*
 *  External data
 */
extern uint32_t _boot_multiboot_flags;
extern uint32_t _boot_multiboot_memory[2];
extern const char _boot_multiboot_cmdline[256];

/*-------------------------------------------------------------------------+
|         Function: bsp_cmdline
|      Description: Call when you want the command line.
| Global Variables: The multiboot values copied from the loader.
|        Arguments: None.
|          Returns: The whole command line.
+--------------------------------------------------------------------------*/
const char* bsp_cmdline( void )
{
  return _boot_multiboot_cmdline;
}

/*-------------------------------------------------------------------------+
|         Function: bsp_cmdline_arg
|      Description: Call to search for an argument.
| Global Variables: The multiboot values copied from the loader.
|        Arguments: The option start.
|          Returns: The option if found or nothing.
+--------------------------------------------------------------------------*/
const char* bsp_cmdline_arg( const char* arg )
{
  return strstr (bsp_cmdline (), arg);
}
