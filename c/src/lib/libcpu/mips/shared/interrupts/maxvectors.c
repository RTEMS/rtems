/* 
 *  This file contains the maximum number of vectors.  This can not
 *  be determined without knowing the RTEMS CPU model. 
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */


/*
 *  The tx3904 attaches 4 of the eight interrupt bits to an on-CPU interrupt
 *  controller so that these four bits map to 16 unique interrupts.
 *  So you have: 2 software interrupts, an NMI, and 16 others.
 */
#if defined(tx3904)
#define MAX_VECTORS 19
#endif

#ifndef MAX
#define MAX_VECTORS 8
#endif

unsigned int mips_interrupt_number_of_vectors = MAX_VECTORS;
