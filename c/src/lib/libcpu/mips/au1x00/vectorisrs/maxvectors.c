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
 *  maxvectors.c,v 1.1 2001/05/24 19:54:22 joel Exp
 */

/*
 *  Reserve first 32 for exceptions.
 */

#include <rtems.h>
#include <libcpu/au1x00.h>

unsigned int mips_interrupt_number_of_vectors = AU1X00_MAXIMUM_VECTORS;

