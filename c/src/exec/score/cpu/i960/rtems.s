/*  rtems.s
 *
 *  This file contains the single entry point code for
 *  the i960 implementation of RTEMS.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

        .text

        .align  4
        .globl  RTEMS

RTEMS:
        ld        __Entry_points[g7*4],r4
        bx        (r4)

