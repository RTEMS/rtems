/* 
 *  mmu.c - this file contains functions for initializing the MMU 
 *
 *  Written by Jay Monkman (jmonkman@frasca.com)
 */

#include <bsp.h>
#include <mpc860.h>

/* Macros for handling all the MMU SPRs */
#define PUT_MI_CTR(r)   __asm__ volatile ("mtspr 0x310,%0\n" ::"r"(r))
#define GET_MI_CTR(r)   __asm__ volatile ("mfspr %0,0x310\n" :"=r"(r))
#define PUT_MD_CTR(r)   __asm__ volatile ("mtspr 0x318,%0\n" ::"r"(r))
#define GET_MD_CTR(r)   __asm__ volatile ("mfspr %0,0x318\n" :"=r"(r))
#define PUT_M_CASID(r)  __asm__ volatile ("mtspr 0x319,%0\n" ::"r"(r))
#define GET_M_CASID(r)  __asm__ volatile ("mfspr %0,0x319\n" :"=r"(r))
#define PUT_MI_EPN(r)   __asm__ volatile ("mtspr 0x313,%0\n" ::"r"(r))
#define GET_MI_EPN(r)   __asm__ volatile ("mfspr %0,0x313\n" :"=r"(r))
#define PUT_MI_TWC(r)   __asm__ volatile ("mtspr 0x315,%0\n" ::"r"(r))
#define GET_MI_TWC(r)   __asm__ volatile ("mfspr %0,0x315\n" :"=r"(r))
#define PUT_MI_RPN(r)   __asm__ volatile ("mtspr 0x316,%0\n" ::"r"(r))
#define GET_MI_RPN(r)   __asm__ volatile ("mfspr %0,0x316\n" :"=r"(r))
#define PUT_MD_EPN(r)   __asm__ volatile ("mtspr 0x313,%0\n" ::"r"(r))
#define GET_MD_EPN(r)   __asm__ volatile ("mfspr %0,0x313\n" :"=r"(r))
#define PUT_M_TWB(r)    __asm__ volatile ("mtspr 0x31c,%0\n" ::"r"(r))
#define GET_M_TWB(r)    __asm__ volatile ("mfspr %0,0x31c\n" :"=r"(r))
#define PUT_MD_TWC(r)   __asm__ volatile ("mtspr 0x31d,%0\n" ::"r"(r))
#define GET_MD_TWC(r)   __asm__ volatile ("mfspr %0,0x31d\n" :"=r"(r))
#define PUT_MD_RPN(r)   __asm__ volatile ("mtspr 0x31e,%0\n" ::"r"(r))
#define GET_MD_RPN(r)   __asm__ volatile ("mfspr %0,0x31e\n" :"=r"(r))
#define PUT_MI_AP(r)    __asm__ volatile ("mtspr 0x312,%0\n" ::"r"(r))
#define GET_MI_AP(r)    __asm__ volatile ("mfspr %0,0x312\n" :"=r"(r))
#define PUT_MD_AP(r)    __asm__ volatile ("mtspr 0x31a,%0\n" ::"r"(r))
#define GET_MD_AP(r)    __asm__ volatile ("mfspr %0,0x31a\n" :"=r"(r))
#define PUT_M_TW(r)     __asm__ volatile ("mtspr 0x31f,%0\n" ::"r"(r))
#define GET_M_TW(r)     __asm__ volatile ("mfspr %0,0x31f\n" :"=r"(r))
#define PUT_MI_DCAM(r)  __asm__ volatile ("mtspr 0x330,%0\n" ::"r"(r))
#define GET_MI_DCAM(r)  __asm__ volatile ("mfspr %0,0x330\n" :"=r"(r))
#define PUT_MI_DRAM0(r) __asm__ volatile ("mtspr 0x331,%0\n" ::"r"(r))
#define GET_MI_DRAM0(r) __asm__ volatile ("mfspr %0,0x331\n" :"=r"(r))
#define PUT_MI_DRAM1(r) __asm__ volatile ("mtspr 0x332,%0\n" ::"r"(r))
#define GET_MI_DRAM1(r) __asm__ volatile ("mfspr %0,0x332\n" :"=r"(r))
#define PUT_MD_DCAM(r)  __asm__ volatile ("mtspr 0x338,%0\n" ::"r"(r))
#define GET_MD_DCAM(r)  __asm__ volatile ("mfspr %0,0x338\n" :"=r"(r))
#define PUT_MD_DRAM0(r) __asm__ volatile ("mtspr 0x339,%0\n" ::"r"(r))
#define GET_MD_DRAM0(r) __asm__ volatile ("mfspr %0,0x339\n" :"=r"(r))
#define PUT_MD_DRAM1(r) __asm__ volatile ("mtspr 0x33a,%0\n" ::"r"(r))
#define GET_MD_DRAM1(r) __asm__ volatile ("mfspr %0,0x33a\n" :"=r"(r))
#define PUT_IC_CST(r)   __asm__ volatile ("mtspr 0x230,%0\n" ::"r"(r))
#define GET_IC_CST(r)   __asm__ volatile ("mfspr %0,0x230\n" :"=r"(r))
#define PUT_DC_CST(r)   __asm__ volatile ("mtspr 0x238,%0\n" ::"r"(r))
#define GET_DC_CST(r)   __asm__ volatile ("mfspr %0,0x238\n" :"=r"(r))
#define PUT_IC_ADR(r)   __asm__ volatile ("mtspr 0x231,%0\n" ::"r"(r))
#define GET_IC_ADR(r)   __asm__ volatile ("mfspr %0,0x231\n" :"=r"(r))
#define PUT_IC_DAT(r)   __asm__ volatile ("mtspr 0x232,%0\n" ::"r"(r))
#define GET_IC_DAT(r)   __asm__ volatile ("mfspr %0,0x232\n" :"=r"(r))

extern rtems_configuration_table BSP_Configuration;

void mmu_init(void)
{
  register unsigned long t1, t2;

  /* Let's clear MSR[IR] and MSR[DR] */ 
  t2 = PPC_MSR_IR | PPC_MSR_DR;
  __asm__ volatile (
    "mfmsr    %0\n"
    "andc     %0, %0, %1\n"
    "mtmsr    %0\n" :"=r"(t1), "=r"(t2):
    "1"(t2));

  /* Invalidate the TLBs */
  __asm__ volatile ("tlbia\n"::);
  __asm__ volatile ("isync\n"::);

  /* make sure no TLB entries are reserved */
  t1 = 0;
  PUT_MI_CTR(t1);

  t1 = M860_MD_CTR_TWAM;   /* 4K pages */
  /*  PUT_MD_CTR(t1); */

  t1 = M860_MI_EPN_VALID;    /* make entry valid */
  /*  PUT_MD_EPN(t1); */
  PUT_MI_EPN(t1);

  t1 = M860_MI_TWC_PS8 | M860_MI_TWC_VALID;   /* 8 MB pages, valid */
  /*  PUT_MD_TWC(t1); */
  PUT_MI_TWC(t1);

  t1 = M860_MD_RPN_CHANGE | M860_MD_RPN_F | M860_MD_RPN_16K | 
       M860_MD_RPN_SHARED | M860_MD_RPN_VALID;
  /*  PUT_MD_RPN(t1); */
  PUT_MI_RPN(t1);

  t1 = M860_MI_AP_Kp << 30;  
  PUT_MI_AP(t1);
  /*  PUT_MD_AP(t1); */
  
  t1 = M860_CACHE_CMD_UNLOCK;
  /*  PUT_DC_CST(t1); */
  PUT_IC_CST(t1);

  t1 = M860_CACHE_CMD_INVALIDATE;
  /*  PUT_DC_CST(t1); */
  PUT_IC_CST(t1);

  t1 = M860_CACHE_CMD_ENABLE;
  PUT_IC_CST(t1);

  t1 = M860_CACHE_CMD_SFWT;
  /*  PUT_DC_CST(t1); */
  t1 = M860_CACHE_CMD_ENABLE;
  /*  PUT_DC_CST(t1);*/



  /* Let's set MSR[IR]  */ 
  t2 = PPC_MSR_IR;
  __asm__ volatile (
    "mfmsr    %0\n"
    "or       %0, %0, %1\n"
    "mtmsr    %0\n" :"=r"(t1), "=r"(t2):
    "1"(t2));

}
