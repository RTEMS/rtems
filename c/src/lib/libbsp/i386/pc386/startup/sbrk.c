/*-------------------------------------------------------------------------+
| sbrk.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| If the BSP wants to dynamically allocate the memory for the C Library heap
| (malloc) and/or be able to extend the heap, then this routine must be
| functional. RTEMS newlib suppport has an implementation of malloc using
| RTEMS regions => the user can do mallocs.
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
|   sbrk.c,v 1.2 1995/12/19 20:07:38 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1999.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.OARcorp.com/rtems/license.html.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/


#include <errno.h>
#include <sys/types.h>

/*-------------------------------------------------------------------------+
|         Function: sbrk
|      Description: Stub for sbrk. 
| Global Variables: None.
|        Arguments: Not relevant.
|          Returns: Not relevant. 
+--------------------------------------------------------------------------*/
void *sbrk(size_t incr)
{
  errno = EINVAL;
  return (void *)NULL;
} /* sbrk */
