/*
 *  SPARC Dependent Source
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

/*
 *  This is the executive's trap table which is installed into the TBR
 *  register.
 *
 *  NOTE:  Unfortunately, this must be aligned on a 4096 byte boundary.
 *         The GNU tools as of binutils 2.5.2 and gcc 2.7.0 would not
 *         align an entity to anything greater than a 512 byte boundary.
 *
 *         Because of this, we pull a little bit of a trick.  We allocate
 *         enough memory so we can grab an address on a 4096 byte boundary
 *         from this area.
 */

#ifndef _libcpu_tbr_h
#define _libcpu_tbr_h

#include <bsp.h>

#ifndef NO_TABLE_MOVE

#define SPARC_TRAP_TABLE_ALIGNMENT 4096

SCORE_EXTERN unsigned8 _CPU_Trap_Table_area[8192]
  __attribute__ ((aligned (SPARC_TRAP_TABLE_ALIGNMENT)));
#endif

#endif
