/*  GRCTM - CCSDS Time Manager - register driver interface.
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRCTM_H__
#define __GRCTM_H__

#define DAT0_IRQ    0x1
#define DAT1_IRQ    0x2
#define DAT2_IRQ    0x4
#define PULSE0_IRQ  0x10
#define PULSE1_IRQ  0x20
#define PULSE2_IRQ  0x40
#define PULSE3_IRQ  0x80
#define PULSE4_IRQ  0x100
#define PULSE5_IRQ  0x200
#define PULSE6_IRQ  0x400
#define PULSE7_IRQ  0x800

struct grctm_regs {
	volatile unsigned int grr;
	volatile unsigned int gcr;
	volatile unsigned int gsr;
	volatile unsigned int unused[2];
	volatile unsigned int pfr;
	volatile unsigned int etcr;
	volatile unsigned int etfr;
	volatile unsigned int dcr0;
	volatile unsigned int dfr0;
	volatile unsigned int dcr1;
	volatile unsigned int dfr1;
	volatile unsigned int dcr2;
	volatile unsigned int dfr2;
	volatile unsigned int stcr;
	volatile unsigned int stfr;
	volatile unsigned int pdr[8];
	volatile unsigned int pimsr;
	volatile unsigned int pimr;
	volatile unsigned int pisr;
	volatile unsigned int pir;
	volatile unsigned int imr;
	volatile unsigned int picr;
	volatile unsigned int unused1[2];
	volatile unsigned int etir;
	volatile unsigned int fsir;
	volatile unsigned int serconf;
	volatile unsigned int unused2;
	volatile unsigned int twsc;
	volatile unsigned int twadj;
	volatile unsigned int twtx;
	volatile unsigned int twrx;
};

struct grctm_stats {

	/* IRQ Stats */
	unsigned int nirqs;
	unsigned int pulse;
};

/* Function ISR callback prototype */
typedef void (*grctm_isr_t)(unsigned int pimr, void *data);

/* Open a GRCTM device by minor number. */
extern void *grctm_open(int minor);

/* Close a previously opened GRCTM device */
extern void grctm_close(void *spwcuc);

/* Hardware Reset of GRCTM */
extern int grctm_reset(void *grctm);

/* Enable Interrupts at Interrupt controller */
extern void grctm_int_enable(void *grctm);

/* Disable Interrupts at Interrupt controller */
extern void grctm_int_disable(void *grctm);

/* Clear Statistics gathered by the driver */
extern void grctm_clr_stats(void *grctm);

/* Get Statistics gathered by the driver */
extern void grctm_get_stats(void *grctm, struct grctm_stats *stats);

/* Register an Interrupt handler and custom data, the function call is
 * removed by setting func to NULL.
 */
extern void grctm_int_register(void *grctm, grctm_isr_t func, void *data);

/* Enable external synchronisation (from spwcuc) */
extern void grctm_enable_ext_sync(void *grctm);

/* Disable external synchronisation (from spwcuc) */
extern void grctm_disable_ext_sync(void *grctm);

/* Enable TimeWire synchronisation */
extern void grctm_enable_tw_sync(void *grctm);

/* Disable TimeWire synchronisation */
extern void grctm_disable_tw_sync(void *grctm);

/* Disable frequency synthesizer from driving ET */
extern void grctm_disable_fs(void *grctm);

/* Enable frequency synthesizer to drive ET */
extern void grctm_enable_fs(void *grctm);

/* Return elapsed coarse time */
extern unsigned int grctm_get_et_coarse(void *grctm);

/* Return elapsed fine time */
extern unsigned int grctm_get_et_fine(void *grctm);

/* Return elapsed time (coarse and fine) */
extern unsigned long long grctm_get_et(void *grctm);

/* Return 1 if specified datation has been latched */
extern int grctm_is_dat_latched(void *grctm, int dat);

/* Set triggering edge of datation input */
extern void grctm_set_dat_edge(void *grctm, int dat, int edge);

/* Return latched datation coarse time */
extern unsigned int grctm_get_dat_coarse(void *grctm, int dat);

/* Return latched datation fine time */
extern unsigned int grctm_get_dat_fine(void *grctm, int dat);

/* Return latched datation ET */
extern unsigned long long grctm_get_dat_et(void *grctm, int dat);

/* Return current pulse configuration */
extern unsigned int grctm_get_pulse_reg(void *grctm, int pulse);

/* Set pulse register */
extern void grctm_set_pulse_reg(void *grctm, int pulse, unsigned int val);

/* Configure pulse: pp = period, pw = width, pl = level, en = enable */
extern void grctm_cfg_pulse(void *grctm, int pulse, int pp, int pw, int pl, int en);

/* Enable pulse output */
extern void grctm_enable_pulse(void *grctm, int pulse);

/* Disable pulse output */
extern void grctm_disable_pulse(void *grctm, int pulse);

/* Clear interrupts */
extern void grctm_clear_irqs(void *grctm, int irqs);

/* Enable interrupts */
extern void grctm_enable_irqs(void *grctm, int irqs);

/* Set Frequency synthesizer increment */
void grctm_set_fs_incr(void *grctm, int incr);

/* Set ET increment */
void grctm_set_et_incr(void *grctm, int incr);

/* Get register base address */
struct grctm_regs *grctm_get_regs(void *grctm);

/* Register the GRCTM driver to Driver Manager */
extern void grctm_register(void);

#endif
