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

#include <stdbool.h>
#include <string.h>

#include <libcpu/powerpc-utility.h>
#include <libcpu/mmu.h>

#include <mpc83xx/mpc83xx.h>

#include <bsp.h>
#include <bsp/u-boot.h>

#define SET_DBAT( n, uv, lv) \
  do { \
    PPC_SET_SPECIAL_PURPOSE_REGISTER( DBAT##n##L, lv); \
    PPC_SET_SPECIAL_PURPOSE_REGISTER( DBAT##n##U, uv); \
  } while (0)

#define SET_IBAT( n, uv, lv) \
  do { \
    PPC_SET_SPECIAL_PURPOSE_REGISTER( IBAT##n##L, lv); \
    PPC_SET_SPECIAL_PURPOSE_REGISTER( IBAT##n##U, uv); \
  } while (0)

static void calc_dbat_regvals(
  BAT *bat_ptr,
  uint32_t base_addr,
  uint32_t size,
  bool flg_w,
  bool flg_i,
  bool flg_m,
  bool flg_g,
  uint32_t flg_bpp
)
{
  uint32_t block_mask = 0xffffffff;
  uint32_t end_addr = base_addr + size - 1;

  /* Determine block mask, that overlaps the whole block */
  while ((end_addr & block_mask) != (base_addr & block_mask)) {
    block_mask <<= 1;
  }

  bat_ptr->batu.bepi = base_addr >> (32 - 15);
  bat_ptr->batu.bl   = ~(block_mask >> (28 - 11));
  bat_ptr->batu.vs   = 1;
  bat_ptr->batu.vp   = 1;

  bat_ptr->batl.brpn = base_addr  >> (32 - 15);
  bat_ptr->batl.w    = flg_w;
  bat_ptr->batl.i    = flg_i;
  bat_ptr->batl.m    = flg_m;
  bat_ptr->batl.g    = flg_g;
  bat_ptr->batl.pp   = flg_bpp;
}

static void clear_mmu_regs( void)
{
  uint32_t i;

  /* Clear segment registers */
  for (i = 0;i < 16;i++) {
    __asm__ volatile( "mtsrin %0, %1\n" : : "r" (i * 0x1000), "r" (i << (31 - 3)));
  }

  /* Clear TLBs */
  for (i = 0;i < 32;i++) {
    __asm__ volatile( "tlbie %0\n" : : "r" (i << (31 - 19)));
  }
}

void cpu_init( void)
{
  BAT dbat, ibat;
  uint32_t msr;

  /* Clear MMU and segment registers */
  clear_mmu_regs();

  /* Clear caches */
  PPC_CLEAR_SPECIAL_PURPOSE_REGISTER_BITS( HID0, HID0_ILOCK | HID0_DLOCK | HID0_ICE | HID0_DCE);
  PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS( HID0, HID0_ICFI | HID0_DCI);
  PPC_CLEAR_SPECIAL_PURPOSE_REGISTER_BITS( HID0, HID0_ICFI | HID0_DCI);

  /*
   * Set up IBAT registers in MMU
   */

  memset(&ibat, 0, sizeof( ibat));
  SET_IBAT( 2, ibat.batu, ibat.batl);
  SET_IBAT( 3, ibat.batu, ibat.batl);
  SET_IBAT( 4, ibat.batu, ibat.batl);
  SET_IBAT( 5, ibat.batu, ibat.batl);
  SET_IBAT( 6, ibat.batu, ibat.batl);
  SET_IBAT( 7, ibat.batu, ibat.batl);

  calc_dbat_regvals(
    &ibat,
    #ifdef HAS_UBOOT
      bsp_uboot_board_info.bi_memstart,
      bsp_uboot_board_info.bi_memsize,
    #else /* HAS_UBOOT */
      (uint32_t) bsp_ram_start,
      (uint32_t) bsp_ram_size,
    #endif /* HAS_UBOOT */
    false,
    false,
    false,
    false,
    BPP_RX
  );
  SET_IBAT( 0, ibat.batu, ibat.batl);

  calc_dbat_regvals(
    &ibat,
    #ifdef HAS_UBOOT
      bsp_uboot_board_info.bi_flashstart,
      bsp_uboot_board_info.bi_flashsize,
    #else /* HAS_UBOOT */
      (uint32_t) bsp_rom_start,
      (uint32_t) bsp_rom_size,
    #endif /* HAS_UBOOT */
    false,
    false,
    false,
    false,
    BPP_RX
  );
  SET_IBAT( 1, ibat.batu, ibat.batl);

  /*
   * Set up DBAT registers in MMU
   */

  memset(&dbat, 0, sizeof( dbat));
  SET_DBAT( 3, dbat.batu, dbat.batl);
  SET_DBAT( 4, dbat.batu, dbat.batl);
  SET_DBAT( 5, dbat.batu, dbat.batl);
  SET_DBAT( 6, dbat.batu, dbat.batl);
  SET_DBAT( 7, dbat.batu, dbat.batl);

  calc_dbat_regvals(
    &dbat,
    #ifdef HAS_UBOOT
      bsp_uboot_board_info.bi_memstart,
      bsp_uboot_board_info.bi_memsize,
    #else /* HAS_UBOOT */
      (uint32_t) bsp_ram_start,
      (uint32_t) bsp_ram_size,
    #endif /* HAS_UBOOT */
    false,
    false,
    false,
    false,
    BPP_RW
  );
  SET_DBAT( 0, dbat.batu, dbat.batl);

  calc_dbat_regvals(
    &dbat,
    #ifdef HAS_UBOOT
      bsp_uboot_board_info.bi_flashstart,
      bsp_uboot_board_info.bi_flashsize,
    #else /* HAS_UBOOT */
      (uint32_t) bsp_rom_start,
      (uint32_t) bsp_rom_size,
    #endif /* HAS_UBOOT */
    true,
    false,
    false,
    false,
    BPP_RX
  );
  SET_DBAT( 1, dbat.batu, dbat.batl);

  calc_dbat_regvals(
    &dbat,
    #ifdef HAS_UBOOT
      bsp_uboot_board_info.bi_immrbar,
    #else /* HAS_UBOOT */
      (uint32_t) IMMRBAR,
    #endif /* HAS_UBOOT */
    #if MPC83XX_CHIP_TYPE / 10 == 830
      2 * 1024 * 1024,
    #else
      1024 * 1024,
    #endif
    false,
    true,
    false,
    true,
    BPP_RW
  );
  SET_DBAT( 2, dbat.batu, dbat.batl);

#if defined(MPC83XX_BOARD_HSC_CM01)
  calc_dbat_regvals(
    &dbat,
    FPGA_START,
    FPGA_SIZE,
    true,
    true,
    true,
    false,
    BPP_RW
  );
  SET_DBAT(3,dbat.batu,dbat.batl);
#endif

#ifdef MPC83XX_BOARD_MPC8313ERDB
  /* Enhanced Local Bus Controller (eLBC) */
  calc_dbat_regvals(
    &dbat,
    0xfa000000,
    128 * 1024,
    false,
    true,
    false,
    true,
    BPP_RW
  );
  SET_DBAT( 3, dbat.batu, dbat.batl);
#endif /* MPC83XX_BOARD_MPC8313ERDB */

  /* Read MSR */
  msr = ppc_machine_state_register();

  /* Enable data and instruction MMU in MSR */
  msr |= MSR_DR | MSR_IR;

  /* Enable FPU in MSR */
  msr |= MSR_FP;

  /* Update MSR */
  ppc_set_machine_state_register( msr);

  /*
   * In HID0:
   *  - Enable dynamic power management
   *  - Enable machine check interrupts
   */
  PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS( HID0, HID0_EMCP | HID0_DPM);

  /* Enable timebase clock */
  mpc83xx.syscon.spcr |= M83xx_SYSCON_SPCR_TBEN;
}
