/*
 *  Intel i960CA Dependent Source
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#if defined(__i960CA__) || defined(__i960_CA__) || defined(__i960CA)
#else
#warning "***    ENTIRE FILE IMPLEMENTED & TESTED FOR CA ONLY     ***"
#warning "*** THIS FILE WILL NOT COMPILE ON ANOTHER FAMILY MEMBER ***"
#endif

#include <rtems/system.h>
#include <rtems/fatal.h>
#include <rtems/core/isr.h>

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 *
 *  OUTPUT PARAMETERS: NONE
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{

  _CPU_Table = *cpu_table;

}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  unsigned32 level;
 
  i960_get_interrupt_level( level );
 
  return level;
}

/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 */
 
#define _Is_vector_caching_enabled( _prcb ) \
   ((_prcb)->control_tbl->icon & 0x2000)

void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  i960ca_PRCB *prcb = _CPU_Table.Prcb;
  proc_ptr    *cached_intr_tbl = NULL;

  /*  The i80960CA does not support vectors 0-7.  The first 9 entries
   *  in the Interrupt Table are used to manage pending interrupts.
   *  Thus vector 8, the first valid vector number, is actually in
   *  slot 9 in the table.
   */

  *old_handler = prcb->intr_tbl[ vector + 1 ];

  prcb->intr_tbl[ vector + 1 ] = new_handler;

  if ( _Is_vector_caching_enabled( prcb ) )
    if ( (vector & 0xf) == 0x2 )       /* cacheable? */
      cached_intr_tbl[ vector >> 4 ] = new_handler;
}

/*PAGE
 *
 *  _CPU__ISR_install_vector
 *
 *  Install the RTEMS vector wrapper in the CPU's interrupt table.
 *
 *  Input parameters:
 *    vector      - interrupt vector number
 *    old_handler - former ISR for this vector number
 *    new_handler - replacement ISR for this vector number
 *
 *  Output parameters:  NONE
 *
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  proc_ptr ignored;

  *old_handler = _ISR_Vector_table[ vector ];

  _CPU_ISR_install_raw_handler( vector, _ISR_Handler, &ignored );

  _ISR_Vector_table[ vector ] = new_handler;
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

#define soft_reset( prcb ) \
 { register i960ca_PRCB *_prcb = (prcb); \
   register unsigned32  *_next=0; \
   register unsigned32   _cmd  = 0x30000; \
   asm volatile( "lda    next,%1; \
                  sysctl %0,%1,%2; \
            next: mov    g0,g0" \
                  : "=d" (_cmd), "=d" (_next), "=d" (_prcb) \
                  : "0"  (_cmd), "1"  (_next), "2"  (_prcb) ); \
 }

void _CPU_Install_interrupt_stack( void )
{
  i960ca_PRCB *prcb = _CPU_Table.Prcb;
  unsigned32   level;

  /*
   *  Set the Interrupt Stack in the PRCB and force a reload of it.
   *  Interrupts are disabled for safety.
   */

  _CPU_ISR_Disable( level );

    prcb->intr_stack = _CPU_Interrupt_stack_low;

    soft_reset( prcb );

  _CPU_ISR_Enable( level );
}
