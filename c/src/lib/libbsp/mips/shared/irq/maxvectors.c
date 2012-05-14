/**
 *  @file
 *  
 *  This file contains the maximum number of vectors.  This can not
 *  be determined without knowing the RTEMS CPU model.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp/irq.h>

unsigned int mips_interrupt_number_of_vectors = BSP_INTERRUPT_VECTOR_MAX;
