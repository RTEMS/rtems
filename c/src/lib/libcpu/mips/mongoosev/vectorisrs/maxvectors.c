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
 *  Reserve first 32 for exceptions.
 */

/*
 *  The Synova Mongoose-V attached one of the eight interrupt bits 
 *  to a Peripheral Function Interrupt Cause Register on-CPU.
 *  This results in: 2 software interrupts, 5 interrupts 
 *  through the IP bits, and 32 more from the PFICR.  Some of
 *  these are reserved but for simplicity in processing, we
 *  reserve slots for those bits anyway.
 */

#include <rtems.h>
#include <libcpu/mongoose-v.h>

unsigned int mips_interrupt_number_of_vectors = MONGOOSEV_MAXIMUM_VECTORS;


