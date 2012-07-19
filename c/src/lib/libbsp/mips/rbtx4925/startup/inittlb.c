/*
 *  inittlb.c
 */

#include <rtems/mips/idtcpu.h>

extern void resettlb( int i );

void init_tlb(void)
{
  int i;

        for (i = 0; i < N_TLB_ENTRIES; i++ )
                resettlb(i);
}

