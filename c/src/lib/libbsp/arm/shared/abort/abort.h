/*
 *  COPYRIGHT (c) 2007 Ray Xu.
 *  mailto: Rayx at gmail dot com
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  Copyright (c) 2002 Advent Networks, Inc
 *      Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifndef _BSPABORT_H
#define _BSPABORT_H

#include <rtems/system.h>
#include <rtems.h>
#include <rtems/bspIo.h>

#define INSN_MASK         0xc5

#define INSN_STM1         0x80
#define INSN_STM2         0x84
#define INSN_STR          0x40
#define INSN_STRB         0x44

#define INSN_LDM1         0x81
#define INSN_LDM23        0x85
#define INSN_LDR          0x41
#define INSN_LDRB         0x45

#define GET_RD(x)         ((x & 0x0000f000) >> 12)
#define GET_RN(x)         ((x & 0x000f0000) >> 16)

#define GET_U(x)          ((x & 0x00800000) >> 23)
#define GET_I(x)          ((x & 0x02000000) >> 25)

#define GET_REG(r, ctx)      (((uint32_t   *)ctx)[r])
#define SET_REG(r, ctx, v)   (((uint32_t   *)ctx)[r] = v)
#define GET_OFFSET(insn)     (insn & 0xfff)

/*
 * Prototypes
 */
void _print_full_context(uint32_t);
void do_data_abort(uint32_t, uint32_t, Context_Control *);

#endif /* _BSPABORT_H */
