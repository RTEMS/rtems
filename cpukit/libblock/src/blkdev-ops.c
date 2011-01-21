/**
 * @file
 *
 * @ingroup rtems_blkdev
 *
 * @brief Block device management.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/blkdev.h>

const rtems_driver_address_table rtems_blkdev_generic_ops = {
	.initialization_entry = NULL,
	.open_entry = rtems_blkdev_generic_open,
	.close_entry = rtems_blkdev_generic_close,
	.read_entry = rtems_blkdev_generic_read,
	.write_entry = rtems_blkdev_generic_write,
	.control_entry = rtems_blkdev_generic_ioctl
};
