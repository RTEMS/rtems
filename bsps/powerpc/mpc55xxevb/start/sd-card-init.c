/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief SD Card initialization code.
 */

/*
 * Copyright (c) 2008 embedded brains GmbH & Co. KG
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

#include <stdio.h>

#include <mpc55xx/mpc55xx.h>
#include <mpc55xx/regs.h>
#include <mpc55xx/dspi.h>

#include <bsp.h>

#include <rtems/status-checks.h>

#ifdef MPC55XX_BOARD_MPC5566EVB

static rtems_status_code mpc55xx_dspi_init(void)
{
	int rv = 0;
	int i = 0;
	char device_name [] = "/dev/spi0";
	union SIU_PCR_tag pcr = MPC55XX_ZERO_FLAGS;

	rv = rtems_libi2c_initialize();
	RTEMS_CHECK_RV_SC( rv, "rtems_libi2c_initialize");

	/* DSPI D inputs are taken from DSPI C */
	SIU.DISR.R = 0x000000FC;

	/* DSPI A signals */
	pcr.B.PA = 1;
	pcr.B.ODE = 0;
	pcr.B.HYS = 0;
	pcr.B.SRC = 3;
	pcr.B.WPE = 1;
	pcr.B.WPS = 1;

	/* SCK */
	pcr.B.OBE = 1;
	pcr.B.IBE = 0;
	SIU.PCR [93].R = pcr.R;

	/* SIN */
	pcr.B.OBE = 0;
	pcr.B.IBE = 1;
	SIU.PCR [94].R = pcr.R;

	/* SOUT */
	pcr.B.OBE = 1;
	pcr.B.IBE = 0;
	SIU.PCR [95].R = pcr.R;

	/* PCSx */
	pcr.B.OBE = 1;
	pcr.B.IBE = 0;
	SIU.PCR [96].R = pcr.R;
	SIU.PCR [97].R = pcr.R;
	SIU.PCR [98].R = pcr.R;
	SIU.PCR [99].R = pcr.R;
	SIU.PCR [100].R = pcr.R;
	SIU.PCR [101].R = pcr.R;

	mpc55xx_dspi_bus_table [3].master = 0;
	for (i = 0; i < MPC55XX_DSPI_NUMBER; ++i) {
		device_name [8] = (char) ('0' + i);
		rv = rtems_libi2c_register_bus( device_name, (rtems_libi2c_bus_t *) &mpc55xx_dspi_bus_table [i]);
		RTEMS_CHECK_RV_SC( rv, device_name);
	}

	return RTEMS_SUCCESSFUL;
}

#include <stdio.h>
#include <rtems/fsmount.h>
#include <rtems/dosfs.h>
#include <rtems/bdpart.h>
#include <rtems/console.h>

#include <libchip/spi-sd-card.h>

#define MPC55XX_DEVICE "sd-card-a"
#define MPC55XX_DEVICE_FILE "/dev/" MPC55XX_DEVICE
#define MPC55XX_PARTITION "/dev/sd-card-a1"
#define MPC55XX_MOUNT_POINT "/mnt"

static fstab_t mpc55xx_fs_table [] = { {
		MPC55XX_PARTITION, MPC55XX_MOUNT_POINT,
		"dosfs", RTEMS_FILESYSTEM_READ_WRITE,
		FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
		FSMOUNT_MNT_OK
	}, {
		MPC55XX_DEVICE_FILE, MPC55XX_MOUNT_POINT,
		"dosfs", RTEMS_FILESYSTEM_READ_WRITE,
		FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
		0
	}
};

sd_card_driver_entry sd_card_driver_table [] = {
	{
		.device_name = "/dev/sd-card-a",
		.bus = 0,
		.transfer_mode = SD_CARD_TRANSFER_MODE_DEFAULT,
		.command = SD_CARD_COMMAND_DEFAULT,
		/* response : whatever, */
		.response_index = SD_CARD_COMMAND_SIZE,
		.n_ac_max = SD_CARD_N_AC_MAX_DEFAULT,
		.block_number = 0,
		.block_size = 0,
		.block_size_shift = 0,
		.busy = true,
		.verbose = true,
		.schedule_if_busy = false
	}
};

size_t sd_card_driver_table_size = sizeof( sd_card_driver_table) / sizeof( sd_card_driver_table [0]);

rtems_status_code mpc55xx_sd_card_init( bool mount)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	sd_card_driver_entry *e = &sd_card_driver_table [0];

	RTEMS_DEBUG_PRINT( "Task started\n");

	sc = mpc55xx_dspi_init();
	RTEMS_CHECK_SC( rv, "Intitalize DSPI bus");

	e->bus = mpc55xx_dspi_bus_table [0].bus_number;

	sc = sd_card_register();
	RTEMS_CHECK_SC( sc, "Register SD Card");

	if (mount) {
		sc = rtems_bdpart_register_from_disk( MPC55XX_DEVICE_FILE);
		RTEMS_CHECK_SC( sc, "Initialize IDE partition table");

		rv = rtems_fsmount( mpc55xx_fs_table, sizeof( mpc55xx_fs_table) / sizeof( mpc55xx_fs_table [0]), NULL);
		RTEMS_CHECK_RV_SC( rv, "Mount file systems");
	}

	return RTEMS_SUCCESSFUL;
}

#endif /* MPC55XX_BOARD_MPC5566EVB */
