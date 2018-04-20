/*
 *  cpuinit.c - this file contains functions for initializing the CPU
 *
 *  Written by Jay Monkman (jmonkman@frasca.com)
 */

#include <bsp.h>

/* Macros for handling all the MMU SPRs */
#define PUT_IC_CST(r)   __asm__ volatile ("mtspr 0x230,%0\n" ::"r"(r))
#define GET_IC_CST(r)   __asm__ volatile ("mfspr %0,0x230\n" :"=r"(r))
#define PUT_DC_CST(r)   __asm__ volatile ("mtspr 0x238,%0\n" ::"r"(r))
#define GET_DC_CST(r)   __asm__ volatile ("mfspr %0,0x238\n" :"=r"(r))

void cpu_init(void)
{
  /* BRGCLK is VCO_OUT/4 */
/*
  m8260.sccr = 0;
*/

#if 0
  register unsigned long t1, t2;

  /* Let's clear MSR[IR] and MSR[DR] */
  t2 = PPC_MSR_IR | PPC_MSR_DR;
  __asm__ volatile (
    "mfmsr    %0\n"
    "andc     %0, %0, %1\n"
    "mtmsr    %0\n" :"=r"(t1), "=r"(t2):
    "1"(t2));

  t1 = M8xx_CACHE_CMD_UNLOCK;
  /*  PUT_DC_CST(t1); */
  PUT_IC_CST(t1);

  t1 = M8xx_CACHE_CMD_INVALIDATE;
  /*  PUT_DC_CST(t1); */
  PUT_IC_CST(t1);

  t1 = M8xx_CACHE_CMD_ENABLE;
  PUT_IC_CST(t1);

  t1 = M8xx_CACHE_CMD_SFWT;
  /*  PUT_DC_CST(t1); */
  t1 = M8xx_CACHE_CMD_ENABLE;
  /*  PUT_DC_CST(t1);*/
#endif
}
