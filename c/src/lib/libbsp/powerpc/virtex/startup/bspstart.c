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
 *
 *  $Id$
 */

#include <string.h>
#include <fcntl.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
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
boolean    bsp_serial_external_clock;
boolean    bsp_serial_xon_xoff;
boolean    bsp_serial_cts_rts;
uint32_t   bsp_serial_rate;
uint32_t   bsp_timer_average_overhead; /* Average overhead of timer in ticks */
uint32_t   bsp_timer_least_valid;      /* Least valid number from timer      */
boolean    bsp_timer_internal_clock;   /* TRUE, when timer runs with CPU clk */


/*      Initialize whatever libc we are using
 *      called from postdriver hook
 */

void bsp_XAssertHandler(const char* file, int line);
void bsp_libc_init( void *, uint32_t, int );


void bsp_XAssertHandler(const char* file, int line) {
  printf("\n***\n*** XAssert Failed!  File: %s, Line: %d\n***\n", file, line);
}

/*
 *  Function:   bsp_pretasking_hook
 *  Created:    95/03/10
 *
 *  Description:
 *      BSP pretasking hook.  Called just before drivers are initialized.
 *      Used to setup libc and install any BSP extensions.
 *
 *  NOTES:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 *
 */

void bsp_pretasking_hook(void)
{


    uint32_t        heap_start;
    uint32_t  	    heap_size;
    uint32_t  	    heap_end;

    /* round up from the top of workspace to next 64k boundary, get
     * default heapsize from linker script  */
    heap_start = (((uint32_t)Configuration.work_space_start +
		   rtems_configuration_get_work_space_size()) + 0x18000) & 0xffff0000;

    heap_end = _heap_start + (uint32_t)&_HeapSize;

    heap_size = (heap_end - heap_start);

    _heap_start = heap_start;
    _heap_end = heap_end;

    _top_of_ram = heap_end;

    bsp_libc_init((void *) heap_start, heap_size, 0); /* 64 * 1024 */

}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */


void bsp_start( void )
{
  extern unsigned char IntrStack_start[];
  extern unsigned char IntrStack_end[];
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
  bsp_serial_external_clock = 0;
  bsp_timer_internal_clock  = 1;
  bsp_serial_xon_xoff = 0;
  bsp_serial_cts_rts = 0;
  bsp_serial_rate = 115200;
  bsp_timer_average_overhead = 2;
  bsp_timer_least_valid = 3;

  /*
   * Initialize default raw exception handlers. 
   */
  ppc_exc_initialize(
	PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
	(uint32_t)IntrStack_start,
	IntrStack_end - IntrStack_start
  );

  /*
   * Install our own set of exception vectors
   */
  BSP_rtems_irq_mng_init(0);

  /*
   *  Allocate the memory for the RTEMS Work Space.  This can come from
   *  a variety of places: hard coded address, malloc'ed from outside
   *  RTEMS world (e.g. simulator or primitive memory manager), or (as
   *  typically done by stock BSPs) by subtracting the required amount
   *  of work space from the last physical address on the CPU board.
   */

  /*
   *  Need to "allocate" the memory for the RTEMS Workspace and
   *  tell the RTEMS configuration where it is.  This memory is
   *  not malloc'ed.  It is just "pulled from the air".
   */
  /* FIME: plan usage of RAM better:
     - make top of ram dynamic,
     - make rest of ram to heap...
     -remove RAM_END from bsp.h, this cannot be valid...
      or must be a function call
   */
  {
    extern int _end;

    /* round _end up to next 64k boundary for start of workspace */
    Configuration.work_space_start = (void *)((((uint32_t)&_end) + 0xffff) & 0xffff0000);
  }

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
 
