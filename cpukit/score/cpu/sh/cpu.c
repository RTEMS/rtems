/**
 * @file
 *
 * @brief SuperH CPU Support
 *
 * This file contains information pertaining to the Hitachi SH
 * processor.
 */

/*
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
 *  COPYRIGHT (c) 1998-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/isr.h>
#include <rtems/score/sh_io.h>
#include <rtems/score/cpu.h>
#include <rtems/score/sh.h>

/* referenced in start.S */
CPU_ISR_raw_handler vectab[256] ;

#if SH_HAS_FPU
Context_Control_fp _CPU_Null_fp_context;
#endif

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS: NONE
 */

void _CPU_Initialize(void)
{
  register uint32_t   level = 0;

  /*
   *  If there is not an easy way to initialize the FP context
   *  during Context_Initialize, then it is usually easier to
   *  save an "uninitialized" FP context here and copy it to
   *  the task's during Context_Initialize.
   */

  /* FP context initialization support goes here */
#if SH_HAS_FPU
  /* FIXME: When not to use SH4_FPSCR_PR ? */
#ifdef __SH4__
  _CPU_Null_fp_context.fpscr = SH4_FPSCR_DN | SH4_FPSCR_RM | SH4_FPSCR_PR;
#endif
#ifdef __SH3E__
  /* FIXME: Wild guess :) */
  _CPU_Null_fp_context.fpscr = SH4_FPSCR_DN | SH4_FPSCR_RM;
#endif
#endif

  /* enable interrupts */
  _CPU_ISR_Set_level( level ) ;
}

/*
 *  _CPU_ISR_Get_level
 */

uint32_t   _CPU_ISR_Get_level( void )
{
  /*
   *  This routine returns the current interrupt level.
   */

  register uint32_t   _mask ;

  sh_get_interrupt_level( _mask );

  return ( _mask);
}

void _CPU_ISR_install_raw_handler(
  uint32_t             vector,
  CPU_ISR_raw_handler  new_handler,
  CPU_ISR_raw_handler *old_handler
)

{
  /*
   *  This is where we install the interrupt handler into the "raw" interrupt
   *  table used by the CPU to dispatch interrupt handlers.
   */
  volatile CPU_ISR_raw_handler *vbr ;

#if SH_PARANOID_ISR
  uint32_t  		level ;

  sh_disable_interrupts( level );
#endif

  /* get vbr */
  __asm__ ( "stc vbr,%0" : "=r" (vbr) );

  *old_handler = vbr[vector] ;
  vbr[vector]  = new_handler ;

#if SH_PARANOID_ISR
  sh_enable_interrupts( level );
#endif
}

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
)
{
#if defined(__sh1__) || defined(__sh2__)
   CPU_ISR_raw_handler ignored ;
#endif
   *old_handler = _ISR_Vector_table[ vector ];

 /*
  *  If the interrupt vector table is a table of pointer to isr entry
  *  points, then we need to install the appropriate RTEMS interrupt
  *  handler for this vector number.
  */
#if defined(__sh1__) || defined(__sh2__)
  _CPU_ISR_install_raw_handler(vector, _Hardware_isr_Table[vector], &ignored );
#endif

 /*
  *  We put the actual user ISR address in '_ISR_Vector_table'.
  *  This will be used by __ISR_Handler so the user gets control.
  */

 _ISR_Vector_table[ vector ] = new_handler;
}

void _CPU_Context_Initialize(
  Context_Control	*_the_context,
  void			*_stack_base,
  uint32_t  		_size,
  uint32_t  		_isr,
  void 	(*_entry_point)(void),
  int			_is_fp,
  void			*_tls_base)
{
  _the_context->r15 = (uint32_t *) ((uint32_t) (_stack_base) + (_size) );
#if defined(__sh1__) || defined(__sh2__) || defined(__SH2E__)
  _the_context->sr  = (_isr << 4) & 0x00f0 ;
#else
  _the_context->sr  = SH4_SR_MD | ((_isr << 4) & 0x00f0);
#endif
  _the_context->pr  = (uint32_t *) _entry_point ;


#if 0 && SH_HAS_FPU
   /* Disable FPU if it is non-fp task */
  if(!_is_fp)
    _the_context->sr |= SH4_SR_FD;
#endif
}
