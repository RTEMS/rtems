/*
 * limits.h - definition of machine & system dependent address limits
 *
 *                   THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  The following software is offered for use in the public domain.
 *  There is no warranty with regard to this software or its performance
 *  and the user must accept the software "AS IS" with all faults.
 *
 *  THE CONTRIBUTORS DISCLAIM ANY WARRANTIES, EXPRESS OR IMPLIED, WITH
 *  REGARD TO THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _MEMLIMITS_H_
#define _MEMLIMITS_H_

/*
 * The macros in this file are specific to a given implementation.
 * The general rules for their construction are as follows:
 *
 * 1.) is_readable(addr,length) should be true if and only if the
 *     region starting at the given virtual address can be read
 *     _without_ causing an exception or other fatal error.  Note
 *     that the stub will use the strictest alignment satisfied
 *     by _both_ addr and length (e.g., if both are divisible by
 *     8 then the region will be read in double-word chunks).
 *
 * 2.) is_writeable(addr,length) should be true if and only if the
 *     region starting at the given virtual address can be written
 *     _without_ causing an exception or other fatal error.  Note
 *     that the stub will use the strictest alignment satisfied
 *     by _both_ addr and length (e.g., if both are divisible by
 *     8 then the region will be written in double-word chunks).
 *
 * 3.) is-steppable(ptr) whould be true if and only if ptr is the
 *     address of a writeable region of memory which may contain
 *     an executable instruction.  At a minimum this requires that
 *     ptr be word-aligned (divisible by 4) and not point to EPROM
 *     or memory-mapped I/O.
 *
 * Note: in order to satisfy constraints related to cacheability
 * of certain memory subsystems it may be necessary for regions
 * of kseg0 and kseg1 which map to the same physical addresses
 * to have different readability and/or writeability attributes.
 */

/*
#define K0_LIMIT_FOR_READ  (K0BASE+0x18000000)
#define K1_LIMIT_FOR_READ  (K1BASE+K1SIZE)

#define is_readable(addr,length) \
 (((K0BASE <= addr) && ((addr + length) <= K0_LIMIT_FOR_READ)) \
  || ((K1BASE <= addr) && ((addr + length) <= K1_LIMIT_FOR_READ)))

#define K0_LIMIT_FOR_WRITE (K0BASE+0x08000000)
#define K1_LIMIT_FOR_WRITE (K1BASE+0x1e000000)

#define is_writeable(addr,length) \
 (((K0BASE <= addr) && ((addr + length) <= K0_LIMIT_FOR_WRITE)) \
  || ((K1BASE <= addr) && ((addr + length) <= K1_LIMIT_FOR_WRITE)))

#define K0_LIMIT_FOR_STEP  (K0BASE+0x08000000)
#define K1_LIMIT_FOR_STEP  (K1BASE+0x08000000)

#define is_steppable(ptr) \
 ((((int)ptr & 0x3) == 0) \
  && (((K0BASE <= (int)ptr) && ((int)ptr < K0_LIMIT_FOR_STEP)) \
      || ((K1BASE <= (int)ptr) && ((int)ptr < K1_LIMIT_FOR_STEP))))

struct memseg
{
      unsigned begin, end, opts;
};

#define MEMOPT_READABLE   1
#define MEMOPT_WRITEABLE  2

#define NUM_MEMSEGS     10

int add_memsegment(unsigned,unsigned,int);
int is_readable(unsigned,unsigned);
int is_writeable(unsigned,unsigned);
int is_steppable(unsigned);
*/

#endif  /* _MEMLIMITS_H_ */
