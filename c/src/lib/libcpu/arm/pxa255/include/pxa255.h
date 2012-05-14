/*
 *  By Yang Xi <hiyangxi@gmail.com>.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __PXA_255_H__
#define __PXA_255_H__

typedef unsigned int word_t;

/*Interrupt*/

#define PRIMARY_IRQS            32
#define GPIO_IRQS               (85 - 2) /* The first two IRQs have level
                                            one interrupts */
#define GPIO_IRQ                10

#define IRQS                    (PRIMARY_IRQS + GPIO_IRQS)

/* Interrupt Controller */
#define INTERRUPT_OFFSET       0xd00000
#define XSCALE_IRQ_OS_TIMER     26
#define XSCALE_IRQ_PMU          12
#define XSCALE_IRQ_STUART       20
#define XSCALE_IRQ_NETWORK      16

#define PMU_IRQ             12
#define CCNT_IRQ_ENABLE     1UL << 6
#define PMN1_IRQ_ENABLE     1UL << 5
#define PMN0_IRQ_ENABLE     1UL << 4

#define IODEVICE_VADDR      0x40000000
#define XSCALE_INT              (IODEVICE_VADDR + INTERRUPT_OFFSET)

#define XSCALE_INT_ICMR         (*(volatile word_t *)(XSCALE_INT + 0x04))   /* Mask register */
#define XSCALE_INT_ICLR         (*(volatile word_t *)(XSCALE_INT + 0x08))   /* FIQ / IRQ selection */
#define XSCALE_INT_ICCR         (*(volatile word_t *)(XSCALE_INT + 0x14))   /* Control register */
#define XSCALE_INT_ICIP         (*(volatile word_t *)(XSCALE_INT + 0x00))   /* IRQ pending */
#define XSCALE_INT_ICFP         (*(volatile word_t *)(XSCALE_INT + 0x0c))   /* FIQ pending */
#define XSCALE_INT_ICPR         (*(volatile word_t *)(XSCALE_INT + 0x10))   /* Pending (unmasked) */

/* GPIO */
#define GPIO_OFFSET            0xe00000
#define PXA_GPIO                (IODEVICE_VADDR + GPIO_OFFSET)

#define PXA_GEDR0       (*(volatile word_t *)(PXA_GPIO + 0x48))   /* GPIO edge detect 0 */
#define PXA_GEDR1       (*(volatile word_t *)(PXA_GPIO + 0x4C))   /* GPIO edge detect 1 */
#define PXA_GEDR2       (*(volatile word_t *)(PXA_GPIO + 0x50))   /* GPIO edge detect 2 */


/* PXA2XX Timer */

#define TIMER_OFFSET           0x0a00000
#define CLOCKS_OFFSET          0x1300000
/*I change the TIMER_RATE to 36864,because when I use 3686400, the period will be calculate
  to 30000*/
#define TIMER_RATE             36864

#define XSCALE_TIMERS           (IODEVICE_VADDR + TIMER_OFFSET)

/* Match registers */
#define XSCALE_OS_TIMER_MR0     (*(volatile word_t *)(XSCALE_TIMERS + 0x00))
#define XSCALE_OS_TIMER_MR1     (*(volatile word_t *)(XSCALE_TIMERS + 0x04))
#define XSCALE_OS_TIMER_MR2     (*(volatile word_t *)(XSCALE_TIMERS + 0x08))
#define XSCALE_OS_TIMER_MR3     (*(volatile word_t *)(XSCALE_TIMERS + 0x0c))

/* Interrupt enable register */
#define XSCALE_OS_TIMER_IER     (*(volatile word_t *)(XSCALE_TIMERS + 0x1c))
/* Watchdog match enable register */
#define XSCALE_OS_TIMER_WMER    (*(volatile word_t *)(XSCALE_TIMERS + 0x18))
/* Timer count register */
#define XSCALE_OS_TIMER_TCR     (*(volatile word_t *)(XSCALE_TIMERS + 0x10))
/* Timer status register */
#define XSCALE_OS_TIMER_TSR     (*(volatile word_t *)(XSCALE_TIMERS + 0x14))

#define XSCALE_CLOCKS           (IODEVICE_VADDR + CLOCKS_VOFFSET)

#define XSCALE_CLOCKS_CCCR      (*(volatile word_t *)(XSCALE_CLOCKS + 0x00))

/*Use ffuart port as the console*/
#define FFUART_BASE   0x40100000

/*Write to SKYEYE_MAGIC_ADDRESS to make SKYEYE  exit*/

#define SKYEYE_MAGIC_ADDRESS (*(volatile word_t *)(0xb0000000))
#define SKYEYE_MAGIC_NUMBER  (0xf0f0f0f0)

/*PMC*/
#define PMC_PMNC 0
#define PMC_CCNT 1
#define PMC_INTEN 2
#define PMC_FLAG 3
#define PMC_EVTSEL 4
#define PMC_PMN0 5
#define PMC_PMN1 6
#define PMC_PMN2 7
#define PMC_PMN3 8

#define PMC_PMNC_E (0x01)
#define PMC_PMNC_PCR (0x01 << 1)
#define PMC_PMNC_CCR (0x01 << 2)
#define PMC_PMNC_CCD (0x01 << 3)
#define PMC_PMNC_PCD (0x01 << 4)

/*LCD*/
#define LCCR0  (*(volatile word_t *)(0x44000000))
#define LCCR1  (*(volatile word_t *)(0x44000004))
#define LCCR2  (*(volatile word_t *)(0x44000008))
#define LCCR3  (*(volatile word_t *)(0x4400000C))

#define FDADR0  (*(volatile word_t *)(0x44000200))
#define FSADR0  (*(volatile word_t *)(0x44000204))
#define FIDR0   (*(volatile word_t *)(0x44000208))
#define LDCMD0  (*(volatile word_t *)(0x4400020C))

#define FDADR1  (*(volatile word_t *)(0x44000210))
#define FSADR1  (*(volatile word_t *)(0x44000214))
#define FIDR1   (*(volatile word_t *)(0x44000218))
#define LDCMD1  (*(volatile word_t *)(0x4400021C))

#define LCCR0_ENB       0x00000001
#define LCCR1_PPL       0x000003FF
#define LCCR2_LPP       0x000003FF
#define LCCR3_BPP       0x07000000
#endif
