/*
 * mc68230.h -- Low level support code for the Motorola 68230 Parallel
 *              Interface/Timer (PIT)
 *
 * Modified by Doug McBride, Colorado Space Grant College
 *
 * Format taken partly from RTEMS code and mostly from Motorola IDP user's
 * manual.  RTEMS copyright information below.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __MC68230_H__
#define __MC68230_H__

/* Some Motorola IDP User manual defines: */
#define PIT_ADDR	0x00c01003		/* base address of the PIT */
#define REGOFF	0x04				/* Difference between addresses */
#define VECT	64
#define H1VECT	0x00
#define H2VECT	0x01
#define H3VECT	0x02
#define H4VECT	0x03

/*
 * mc68230 register offsets
 */
#define PGCR	0x00
#define PSRR	1*REGOFF
#define PADDR	2*REGOFF
#define PBDDR	3*REGOFF
#define PCDDR	4*REGOFF
#define PIVR	5*REGOFF
#define PACR	6*REGOFF
#define PBCR	7*REGOFF
#define PADR	8*REGOFF
#define PBDR	9*REGOFF
#define PAAR	10*REGOFF
#define PBAR	11*REGOFF
#define PCDR	12*REGOFF
#define PITSR	13*REGOFF
#define TCR		16*REGOFF
#define TIVR	17*REGOFF
#define CPRH	19*REGOFF
#define CPRM	20*REGOFF
#define CPRL	21*REGOFF
#define CNTRH	23*REGOFF
#define CNTRM	24*REGOFF
#define CNTRL	25*REGOFF
#define TSR		26*REGOFF

/* Some RTEMS style defines: */
#ifndef VOL8
#define VOL8( ptr )   ((volatile rtems_unsigned8 *)(ptr))
#endif

#define MC68230_WRITE( reg, data ) \
   *(VOL8(PIT_ADDR+reg)) = (data)

#define MC68230_READ( reg, data ) \
   (data) = *(VOL8(PIT_ADDR+reg))

#endif
