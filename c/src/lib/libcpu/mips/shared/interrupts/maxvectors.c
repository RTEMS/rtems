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
 *  The Toshiba TX3904 attaches 4 of the eight interrupt bits to an
 *  on-CPU interrupt controller so that these four bits map to 16
 *  unique interrupts.  So you have: 2 software interrupts, an NMI,
 *  and 16 others.
 */

#if defined(TX39)
#define MAX_VECTORS 19
#endif

/*
 *  The Synova Mongoose-V attached one of the eight interrupt bits 
 *  to a Peripheral Function Interrupt Cause Register on-CPU.
 *  This results in: 2 software interrupts, 5 interrupts 
 *  through the IP bits, and 32 more from the PFICR.  Some of
 *  these are reserved but for simplicity in processing, we
 *  reserve slots for those bits anyway.
 */

#if defined(MONGOOSEV)
#define MAX_VECTORS 37
#endif

#ifndef MAX_VECTORS
#define MAX_VECTORS 8
#endif

unsigned int mips_interrupt_number_of_vectors = MAX_VECTORS;
