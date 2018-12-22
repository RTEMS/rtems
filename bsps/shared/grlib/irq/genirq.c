/*
 * Generic interrupt helpers mainly for GRLIB PCI peripherals
 *
 * COPYRIGHT (c) 2008.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/bspIo.h>
#include <stdlib.h>
#include <string.h>
#include <grlib/genirq.h>

#include <grlib/grlib_impl.h>

struct genirq_handler_entry {
	struct genirq_handler_entry	*next;		/* Next ISR entry for this IRQ number */
	genirq_handler			isr;		/* ISR function called upon IRQ */
	void				*arg;		/* custom argument to ISR */
	int				enabled;	/* Inidicates if IRQ is enabled */
};

struct genirq_irq_entry {
	struct genirq_handler_entry	*head;
	struct genirq_stats		stats;
};

struct genirq_priv {
	/* Maximum number of interrupt */
	int				genirq_max;
	/* IRQ Table index N reflect IRQ number N */
	struct genirq_irq_entry		genirq_table[0]; /* Length depends on */
};

genirq_t genirq_init(int number_of_irqs)
{
	size_t size;
	struct genirq_priv *priv;

	size = sizeof(*priv) +
	       number_of_irqs * sizeof(priv->genirq_table[0]);

	priv = grlib_calloc(1, size);
	if ( !priv )
		return NULL;
	priv->genirq_max = number_of_irqs - 1;

	return priv;
}

void genirq_destroy(genirq_t d)
{
	struct genirq_priv *priv = d;
	struct genirq_irq_entry *irqentry;
	struct genirq_handler_entry *isrentry, *tmp;
	int i;

	/* Free all registered interrupts */
	for ( i=0; i<priv->genirq_max; i++) {
		irqentry = &priv->genirq_table[i];
		isrentry = irqentry->head;
		while ( isrentry ) {
			tmp = isrentry;
			isrentry = isrentry->next;
			genirq_free_handler(tmp);
		}
	}

	free(priv);
}

int genirq_check(genirq_t d, int irq)
{
	struct genirq_priv *priv = d;

	if ( (irq <= 0) || (irq > priv->genirq_max) )
		return -1;
	else
		return 0;
}

void *genirq_alloc_handler(genirq_handler isr, void *arg)
{
	struct genirq_handler_entry *newentry;

	newentry = grlib_malloc(sizeof(*newentry));
	if ( newentry ) {
		/* Initialize ISR entry */
		newentry->isr     = isr;
		newentry->arg     = arg;
		newentry->enabled = 0;
	}
	return newentry;
}

int genirq_register(genirq_t d, int irq, void *handler)
{
	struct genirq_priv *priv = d;
	struct genirq_irq_entry *irqentry;
	struct genirq_handler_entry *isrentry, *newentry = handler;

	if ( genirq_check(d, irq) )
		return -1;

	/* Insert new ISR entry first into table */
	irqentry = &priv->genirq_table[irq];
	isrentry = irqentry->head;
	irqentry->head = newentry;
	newentry->next = isrentry;

	if ( isrentry )
		return 1; /* This is the first handler on this IRQ */
	return 0;
}

void *genirq_unregister(genirq_t d, int irq, genirq_handler isr, void *arg)
{
	struct genirq_priv *priv = d;
	struct genirq_irq_entry *irqentry;
	struct genirq_handler_entry *isrentry, **prev;
	void *ret;

	if ( genirq_check(d, irq) )
		return NULL;

	/* Remove isr[arg] from ISR list */
	irqentry = &priv->genirq_table[irq];
	ret = NULL;

	prev = &irqentry->head;
	isrentry = irqentry->head;
	while ( isrentry ) {
		if ( (isrentry->arg == arg) && (isrentry->isr == isr) ) {
			/* Found ISR, remove it from list */
			if ( isrentry->enabled ) {
				/* Can not remove enabled ISRs, disable first */
				ret = NULL;
				break;
			}
			*prev = isrentry->next;
			ret = isrentry;
			break;
		}
		prev = &isrentry->next;
		isrentry = isrentry->next;
	}

	return ret;
}

/* Enables or Disables ISR handler. Internal function to reduce footprint
 * of enable/disable functions.
 *
 * \param action 1=enable, 0=disable ISR
 */
static int genirq_set_active(
	struct genirq_priv *priv,
	int irq,
	genirq_handler isr,
	void *arg,
	int action)
{
	struct genirq_irq_entry *irqentry;
	struct genirq_handler_entry *isrentry, *e = NULL;
	int enabled;

	if ( genirq_check(priv, irq) )
		return -1;

	/* Find isr[arg] in ISR list */
	irqentry = &priv->genirq_table[irq];
	enabled = 0;

	isrentry = irqentry->head;
	while ( isrentry ) {
		if ( (isrentry->arg == arg) && (isrentry->isr == isr) ) {
			/* Found ISR */
			if ( isrentry->enabled == action ) {
				/* The ISR is already enabled or disabled 
				 * depending on request, neccessary actions
				 * were taken last time the same action was
				 * requested.
				 */
				return 1;
			}
			e = isrentry;
		} else {
			enabled += isrentry->enabled;
		}
		isrentry = isrentry->next;
	}

	if ( !e )
		return -1;

	e->enabled = action;

	return enabled;
}

int genirq_enable(genirq_t d, int irq, genirq_handler isr, void *arg)
{
	struct genirq_priv *priv = d;
	return genirq_set_active(priv, irq, isr, arg, 1);
}

int genirq_disable(genirq_t d, int irq, genirq_handler isr, void *arg)
{
	struct genirq_priv *priv = d;
	return genirq_set_active(priv, irq, isr, arg, 0);
}

void genirq_doirq(genirq_t d, int irq)
{
	struct genirq_priv *priv = d;
	struct genirq_irq_entry *irqentry;
	struct genirq_handler_entry *isrentry;
	int enabled;

	irqentry = &priv->genirq_table[irq];
	irqentry->stats.irq_cnt++;

	enabled = 0;

	isrentry = irqentry->head;
	while ( isrentry ) {
		if ( isrentry->enabled ) {
			enabled = 1;
			/* Call the ISR */
			isrentry->isr(isrentry->arg);
		}
		isrentry = isrentry->next;
	}

	/* Was the IRQ an IRQ without source? */
	if ( enabled == 0 ) {
		/* This should not happen */
		printk("Spurious IRQ happened on IRQ %d\n", irq);
	}
}
