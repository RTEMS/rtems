/*
 * Copyright 2016 Chris Johns <chrisj@rtems.org>
 *
 * Header for the FreeBSD ported elcr.c
 */

#ifndef _IRQ_ELCR_H_
#define _IRQ_ELCR_H_

#include <sys/types.h>

enum intr_trigger {
  INTR_TRIGGER_EDGE,
  INTR_TRIGGER_LEVEL
};

/*
 * Check to see if we have what looks like a valid ELCR.  We do this by
 * verifying that IRQs 0, 1, 2, and 13 are all edge triggered.
 */
int elcr_probe(void);

/*
 * Returns 1 for level trigger, 0 for edge.
 */
enum intr_trigger elcr_read_trigger(u_int irq);

/*
 * Set the trigger mode for a specified IRQ.  Mode of 0 means edge triggered,
 * and a mode of 1 means level triggered.
 */
void elcr_write_trigger(u_int irq, enum intr_trigger trigger);

void elcr_resume(void);


#endif
