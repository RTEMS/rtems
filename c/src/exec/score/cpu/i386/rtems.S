/*  rtems.s
 *
 *  This file contains the single entry point code for
 *  the i386 implementation of RTEMS.
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

#include <asm.h>

        EXTERN (_Entry_points)

        BEGIN_CODE

        .align  2
        PUBLIC (RTEMS)

SYM (RTEMS):
        jmpl      *SYM (_Entry_points)(,eax,4)

        END_CODE

END
