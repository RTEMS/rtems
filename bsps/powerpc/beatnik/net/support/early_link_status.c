#include <rtems.h>
#include <bsp/early_enet_link_status.h>
#include <rtems/bspIo.h>

/* T. Straumann, 2005; see ../../LICENSE */

static const char *ename = ": rtems_em_early_check_link_status() - ";

int
BSP_early_check_link_status(int unit, rtems_bsdnet_early_link_check_ops *ops)
{
int status;

	unit--;
	if ( unit < 0 || unit >= ops->num_slots ) {
		printk("%s%sinvalid unit # %i (not in %i..%i)\n",
				ops->name, ename, unit+1, 1, ops->num_slots);
		return -1;
	}

	if ( !ops->initialized ) {
		if ( ops->init(unit) ) {
			printk("%s%sFAILURE to init hardware\n",ops->name, ename);
			return -1;
		}
		/* Start autoneg */
		if ( ops->write_phy(unit, 0, 0x1200) ) {
			printk("%s%sFAILURE to start autonegotiation\n",ops->name, ename);
			return -1;
		}
		/* Dont wait here; the caller can do this on various interfaces
		 * and wait herself.
	 	 */
		ops->initialized = 1;
	}
	if ( (status = ops->read_phy(unit, 1)) < 0 ) {
		printk("%s%sFAILURE to read phy status\n", ops->name, ename);
	}
	return status;
}
