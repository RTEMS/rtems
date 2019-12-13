/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycV
 */

/*
 * Copyright (c) 2017, 2019 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/bootcard.h>
#include <bsp/fdt.h>
#include <bsp/linker-symbols.h>

#ifdef BSP_FDT_IS_SUPPORTED

#include <bsp/arm-cp15-start.h>

#include <libcpu/arm-cp15.h>

#include <libfdt.h>

#include <rtems/sysinit.h>

#define AREA_COUNT_MAX 16

static const char memory_path[] = "/memory";

static const char reserved_memory_path[] = "/reserved-memory";

static void adjust_memory_size(const void *fdt, Memory_Area *area)
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
      _Memory_Set_end(area, (void *) (begin + size));
    }
  }
}

static Memory_Area *find_area(
  Memory_Area *areas,
  size_t area_count,
  uint32_t begin
)
{
  size_t i;

  for (i = 0; i < area_count; ++i) {
    uintptr_t b;
    uintptr_t e;

    b = (uintptr_t) _Memory_Get_begin(&areas[i]);
    e = (uintptr_t) _Memory_Get_end(&areas[i]);

    if (b <= begin && begin < e) {
      return &areas[i];
    }
  }

  return NULL;
}

static size_t remove_reserved_memory(
  const void *fdt,
  Memory_Area *areas,
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
      uintptr_t area_end;
      uintptr_t hole_begin;
      uintptr_t hole_end;
      Memory_Area *area;

      val = fdt_getprop(fdt, node, "reg", &len);
      if (len == 8) {
        hole_begin = fdt32_to_cpu(((fdt32_t *) val)[0]);
        hole_end = hole_begin + fdt32_to_cpu(((fdt32_t *) val)[1]);
      } else {
        rtems_panic("unexpected reserved memory area");
      }

      area = find_area(areas, area_count, hole_begin);
      area_end = (uintptr_t) _Memory_Get_end(area);
      _Memory_Set_end(area, (void *) hole_end);

      if (hole_end <= area_end) {
        if (area_count >= AREA_COUNT_MAX) {
          rtems_panic("too many reserved memory areas");
        }

        area = &areas[area_count];
        ++area_count;
        _Memory_Initialize(area, (void *) hole_end, (void *) area_end);
      }

      node = fdt_next_subnode(fdt, node);
    }
  }

  return area_count;
}

static Memory_Area _Memory_Areas[AREA_COUNT_MAX];

static void bsp_memory_initialize(void)
{
  size_t area_count;
  const void *fdt;
  size_t i;

  _Memory_Initialize(
    &_Memory_Areas[0],
    bsp_section_work_begin,
    bsp_section_work_end
  );

  area_count = 1;
  fdt = bsp_fdt_get();
  adjust_memory_size(fdt, &_Memory_Areas[0]);
  area_count = remove_reserved_memory(fdt, &_Memory_Areas[0], area_count);

  for (i = 0; i < area_count; ++i) {
    arm_cp15_set_translation_table_entries(
      _Memory_Get_begin(&_Memory_Areas[i]),
      _Memory_Get_end(&_Memory_Areas[i]),
      ARMV7_MMU_READ_WRITE_CACHED
    );
  }
}

RTEMS_SYSINIT_ITEM(
  bsp_memory_initialize,
  RTEMS_SYSINIT_MEMORY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

#else /* !BSP_FDT_IS_SUPPORTED */

static Memory_Area _Memory_Areas[] = {
  MEMORY_INITIALIZER(bsp_section_work_begin, bsp_section_work_end)
};

#endif /* BSP_FDT_IS_SUPPORTED */

static const Memory_Information _Memory_Information =
  MEMORY_INFORMATION_INITIALIZER( _Memory_Areas );

const Memory_Information *_Memory_Get( void )
{
  return &_Memory_Information;
}
