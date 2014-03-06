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
 *  http://www.rtems.com/license/LICENSE.
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
  /* 0x210, align to 4Kb boundary */
  volatile unsigned int resv1[(0x1000-0x210)/4];
};

/* GPTIMER Timer instance */
struct gptimer_timer_regs {
  volatile unsigned int value;
  volatile unsigned int reload;
  volatile unsigned int ctrl;
  volatile unsigned int notused;
};

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
};

/* L2C - Level 2 Cache Controller registers */
struct l2c_regs {
  volatile unsigned int control;
  volatile unsigned int status;
  volatile unsigned int flush_mem_addr;
  volatile unsigned int flush_set_index;
  volatile unsigned int access_counter;
  volatile unsigned int hit_counter;
  volatile unsigned int bus_cycle_counter;
  volatile unsigned int bus_usage_counter;
  volatile unsigned int error_status_control;
  volatile unsigned int error_addr;
  volatile unsigned int tag_check_bit;
  volatile unsigned int data_check_bit;
  volatile unsigned int scrub_control_status;
  volatile unsigned int scrub_delay;
  volatile unsigned int error_injection;
  volatile unsigned int reserved_3c[17];
  volatile unsigned int mtrr;
  volatile unsigned int reserved_84[131039];
  volatile unsigned int diag_iface_tag[16384];
  volatile unsigned int reserved_90000[376832];
  volatile unsigned int diag_iface_data[524288];
};

#ifdef __cplusplus
}
#endif

#endif
