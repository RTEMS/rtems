/**
 * @file
 * @ingroup amba
 * @brief Common GRLIB AMBA Core Register definitions
 */

/*
 *  COPYRIGHT (c) 2012
 *  Aeroflex Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRLIB_H__
#define __GRLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ESA MEMORY CONTROLLER */
struct mctrl_regs {
  unsigned int mcfg1;
  unsigned int mcfg2;
  unsigned int mcfg3;
};

/* APB UART */
struct apbuart_regs {
  volatile unsigned int data;
  volatile unsigned int status;
  volatile unsigned int ctrl;
  volatile unsigned int scaler;
};

/* IRQMP and IRQAMP interrupt controller timestamps */
struct irqmp_timestamp_regs {
  volatile unsigned int counter;     /* 0x00 */
  volatile unsigned int control;     /* 0x04 */
  volatile unsigned int assertion;   /* 0x08 */
  volatile unsigned int ack;         /* 0x0c */
};

/* IRQMP and IRQAMP interrupt controllers */
struct irqmp_regs {
  volatile unsigned int ilevel;      /* 0x00 */
  volatile unsigned int ipend;       /* 0x04 */
  volatile unsigned int iforce;      /* 0x08 */
  volatile unsigned int iclear;      /* 0x0c */
  volatile unsigned int mpstat;      /* 0x10 */
  volatile unsigned int bcast;       /* 0x14 */
  volatile unsigned int notused02;   /* 0x18 */
  volatile unsigned int wdgctrl;     /* 0x1c */
  volatile unsigned int ampctrl;     /* 0x20 */
  volatile unsigned int icsel[2];    /* 0x24,0x28 */
  volatile unsigned int notused13;   /* 0x2c */
  volatile unsigned int notused20;   /* 0x30 */
  volatile unsigned int notused21;   /* 0x34 */
  volatile unsigned int notused22;   /* 0x38 */
  volatile unsigned int notused23;   /* 0x3c */
  volatile unsigned int mask[16];    /* 0x40 */
  volatile unsigned int force[16];   /* 0x80 */
  /* Extended IRQ registers */
  volatile unsigned int intid[16];   /* 0xc0 */
  volatile struct irqmp_timestamp_regs timestamp[16]; /* 0x100 */
  volatile unsigned int resetaddr[4]; /* 0x200 */
  volatile unsigned int resv0[12];    /* 0x210 - 0x23C */
  volatile unsigned int pboot;        /* 0x240 */
  volatile unsigned int resv1[47];    /* 0x244 - 0x2FC */
  volatile unsigned int irqmap[8];    /* 0x300 - 0x31C */
  volatile unsigned int resv2[824];   /* 0x320 - 0x1000 */
};

/* GPTIMER Timer instance */
struct gptimer_timer_regs {
  volatile unsigned int value;
  volatile unsigned int reload;
  volatile unsigned int ctrl;
  volatile unsigned int notused;
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
  volatile unsigned int scaler_value;   /* common timer registers */
  volatile unsigned int scaler_reload;
  volatile unsigned int cfg;
  volatile unsigned int notused;
  struct gptimer_timer_regs timer[7];
};

/* GRGPIO GPIO */
struct grgpio_regs {
  volatile unsigned int data;        /* 0x00 I/O port data register */
  volatile unsigned int output;      /* 0x04 I/O port output register */
  volatile unsigned int dir;         /* 0x08 I/O port direction register */
  volatile unsigned int imask;       /* 0x0C Interrupt mask register */
  volatile unsigned int ipol;        /* 0x10 Interrupt polarity register */
  volatile unsigned int iedge;       /* 0x14 Interrupt edge register */
  volatile unsigned int bypass;      /* 0x18 Bypass register */
  volatile unsigned int cap;         /* 0x1C Capability register */
  volatile unsigned int irqmap[4];   /* 0x20 - 0x2C Interrupt map registers */
  volatile unsigned int res_30;      /* 0x30 Reserved */
  volatile unsigned int res_34;      /* 0x34 Reserved */
  volatile unsigned int res_38;      /* 0x38 Reserved */
  volatile unsigned int res_3C;      /* 0x3C Reserved */
  volatile unsigned int iavail;      /* 0x40 Interrupt available register */
  volatile unsigned int iflag;       /* 0x44 Interrupt flag register */
  volatile unsigned int res_48;      /* 0x48 Reserved */
  volatile unsigned int pulse;       /* 0x4C Pulse register */
  volatile unsigned int res_50;      /* 0x50 Reserved */
  volatile unsigned int output_or;   /* 0x54 I/O port output register, logical-OR */
  volatile unsigned int dir_or;      /* 0x58 I/O port direction register, logical-OR */
  volatile unsigned int imask_or;    /* 0x5C Interrupt mask register, logical-OR */
  volatile unsigned int res_60;      /* 0x60 Reserved */
  volatile unsigned int output_and;  /* 0x64 I/O port output register, logical-AND */
  volatile unsigned int dir_and;     /* 0x68 I/O port direction register, logical-AND */
  volatile unsigned int imask_and;   /* 0x6C Interrupt mask register, logical-AND */
  volatile unsigned int res_70;      /* 0x70 Reserved */
  volatile unsigned int output_xor;  /* 0x74 I/O port output register, logical-XOR */
  volatile unsigned int dir_xor;     /* 0x78 I/O port direction register, logical-XOR */
  volatile unsigned int imask_xor;   /* 0x7C Interrupt mask register, logical-XOR */
};

/* L2C - Level 2 Cache Controller registers */
struct l2c_regs {
  volatile unsigned int control;                /* 0x00 Control register */
  volatile unsigned int status;                 /* 0x04 Status register */
  volatile unsigned int flush_mem_addr;         /* 0x08 Flush (Memory address) */
  volatile unsigned int flush_set_index;        /* 0x0c Flush (set, index) */
  volatile unsigned int access_counter;         /* 0x10 */
  volatile unsigned int hit_counter;            /* 0x14 */
  volatile unsigned int bus_cycle_counter;      /* 0x18 */
  volatile unsigned int bus_usage_counter;      /* 0x1c */
  volatile unsigned int error_status_control;   /* 0x20 Error status/control */
  volatile unsigned int error_addr;             /* 0x24 Error address */
  volatile unsigned int tag_check_bit;          /* 0x28 TAG-check-bit */
  volatile unsigned int data_check_bit;         /* 0x2c Data-check-bit */
  volatile unsigned int scrub_control_status;   /* 0x30 Scrub Control/Status */
  volatile unsigned int scrub_delay;            /* 0x34 Scrub Delay */
  volatile unsigned int error_injection;        /* 0x38 Error injection */
  volatile unsigned int access_control;         /* 0x3c Access control */
  volatile unsigned int reserved_40[16];        /* 0x40 Reserved */
  volatile unsigned int mtrr[32];               /* 0x80 - 0xFC MTRR registers */
  volatile unsigned int reserved_100[131008];   /* 0x100 Reserved */
  volatile unsigned int diag_iface_tag[16384];  /* 0x80000 - 0x8FFFC Diagnostic interface (Tag) */
  volatile unsigned int reserved_90000[376832]; /* 0x90000 Reserved */
  volatile unsigned int diag_iface_data[524288];/* 0x200000 - 0x3FFFFC Diagnostic interface (Data) */
};

#ifdef __cplusplus
}
#endif

#endif
