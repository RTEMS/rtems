/*
 * Copyright (c) 2017 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/bootcard.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/fdt.h>
#include <bsp/linker-symbols.h>

#include <libcpu/arm-cp15.h>

#include <libfdt.h>

#define AREA_COUNT_MAX 16

static const char memory_path[] = "/memory";

static void adjust_memory_size(const void *fdt, Heap_Area *area)
{
  int node;

  node = fdt_path_offset_namelen(
    fdt,
    memory_path,
    (int) sizeof(memory_path) - 1
  );

  if (node >= 0) {
    int len;
    const void *val;
    uintptr_t begin;
    uintptr_t size;
    uintptr_t a_bit;

    val = fdt_getprop(fdt, node, "reg", &len);
    if (len == 8) {
      begin = fdt32_to_cpu(((fdt32_t *) val)[0]);
      size = fdt32_to_cpu(((fdt32_t *) val)[1]);
    } else {
      begin = 0;
      size = 0;
    }

    /*
     * Remove a bit to avoid problems with speculative memory accesses beyond
     * the valid memory area.
     */
    a_bit = 0x100000;
    if (size >= a_bit) {
      size -= a_bit;
    }

    if (
      begin == 0
        && size > (uintptr_t) bsp_section_work_end
        && (uintptr_t) bsp_section_nocache_end
          < (uintptr_t) bsp_section_work_end
    ) {
      area->size += size - (uintptr_t) bsp_section_work_end;
    }
  }
}

void bsp_work_area_initialize(void)
{
  const void *fdt;
  Heap_Area areas[AREA_COUNT_MAX];
  size_t area_count;
  size_t i;

  areas[0].begin = bsp_section_work_begin;
  areas[0].size = (uintptr_t) bsp_section_work_size;
  area_count = 1;

  fdt = bsp_fdt_get();

  adjust_memory_size(fdt, &areas[0]);

  for (i = 0; i < area_count; ++i) {
    arm_cp15_set_translation_table_entries(
      areas[i].begin,
      (void *) ((uintptr_t) areas[i].begin + areas[i].size),
      ARMV7_MMU_READ_WRITE_CACHED
    );
  }

  bsp_work_area_initialize_with_table(areas, area_count);
}
