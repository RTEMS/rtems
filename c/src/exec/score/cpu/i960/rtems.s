/*  rtems.s
 *
 *  This file contains the single entry point code for
 *  the i960 implementation of RTEMS.
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

        .text

        .align  4
        .globl  RTEMS

RTEMS:
        ld        __Entry_points[g7*4],r4
        bx        (r4)

