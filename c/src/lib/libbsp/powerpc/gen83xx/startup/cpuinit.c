/*===============================================================*\
| Project: RTEMS generic MPC83xx BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the code to initialize the cpu               |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       cpuinit.c                                           */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS MPC5x00 C level startup code                  */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:  This file contains additional functions for         */
/*                 initializing the MPC5x00 CPU                        */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   MPC8260ads additional CPU initialization            */
/*   Module:       cpuinit.c                                           */
/*   Project:      RTEMS 4.6.0pre1 / MCF8260ads BSP                    */
/*   Version       1.1                                                 */
/*   Date:         10/22/2002                                          */
/*                                                                     */
/*   Author(s) / Copyright(s):                                         */
/*                                                                     */
/*   Written by Jay Monkman (jmonkman@frasca.com)                      */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Partially based on the code references which are named above.     */
/*   Adaptions, modifications, enhancements and any recent parts of    */
/*   the code are under the right of                                   */
/*                                                                     */
/*         IPR Engineering, Dachauer Straße 38, D-80335 München        */
/*                        Copyright(C) 2003                            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   IPR Engineering makes no representation or warranties with        */
/*   respect to the performance of this computer program, and          */
/*   specifically disclaims any responsibility for any damages,        */
/*   special or consequential, connected with the use of this program. */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Version history:  1.0                                             */
/*                                                                     */
/***********************************************************************/

#include <bsp.h>
#include <rtems/powerpc/registers.h>
#include <mpc83xx/mpc83xx.h>

#include <libcpu/mmu.h>
#include <libcpu/spr.h>
#include <string.h>

#define USE_IMMU

/* Macros for HID0 access */
#define SET_HID0(r)   __asm__ volatile ("mtspr 0x3F0,%0\n" ::"r"(r))
#define GET_HID0(r)   __asm__ volatile ("mfspr %0,0x3F0\n" :"=r"(r))

#define DBAT_MTSPR(val,name) __MTSPR(val,name);
#define SET_DBAT(n,uv,lv) {DBAT_MTSPR(lv,DBAT##n##L);DBAT_MTSPR(uv,DBAT##n##U);}
#if defined(USE_IMMU )
#define IBAT_MTSPR(val,name) __MTSPR(val,name);
#define SET_IBAT(n,uv,lv) {IBAT_MTSPR(lv,IBAT##n##L);IBAT_MTSPR(uv,IBAT##n##U);}
#endif

void calc_dbat_regvals(BAT *bat_ptr,
		       uint32_t base_addr,
		       uint32_t size,
		       boolean flg_w,
		       boolean flg_i,
		       boolean flg_m,
		       boolean flg_g,
		       boolean flg_bpp)
{
  uint32_t block_mask;
  uint32_t end_addr;

  /*
   * determine block mask, that overlaps the whole block
   */
  end_addr = base_addr+size-1;
  block_mask = ~0;
  while ((end_addr & block_mask) != (base_addr & block_mask)) {
    block_mask <<= 1;
  }
  
  bat_ptr->batu.bepi  = base_addr  >> (32-15); 
  bat_ptr->batu.bl    = ~(block_mask >> (28-11));
  bat_ptr->batu.vs    = 1;
  bat_ptr->batu.vp    = 1;
  
  bat_ptr->batl.brpn  = base_addr  >> (32-15); 
  bat_ptr->batl.w  = flg_w; 
  bat_ptr->batl.i  = flg_i; 
  bat_ptr->batl.m  = flg_m; 
  bat_ptr->batl.g  = flg_g; 
  bat_ptr->batl.pp = flg_bpp; 
}

void clear_mmu_regs(void)
{
  uint32_t i;
  /*
   * clear segment registers
   */
  for (i = 0;i < 16;i++) {
    asm volatile(" mtsrin	%0, %1\n"::"r" (i * 0x1000),"r"(i<<(31-3)));
  }
  /*
   * clear TLBs
   */
  for (i = 0;i < 32;i++) {
    asm volatile(" tlbie	%0\n"::"r" (i << (31-19)));
  }
}

void cpu_init(void)
{
  register unsigned long reg;
  BAT dbat,ibat;

  /*
   * clear MMU/Segment registers
   */
  clear_mmu_regs();
  /*
   * clear caches
   */
  GET_HID0(reg);
  reg |=  (HID0_ICFI | HID0_DCI);
  SET_HID0(reg);
  reg &= ~(HID0_ICFI | HID0_DCI);
  SET_HID0(reg);
  
  /*
   * set up IBAT registers in MMU
   */
  memset(&ibat,0,sizeof(ibat));
  SET_IBAT(2,ibat.batu,ibat.batl);
  SET_IBAT(3,ibat.batu,ibat.batl);
  SET_IBAT(4,ibat.batu,ibat.batl);
  SET_IBAT(5,ibat.batu,ibat.batl);
  SET_IBAT(6,ibat.batu,ibat.batl);
  SET_IBAT(7,ibat.batu,ibat.batl);

  calc_dbat_regvals(&ibat,RAM_START,RAM_SIZE,0,0,0,0,BPP_RX);
  SET_IBAT(0,ibat.batu,ibat.batl);
  calc_dbat_regvals(&ibat,ROM_START,ROM_SIZE,0,0,0,0,BPP_RX);
  SET_IBAT(1,ibat.batu,ibat.batl);

  /*
   * set up DBAT registers in MMU
   */
  memset(&dbat,0,sizeof(dbat));
  SET_DBAT(3,dbat.batu,dbat.batl);
  SET_DBAT(4,dbat.batu,dbat.batl);
  SET_DBAT(5,dbat.batu,dbat.batl);
  SET_DBAT(6,dbat.batu,dbat.batl);
  SET_DBAT(7,dbat.batu,dbat.batl);

  calc_dbat_regvals(&dbat,RAM_START,RAM_SIZE,1,0,1,0,BPP_RW);
  SET_DBAT(0,dbat.batu,dbat.batl);

  calc_dbat_regvals(&dbat,ROM_START,ROM_SIZE,1,0,1,0,BPP_RX);
  SET_DBAT(1,dbat.batu,dbat.batl);

  calc_dbat_regvals(&dbat,IMMRBAR,1024*1024,1,1,1,1,BPP_RW);
  SET_DBAT(2,dbat.batu,dbat.batl);

  /*
   * enable data/instruction MMU in MSR
   */
  _write_MSR(_read_MSR() | MSR_DR/* | MSR_IR*/);

  /*
   * enable FPU in MSR
   */
  _write_MSR(_read_MSR() | MSR_FP);

  /*
   * in HID0: 
   * - enable dynamic power management
   * - enable machine check interrupts
   */
  GET_HID0(reg);
  reg |=  (HID0_EMCP | HID0_DPM) ;
  SET_HID0(reg);

  /*
   * enable timebase clock 
   */
  mpc83xx.syscon.spcr |= M83xx_SYSCON_SPCR_TBEN;
}
