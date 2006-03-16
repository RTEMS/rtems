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


#include <rtems.h>
#include <libcpu/rm5231.h>

unsigned int mips_interrupt_number_of_vectors = RM5231_MAXIMUM_VECTORS;

