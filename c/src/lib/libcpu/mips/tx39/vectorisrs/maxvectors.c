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
 *  The Toshiba TX3904 attaches 4 of the eight interrupt bits to an
 *  on-CPU interrupt controller so that these four bits map to 16
 *  unique interrupts.  So you have: 2 software interrupts, an NMI,
 *  and 16 others.
 */

#include <rtems.h>
#include <libcpu/tx3904.h>

unsigned int mips_interrupt_number_of_vectors = TX3904_MAXIMUM_VECTORS;

