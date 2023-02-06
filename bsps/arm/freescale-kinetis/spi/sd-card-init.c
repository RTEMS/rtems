#include <stdio.h>
#include <rtems/fsmount.h>
#include <rtems/dosfs.h>
#include <rtems/bdpart.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK
#include <rtems/status-checks.h>

#include <bsp.h>
#include <bsp/spi_sd.h>
#include <bsp/sd-blkdev.h>

static fstab_t kinetis_fs_table [] = {
    {
		"/dev/sd-card-a1", "/mnt",
		"dosfs", RTEMS_FILESYSTEM_READ_WRITE,
		FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
		FSMOUNT_MNT_OK
	}
};


rtems_status_code kinetis_sd_card_init( bool mount)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	unsigned int sectors = 0;
	int rv = 0;

	/* Initialize SD Card */
	sc = sd_init();
	RTEMS_CHECK_SC( sc, "Initialize SD Card");

	/* Create disk device */
    sc = sd_card_blkdev_bnum(&sectors);
    RTEMS_CHECK_SC( sc, "Get SD Card Capacity");
	sc = rtems_blkdev_create( "/dev/sd-card-a", 512, sectors, sd_card_blkdev_ioctl, NULL);
	RTEMS_CHECK_SC( sc, "Create disk device");

	if (mount) {
		sc = rtems_bdpart_register_from_disk( "/dev/sd-card-a");
		RTEMS_CHECK_SC( sc, "Initialize IDE partition table");

		rv = rtems_fsmount( kinetis_fs_table, sizeof( kinetis_fs_table) / sizeof( kinetis_fs_table [0]), NULL);
		RTEMS_CHECK_RV_SC( rv, "Mount file systems");
	}

	return RTEMS_SUCCESSFUL;
}