/*
 *  Intel i960CA Dependent Source
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
/*
 * 1999/04/26: added support for Intel i960RP
 */

#if defined(__i960CA__) || defined(__i960_CA__) || defined(__i960CA)
#elif defined(__i960RP__)
#else
#warning "***  ENTIRE FILE IMPLEMENTED & TESTED FOR CA & RP ONLY  ***"
#warning "*** THIS FILE WILL NOT COMPILE ON ANOTHER FAMILY MEMBER ***"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>

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

#if defined(__i960CA__) || defined(__i960_CA__) || defined(__i960CA)
#define _Is_vector_caching_enabled( _prcb ) \
   ((_prcb)->control_tbl->icon & 0x2000)
#elif defined(__i960RP__)
#define _Is_vector_caching_enabled( _prcb ) \
   ((*((unsigned int *) ICON_ADDR)) & 0x2000)
#endif

void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  i960_PRCB   *prcb = _CPU_Table.Prcb;
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

#if defined(__i960CA__) || defined(__i960_CA__) || defined(__i960CA)
#define soft_reset( prcb ) \
 { register i960_PRCB *_prcb = (prcb); \
   register unsigned32  *_next=0; \
   register unsigned32   _cmd  = 0x30000; \
   asm volatile( "lda    next,%1; \
                  sysctl %0,%1,%2; \
            next: mov    g0,g0" \
                  : "=d" (_cmd), "=d" (_next), "=d" (_prcb) \
                  : "0"  (_cmd), "1"  (_next), "2"  (_prcb) ); \
 }
#else
#if defined(__i960RP__) || defined(__i960_RP__) || defined(__i960RP)
#define soft_reset( prcb ) \
 { register i960_PRCB *_prcb = (prcb); \
   register unsigned32  *_next=0; \
   register unsigned32   _cmd  = 0x300; \
   asm volatile( "lda    next,%1; \
                  sysctl %0,%1,%2; \
            next: mov    g0,g0" \
                  : "=d" (_cmd), "=d" (_next), "=d" (_prcb) \
                  : "0"  (_cmd), "1"  (_next), "2"  (_prcb) ); \
 }
#endif
#endif

void _CPU_Install_interrupt_stack( void )
{
  i960_PRCB *prcb = _CPU_Table.Prcb;
  unsigned32   level;
#if defined(__i960RP__) || defined(__i960_RP__)
  int *isp = (int *) ISP_ADDR;
#endif

  /*
   *  Set the Interrupt Stack in the PRCB and force a reload of it.
   *  Interrupts are disabled for safety.
   */

  _CPU_ISR_Disable( level );

    prcb->intr_stack = _CPU_Interrupt_stack_low;

#if defined(__i960CA__) || defined(__i960_CA__) || defined(__i960CA)
    soft_reset( prcb );
#elif defined(__i960RP__) || defined(__i960_RP__) || defined(__i960RP)
    *isp = prcb->intr_stack;
#endif

  _CPU_ISR_Enable( level );
}
