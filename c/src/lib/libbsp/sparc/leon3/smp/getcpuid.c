/**
 *  @file
 *
 *  LEON3 SMP Obtain CPU Core Number
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

int bsp_smp_processor_id(void)
{
  unsigned int id;
  __asm__ __volatile__( "rd     %%asr17,%0\n\t" : "=r" (id) : );

  return ((id >> 28) & 0xff);
}

