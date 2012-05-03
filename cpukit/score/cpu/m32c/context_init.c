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
  uint16_t sbLow;
  uint16_t sbHigh;       /* push/pop sb */
  uint16_t flg;          /* push/pop flg */
  uint32_t a1;           /* pushm */
  uint32_t a0;
  uint32_t r0r2;
  uint32_t r1r3;
  uint16_t frameLow;     /* exitd */
  uint16_t frameHigh;
  uint16_t startLow;
  uint16_t startHigh;
  uint16_t zero;
} Starting_Frame;

#if defined(__r8c_cpu__)
  #warning "_get_sb: not implemented on R8C"
  #define _get_sb( _sb )
#else
  #define _get_sb( _sb ) \
    __asm__ volatile( "stc sb, %0" : "=r" (_sb))
#endif

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  size_t            size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp
)
{
  void *stackEnd = stack_base;
  register uint32_t sb;
  Starting_Frame *frame;

  _get_sb( sb );
  stackEnd += size;

  frame = (Starting_Frame *)stackEnd;
  frame--;

  frame->zero = 0;
  frame->sbLow = ((uint32_t)sb) & 0xffff;
  frame->sbHigh = ((uint32_t)sb >> 16) & 0xffff;
  frame->flg = 0x80;        /* User stack */
  if ( !new_level )         /* interrupt level 0 --> enabled */
    frame->flg |= 0x40;
  frame->a0 = 0x01020304;
  frame->a1 =0xa1a2a3a4;
  frame->r0r2 = 0;
  frame->r1r3 = 0;
#if defined(__r8c_cpu__)
  #warning "not implemented on R8C"
#else
  frame->frameLow  = (uint16_t) (((uint32_t)frame)  & 0xffff);
  frame->frameHigh = (uint16_t) (((uint32_t)frame >> 16) & 0xffff);
  frame->startLow  = (uint16_t) (((uint32_t)entry_point) & 0xffff);
  frame->startHigh = (uint16_t) (((uint32_t)entry_point >> 16) & 0xffff);
#endif
  the_context->sp = (uintptr_t)frame;
  the_context->fb = (uintptr_t)&frame->frameLow;
}
