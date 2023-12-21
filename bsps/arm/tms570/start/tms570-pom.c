/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the Parameter Overlay Module (POM) support
 *   implementation.
 */

/*
 * Copyright (C) 2014 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <string.h>
#include <bsp/tms570-pom.h>
#include <bsp/linker-symbols.h>
#include <rtems/score/armv4.h>
#include <bsp.h>

/*
 * Placement of exceptions target addresses in memory
 * when insructions with opcode 0xe59ff018
 *      ldr pc, [pc, #0x18]
 * are used to fill ARM exception vectors area
 */
typedef struct{
  uint32_t reserved1;
  uint32_t except_addr_undef;
  uint32_t except_addr_swi;
  uint32_t except_addr_prefetch;
  uint32_t except_addr_abort;
  uint32_t reserved2;
  uint32_t except_addr_irq;
  uint32_t except_addr_fiq;
}vec_remap_table;

void bsp_block_on_exception(void);

void bsp_block_on_exception(void){
  while(1);
}

extern char bsp_int_vec_overlay_start[];

/*
 * Global overlay target address holds shared MSB bits for all regions
 * It is set to linker RAM_INT_VEC region - i.e. area reserved
 * at internal SRAM memory start, address 0x08000000
 */
uint32_t pom_global_overlay_target_address_start =
                                 (uintptr_t)bsp_int_vec_overlay_start;

/**
 * @brief initialize and clear parameters overlay module (POM)
 *
 * clears all remap regions. The actual POM enable is left to the first user.
 *
 * @retval Void
 */
void tms570_pom_initialize_and_clear(void)
{
  int i;

  TMS570_POM.GLBCTRL = 0 | (TMS570_POM_GLBCTRL_OTADDR(~0) &
                            pom_global_overlay_target_address_start);

  for ( i = 0; i < TMS570_POM_REGIONS; ++i ) {
    TMS570_POM.REG[i].REGSIZE = TMS570_POM_REGSIZE_SIZE(TMS570_POM_REGSIZE_DISABLED);
  }
}

/**
 * @brief remaps vector table
 *
 * transfer the rtems start vector table to address 0x0
 *
 * @retval Void
 */
void tms570_pom_remap(void)
{
  void *vec_overlay_start = (void *) pom_global_overlay_target_address_start;
  void *addr_tab = (char *) bsp_start_vector_table_begin + 64;

  if (vec_overlay_start == addr_tab) {
    return;
  }

  /*
   * Copy RTEMS the first level exception processing code
   * to RAM area which can be used for later as POM overlay
   * of Flash vectors. The code is expected to have for of eight
   *   ldr pc, [pc,#0x18]
   * instructions followed by eight words with actual exception
   * service routines target addresses. This is case of RTEMS default
   * table found in
   *   c/src/lib/libbsp/arm/shared/start/start.S
   */
  rtems_cache_invalidate_multiple_data_lines(addr_tab, 64);
  memcpy(vec_overlay_start, addr_tab, 64);
  rtems_cache_flush_multiple_data_lines(vec_overlay_start, 64);
  rtems_cache_invalidate_multiple_instruction_lines(vec_overlay_start, 64);

  #if 0
  {
    /* Fill exception table by catch error infinite loop for debugging */
    vec_remap_table* vec_table = (vec_remap_table*)(vec_overlay_start+32);
    vec_table->except_addr_undef = (uint32_t)bsp_block_on_exception;
    vec_table->except_addr_swi = (uint32_t)bsp_block_on_exception;
    vec_table->except_addr_prefetch = (uint32_t)bsp_block_on_exception;//_ARMV4_Exception_prefetch_abort;
    vec_table->except_addr_abort = (uint32_t)bsp_block_on_exception;//_ARMV4_Exception_data_abort;
    vec_table->except_addr_irq = (uint32_t)_ARMV4_Exception_interrupt;
    vec_table->except_addr_fiq = (uint32_t)bsp_block_on_exception;//_ARMV4_Exception_interrupt;
  }
  #endif

  /*
   * The overlay vectors replacement area cannot be used directly
   * to replace jump instructions on start of Flash because instruction
   * fetch through POM is not reliable supported (works in most times
   * but sometimes fails).
   * Area of 64 bytes starting at address 0x00000040 is replaced.
   * This way target addresses are placed between 0x00000060
   * and 0x0000007F. If boot loader startup code contains instructions
   *   ldr pc,[pc,#0x58]
   * (opcode 0xe59ff058) then the jump target addresses are replaced
   * by pointers to actual RTEMS exceptions service functions.
   */
  TMS570_POM.REG[0].PROGSTART = TMS570_POM_PROGSTART_STARTADDRESS(64);
  TMS570_POM.REG[0].OVLSTART = TMS570_POM_OVLSTART_STARTADDRESS(vec_overlay_start);
  TMS570_POM.REG[0].REGSIZE = TMS570_POM_REGSIZE_SIZE(TMS570_POM_REGSIZE_64B);
  TMS570_POM.GLBCTRL = TMS570_POM_GLBCTRL_ON_OFF(0xa) |
                       TMS570_POM_GLBCTRL_ETO(0xa) |
                       (TMS570_POM_GLBCTRL_OTADDR(~0) &
                        pom_global_overlay_target_address_start);
}
