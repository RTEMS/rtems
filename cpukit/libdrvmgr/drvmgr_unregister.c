/* Driver Manager Device Unregister (removal) implementation
 *
 * COPYRIGHT (c) 2011 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>

#include <drvmgr/drvmgr.h>
#include <drvmgr/drvmgr_list.h>
#include "drvmgr_internal.h"

/* Unregister all children on a bus.
 *
 * This function is called from the bus driver, from a "safe" state where
 * devices will not be added or removed on this particular bus at this time
 */
int drvmgr_children_unregister(struct drvmgr_bus *bus)
{
	int err;

	while (bus->children != NULL) {
		err = drvmgr_dev_unregister(bus->children);
		if (err != DRVMGR_OK) {
			/* An error occured */
			bus->children->error = err;
			return err;
		}
	}

	return DRVMGR_OK;
}

/* Unregister a BUS and all it's devices.
 *
 * It is up to the bus driver to remove all it's devices, either manually
 * one by one calling drvmgr_dev_unregister(), or by letting the driver
 * manager unregister all children by calling drvmgr_children_unregister().
 */
int drvmgr_bus_unregister(struct drvmgr_bus *bus)
{
	struct drvmgr *mgr = &drvmgr;
	struct drvmgr_list *list;

	if (bus->ops->remove == NULL)
		return DRVMGR_ENOSYS;

	/* Call Bus driver to clean things up, it must remove all children */
	bus->error = bus->ops->remove(bus);
	if (bus->error != DRVMGR_OK)
		return bus->error;
	/* Check that bus driver has done its job and removed all children */
	if (bus->children != NULL)
		return DRVMGR_FAIL;
	/* Remove References to bus */
	bus->dev->bus = NULL;

	DRVMGR_LOCK_WRITE();

	/* Remove bus from bus-list */
	if (bus->state & BUS_STATE_LIST_INACTIVE)
		list = &mgr->buses_inactive;
	else
		list = &mgr->buses[bus->level];
	drvmgr_list_remove(list, bus);

	DRVMGR_UNLOCK();

	/* All references to this bus has been removed at this point */
	free(bus);

	return DRVMGR_OK;
}

/* Separate Driver and Device from each other */
int drvmgr_dev_drv_separate(struct drvmgr_dev *dev)
{
	struct drvmgr *mgr = &drvmgr;
	struct drvmgr_dev *subdev, **pprev;
	int rc;

	/* Remove children if this device exports a bus of devices. All
	 * children must be removed first as they depend upon the bus
	 * services this bridge provide.
	 */
	if (dev->bus) {
		rc = drvmgr_bus_unregister(dev->bus);
		if (rc != DRVMGR_OK)
			return rc;
	}

	if (dev->drv == NULL)
		return DRVMGR_OK;

	/* Remove device by letting assigned driver take care of hardware
	 * issues
	 */
	if (!dev->drv->ops->remove) {
		/* No remove function is considered severe when someone
		 * is trying to remove the device
		 */
		return DRVMGR_ENOSYS;
	}
	dev->error = dev->drv->ops->remove(dev);
	if (dev->error != DRVMGR_OK)
		return DRVMGR_FAIL;

	DRVMGR_LOCK_WRITE();

	/* Delete device from driver's device list */
	pprev = &dev->drv->dev;
	subdev = dev->drv->dev;
	while (subdev != dev) {
		pprev = &subdev->next_in_drv;
		subdev = subdev->next_in_drv;
	}
	*pprev = subdev->next_in_drv;
	dev->drv->dev_cnt--;

	/* Move device to inactive list */
	drvmgr_list_remove(&mgr->devices[dev->level], dev);
	dev->level = 0;
	dev->state &= ~(DEV_STATE_UNITED|DEV_STATE_INIT_DONE);
	dev->state |= DEV_STATE_LIST_INACTIVE;
	drvmgr_list_add_tail(&mgr->devices_inactive, dev);

	DRVMGR_UNLOCK();

	/* Free Device Driver Private memory if allocated previously by
	 * Driver manager.
	 */
	if (dev->drv->dev_priv_size && dev->priv) {
		free(dev->priv);
		dev->priv = NULL;
	}
	dev->drv = NULL;

	return DRVMGR_OK;
}

/* Unregister device,
 *  - let assigned driver handle deletion
 *  - remove from device list
 *  - remove from driver list
 *  - remove from bus list
 */
int drvmgr_dev_unregister(struct drvmgr_dev *dev)
{
	struct drvmgr *mgr = &drvmgr;
	struct drvmgr_dev *subdev, **pprev;
	int err;

	/* Separate device from driver, if the device is united with a driver.
	 *
	 * If this device is a bridge all child buses/devices are also removed.
	 */
	err = drvmgr_dev_drv_separate(dev);
	if (err != DRVMGR_OK)
		return err;

	DRVMGR_LOCK_WRITE();

	/* Remove it from inactive list */
	drvmgr_list_remove(&mgr->devices_inactive, dev);

	/* Remove device from parent bus list (no check if dev not in list) */
	pprev = &dev->parent->children;
	subdev = dev->parent->children;
	while (subdev != dev) {
		pprev = &subdev->next_in_bus;
		subdev = subdev->next_in_bus;
	}
	*pprev = subdev->next_in_bus;
	dev->parent->dev_cnt--;

	DRVMGR_UNLOCK();

	/* All references to this device has been removed at this point */
	free(dev);

	return DRVMGR_OK;
}
