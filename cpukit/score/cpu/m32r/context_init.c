/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <rtems/system.h>

typedef struct {
  uint32_t marker;
} Starting_Frame;

#define _get_r12( _r12 ) \
  __asm__ volatile( "mv r12, %0" : "=r" (_r12))

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp
)
{
  void *stackEnd = stack_base;
  Starting_Frame *frame;
  uint32_t r12;

  stackEnd += size;

  frame = (Starting_Frame *)stackEnd;
  frame--;
  frame->marker = 0xa5a5a5a5;

  _get_r12( r12 );

  the_context->r8     = 0x88888888;
  the_context->r9     = 0x99999999;
  the_context->r10    = 0xaaaaaaaa;
  the_context->r11    = 0xbbbbbbbb;
  the_context->r12    = r12;
  the_context->r13_fp = 0;
  the_context->r14_lr = (uintptr_t) entry_point;
  the_context->r15_sp = (uintptr_t) frame;

}
