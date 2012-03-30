/*  bsp_start()
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  INPUT:  NONE
 *
 *  OUTPUT: NONE
 *
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

#include <string.h>
#include <fcntl.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/vectors.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/sptables.h>             /* for RTEMS_VERSION */
#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>

#define DO_DOWN_ALIGN(x,a) ((x) & ~((a)-1))

#define DO_UP_ALIGN(x,a)   DO_DOWN_ALIGN(((x) + (a) - 1 ),a)

#define CPU_DOWN_ALIGN(x)  DO_DOWN_ALIGN(x, CPU_ALIGNMENT)
#define CPU_UP_ALIGN(x)    DO_UP_ALIGN(x, CPU_ALIGNMENT)


/* Expected by clock.c */
uint32_t    bsp_clicks_per_usec;
bool        bsp_timer_internal_clock;   /* true, when timer runs with CPU clk */
uint32_t    bsp_timer_least_valid;
uint32_t    bsp_timer_average_overhead;


/* Defined in linkcmds linker script */
LINKER_SYMBOL(RamBase);
LINKER_SYMBOL(RamSize);
LINKER_SYMBOL(__bsp_ram_start);
LINKER_SYMBOL(__bsp_ram_end);
LINKER_SYMBOL(__rtems_end);
LINKER_SYMBOL(_stack);
LINKER_SYMBOL(StackSize);
LINKER_SYMBOL(__stack_base);
LINKER_SYMBOL(WorkAreaBase);
LINKER_SYMBOL(MsgAreaBase);
LINKER_SYMBOL(MsgAreaSize);
LINKER_SYMBOL(__phy_ram_end);


/*
 * Provide weak aliases so that RTEMS distribution builds
 */
static void _noopfun(void) {}
static void _bsp_start(void)
{
  rtems_status_code sc             = RTEMS_SUCCESSFUL;
  uintptr_t         intrStackStart = CPU_UP_ALIGN((uint32_t)__bsp_ram_start);
  uintptr_t         intrStackSize  = rtems_configuration_get_interrupt_stack_size();

  /*
   * Initialize default raw exception handlers.
   *
   * This BSP does not assume anything about firmware possibly loaded in the
   * FPGA, so the external interrupt should not be enabled in order to avoid
   * spurious interrupts.
   */
  sc = ppc_exc_initialize(PPC_INTERRUPT_DISABLE_MASK_DEFAULT & ~MSR_EE,
                          intrStackStart,
                          intrStackSize);
  if (sc != RTEMS_SUCCESSFUL)  BSP_panic("Cannot initialize exceptions");

  /* Install our own set of exception vectors */
  BSP_rtems_irq_mngt_init(0);
}


void app_bsp_start(void)
__attribute__(( weak, alias("_bsp_start") ));

void app_bsp_pretasking_hook(void)
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


void BSP_panic(char *s)
{
  printk("\n%s PANIC %s\n", _RTEMS_version, s);
  BSP_ask_for_reset();
}


void _BSP_Fatal_error(unsigned int v)
{
  printk("\n%s FATAL ERROR %x\n", _RTEMS_version, v);
  BSP_ask_for_reset();
}


/*===================================================================*/

/*
 *  BSP start routine.  Called by boot_card().
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start(void)
{
  uintptr_t          intrStackStart;
  uintptr_t          intrStackSize;
  ppc_cpu_id_t       myCpu;
  ppc_cpu_revision_t myCpuRevision;

  /* Set the character output function;  The application may override this */
  BSP_output_char = __bsp_outchar_to_memory;

  printk("\nWelcome to RTEMS %s\n", _RTEMS_version );

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

  /* Timebase register ticks/microsecond;  The application may override these */
  bsp_clicks_per_usec        = 450;
  bsp_timer_internal_clock   = true;
  bsp_timer_average_overhead = 2;
  bsp_timer_least_valid      = 3;

  /*
   * Initialize the interrupt related settings.
   */
  intrStackStart = CPU_UP_ALIGN((uint32_t)__bsp_ram_start);
  intrStackSize  = rtems_configuration_get_interrupt_stack_size();
  printk("                  Base/Start     End         Size\n"
         "RAM:              0x%08x              0x%x\n"
         "RTEMS:                        0x%08x\n"
         "Interrupt Stack:  0x%08x              0x%x\n"
         "Stack:            0x%08x  0x%08x  0x%x\n"
         "Workspace:        0x%08x  0x%08x\n"
         "MsgArea:          0x%08x              0x%x\n"
         "Physical RAM                  0x%08x\n",
         (uint32_t)RamBase,                               (uint32_t)RamSize,
         (uint32_t)__rtems_end,
         intrStackStart,                                  intrStackSize,
         (uint32_t)__stack_base, (uint32_t)_stack,        (uint32_t)StackSize,
         (uint32_t)WorkAreaBase, (uint32_t)__bsp_ram_end,
         (uint32_t)MsgAreaBase,                           (uint32_t)MsgAreaSize,
         (uint32_t)__phy_ram_end);

  /* Continue with application-specific initialization */
  app_bsp_start();
}


/*
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 *
 *  Must not use libc (to do io) from here, since drivers are not yet
 *  initialized.
 */

void bsp_pretasking_hook(void)
{
  app_bsp_pretasking_hook();
}


/*
 *  BSP predriver hook.  Called by boot_card() just before drivers are
 *  initialized.  Clear out any stale interrupts here.
 */
void bsp_predriver_hook(void)
{
  app_bsp_predriver_hook();
}
