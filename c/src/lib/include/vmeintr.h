/*
 *  vmeintr.h
 *
 *  This file is the specification for the VMEbus interface library
 *  which should be provided by all BSPs for VMEbus Single Board
 *  Computers but currently only a few do so.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __VME_INTERRUPT_h
#define __VME_INTERRUPT_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This defines the mask which is used to determine which
 *  interrupt levels are affected by a call to this package.
 *  The LSB corresponds to VME interrupt 0 and the MSB
 *  to VME interrupt 7.
 *
 */

typedef rtems_unsigned8 VME_interrupt_Mask;

/*
 *  VME_interrupt_Disable
 *
 */

void VME_interrupt_Disable (
  VME_interrupt_Mask                mask                        /* IN  */
);

/*
 *  VME_interrupt_Disable
 *
 */

void VME_interrupt_Enable (
  VME_interrupt_Mask                mask                        /* IN  */
);

#ifdef __cplusplus
}
#endif

#endif /* end of include file */
