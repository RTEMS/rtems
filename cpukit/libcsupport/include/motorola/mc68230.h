/**
 * @file rtems/motorola/mc68230.h
 *
 * Low level support code for the Motorola 68230 Parallel
 * Interface/Timer (PIT)
 */

/*
 * Modified by Doug McBride, Colorado Space Grant College
 *
 * Format taken partly from RTEMS code and mostly from Motorola IDP user's
 * manual.  RTEMS copyright information below.
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_MOTOROLA_MC68230_H
#define _RTEMS_MOTOROLA_MC68230_H

/* Some Motorola IDP User manual defines: */
#define MC68230_PIT_ADDR	0x00c01003		/* base address of the PIT */
#define MC68230_REGOFF	0x04				/* Difference between addresses */
#define MC68230_VECT	64
#define MC68230_H1VECT	0x00
#define MC68230_H2VECT	0x01
#define MC68230_H3VECT	0x02
#define MC68230_H4VECT	0x03

/*
 * mc68230 register offsets
 */
#define MC68230_PGCR	0x00
#define MC68230_PSRR	1*MC68230_REGOFF
#define MC68230_PADDR	2*MC68230_REGOFF
#define MC68230_PBDDR	3*MC68230_REGOFF
#define MC68230_PCDDR	4*MC68230_REGOFF
#define MC68230_PIVR	5*MC68230_REGOFF
#define MC68230_PACR	6*MC68230_REGOFF
#define MC68230_PBCR	7*MC68230_REGOFF
#define MC68230_PADR	8*MC68230_REGOFF
#define MC68230_PBDR	9*MC68230_REGOFF
#define MC68230_PAAR	10*MC68230_REGOFF
#define MC68230_PBAR	11*MC68230_REGOFF
#define MC68230_PCDR	12*MC68230_REGOFF
#define MC68230_PITSR	13*MC68230_REGOFF
#define MC68230_TCR	16*MC68230_REGOFF
#define MC68230_TIVR	17*MC68230_REGOFF
#define MC68230_CPRH	19*MC68230_REGOFF
#define MC68230_CPRM	20*MC68230_REGOFF
#define MC68230_CPRL	21*MC68230_REGOFF
#define MC68230_CNTRH	23*MC68230_REGOFF
#define MC68230_CNTRM	24*MC68230_REGOFF
#define MC68230_CNTRL	25*MC68230_REGOFF
#define MC68230_TSR	26*MC68230_REGOFF

/* Some RTEMS style defines: */
#ifndef MC68230_VOL8
#define MC68230_VOL8( ptr )   ((volatile uint8_t   *)(ptr))
#endif

#define MC68230_WRITE( reg, data ) \
   *(MC68230_VOL8(MC68230_PIT_ADDR+reg)) = (data)

#define MC68230_READ( reg, data ) \
   (data) = *(MC68230_VOL8(MC68230_PIT_ADDR+reg))

#endif
