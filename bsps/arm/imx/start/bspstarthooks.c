/*
 * Copyright (c) 2013, 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#define ARM_CP15_TEXT_SECTION BSP_START_TEXT_SECTION

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/fdt.h>
#include <bsp/linker-symbols.h>
#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/arm-a9mpcore-start.h>

#include <rtems/sysinit.h>

#include <libfdt.h>

BSP_START_DATA_SECTION static arm_cp15_start_section_config
imx_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
  {
    .begin = 0x00a00000U,
    .end = 0x70000000U,
    .flags = ARMV7_MMU_DEVICE
  }
};

BSP_START_DATA_SECTION static char memory_path[] = "/memory";

BSP_START_TEXT_SECTION static void setup_mmu_and_cache(void)
{
  const void *fdt;
  int node;
  uint32_t ctrl;

  fdt = bsp_fdt_get();
  node = fdt_path_offset_namelen(
    fdt,
    memory_path,
    (int) sizeof(memory_path) - 1
  );

  if (node >= 0) {
    int len;
    const void *val;

    val = fdt_getprop(fdt, node, "reg", &len);
    if (len == 8) {
      uint32_t begin;
      uint32_t size;

      begin = fdt32_to_cpu(((fdt32_t *) val)[0]);
      size = fdt32_to_cpu(((fdt32_t *) val)[1]);

      /* The heap code does not like an end address of zero */
      if (begin + size == 0) {
        size -= 4;
      }

      imx_mmu_config_table[ARMV7_CP15_START_WORKSPACE_ENTRY_INDEX].end =
        begin + size;
    }
  }

  ctrl = arm_cp15_start_setup_mmu_and_cache(
    ARM_CP15_CTRL_A,
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &imx_mmu_config_table[0],
    RTEMS_ARRAY_SIZE(imx_mmu_config_table)
  );
}

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
#ifdef RTEMS_SMP
  uint32_t cpu_id = arm_cortex_a9_get_multiprocessor_cpu_id();

  arm_a9mpcore_start_enable_smp_in_auxiliary_control();

  if (cpu_id != 0) {
    arm_a9mpcore_start_on_secondary_processor();
  }
#endif
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  arm_a9mpcore_start_set_vector_base();
  bsp_start_copy_sections();
  setup_mmu_and_cache();
  bsp_start_clear_bss();
}

static Memory_Area _Memory_Areas[1];

static void bsp_memory_initialize(void)
{
  _Memory_Initialize(
    &_Memory_Areas[0],
    imx_mmu_config_table[ARMV7_CP15_START_WORKSPACE_ENTRY_INDEX].begin,
    imx_mmu_config_table[ARMV7_CP15_START_WORKSPACE_ENTRY_INDEX].end
  );
}

RTEMS_SYSINIT_ITEM(
  bsp_memory_initialize,
  RTEMS_SYSINIT_MEMORY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

static const Memory_Information _Memory_Information =
  MEMORY_INFORMATION_INITIALIZER(_Memory_Areas);

const Memory_Information *_Memory_Get(void)
{
  return &_Memory_Information;
}
