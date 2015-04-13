/*  SPWCUC - SpaceWire - CCSDS unsegmented Code Transfer Protocol GRLIB core
 *  register driver interface.
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __SPWCUC_H__
#define __SPWCUC_H__

#define PKT_INIT_IRQ       0x1
#define PKT_ERR_IRQ        0x2
#define PKT_RX_IRQ         0x4
#define WRAP_ERR_IRQ       0x8
#define WRAP_IRQ           0x10
#define SYNC_ERR_IRQ       0x20
#define SYNC_IRQ           0x40
#define TOL_ERR_IRQ        0x80
#define TICK_RX_ERR_IRQ    0x100
#define TICK_RX_WRAP_IRQ   0x200
#define TICK_RX_IRQ        0x400
#define TICK_TX_WRAP_IRQ   0x800
#define TICK_TX_IRQ        0x1000

/* SPWCUC Register layout */
struct spwcuc_regs {
	volatile unsigned int config;        /* 00 */
	volatile unsigned int status;        /* 04 */
	volatile unsigned int control;       /* 08 */
	volatile unsigned int unused0;       /* 0c */
	volatile unsigned int dla;           /* 10 */
	volatile unsigned int pid;           /* 14 */
	volatile unsigned int offset;        /* 18 */
	volatile unsigned int unused1;       /* 1c */
	volatile unsigned int pkt_ct;        /* 20 */
	volatile unsigned int pkt_ft;        /* 24 */   
	volatile unsigned int pkt_pf_crc;    /* 28 */
	volatile unsigned int unused2;       /* 2c */
	volatile unsigned int etct;          /* 30 */
	volatile unsigned int etft;          /* 34 */
	volatile unsigned int etct_next;     /* 38 */
	volatile unsigned int etft_next;     /* 3c */
	volatile unsigned int unused3[8];    /* 40-5c */
	volatile unsigned int pimsr;         /* 60 */
	volatile unsigned int pimr;          /* 64 */
	volatile unsigned int pisr;          /* 68 */
	volatile unsigned int pir;           /* 6c */
	volatile unsigned int imr;           /* 70 */
	volatile unsigned int picr;          /* 74 */
};

struct spwcuc_cfg {
	unsigned char sel_out;         /* Bits 3-0 enable time code transmission on respective output */
	unsigned char sel_in;          /* Select SpW to receive time codes on, 0-3 */
	unsigned char mapping;         /* Define mapping of time code time info into T-field, 0-31 */
	unsigned char tolerance;       /* Define SpaceWire time code reception tolerance, 0-31 */
	unsigned char tid;             /* Define CUC P-Field time code identification, 1 = Level 1, 2 = Level 2 */
	unsigned char ctf;             /* If 1 check time code flags to be all zero */
	unsigned char cp;              /* If 1 check P-Field time code id against tid */

	unsigned char txen;            /* Enable SpaceWire time code transmission */
	unsigned char rxen;            /* Enable SpaceWire time code reception */
	unsigned char pktsyncen;       /* Enable SpaceWire time CUC packet sync */
	unsigned char pktiniten;       /* Enable SpaceWire time CUC packet init */
	unsigned char pktrxen;         /* Enable SpaceWire time CUC packet reception */

	unsigned char dla;             /* SpaceWire destination logical address */ 
	unsigned char dla_mask;        /* SpaceWire destination logical address mask */ 
	unsigned char pid;             /* SpaceWire protocol ID */
	
	unsigned int offset;           /* Packet reception offset */
};

/* SPWCUC Statistics gathered by driver */
struct spwcuc_stats {

	/* IRQ Stats */
	unsigned int nirqs;
	unsigned int tick_tx;
	unsigned int tick_tx_wrap;
	unsigned int tick_rx;
	unsigned int tick_rx_wrap;
	unsigned int tick_rx_error;
	unsigned int tolerr;
	unsigned int sync;
	unsigned int syncerr;
	unsigned int wrap;
	unsigned int wraperr;
	unsigned int pkt_rx;
	unsigned int pkt_err;
	unsigned int pkt_init;
};

/* Function ISR callback prototype
 *
 * pimr    - PIMR/PIR register of the SPWCUC core read by ISR
 * data    - Custom data provided by user
 */
typedef void (*spwcuc_isr_t)(unsigned int pimr, void *data);

/* Open a SPWCUC device by minor number. A SPWCUC device can only by opened
 * once. The handle returned must be used as the input parameter 'spwcuc' in 
 * the rest of the calls in the function interface.
 */
extern void *spwcuc_open(int minor);

/* Close a previously opened SPWCUC device */
extern void spwcuc_close(void *spwcuc);

/* Reset SPWCUC Core */
extern int spwcuc_reset(void *spwcuc);

/* Enable Interrupts at Interrupt controller */
extern void spwcuc_int_enable(void *spwcuc);

/* Disable Interrupts at Interrupt controller */
extern void spwcuc_int_disable(void *spwcuc);

/* Clear Statistics gathered by the driver */
extern void spwcuc_clr_stats(void *spwcuc);

/* Get Statistics gathered by the driver. The statistics are stored into
 * the location pointed to by 'stats'.
 */
extern void spwcuc_get_stats(void *spwcuc, struct spwcuc_stats *stats);

/* Register an Interrupt handler and custom data, the function call is
 * removed by setting func to NULL.
 *
 * The driver's interrupt handler is installed on open(), however the user
 * callback called from the driver's ISR is installed using this function.
 */
extern void spwcuc_int_register(void *spwcuc, spwcuc_isr_t func, void *data);

/* Configure the spwcuc core. The configuration is taken from the data
 * structure pointed to by 'cfg'. See data structure spwcuc_cfg fields.
 */
extern void spwcuc_config(void *spwcuc, struct spwcuc_cfg *cfg);

/* Return elapsed coarse time */
extern unsigned int spwcuc_get_et_coarse(void *spwcuc);

/* Return elapsed fine time */
extern unsigned int spwcuc_get_et_fine(void *spwcuc);

/* Return elapsed time (coarse and fine) 64-bit value */
extern unsigned long long spwcuc_get_et(void *spwcuc);

/* Return next elapsed coarse time (for use when sending SpW time packet) */
extern unsigned int spwcuc_get_next_et_coarse(void *spwcuc);

/* Return next elapsed fine time (for use when sending SpW time packet) */
extern unsigned int spwcuc_get_next_et_fine(void *spwcuc);

/* Return next elapsed time (for use when sending SpW time packet) */
extern unsigned long long spwcuc_get_next_et(void *spwcuc);

/* Force/Set the elapsed time (coarse 32-bit and fine 24-bit) by writing the
 * T-Field Time Packet Registers then the FORCE bit.
 */
extern void spwcuc_force_et(void *spwcuc, unsigned long long time);

/* Return received (from time packet) elapsed coarse time */
extern unsigned int spwcuc_get_tp_et_coarse(void *spwcuc);

/* Return received (from time packet) elapsed fine time */
extern unsigned int spwcuc_get_tp_et_fine(void *spwcuc);

/* Return received (from time packet) elapsed time (coarse and fine) */
extern unsigned long long spwcuc_get_tp_et(void *spwcuc);

/* Clear interrupts */
extern void spwcuc_clear_irqs(void *spwcuc, int irqs);

/* Enable interrupts */
extern void spwcuc_enable_irqs(void *spwcuc, int irqs);

/* Get Register */
extern struct spwcuc_regs *spwcuc_get_regs(void *spwcuc);

/* Register the SPWCUC Driver to the Driver Manager */
extern void spwcuc_register(void);

#endif
