/*
 * RTEMS for Nintendo DS platform initialization.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 *
 * $Id$
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <nds.h>

/*
 * This definition comes from ARM cpu code.
 */
extern unsigned int arm_cpu_mode;

/*
 *  This method returns the base address and size of the area which
 *  is to be allocated between the RTEMS Workspace and the C Program
 *  Heap.
 */
void bsp_get_work_area(
  void   **work_area_start,
  size_t  *work_area_size,
  void   **heap_start,
  size_t  *heap_size
)
{
  extern uint8_t _end;
  extern uint8_t __ewram_end;

  *work_area_start       = &_end;
  *work_area_size       = (void *)&__ewram_end - (void *)&_end;
  *heap_start = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;
}

/*
 * start the platform.
 */

void
bsp_start (void)
{
  /* initialize irq management */
  BSP_rtems_irq_mngt_init ();

  /* setup console mode for lower screen */
  irqEnable (IRQ_VBLANK);
  videoSetMode (0);
  videoSetModeSub (MODE_0_2D | DISPLAY_BG0_ACTIVE);
  vramSetBankC (VRAM_C_SUB_BG);

  SUB_BG0_CR = BG_MAP_BASE (31);
  BG_PALETTE_SUB[255] = RGB15 (31, 31, 31);
  consoleInitDefault ((u16 *) SCREEN_BASE_BLOCK_SUB (31),
                      (u16 *) CHAR_BASE_BLOCK_SUB (0), 16);

  /* print status message */
  printk ("[+] kernel console started\n");

  /* set the cpu mode to system user */
  arm_cpu_mode = 0x1f;

  /* configure clock period */
  Configuration.microseconds_per_tick = 10000;  /* us */
}

/*
 * reset bss area.
 */

void
bss_reset (void)
{
  extern uint8_t __bss_start;
  extern uint8_t __bss_end;

  memset (&__bss_start, 0, (uint32_t) & __bss_end - (uint32_t) & __bss_start);
}

/*
 * reset the platform using bios call.
 */

void
bsp_reset (void)
{
  swiSoftReset ();
}

/*
 * clean up platform before reset.
 */

void
bsp_cleanup (void)
{
  printk ("[!] executive ended, rebooting\n");

  bsp_reset ();
}

/*
 * A few symbols needed by libnds but not used.
 */

#include "../include/sys/iosupport.h"
const devoptab_t *devoptab_list[STD_MAX];
void *punixTime;
