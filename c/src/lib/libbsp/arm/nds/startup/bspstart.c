/*
 * RTEMS for Nintendo DS platform initialization.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/linker-symbols.h>
#include <nds.h>

extern void defaultExceptionHandler ();
extern void BSP_rtems_irq_mngt_init (void);
/*
 * start the platform.
 */

void bsp_start (void)
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
  rtems_configuration_set_microseconds_per_tick (10000);

  defaultExceptionHandler ();
}

/*
 * reset bss area.
 */

void
bss_reset (void)
{

  memset (bsp_section_bss_begin, 0, (size_t) bsp_section_bss_size);
}

/*
 * A few symbols needed by libnds but not used.
 */

#include "../include/sys/iosupport.h"
const devoptab_t *devoptab_list[STD_MAX];
void *punixTime;
