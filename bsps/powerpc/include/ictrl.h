#ifndef _ICTRL_H
#define _ICTRL_H

/*
 * mpc505/509 external interrupt controller management.
 *
 * FIXME: should be somehow merged into general RTEMS interrupt
 * management code.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define _SIU_IRQENABLE  ((unsigned long volatile *const)0x8007EFA8)
#define _SIU_IRQPEND    ((unsigned long volatile *const)0x8007EFA0)

/* Interrupt masks. */
enum {
  IMASK_EXT0  = 0x80000000,
  IMASK_EXT1  = 0x20000000,
  IMASK_EXT2  = 0x08000000,
  IMASK_EXT3  = 0x02000000,
  IMASK_EXT4  = 0x00800000,
  IMASK_EXT5  = 0x00200000,
  IMASK_EXT6  = 0x00080000,
  IMASK_ALL   = IMASK_EXT0 | IMASK_EXT1 | IMASK_EXT2 | IMASK_EXT3 |
                IMASK_EXT4 | IMASK_EXT5 | IMASK_EXT6
};

/* Interrupt numbers. */
typedef enum {
  IRQ_EXT0,
  IRQ_EXT1,
  IRQ_EXT2,
  IRQ_EXT3,
  IRQ_EXT4,
  IRQ_EXT5,
  IRQ_EXT6,
  NUM_IRQS
} ExtInt;

/* Type of external interrupt handlers */
typedef void (*ExtIsrHandler) (void);

/* Initialization. Must be called once after RTEMS interrupts sybsystem
   is initiailized. 'predriver_hook' is one of such places. */
extern void extIsrInit( void );

/* Set interrupt handler 'handler' for external interrupt number
   'interrupt'. */
extern void extIrqSetHandler(ExtInt interrupt, ExtIsrHandler handler);

/* Check is external interrupt 'irq' (IMASK_XXXX) is pended. */
#define extIrqIsSet(irq) \
  (*_SIU_IRQPEND & (irq))

/* Enable external interrupt 'irq' (IMASK_XXXX) processing. */
#define extIrqEnable(irq) \
  (*_SIU_IRQENABLE |= (irq))

/* Disable external interrupt 'irq' (IMASK_XXXX) processing. */
#define extIrqDisable(irq) \
  (*_SIU_IRQENABLE &= ~(irq))

/* Check if external interrupt 'irq' (IMASK_XXXX) processing is
   enabled. */
#define extIrqGetEnable \
  (*_SIU_IRQENABLE)

#ifdef __cplusplus
}
#endif

#endif /* _ICTRL_H */
