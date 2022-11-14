/* SPDX-License-Identifier: BSD-2-Clause */

/*  B1553RT driver interface
 *
 *  COPYRIGHT (c) 2009.
 *  Aeroflex Gaisler AB
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __B1553RT_H__
#define __B1553RT_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

struct rt_reg {
    volatile unsigned int stat;            /* 0x00 */
    volatile unsigned int ctrl;            /* 0x04 */
    volatile unsigned int vword;           /* 0x08 */
    volatile unsigned int irq;             /* 0x0C */
    volatile unsigned int addr;            /* 0x10 */
		volatile unsigned int ipm;             /* 0x14 */
};


struct rt_msg {
    unsigned short miw;
    unsigned short time;
    unsigned short data[32];
    unsigned short desc;
};

#define RT_FREQ_12MHZ 0
#define RT_FREQ_16MHZ 1
#define RT_FREQ_20MHZ 2
#define RT_FREQ_24MHZ 3
#define RT_FREQ_MASK 0x3

/* IOCTLs */
#define RT_SET_ADDR    3
#define RT_SET_BCE     5
#define RT_RX_BLOCK    8
#define RT_CLR_STATUS  12
#define RT_GET_STATUS  13
#define RT_SET_EVENTID 14

#define RT_SET_VECTORW 32
#define RT_SET_EXTMDATA 33

#define RT_ILLCMD_IRQ  128   
#define RT_MERR_IRQ       2048
#define RT_DMAF_IRQ       32768                /* DMA Fail irq */

#define RT_TSW_OK  (1<<14)
#define RT_TSW_BUS (1<<13)
#define RT_TSW_BC  (1<<12)
#define RT_TSW_LPBKERRB  (1<<11)
#define RT_TSW_LPBKERRA  (1<<10)
#define RT_TSW_ILL  (1<<9)
#define RT_TSW_MEM  (1<<8)
#define RT_TSW_MAN  (1<<7)
#define RT_TSW_PAR  (1<<6)
#define RT_TSW_WC   (1<<5)

void b1553rt_print_dev(struct drvmgr_dev *dev, int options);
void b1553rt_print(int options);

void b1553rt_register_drv(void);

#ifdef __cplusplus
}
#endif

#endif /* __RT_H__ */

