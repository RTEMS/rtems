/* 
 *  This file contains the single entry point code for
 *  the SH implementation of RTEMS.
 *
 *  NOTE:  This is supposed to be a .S or .s file NOT a C file.
 *
 *  NOTE: UNTESTED, very likely this does not not work.
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *
 *  COPYRIGHT (c) 1998-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

/*
 *  This is supposed to be an assembly file.  This means that system.h
 *  and cpu.h should not be included in a "real" rtems file.
 */

/* #include <rtems/system.h> */
/* #include <rtems/score/cpu.h> */
/* #include "asm.h" */

/*
 * This should work but due to a bug in rtems building scheme it doesn't work
 */
 
/* #include <rtems/directives.h> */

extern void* _Entry_points[] ;

/*
 *  RTEMS
 *
 *  This routine jumps to the directive indicated in the
 *  CPU defined register.  This routine is used when RTEMS is
 *  linked by itself and placed in ROM.  This routine is the
 *  first address in the ROM space for RTEMS.  The user "calls"
 *  this address with the directive arguments in the normal place.
 *  This routine then jumps indirectly to the correct directive
 *  preserving the arguments.  The directive should not realize
 *  it has been "wrapped" in this way.  The table "_Entry_points"
 *  is used to look up the directive.
 */

void __RTEMS()
{
  asm volatile ( 
       ".global _RTEMS
_RTEMS:" );

  asm volatile ( 
       "jmp %0
   	rts
   	nop"
    :: "m" (_Entry_points) );
}
