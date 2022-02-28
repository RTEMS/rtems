/* SPDX-License-Identifier: BSD-2-Clause */

/* Iterate over one list of devices used internally by driver manager
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
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

#include <drvmgr/drvmgr.h>
#include <drvmgr/drvmgr_list.h>
#include "drvmgr_internal.h"

int drvmgr_for_each_listdev(
	struct drvmgr_list *devlist,
	unsigned int state_set_mask,
	unsigned int state_clr_mask,
	int (*func)(struct drvmgr_dev *dev, void *arg),
	void *arg
	)
{
	struct drvmgr_dev *dev;
	int ret = 0;

	DRVMGR_LOCK_READ();

	/* Get First Device */
	dev = DEV_LIST_HEAD(devlist);
	while (dev) {
		if (((state_set_mask != 0) && ((dev->state & state_set_mask) == state_set_mask)) ||
		    ((state_clr_mask != 0) && ((dev->state & state_clr_mask) == 0)) ||
		    ((state_set_mask == 0) && (state_clr_mask == 0))) {
			ret = func(dev, arg);
			if (ret != 0)
				break;
		}
		dev = dev->next;
	}

	DRVMGR_UNLOCK();

	return ret;
}
