#include <bsp.h>
#include <rtems/mips/idtcpu.h>

void init_tlb(void)
{
  int i;

        for (i = 0; i < N_TLB_ENTRIES; i++ )
                resettlb(i);
}
