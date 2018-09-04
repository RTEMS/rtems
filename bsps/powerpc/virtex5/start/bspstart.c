/*
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 */

/*
 *  Author:	Thomas Doerfler <td@imd.m.isar.de>
 *              IMD Ingenieurbuero fuer Microcomputertechnik
 *
 *  COPYRIGHT (c) 1998 by IMD
 *
 *  Changes from IMD are covered by the original distributions terms.
 *  This file has been derived from the papyrus BSP:
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Modifications for spooling console driver and control of memory layout
 *  with linker command file by
 *              Thomas Doerfler <td@imd.m.isar.de>
 *  for these modifications:
 *  COPYRIGHT (c) 1997 by IMD, Puchheim, Germany.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies. IMD makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libbsp/no_cpu/no_bsp/startup/bspstart.c:
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications for PPC405GP by Dennis Ehlin
 *  Modifications for Virtex5 by Richard Claus <claus@slac.stanford.edu>
 */
#include <rtems.h>
#include <rtems/config.h>
#include <rtems/bspIo.h>
#include <rtems/counter.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/sysinit.h>

#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>

#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq.h>

#include <string.h>
#include <fcntl.h>
#include <inttypes.h>

#define DO_DOWN_ALIGN(x,a) ((x) & ~((a)-1))

#define DO_UP_ALIGN(x,a)   DO_DOWN_ALIGN(((x) + (a) - 1 ),a)

#define CPU_DOWN_ALIGN(x)  DO_DOWN_ALIGN(x, CPU_ALIGNMENT)
#define CPU_UP_ALIGN(x)    DO_UP_ALIGN(x, CPU_ALIGNMENT)


/* Defined in linkcmds linker script */
LINKER_SYMBOL(RamBase);
LINKER_SYMBOL(RamSize);
LINKER_SYMBOL(__bsp_ram_start);
LINKER_SYMBOL(__bsp_ram_end);
LINKER_SYMBOL(__rtems_end);
LINKER_SYMBOL(WorkAreaBase);
LINKER_SYMBOL(MsgAreaBase);
LINKER_SYMBOL(MsgAreaSize);
LINKER_SYMBOL(__phy_ram_end);
LINKER_SYMBOL(bsp_exc_vector_base);


/* Expected by clock.c */
uint32_t    bsp_clicks_per_usec;

/*
 * Bus Frequency
 */
unsigned int BSP_bus_frequency;
/*
 * processor clock frequency
 */
unsigned int BSP_processor_frequency;

/*
 * Time base divisior (bus freq / TB clock)
 */
unsigned int BSP_time_base_divisor;

/*
 * Provide weak aliases so that RTEMS distribution builds
 */
static void _noopfun(void) {}


void app_bsp_start(void)
__attribute__(( weak, alias("_noopfun") ));

void app_bsp_predriver_hook(void)
__attribute__(( weak, alias("_noopfun") ));


static char* bspMsgBuffer = (char*)MsgAreaBase;

static void __bsp_outchar_to_memory(char c)
{
  static char* msgBuffer = (char*)MsgAreaBase;
  *msgBuffer++ = c;
  if (msgBuffer >= &bspMsgBuffer[(int)MsgAreaSize])  msgBuffer = bspMsgBuffer;
  *msgBuffer   = 0x00;                /* Overwrite next location to show EOM */
}


void BSP_ask_for_reset(void)
{
  printk("\nSystem stopped, issue RESET");

  for(;;);
}

uint32_t _CPU_Counter_frequency(void)
{
  return BSP_bus_frequency / (BSP_time_base_divisor / 1000);
}

/*===================================================================*/

/*
 *  BSP start routine.  Called by boot_card().
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start(void)
{
  ppc_cpu_id_t       myCpu;
  ppc_cpu_revision_t myCpuRevision;

  /* Set the character output function;  The application may override this */
  BSP_output_char = __bsp_outchar_to_memory;

  printk("RTEMS %s\n", rtems_get_version_string());

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function stores the result in global variables so that it can be used later...
   */
  myCpu         = get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();
  printk("CPU: 0x%04x,  Revision: 0x%04x = %d,  Name: %s\n",
         myCpu, myCpuRevision, myCpuRevision, get_ppc_cpu_type_name(myCpu));

  /*
   *  Initialize the device driver parameters
   */

  /* For mpc6xx clock driver: */
  BSP_bus_frequency       = 465000000;
  BSP_processor_frequency = 465000000;  /* Measured with a DPM 440 2012/8/13 */
  BSP_time_base_divisor   = 1000;

  /* Timebase register ticks/microsecond;  The application may override these */
  bsp_clicks_per_usec        = BSP_bus_frequency/(BSP_time_base_divisor * 1000);

  ppc_exc_initialize();

  /* Let the user know what parameters we were compiled with */
  printk("                  Base/Start     End         Size\n"
         "RAM:              %p                    %p\n"
         "RTEMS:                           %p\n"
         "Workspace:        %p             %p\n"
         "MsgArea:          %p             %p\n"
         "Physical RAM                     %p\n",
         RamBase,        RamSize,
         __rtems_end,
         WorkAreaBase,   __bsp_ram_end,
         MsgAreaBase,    MsgAreaSize,
         __phy_ram_end);

  /*
   * Initialize RTEMS IRQ system
   */
  BSP_rtems_irq_mngt_init(0);

  /* Continue with application-specific initialization */
  app_bsp_start();
}


/*
 *  BSP predriver hook.  Called by boot_card() just before drivers are
 *  initialized.  Clear out any stale interrupts here.
 */
static void virtex5_pre_driver_hook(void)
{
  app_bsp_predriver_hook();
}

RTEMS_SYSINIT_ITEM(
  virtex5_pre_driver_hook,
  RTEMS_SYSINIT_BSP_PRE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
