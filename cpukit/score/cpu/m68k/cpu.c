/**
 *  @file
 *
 *  @brief Motorola MC68xxx Dependent Source
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/tls.h>

#if defined( __mcoldfire__ ) && ( M68K_HAS_FPU == 1 )
  uint32_t _CPU_cacr_shadow;
#endif

void _CPU_Initialize(void)
{
#if ( M68K_HAS_VBR == 0 )
  /* fill the isr redirect table with the code to place the format/id
     onto the stack */

  uint32_t   slot;

  for (slot = 0; slot < CPU_INTERRUPT_NUMBER_OF_VECTORS; slot++)
  {
    _CPU_ISR_jump_table[slot].move_a7 = M68K_MOVE_A7;
    _CPU_ISR_jump_table[slot].format_id = slot << 2;
    _CPU_ISR_jump_table[slot].jmp = M68K_JMP;
    _CPU_ISR_jump_table[slot].isr_handler = (uint32_t) 0xDEADDEAD;
  }
#endif /* M68K_HAS_VBR */
}

uint32_t   _CPU_ISR_Get_level( void )
{
  uint32_t   level;

  m68k_get_interrupt_level( level );

  return level;
}

/*
 *  _CPU_ISR_install_raw_handler
 */

void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  proc_ptr *interrupt_table = NULL;

#if (M68K_HAS_FPSP_PACKAGE == 1)
  /*
   *  If this vector being installed is one related to FP, then the
   *  FPSP will install the handler itself and handle it completely
   *  with no intervention from RTEMS.
   */

  if (*_FPSP_install_raw_handler &&
      (*_FPSP_install_raw_handler)(vector, new_handler, *old_handler))
        return;
#endif


  /*
   *  On CPU models without a VBR, it is necessary for there to be some
   *  header code for each ISR which saves a register, loads the vector
   *  number, and jumps to _ISR_Handler.
   */

  m68k_get_vbr( interrupt_table );
#if ( M68K_HAS_VBR == 1 )
  *old_handler = interrupt_table[ vector ];
  interrupt_table[ vector ] = new_handler;
#else

  /*
   *  Install handler into RTEMS jump table and if VBR table is in
   *  RAM, install the pointer to the appropriate jump table slot.
   *  If the VBR table is in ROM, it is the BSP's responsibility to
   *  load it appropriately to vector to the RTEMS jump table.
   */

  *old_handler = (proc_ptr) _CPU_ISR_jump_table[vector].isr_handler;
  _CPU_ISR_jump_table[vector].isr_handler = (uint32_t) new_handler;
  if ( (uint32_t) interrupt_table != 0xFFFFFFFF )
    interrupt_table[ vector ] = (proc_ptr) &_CPU_ISR_jump_table[vector];
#endif /* M68K_HAS_VBR */
}

void _CPU_ISR_install_vector(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  proc_ptr ignored = 0;  /* to avoid warning */

  *old_handler = _ISR_Vector_table[ vector ];

  _CPU_ISR_install_raw_handler( vector, _ISR_Handler, &ignored );

  _ISR_Vector_table[ vector ] = new_handler;
}


/*
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
#if ( M68K_HAS_SEPARATE_STACKS == 1 )
  void *isp = _CPU_Interrupt_stack_high;

  __asm__ volatile ( "movec %0,%%isp" : "=r" (isp) : "0" (isp) );
#endif
}

#if ( M68K_HAS_BFFFO != 1 )
/*
 * Returns table for duplication of the BFFFO instruction (16 bits only)
 */
const unsigned char _CPU_m68k_BFFFO_table[256] = {
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

/*
 *  The following code context switches the software FPU emulation
 *  code provided with GCC.
 */

#if (CPU_SOFTWARE_FP == TRUE)
extern Context_Control_fp _fpCCR;

void _CPU_Context_save_fp (Context_Control_fp **fp_context_ptr)
{
  Context_Control_fp *fp;

  fp = *fp_context_ptr;

  *fp = _fpCCR;
}

void _CPU_Context_restore_fp (Context_Control_fp **fp_context_ptr)
{
  Context_Control_fp *fp;

  fp = *fp_context_ptr;

  _fpCCR = *fp;
}
#endif

void _CPU_Context_Initialize(
  Context_Control *the_context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
)
{
  uint32_t stack;

  the_context->sr      = 0x3000 | (new_level << 8);
  stack                = (uint32_t)stack_area_begin + stack_area_size - 4;
  the_context->a7_msp  = (void *)stack;
  *(void **)stack      = (void *)entry_point;

#if (defined(__mcoldfire__) && ( M68K_HAS_FPU == 1 ))
  the_context->fpu_dis = is_fp ? 0x00 : 0x10;
#endif

  if ( tls_area != NULL ) {
    _TLS_TCB_before_TLS_block_initialize( tls_area );
  }
}
