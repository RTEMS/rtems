/**
 * @file
 *
 * @ingroup bsp_bootcard
 *
 * @brief Standard system startup.
 */

/*
 * Copyright (c) 2008-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/**
 * @defgroup bsp_kit Board Support Package
 *
 * @brief Board support package dependent code.
 */

#ifndef LIBBSP_SHARED_BOOTCARD_H
#define LIBBSP_SHARED_BOOTCARD_H

#include <string.h>

#include <rtems/config.h>
#include <rtems/bspIo.h>
#include <rtems/malloc.h>
#include <rtems/score/wkspace.h>

#include <bspopts.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup bsp_bootcard Bootcard
 *
 * @ingroup bsp_kit
 *
 * @brief Standard system startup.
 *
 * @{
 */

/**
 * @brief Global pointer to the command line of boot_card().
 */
extern const char *bsp_boot_cmdline;

void bsp_start(void);

void bsp_pretasking_hook(void);

void bsp_predriver_hook(void);

void bsp_postdriver_hook(void);

void bsp_reset(void);

/**
 * @brief Standard system initialization procedure.
 *
 * You may pass a command line in @a cmdline.  It is later available via the
 * global @ref bsp_boot_cmdline variable.
 *
 * This is the C entry point for ALL RTEMS BSPs.  It is invoked from the
 * assembly language initialization file usually called @c start.S which does
 * the basic CPU setup (stack, C runtime environment, zero BSS, load other
 * sections) and calls afterwards boot_card().  The boot card function provides
 * the framework for the BSP initialization sequence.  The basic flow of
 * initialization is:
 *
 * - disable interrupts, interrupts will be enabled during the first context
 *   switch
 * - bsp_start() - more advanced initialization
 * - bsp_work_area_initialize() - initialize the RTEMS Workspace and the C
 *   Program Heap
 * - rtems_initialize_data_structures()
 * - initialize C Library
 * - bsp_pretasking_hook()
 * - if defined( RTEMS_DEBUG )
 *   - rtems_debug_enable( RTEMS_DEBUG_ALL_MASK )
 * - rtems_initialize_before_drivers()
 * - bsp_predriver_hook()
 * - rtems_initialize_device_drivers()
 *   - initialization of all device drivers
 * - bsp_postdriver_hook()
 * - rtems_initialize_start_multitasking()
 *   - 1st task executes C++ global constructors
 *   - .... application runs ...
 *   - exit
 * - will not return to here
 *
 * This style of initialization ensures that the C++ global constructors are
 * executed after RTEMS is initialized.
 */
void boot_card(const char *cmdline) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#ifdef CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK
  /**
   * @brief Gives the BSP a chance to reduce the work area size with sbrk()
   * adding more later.
   *
   * bsp_sbrk_init() may reduce the work area size passed in. The routine
   * returns the 'sbrk_amount' to be used when extending the heap.  Note that
   * the return value may be zero.
   *
   * In case the @a area size is altered, then the remaining size of the
   * @a area must be greater than or equal to @a min_size.
   */
  ptrdiff_t bsp_sbrk_init(Heap_Area *area, uintptr_t min_size);
#endif

static inline void bsp_work_area_initialize_default(
  void *area_begin,
  uintptr_t area_size
)
{
  Heap_Area area = {
    .begin = area_begin,
    .size = area_size
  };

  #if BSP_DIRTY_MEMORY == 1
    memset(area.begin, 0xCF,  area.size);
  #endif

  #ifdef CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK
    {
      uintptr_t overhead = _Heap_Area_overhead(CPU_HEAP_ALIGNMENT);
      uintptr_t work_space_size = rtems_configuration_get_work_space_size();
      ptrdiff_t sbrk_amount = bsp_sbrk_init(
        &area,
        work_space_size
          + overhead
          + (rtems_configuration_get_unified_work_area() ? 0 : overhead)
      );

      rtems_heap_set_sbrk_amount(sbrk_amount);
    }
  #endif

  /*
   *  The following may be helpful in debugging what goes wrong when
   *  you are allocating the Work Area in a new BSP.
   */
  #ifdef BSP_GET_WORK_AREA_DEBUG
    {
      void *sp = __builtin_frame_address(0);
      void *end = (char *) area.begin + area.size;
      printk(
        "work_area_start = 0x%p\n"
        "work_area_size = %lu 0x%08lx\n"
        "end = 0x%p\n"
        "current stack pointer = 0x%p%s\n",
        area.begin,
        (unsigned long) area.size,  /* decimal */
        (unsigned long) area.size,  /* hexadecimal */
        end,
        sp,
        (uintptr_t) sp >= (uintptr_t) area.begin
          && (uintptr_t) sp <= (uintptr_t) end ?
            " OVERLAPS!" : ""
      );
    }
  #endif

  _Workspace_Handler_initialization(&area, 1, NULL);

  #ifdef BSP_GET_WORK_AREA_DEBUG
    printk(
      "heap_start = 0x%p\n"
      "heap_size = %lu\n",
      area.begin,
      (unsigned long) area.size
    );
  #endif

  RTEMS_Malloc_Initialize(&area, 1, NULL);
}

static inline void bsp_work_area_initialize_with_table(
  Heap_Area *areas,
  size_t area_count
)
{
  _Workspace_Handler_initialization(areas, area_count, _Heap_Extend);
  RTEMS_Malloc_Initialize(areas, area_count, _Heap_Extend);
}

void bsp_work_area_initialize(void);

void bsp_libc_init(void);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_BOOTCARD_H */
