/* General Shared Interrupt handling function interface 
 *
 * The functions does not manipulate the IRQ controller or the 
 * interrupt level of the CPU. It simply helps the caller with
 * managing shared interrupts where multiple interrupt routines
 * share on interrupt vector/number.
 *
 * COPYRIGHT (c) 2008.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __GENIRQ_H__
#define __GENIRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*genirq_handler)(void *arg);
typedef void* genirq_t;

struct genirq_stats {
	unsigned int	irq_cnt;
};

/* Initialize the genirq interface. Must be the first function
 * called.
 *
 * Returns zero on success, otherwise failure.
 */
extern genirq_t genirq_init(int number_of_irqs);

/* Free the dynamically allocated memory that the genirq interface has 
 * allocated.
 *
 * Returns zero on success, otherwise failure.
 */
extern void genirq_destroy(genirq_t d);

/* Check IRQ number validity 
 * 
 * Returns zero for valid IRQ numbers, -1 of invalid IRQ numbers.
 */
extern int genirq_check(genirq_t d, int irq);

/* Register shared interrupt handler.
 *
 * \param irq    The interrupt number to register ISR on
 * \param isr    The interrupt service routine called upon IRQ
 * \param arg    The argument given to isr() when called.
 *
 * Return Values
 * -1  = Failed
 * 0   = Handler registered Successfully, first handler on this IRQ
 * 1   = Handler registered Successfully, _not_ first handler on this IRQ
 */
extern int genirq_register(genirq_t d, int irq, genirq_handler isr, void *arg);

/* Unregister an previous registered interrupt handler 
 *
 * Return Values
 *  -1 = ISR not registered before
 *  0  = ISR unregistered
 *  1  = Unable to unregister enabled ISR
 */
extern int genirq_unregister(genirq_t d, int irq, genirq_handler isr, void *arg);

/* Enables IRQ only for this isr[arg] combination. Records if this 
 * is the first interrupt enable, only then must interrupts be enabled
 * on the interrupt controller.
 *
 * IRQs must be disabled before entering this function.
 *
 * Return values
 *  -1 = Failure, for example isr[arg] not registered on this irq
 *  0  = IRQ must be enabled, it is the first IRQ handler to be enabled
 *  1  = IRQ has already been enabled, either by isr[arg] or by another handler
 */
extern int genirq_enable(genirq_t d, int irq, genirq_handler isr, void *arg);

/* Disables IRQ only for this isr[arg] combination. Records if this 
 * is the only interrupt handler that is enabled on this IRQ, only then
 * must interrupts be disabled on the interrupt controller.
 *
 * IRQs must be disabled before entering this function.
 *
 * Return values
 *  -1 = Failure, for example isr[arg] not registered on this irq
 *  0  = IRQ must be disabled, no ISR are enabled for this IRQ
 *  1  = ISR has already been disabled, or other ISRs are still enabled
 */
extern int genirq_disable(genirq_t d, int irq, genirq_handler isr, void *arg);

/* Must be called by user when an IRQ has fired, the argument 'irq' 
 * is the IRQ number of the IRQ which was fired.
 */
extern void genirq_doirq(genirq_t d, int irq);

#ifdef __cplusplus
}
#endif

#endif
