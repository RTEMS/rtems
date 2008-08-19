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

#include <bsp.h>
#include <rtems/libio.h>

#include <string.h>

/*
 *  Use the shared implementations of the following routines
 */

void bsp_libc_init( void *, uint32_t, int );
void bsp_pretasking_hook(void);               /* m68k version */

/*
 *  Call Spurious_Initialize in bsp_predriver_hook because
 *  bsp_predriver_hook is call after the _ISR_Vector_Table allocation
 */

void bsp_predriver_hook(void)
{
  extern void Spurious_Initialize(void);
  Spurious_Initialize();
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  void           *vbr;
  extern void    *_WorkspaceBase;
  extern void    *_RamSize;
  extern unsigned long _M68k_Ramsize;

  /* RAM size set in linker script */
  _M68k_Ramsize = (unsigned long)&_RamSize;

  m68k_get_vbr( vbr );

  Configuration.work_space_start = (void *) &_WorkspaceBase;
}
