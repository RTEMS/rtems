/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief TMS570 Parameter Overlay Module functions definitions.
 */

 /*
 * Copyright (c) 2014 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
void tms570_initialize_and_clear(void)
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
  uint32_t vec_overlay_start = pom_global_overlay_target_address_start;

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
  memcpy((void*)vec_overlay_start, bsp_start_vector_table_begin, 64);

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
