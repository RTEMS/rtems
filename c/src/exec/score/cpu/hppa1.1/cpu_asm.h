/*
 * Copyright (c) 1990,1991 The University of Utah and
 * the Center for Software Science (CSS).  All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software is hereby
 * granted provided that (1) source code retains these copyright, permission,
 * and disclaimer notices, and (2) redistributions including binaries
 * reproduce the notices in supporting documentation, and (3) all advertising
 * materials mentioning features or use of this software display the following
 * acknowledgement: ``This product includes software developed by the Center
 * for Software Science at the University of Utah.''
 *
 * THE UNIVERSITY OF UTAH AND CSS ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS
 * IS" CONDITION.  THE UNIVERSITY OF UTAH AND CSS DISCLAIM ANY LIABILITY OF
 * ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * CSS requests users of this software to return to css-dist@cs.utah.edu any
 * improvements that they make and grant CSS redistribution rights.
 *
 *      Utah $Hdr: asm.h 1.6 91/12/03$
 *
 *  RTEMS: $Id$
 */

/*
 * Hardware Space Registers
 */
sr0     .reg    %sr0
sr1     .reg    %sr1
sr2     .reg    %sr2
sr3     .reg    %sr3
sr4     .reg    %sr4
sr5     .reg    %sr5
sr6     .reg    %sr6
sr7     .reg    %sr7

/*
 * Control register aliases
 */

rctr    .reg    %cr0
pidr1   .reg    %cr8
pidr2   .reg    %cr9
ccr     .reg    %cr10
sar     .reg    %cr11
pidr3   .reg    %cr12
pidr4   .reg    %cr13
iva     .reg    %cr14
eiem    .reg    %cr15
itmr    .reg    %cr16
pcsq    .reg    %cr17
pcoq    .reg    %cr18
iir     .reg    %cr19
isr     .reg    %cr20
ior     .reg    %cr21
ipsw    .reg    %cr22
eirr    .reg    %cr23

/*
 * Calling Convention
 */
rp      .reg    %r2
arg3    .reg    %r23
arg2    .reg    %r24
arg1    .reg    %r25
arg0    .reg    %r26
dp      .reg    %r27
ret0    .reg    %r28
ret1    .reg    %r29
sl      .reg    %r29
sp      .reg    %r30


