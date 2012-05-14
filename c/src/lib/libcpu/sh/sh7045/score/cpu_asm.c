/*
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language
 *
 *  NOTE:  This port uses a C file with inline assembler instructions
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*
 *  This is supposed to be an assembly file.  This means that system.h
 *  and cpu.h should not be included in a "real" cpu_asm file.  An
 *  implementation in assembly should include "cpu_asm.h"
 */

#include <rtems/system.h>
#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/sh.h>

#include <rtems/score/ispsh7045.h>
#include <rtems/score/iosh7045.h>
#include <rtems/score/sh_io.h>

/* from cpu_isps.c */
extern proc_ptr         _Hardware_isr_Table[];

#if (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  unsigned long    *_old_stack_ptr;
#endif

register unsigned long  *stack_ptr __asm__ ("r15");

/*
 * sh_set_irq_priority
 *
 * this function sets the interrupt level of the specified interrupt
 *
 * parameters:
 *             - irq : interrupt number
 *             - prio: priority to set for this interrupt number
 *
 * returns:    0 if ok
 *             -1 on error
 */

unsigned int sh_set_irq_priority(
  unsigned int irq,
  unsigned int prio )
{
  uint32_t   shiftcount;
  uint32_t   prioreg;
  uint16_t   temp16;
  ISR_Level  level;

  /*
   * first check for valid interrupt
   */
  if (( irq > 156) || (irq < 64) || (_Hardware_isr_Table[irq] == _dummy_isp))
    return -1;
  /*
   * check for valid irq priority
   */
  if ( prio > 15 )
    return -1;

  /*
   * look up appropriate interrupt priority register
   */
  if ( irq > 71)
    {
      irq = irq - 72;
      shiftcount = 12 - ((irq & ~0x03) % 16);

      switch( irq / 16)
	{
	case 0: { prioreg = INTC_IPRC; break;}
	case 1: { prioreg = INTC_IPRD; break;}
	case 2: { prioreg = INTC_IPRE; break;}
	case 3: { prioreg = INTC_IPRF; break;}
	case 4: { prioreg = INTC_IPRG; break;}
	case 5: { prioreg = INTC_IPRH; break;}
	default: return -1;
	}
    }
  else
    {
      shiftcount = 12 - 4 * ( irq % 4);
      if ( irq > 67)
	prioreg = INTC_IPRB;
      else
	prioreg = INTC_IPRA;
    }

  /*
   * Set the interrupt priority register
   */
  _ISR_Disable( level );

    temp16 = read16( prioreg);
    temp16 &= ~( 15 << shiftcount);
    temp16 |= prio << shiftcount;
    write16( temp16, prioreg);

  _ISR_Enable( level );

  return 0;
}

/*
 *  This routine provides the RTEMS interrupt management.
 */

void __ISR_Handler( uint32_t   vector)
{
  ISR_Level level;

  _ISR_Disable( level );

  _Thread_Dispatch_increment_disable_level();

#if (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  if ( _ISR_Nest_level == 0 )
    {
      /* Install irq stack */
      _old_stack_ptr = stack_ptr;
      stack_ptr = _CPU_Interrupt_stack_high;
    }

#endif

  _ISR_Nest_level++;

  _ISR_Enable( level );

  /* call isp */
  if ( _ISR_Vector_table[ vector])
    (*_ISR_Vector_table[ vector ])( vector );

  _ISR_Disable( level );

  _Thread_Dispatch_decrement_disable_level();

  _ISR_Nest_level--;

#if (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)

  if ( _ISR_Nest_level == 0 )
    /* restore old stack pointer */
    stack_ptr = _old_stack_ptr;
#endif

  _ISR_Enable( level );

  if ( _ISR_Nest_level )
    return;

  if ( _Thread_Dispatch_in_critical_section() ) {
    return;
  }

  if ( _Thread_Dispatch_necessary ) {
    _Thread_Dispatch();
  }
}
