/*  rtems.s
 *
 *  This file contains the single entry point code for
 *  the i386 implementation of RTEMS.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <asm.h>

        EXTERN (_Entry_points)

        BEGIN_CODE

        .align  2
        PUBLIC (RTEMS)

SYM (RTEMS):
        jmpl      SYM (_Entry_points)(,eax,4)

        END_CODE

END
