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

#include <rtems/score/isr.h>
#include <rtems/score/percpu.h>
#include <rtems/score/tls.h>
#include <rtems/config.h>

#if ( M68K_HAS_VBR == 0 )

/*
 * Table of ISR handler entries that resides in RAM. The FORMAT/ID is
 * pushed onto the stack. This is not is the same order as VBR processors.
 * The ISR handler takes the format and uses it for dispatching the user
 * handler.
 */

typedef struct {
  uint16_t   move_a7;            /* move #FORMAT_ID,%a7@- */
  uint16_t   format_id;
  uint16_t   jmp;                /* jmp  _ISR_Handlers */
  uint32_t   isr_handler;
} _CPU_ISR_handler_entry;

#define M68K_MOVE_A7 0x3F3C
#define M68K_JMP     0x4EF9

/* points to jsr-exception-table in targets wo/ VBR register */
static _CPU_ISR_handler_entry
_CPU_ISR_jump_table[ CPU_INTERRUPT_NUMBER_OF_VECTORS ];

#endif /* M68K_HAS_VBR */

#if (M68K_HAS_FPSP_PACKAGE == 1)
int (*_FPSP_install_raw_handler)(
  uint32_t   vector,
  CPU_ISR_raw_handler new_handler,
  CPU_ISR_raw_handler *old_handler
);
#endif

#if defined( __mcoldfire__ ) && ( M68K_HAS_FPU == 1 )
  uint32_t _CPU_cacr_shadow;
#endif

static void m68k_install_interrupt_stack( void )
{
#if ( M68K_HAS_SEPARATE_STACKS == 1 )
  uintptr_t isp = (uintptr_t) _ISR_Stack_area_end;

  __asm__ volatile ( "movec %0,%%isp" : "=r" (isp) : "0" (isp) );
#endif
}

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

  m68k_install_interrupt_stack();
}

uint32_t   _CPU_ISR_Get_level( void )
{
  uint32_t   level;

  m68k_get_interrupt_level( level );

  return level;
}

void _CPU_ISR_install_raw_handler(
  uint32_t             vector,
  CPU_ISR_raw_handler  new_handler,
  CPU_ISR_raw_handler *old_handler
)
{
#if ( M68K_HAS_VBR == 1 )
  CPU_ISR_raw_handler *interrupt_table = NULL;
#else
  _CPU_ISR_handler_entry *interrupt_table = NULL;
#endif

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

  *old_handler = (CPU_ISR_raw_handler) _CPU_ISR_jump_table[vector].isr_handler;
  _CPU_ISR_jump_table[vector].isr_handler = (uint32_t) new_handler;
  if ( (uint32_t) interrupt_table != 0xFFFFFFFF )
    interrupt_table[ vector ] = &_CPU_ISR_jump_table[vector];
#endif /* M68K_HAS_VBR */
}

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
)
{
  CPU_ISR_raw_handler ignored = 0;  /* to avoid warning */

  *old_handler = _ISR_Vector_table[ vector ];

  _CPU_ISR_install_raw_handler( vector, _ISR_Handler, &ignored );

  _ISR_Vector_table[ vector ] = new_handler;
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
