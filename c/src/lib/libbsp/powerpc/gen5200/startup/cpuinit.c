/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
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
#include "../include/mpc5200.h"

#include <libcpu/mmu.h>
#include <libcpu/spr.h>
#include <string.h>

/* Macros for HID0 access */
#define SET_HID0(r)   __asm__ volatile ("mtspr 0x3F0,%0\n" ::"r"(r))
#define GET_HID0(r)   __asm__ volatile ("mfspr %0,0x3F0\n" :"=r"(r))

#define DBAT_MTSPR(val,name) __MTSPR(val,name);
#define SET_DBAT(n,uv,lv) {DBAT_MTSPR(uv,DBAT##n##U);DBAT_MTSPR(lv,DBAT##n##L);}
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
   * clear dbat
   */
  memset(bat_ptr, 0,sizeof(BAT));

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

#if defined (BRS5L)
void cpu_init_bsp(void)
{
  BAT dbat;

  calc_dbat_regvals(&dbat,RAM_START,RAM_SIZE,1,0,0,0,BPP_RW);
  SET_DBAT(0,dbat.batu,dbat.batl);

  calc_dbat_regvals(&dbat,ROM_START,ROM_SIZE,1,0,0,0,BPP_RX);
  SET_DBAT(1,dbat.batu,dbat.batl);

  calc_dbat_regvals(&dbat,MBAR,128*1024,1,1,1,1,BPP_RW);
  SET_DBAT(2,dbat.batu,dbat.batl);

  calc_dbat_regvals(&dbat,DPRAM_START,128*1024,1,1,1,1,BPP_RW);
  SET_DBAT(3,dbat.batu,dbat.batl);
}
#elif defined (HAS_UBOOT)
void cpu_init_bsp(void)
{
  BAT dbat;
  
  /*
   * Program BAT0 for RAM
   */
  calc_dbat_regvals(&dbat,
		    uboot_bdinfo_ptr->bi_memstart,
		    uboot_bdinfo_ptr->bi_memsize,
		    1,0,0,0,BPP_RW);
  SET_DBAT(0,dbat.batu,dbat.batl);

  /*
   * Program BAT1 for Flash
   *
   * WARNING!! Some Freescale LITE5200B boards ship with a version of
   * U-Boot that lies about the starting address of Flash.  This check
   * corrects that.
   */
  if ( (uboot_bdinfo_ptr->bi_flashstart + uboot_bdinfo_ptr->bi_flashsize) <
	uboot_bdinfo_ptr->bi_flashstart ) {
    uint32_t start = 0 - uboot_bdinfo_ptr->bi_flashsize;
    calc_dbat_regvals(&dbat,
		      start, uboot_bdinfo_ptr->bi_flashsize, 1,0,0,0,BPP_RX);
  } else {
    calc_dbat_regvals(&dbat,
		      uboot_bdinfo_ptr->bi_flashstart,
		      uboot_bdinfo_ptr->bi_flashsize,
		      1,0,0,0,BPP_RX);
  }
  SET_DBAT(1,dbat.batu,dbat.batl);

  /*
   * Program BAT2 for the MBAR
   */
  calc_dbat_regvals(&dbat,MBAR,128*1024,1,1,1,1,BPP_RW);
  SET_DBAT(2,dbat.batu,dbat.batl);

  /*
   * If there is SRAM, program BAT3 for that memory
   */
  if (uboot_bdinfo_ptr->bi_sramsize != 0) {
    calc_dbat_regvals(&dbat,
		      uboot_bdinfo_ptr->bi_sramstart,
		      uboot_bdinfo_ptr->bi_sramsize,
		      0,1,1,1,BPP_RW);
    SET_DBAT(3,dbat.batu,dbat.batl);
  }
}
#else
#warning "Using BAT register values set by environment"
#endif



void cpu_init(void)
{
  register unsigned long reg;

  /*
   * Enable instruction cache
   */
  GET_HID0(reg);
  reg |= HID0_ICE;
  SET_HID0(reg);

  /*
   * set up DBAT registers in MMU
   */
  cpu_init_bsp();

  #if defined(SHOW_MORE_INIT_SETTINGS)
    { extern void ShowBATS(void);
      ShowBATS();
    }
  #endif

  /*
   * enable data MMU in MSR
   */
  _write_MSR(_read_MSR() | MSR_DR);

  /* 
   * enable data cache 
   *
   * NOTE: TRACE32 now supports data cache for MGT5x00
   */
  GET_HID0(reg);
  reg |= HID0_DCE;
  SET_HID0(reg);
}
