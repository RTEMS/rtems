/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup amba
 * @brief Common GRLIB AMBA Core Register definitions
 */

/*
 *  COPYRIGHT (c) 2012
 *  Aeroflex Gaisler
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

#ifndef __GRLIB_H__
#define __GRLIB_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSBSPsSharedGRLIB GRLIB
 *
 * @ingroup RTEMSBSPsShared
 *
 * @brief Driver support for GRLIB IP Library.
 */

/* ESA MEMORY CONTROLLER */
struct mctrl_regs {
  unsigned int mcfg1;
  unsigned int mcfg2;
  unsigned int mcfg3;
};

/* APB UART */
struct apbuart_regs {
  volatile uint32_t data;
  volatile uint32_t status;
  volatile uint32_t ctrl;
  volatile uint32_t scaler;
};

/* IRQMP and IRQAMP interrupt controller timestamps */
struct irqmp_timestamp_regs {
  volatile uint32_t counter;   /* 0x00 */
  volatile uint32_t control;   /* 0x04 */
  volatile uint32_t assertion; /* 0x08 */
  volatile uint32_t ack;       /* 0x0c */
};

static inline bool irqmp_has_timestamp(
  volatile struct irqmp_timestamp_regs *irqmp_ts
)
{
  return ( irqmp_ts->control >> 27 ) > 0;
}

/* IRQMP and IRQAMP interrupt controllers */
struct irqmp_regs {
  volatile uint32_t                    ilevel;      /* 0x00 */
  volatile uint32_t                    ipend;       /* 0x04 */
  volatile uint32_t                    iforce;      /* 0x08 */
  volatile uint32_t                    iclear;      /* 0x0c */
  volatile uint32_t                    mpstat;      /* 0x10 */
  volatile uint32_t                    bcast;       /* 0x14 */
  volatile uint32_t                    notused02;   /* 0x18 */
  volatile uint32_t                    wdgctrl;     /* 0x1c */
  volatile uint32_t                    ampctrl;     /* 0x20 */
  volatile uint32_t                    icsel[ 2 ];  /* 0x24,0x28 */
  volatile uint32_t                    notused13;   /* 0x2c */
  volatile uint32_t                    notused20;   /* 0x30 */
  volatile uint32_t                    notused21;   /* 0x34 */
  volatile uint32_t                    notused22;   /* 0x38 */
  volatile uint32_t                    notused23;   /* 0x3c */
  volatile uint32_t                    mask[ 16 ];  /* 0x40 */
  volatile uint32_t                    force[ 16 ]; /* 0x80 */
  /* Extended IRQ registers */
  volatile uint32_t                    intid[ 16 ];     /* 0xc0 */
  volatile struct irqmp_timestamp_regs timestamp[ 16 ]; /* 0x100 */
  volatile uint32_t                    resetaddr[ 4 ];  /* 0x200 */
  volatile uint32_t                    resv0[ 12 ];     /* 0x210 - 0x23C */
  volatile uint32_t                    pboot;           /* 0x240 */
  volatile uint32_t                    resv1[ 47 ];     /* 0x244 - 0x2FC */
  volatile uint32_t                    irqmap[ 8 ];     /* 0x300 - 0x31C */
  volatile uint32_t                    resv2[ 824 ];    /* 0x320 - 0x1000 */
};

/* GPTIMER Timer instance */
struct gptimer_timer_regs {
  volatile uint32_t value;
  volatile uint32_t reload;
  volatile uint32_t ctrl;
  volatile uint32_t notused;
};

#define GPTIMER_TIMER_CTRL_EN 0x00000001U
#define GPTIMER_TIMER_CTRL_RS 0x00000002U
#define GPTIMER_TIMER_CTRL_LD 0x00000004U
#define GPTIMER_TIMER_CTRL_IE 0x00000008U
#define GPTIMER_TIMER_CTRL_IP 0x00000010U
#define GPTIMER_TIMER_CTRL_CH 0x00000020U
#define GPTIMER_TIMER_CTRL_DH 0x00000040U

/* GPTIMER common registers */
struct gptimer_regs {
  volatile uint32_t         scaler_value; /* common timer registers */
  volatile uint32_t         scaler_reload;
  volatile uint32_t         cfg;
  volatile uint32_t         notused;
  struct gptimer_timer_regs timer[ 7 ];
};

/* GRGPIO GPIO */
struct grgpio_regs {
  volatile uint32_t data;        /* 0x00 I/O port data register */
  volatile uint32_t output;      /* 0x04 I/O port output register */
  volatile uint32_t dir;         /* 0x08 I/O port direction register */
  volatile uint32_t imask;       /* 0x0C Interrupt mask register */
  volatile uint32_t ipol;        /* 0x10 Interrupt polarity register */
  volatile uint32_t iedge;       /* 0x14 Interrupt edge register */
  volatile uint32_t bypass;      /* 0x18 Bypass register */
  volatile uint32_t cap;         /* 0x1C Capability register */
  volatile uint32_t irqmap[ 4 ]; /* 0x20 - 0x2C Interrupt map registers */
  volatile uint32_t res_30;      /* 0x30 Reserved */
  volatile uint32_t res_34;      /* 0x34 Reserved */
  volatile uint32_t res_38;      /* 0x38 Reserved */
  volatile uint32_t res_3C;      /* 0x3C Reserved */
  volatile uint32_t iavail;      /* 0x40 Interrupt available register */
  volatile uint32_t iflag;       /* 0x44 Interrupt flag register */
  volatile uint32_t res_48;      /* 0x48 Reserved */
  volatile uint32_t pulse;       /* 0x4C Pulse register */
  volatile uint32_t res_50;      /* 0x50 Reserved */
  volatile uint32_t output_or; /* 0x54 I/O port output register, logical-OR */
  volatile uint32_t dir_or; /* 0x58 I/O port direction register, logical-OR */
  volatile uint32_t imask_or; /* 0x5C Interrupt mask register, logical-OR */
  volatile uint32_t res_60;   /* 0x60 Reserved */
  volatile uint32_t
    output_and; /* 0x64 I/O port output register, logical-AND */
  volatile uint32_t
    dir_and; /* 0x68 I/O port direction register, logical-AND */
  volatile uint32_t imask_and; /* 0x6C Interrupt mask register, logical-AND */
  volatile uint32_t res_70;    /* 0x70 Reserved */
  volatile uint32_t
    output_xor; /* 0x74 I/O port output register, logical-XOR */
  volatile uint32_t
    dir_xor; /* 0x78 I/O port direction register, logical-XOR */
  volatile uint32_t imask_xor; /* 0x7C Interrupt mask register, logical-XOR */
};

/* L2C - Level 2 Cache Controller registers */
struct l2c_regs {
  volatile uint32_t control;                /* 0x00 Control register */
  volatile uint32_t status;                 /* 0x04 Status register */
  volatile uint32_t flush_mem_addr;         /* 0x08 Flush (Memory address) */
  volatile uint32_t flush_set_index;        /* 0x0c Flush (set, index) */
  volatile uint32_t access_counter;         /* 0x10 */
  volatile uint32_t hit_counter;            /* 0x14 */
  volatile uint32_t bus_cycle_counter;      /* 0x18 */
  volatile uint32_t bus_usage_counter;      /* 0x1c */
  volatile uint32_t error_status_control;   /* 0x20 Error status/control */
  volatile uint32_t error_addr;             /* 0x24 Error address */
  volatile uint32_t tag_check_bit;          /* 0x28 TAG-check-bit */
  volatile uint32_t data_check_bit;         /* 0x2c Data-check-bit */
  volatile uint32_t scrub_control_status;   /* 0x30 Scrub Control/Status */
  volatile uint32_t scrub_delay;            /* 0x34 Scrub Delay */
  volatile uint32_t error_injection;        /* 0x38 Error injection */
  volatile uint32_t access_control;         /* 0x3c Access control */
  volatile uint32_t reserved_40[ 16 ];      /* 0x40 Reserved */
  volatile uint32_t mtrr[ 32 ];             /* 0x80 - 0xFC MTRR registers */
  volatile uint32_t reserved_100[ 131008 ]; /* 0x100 Reserved */
  volatile uint32_t
    diag_iface_tag[ 16384 ]; /* 0x80000 - 0x8FFFC Diagnostic interface (Tag) */
  volatile uint32_t reserved_90000[ 376832 ]; /* 0x90000 Reserved */
  volatile uint32_t diag_iface_data
    [ 524288 ]; /* 0x200000 - 0x3FFFFC Diagnostic interface (Data) */
};

#ifdef __cplusplus
}
#endif

#endif
