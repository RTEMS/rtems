/**
 *  @file
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/mips/idtcpu.h>

extern void resettlb( int i );

void init_tlb(void);

void init_tlb(void)
{
  int i;

  for (i = 0; i < N_TLB_ENTRIES; i++ )
    resettlb(i);
}
