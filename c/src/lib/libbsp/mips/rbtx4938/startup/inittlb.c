/*
 *  inittlb.c,v 1.2 1999/03/31 23:21:19 joel Exp
 */

#include <rtems/mips/idtcpu.h>

extern void resettlb( int i );

void init_tlb(void)
{
  int i;

        for (i = 0; i < N_TLB_ENTRIES; i++ )
                resettlb(i);
}

