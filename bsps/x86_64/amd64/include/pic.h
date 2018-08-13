/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _AMD64_PIC_H
#define _AMD64_PIC_H

#ifdef __cplusplus
extern "C" {
#endif

#define PIC1          0x20  /* IO base address for master PIC */
#define PIC2          0xA0  /* IO base address for slave PIC */
#define PIC1_COMMAND  PIC1
#define PIC1_DATA    (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA    (PIC2+1)

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define PIC_ICW1_ICW4       0x01  /* ICW4 (not) needed */
#define PIC_ICW1_SINGLE     0x02  /* Single (cascade) mode */
#define PIC_ICW1_INTERVAL4  0x04  /* Call address interval 4 (8) */
#define PIC_ICW1_LEVEL      0x08  /* Level triggered (edge) mode */
#define PIC_ICW1_INIT       0x10  /* Initialization - required! */

#define PIC_ICW4_8086       0x01  /* 8086/88 (MCS-80/85) mode */
#define PIC_ICW4_AUTO       0x02  /* Auto (normal) EOI */
#define PIC_ICW4_BUF_SLAVE  0x08  /* Buffered mode/slave */
#define PIC_ICW4_BUF_MASTER 0x0C  /* Buffered mode/master */
#define PIC_ICW4_SFNM       0x10  /* Special fully nested (not) */

/* This remaps IRQ0 to vector number 0x20 and so on (i.e. IDT[32]) */
#define PIC1_REMAP_DEST           0x20
#define PIC2_REMAP_DEST           0x28

/* Remap PIC1's interrupts to offset1 and PIC2's to offset2 */
void pic_remap(uint8_t offset1, uint8_t offset2);

/**
 * Mask all interrupt requests on PIC.
 *
 * @note Even with all interrupts masked, the PIC may still send spurious
 * interrupts (IRQ7), so we should handle them still.
 */
void pic_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* _AMD64_PIC_H */
