/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 * 
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
 
unsigned char *duart_base;
extern struct duart_regs duart_info;

#define DELAY 5000

void led_putnum();

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */
 
extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;

rtems_cpu_table Cpu_table;
 
char *rtems_progname;

/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );
void bsp_pretasking_hook(void);               /* m68k version */

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  m68k_isr_entry *monitors_vector_table;
  int             index;
  extern void          *_WorkspaceBase;
  extern void          *_RamSize;
  extern unsigned long  _M68k_Ramsize;

  _M68k_Ramsize = (unsigned long)&_RamSize;		/* RAM size set in linker script */

  duart_base = (unsigned char *)DUART_ADDR;

  /* 
   *  Set the VBR here to the monitor's default.
   */

  monitors_vector_table = (m68k_isr_entry *)0;
   /* This is where you set vector base register = 0 */
  m68k_set_vbr( monitors_vector_table );

  /* The vector interrupt table for the 680x0 is in appendix B-2 
	 of the M68000 Family Programmer's reference table */
  for ( index=2 ; index<=255 ; index++ )
    M68Kvec[ index ] = monitors_vector_table[ 32 ];


  M68Kvec[  2 ] = monitors_vector_table[  2 ];   /* bus error vector */
  M68Kvec[  4 ] = monitors_vector_table[  4 ];   /* breakpoints vector */
  M68Kvec[  9 ] = monitors_vector_table[  9 ];   /* trace vector */

  /* 
   *  Set the VBR here if you do not want to use the monitor's vector table.
   */

  m68k_set_vbr( &M68Kvec );

  rtems_cache_enable_instruction();
  rtems_cache_enable_data();

  /*
   *  we only use a hook to get the C library initialized.
   */
 
  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.interrupt_vector_table = (m68k_isr_entry *) &M68Kvec;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

  BSP_Configuration.work_space_start = (void *) &_WorkspaceBase;
 
/*  led_putnum('e'); * for debugging purposes only */

  /* Clock_exit is done as an atexit() function */
}
