/**
 *  @file
 *  
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <libcpu/isr_entries.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

/* Structure filled in by get_mem_info.  Only the size field is
 * actually used (to clear bss), so the others aren't even filled in.
 */
struct s_mem
{
  unsigned int size;
  unsigned int icsize;
  unsigned int dcsize;
};

void bsp_start( void );
void clear_cache( void *address, size_t n );
void get_mem_info( struct s_mem *mem );

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  mips_set_sr( 0xff00 );  /* all interrupts unmasked but globally off */
                          /* depend on the IRC to take care of things */
  bsp_interrupt_initialize();
}

/*
 *  Required routine by some gcc run-times.
 */
void clear_cache( void *address, size_t n )
{
}


void get_mem_info(
  struct s_mem *mem
)
{
  mem->size = 0x1000000;        /* XXX figure out something here */
}
