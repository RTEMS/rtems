/* 
 *  This file contains the maximum number of vectors.  This can not
 *  be determined without knowing the RTEMS CPU model. 
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  maxvectors.c,v 1.1.6.1 2003/09/04 18:45:49 joel Exp
 */

/*
 *  Reserve first 32 for exceptions.
 */

/*
 *  The Toshiba TX4925 attaches 5 of the eight interrupt bits to an
 *  on-CPU interrupt controller so that these five bits map to 32
 *  unique interrupts.  So you have: 2 software interrupts, an NMI,
 *  and 32 others.
 */

#include <rtems.h>
#include <libcpu/tx4925.h>

unsigned int mips_interrupt_number_of_vectors = TX4925_MAXIMUM_VECTORS;

