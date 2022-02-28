/* SPDX-License-Identifier: BSD-2-Clause */

/* Driver Manager Driver Interface Implementation.
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
 *
 */

/*
 * This is the part the device driver API, the functions rely on that the
 * parent bus driver has implemented the neccessary operations correctly.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drvmgr/drvmgr.h>
#include "drvmgr_internal.h"

/* Get device pointer from knowing the Driver and the Driver minor
 * that was assigned to it
 */
int drvmgr_get_dev(
	struct drvmgr_drv *drv,
	int minor,
	struct drvmgr_dev **pdev)
{
	struct drvmgr_dev *dev;
	if (!drv)
		return -1;

	DRVMGR_LOCK_READ();
	dev = drv->dev;
	while (dev) {
		if (dev->minor_drv == minor)
			break;
		dev = dev->next_in_drv;
	}
	DRVMGR_UNLOCK();
	if (!dev)
		return -1;
	if (pdev)
		*pdev = dev;
	return 0;
}

/* Get Bus frequency in HZ from bus driver */
int drvmgr_freq_get(
	struct drvmgr_dev *dev,
	int options,
	unsigned int *freq_hz)
{
	if (!dev || !dev->parent || !dev->parent->ops->get_freq)
		return -1;

	return dev->parent->ops->get_freq(dev, options, freq_hz);
}

/* Get driver prefix */
int drvmgr_get_dev_prefix(struct drvmgr_dev *dev, char *dev_prefix)
{
	struct drvmgr_bus_params params;
	if (!dev || !dev->parent || !dev->parent->ops->get_params)
		return -1;

	dev->parent->ops->get_params(dev, &params);
	if (!params.dev_prefix)
		return -1;
	if (dev_prefix)
		strcpy(dev_prefix, params.dev_prefix);
	return 0;
}

/* Register an interrupt */
int drvmgr_interrupt_register(
	struct drvmgr_dev *dev,
	int index,
	const char *info,
	drvmgr_isr isr,
	void *arg)
{
	if (!dev || !dev->parent || !dev->parent->ops->int_register)
		return -1;

	if (!isr)
		return -1;

	return dev->parent->ops->int_register(dev, index, info, isr, arg);
}

/* Unregister an interrupt */
int drvmgr_interrupt_unregister(
	struct drvmgr_dev *dev,
	int index,
	drvmgr_isr isr,
	void *arg)
{
	if (!dev || !dev->parent || !dev->parent->ops->int_unregister)
		return -1;

	if (!isr)
		return -1;

	return dev->parent->ops->int_unregister(dev, index, isr, arg);
}

int drvmgr_interrupt_clear(
	struct drvmgr_dev *dev,
	int index)
{
	if (!dev || !dev->parent || !dev->parent->ops->int_clear)
		return -1;

	return dev->parent->ops->int_clear(dev, index);
}

int drvmgr_interrupt_unmask(
	struct drvmgr_dev *dev,
	int index)
{
	if (!dev || !dev->parent || !dev->parent->ops->int_unmask)
		return -1;

	return dev->parent->ops->int_unmask(dev, index);
}

int drvmgr_interrupt_mask(
	struct drvmgr_dev *dev,
	int index)
{
	if (!dev || !dev->parent || !dev->parent->ops->int_mask)
		return -1;

	return dev->parent->ops->int_mask(dev, index);
}

#ifdef RTEMS_SMP
int drvmgr_interrupt_set_affinity(
	struct drvmgr_dev *dev,
	int index,
	const Processor_mask *cpus)
{
	if (!dev || !dev->parent || !dev->parent->ops->int_set_affinity)
		return -1;

	return dev->parent->ops->int_set_affinity(dev, index, cpus);
}
#endif

int drvmgr_on_rootbus(struct drvmgr_dev *dev)
{
	if (dev->parent && dev->parent->dev && dev->parent->dev->parent)
		return 0;
	else
		return 1;
}
