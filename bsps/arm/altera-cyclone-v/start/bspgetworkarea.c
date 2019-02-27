/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycV
 */

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

#ifdef BSP_FDT_IS_SUPPORTED

#define AREA_COUNT_MAX 16

static const char memory_path[] = "/memory";

static const char reserved_memory_path[] = "/reserved-memory";

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

static Heap_Area *find_area(
  Heap_Area *areas,
  size_t area_count,
  uint32_t begin
)
{
  size_t i;

  for (i = 0; i < area_count; ++i) {
    uintptr_t b;
    uintptr_t e;

    b = (uintptr_t) areas[i].begin;
    e = b + (uintptr_t) areas[i].size;

    if (b <= begin && begin < e) {
      return &areas[i];
    }
  }

  return NULL;
}

static size_t remove_reserved_memory(
  const void *fdt,
  Heap_Area *areas,
  size_t area_count
)
{
  int node;

  node = fdt_path_offset_namelen(
    fdt,
    reserved_memory_path,
    (int) sizeof(reserved_memory_path) - 1
  );

  if (node >= 0) {
    node = fdt_first_subnode(fdt, node);

    while (node >= 0) {
      int len;
      const void *val;
      uintptr_t area_begin;
      uintptr_t area_end;
      uintptr_t hole_begin;
      uintptr_t hole_end;
      Heap_Area *area;

      val = fdt_getprop(fdt, node, "reg", &len);
      if (len == 8) {
        hole_begin = fdt32_to_cpu(((fdt32_t *) val)[0]);
        hole_end = hole_begin + fdt32_to_cpu(((fdt32_t *) val)[1]);
      } else {
        rtems_panic("unexpected reserved memory area");
      }

      area = find_area(areas, area_count, hole_begin);
      area_begin = (uintptr_t) area->begin;
      area_end = area_begin + (uintptr_t) area->size;
      area->size = hole_begin - area_begin;

      if (hole_end <= area_end) {
        if (area_count >= AREA_COUNT_MAX) {
          rtems_panic("too many reserved memory areas");
        }

        area = &areas[area_count];
        ++area_count;
        area->begin = (void *) hole_end;
        area->size = area_end - hole_end;
      }

      node = fdt_next_subnode(fdt, node);
    }
  }

  return area_count;
}

#else /* !BSP_FDT_IS_SUPPORTED */

#define AREA_COUNT_MAX 1

#endif /* BSP_FDT_IS_SUPPORTED */

void bsp_work_area_initialize(void)
{
  Heap_Area areas[AREA_COUNT_MAX];
  size_t area_count;
#ifdef BSP_FDT_IS_SUPPORTED
  const void *fdt;
  size_t i;
#endif

  areas[0].begin = bsp_section_work_begin;
  areas[0].size = (uintptr_t) bsp_section_work_size;
  area_count = 1;

#ifdef BSP_FDT_IS_SUPPORTED
  fdt = bsp_fdt_get();

  adjust_memory_size(fdt, &areas[0]);
  area_count = remove_reserved_memory(fdt, areas, area_count);

  for (i = 0; i < area_count; ++i) {
    arm_cp15_set_translation_table_entries(
      areas[i].begin,
      (void *) ((uintptr_t) areas[i].begin + areas[i].size),
      ARMV7_MMU_READ_WRITE_CACHED
    );
  }
#endif

  bsp_work_area_initialize_with_table(areas, area_count);
}
