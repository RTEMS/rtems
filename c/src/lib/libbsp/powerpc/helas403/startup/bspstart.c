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
 *  $Id$
 */

#include <string.h>
#include <fcntl.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <ictrl.h>
 
/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table Configuration;

rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;
void *bsp_ram_end = (void *)RAM_END;  /* first addr behind avail. ram area */

/*      Initialize whatever libc we are using
 *      called from postdriver hook
 */
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

/*
 *
 *  bsp_predriver_hook
 *
 *  Before drivers are setup.
 */ 

void bsp_predriver_hook(void)
{
  rtems_status_code status;
  /* init the PPC403GA external interrupt controller handler... */
  status = ictrl_init();
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
    extern int _end;
    rtems_unsigned32        heap_start;

    heap_start = (rtems_unsigned32) &_end;
    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    bsp_libc_init((void *) heap_start, 64 * 1024, 0);
 
#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}
 

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
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
     - take out some part for persistant log
     - make rest of ram to heap... 
     -remove RAM_END from bsp.h, this cannot be valid... 
      or must be a function call
   */
  BSP_Configuration.work_space_start = (void *)
      ((char *)(bsp_ram_end)) - BSP_Configuration.work_space_size;

  /*
   *  initialize the CPU table for this BSP
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.predriver_hook  = bsp_predriver_hook;
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

  Cpu_table.clicks_per_usec = 25;
  Cpu_table.serial_per_sec = 25000000;
  Cpu_table.serial_external_clock = 0;
  Cpu_table.timer_internal_clock  = 1;
  Cpu_table.serial_xon_xoff = 0;
  Cpu_table.serial_cts_rts = 1;
  Cpu_table.serial_rate = 9600;
  Cpu_table.timer_average_overhead = 2;
  Cpu_table.timer_least_valid = 3;
  Cpu_table.exceptions_in_RAM = TRUE;
}
