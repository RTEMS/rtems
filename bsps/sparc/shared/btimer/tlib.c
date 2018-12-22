/*
 *  Timer Library (TLIB)
 *
 *  COPYRIGHT (c) 2011.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <grlib/tlib.h>

struct tlib_dev *tlib_dev_head = NULL;
struct tlib_dev *tlib_dev_tail = NULL;
static int tlib_dev_cnt = 0;

/* Register Timer device to Timer Library */
int tlib_dev_reg(struct tlib_dev *newdev)
{
	/* Reset device */
	newdev->status = 0;
	newdev->isr_func = NULL;
	newdev->index = tlib_dev_cnt;

	/* Insert last in queue */
	newdev->next = NULL;
	if ( tlib_dev_tail == NULL ) {
		tlib_dev_head = newdev;
	} else {
		tlib_dev_tail->next = newdev;
	}
	tlib_dev_tail = newdev;

	/* Return Index of Registered Timer */
	return tlib_dev_cnt++;
}

void *tlib_open(int timer_no)
{
	struct tlib_dev *dev;

	if ( timer_no < 0 )
		return NULL;

	dev = tlib_dev_head;
	while ( (timer_no > 0) && dev ) {
		timer_no--;
		dev = dev->next;
	}
	if ( dev ) {
		if ( dev->status )
			return NULL;
		dev->status = 1;
		/* Reset Timer to initial state */
		tlib_reset(dev);
	}
	return dev;
}

void tlib_close(void *hand)
{
	struct tlib_dev *dev = hand;

	/* Stop any ongoing timer operation and unregister IRQ if registered */
	tlib_stop(dev);
	tlib_irq_unregister(dev);

	/* Mark not open */
	dev->status = 0;
}

int tlib_ntimer(void)
{
	return tlib_dev_cnt;
}
