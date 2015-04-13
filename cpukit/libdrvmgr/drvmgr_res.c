/* Driver Manager Driver Resource Interface Implementation.
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <string.h>
#include <drvmgr/drvmgr.h>

/* Find all the resource keys for a device among all bus resources */
int drvmgr_keys_get(struct drvmgr_dev *dev, struct drvmgr_key **keys)
{
	struct drvmgr_bus *bus;
	struct drvmgr_bus_res *node;
	struct drvmgr_drv_res *res;
	uint64_t drv_id;

	bus = dev->parent;
	if (!bus || !dev->drv)
		return -1;

	drv_id = dev->drv->drv_id;

	/* Loop all resource arrays */
	node = bus->reslist;
	while (node) {
		/* Find driver ID in resource array */
		res = &node->resource[0];
		while (res->drv_id) {
			if (res->drv_id == drv_id) {
				/* Found resource matching driver, now check
				 * that this resource is for this device.
				 */
				if (dev->minor_bus == res->minor_bus) {
					/* Matching driver and core number */
					if (keys)
						*keys = res->keys;
					return 0;
				}
			}
			res++;
		}
		node = node->next;
	}
	if (keys)
		*keys = NULL;
	return 1;
}

/* Return key that matches key name */
struct drvmgr_key *drvmgr_key_get(
	struct drvmgr_key *keys,
	char *key_name)
{
	struct drvmgr_key *key;

	if (!keys)
		return NULL;

	key = keys;
	while (key->key_type != DRVMGR_KT_NONE) {
		if (strcmp(key_name, key->key_name) == 0)
			return key;
		key++;
	}
	return NULL;
}

union drvmgr_key_value *drvmgr_key_val_get(
	struct drvmgr_key *keys,
	char *key_name,
	enum drvmgr_kt key_type)
{
	struct drvmgr_key *key_match;

	key_match = drvmgr_key_get(keys, key_name);
	if (key_match) {
		/* Found key, put pointer to value into */
		if ((key_type == DRVMGR_KT_ANY) ||
		    (key_match->key_type == key_type))
			return &key_match->key_value;
	}
	return NULL;
}

union drvmgr_key_value *drvmgr_dev_key_get(
	struct drvmgr_dev *dev,
	char *key_name,
	enum drvmgr_kt key_type)
{
	struct drvmgr_key *keys = NULL;

	/* Find first entry in key array for the device */
	if (drvmgr_keys_get(dev, &keys))
		return NULL;

	/* Find a specific key among the device keys */
	return drvmgr_key_val_get(keys, key_name, key_type);
}
