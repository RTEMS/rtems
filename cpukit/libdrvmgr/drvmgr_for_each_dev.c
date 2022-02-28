/* SPDX-License-Identifier: BSD-2-Clause */

/* Iterate over device tree topology, breadth or depth-first
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

#include <string.h>
#include <drvmgr/drvmgr.h>
#include <drvmgr/drvmgr_list.h>
#include "drvmgr_internal.h"

/* Traverse device tree breadth-first. Supports up to 31 buses */
static intptr_t drvmgr_for_each_dev_breadth(
	intptr_t (*func)(struct drvmgr_dev *dev, void *arg),
	void *arg
	)
{
	int ret = 0, i, pos;
	struct drvmgr_bus *bus, *buses[32];
	struct drvmgr_dev *dev;

	pos = 0;
	memset(&buses[0], 0, sizeof(buses));
	buses[pos++] = drvmgr.root_dev.bus; 	/* Get root bus */

	for (i = 0, bus = buses[0]; buses[i]; i++, bus = buses[i]) {
		dev = bus->children;
		while (dev) {
			ret = func(dev, arg);
			if (ret != 0)
				break;
			if (dev->bus && pos < 31)
				buses[pos++] = dev->bus;

			dev = dev->next_in_bus;
		}
	}

	return ret;
}

/* Traverse device tree depth-first. */
static intptr_t drvmgr_for_each_dev_depth(
	intptr_t (*func)(struct drvmgr_dev *dev, void *arg),
	void *arg
	)
{
	int ret = 0;
	struct drvmgr_dev *dev;

	/* Get first device */
	dev = drvmgr.root_dev.bus->children;

	while (dev) {
		ret = func(dev, arg);
		if (ret != 0)
			break;
		if (dev->bus && dev->bus->children) {
			dev = dev->bus->children;
		} else {
next_dev:
			if (dev->next_in_bus == NULL) {
				/* Step up one level... back to parent bus */
				dev = dev->parent->dev;
				if (dev == &drvmgr.root_dev)
					break;
				goto next_dev;
			} else {
				dev = dev->next_in_bus;
			}
		}
	}

	return ret;
}

/* Traverse device tree depth-first or breadth-first */
intptr_t drvmgr_for_each_dev(
	intptr_t (*func)(struct drvmgr_dev *dev, void *arg),
	void *arg,
	int options
	)
{
	intptr_t ret;

	DRVMGR_LOCK_READ();

	/* Get Root Device */
	if (drvmgr.root_dev.bus->children != NULL) {
		if (options & DRVMGR_FED_BF)
			ret = drvmgr_for_each_dev_breadth(func, arg);
		else
			ret = drvmgr_for_each_dev_depth(func, arg);
	} else
		ret = 0;

	DRVMGR_UNLOCK();

	return ret;
}
