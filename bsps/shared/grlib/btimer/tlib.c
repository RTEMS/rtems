/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Timer Library (TLIB)
 *
 *  COPYRIGHT (c) 2011.
 *  Cobham Gaisler AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
