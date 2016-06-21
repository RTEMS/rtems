/* Driver Manager Driver Translate Interface Implementation
 *
 * COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <drvmgr/drvmgr.h>

#include <rtems/bspIo.h>

/* Calls drvmgr_translate() to translate an address range and check the result,
 * a printout is generated if the check fails. See paramters of
 * drvmgr_translate().
 * If size=0 only the starting address is not checked.
 */
int drvmgr_translate_check(
	struct drvmgr_dev *dev,
	unsigned int options,
	void *src_address,
	void **dst_address,
	unsigned int size)
{
	unsigned int max;

	max = drvmgr_translate(dev, options, src_address, dst_address);
	if (max == 0 || (max < size && (size != 0))) {
		printk(" ### dev %p (%s) failed mapping %p\n",
			dev, dev->name ? dev->name : "unnamed", src_address);
		return -1;
	}

	return 0;
}
