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
 */

#include <string.h>
#include <fcntl.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>
#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>
#include <rtems/powerpc/powerpc.h>

#include RTEMS_XPARAMETERS_H
#include <stdio.h>

uint32_t _heap_start;
uint32_t _heap_end;
uint32_t _top_of_ram;

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clicks_per_usec;
uint32_t   bsp_serial_per_sec;	       /* Serial clocks per second */
bool       bsp_serial_external_clock;
bool       bsp_serial_xon_xoff;
bool       bsp_serial_cts_rts;
uint32_t   bsp_serial_rate;
uint32_t   bsp_timer_average_overhead; /* Average overhead of timer in ticks */
uint32_t   bsp_timer_least_valid;      /* Least valid number from timer      */
bool       bsp_timer_internal_clock;   /* TRUE, when timer runs with CPU clk */

extern unsigned char IntrStack_start[];
extern unsigned char IntrStack_end[];

/*      Initialize whatever libc we are using
 *      called from postdriver hook
 */

void bsp_XAssertHandler(const char* file, int line);

void bsp_XAssertHandler(const char* file, int line) {
  printf("\n***\n*** XAssert Failed!  File: %s, Line: %d\n***\n", file, line);
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function store the result in global variables
   * so that it can be used latter...
   */
  myCpu 	    = get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  /*
   *  initialize the device driver parameters
   */

  /* timebase register ticks/microsecond */
  bsp_clicks_per_usec = (250000000 / 1000000);
  bsp_serial_per_sec = 14625000;
  bsp_serial_external_clock = false;
  bsp_timer_internal_clock  = true;
  bsp_serial_xon_xoff = false;
  bsp_serial_cts_rts = false;
  bsp_serial_rate = 115200;
  bsp_timer_average_overhead = 2;
  bsp_timer_least_valid = 3;

  /*
   * Initialize default raw exception handlers.
   */
  sc = ppc_exc_initialize(
    PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
    (uint32_t)IntrStack_start,
    IntrStack_end - IntrStack_start
  );
  if (sc != RTEMS_SUCCESSFUL) {
    BSP_panic("cannot initialize exceptions");
  }

  /*
   * Install our own set of exception vectors
   */
  BSP_rtems_irq_mng_init(0);
}

void BSP_ask_for_reset(void)
{
  printk("system stopped, press RESET");
  while(1) {};
}

void BSP_panic(char *s)
{
  printk("%s PANIC %s\n",_RTEMS_version, s);
  BSP_ask_for_reset();
}

void _BSP_Fatal_error(unsigned int v)
{
  printk("%s PANIC ERROR %x\n",_RTEMS_version, v);
  BSP_ask_for_reset();
}
