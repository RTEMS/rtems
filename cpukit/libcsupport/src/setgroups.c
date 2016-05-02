/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <unistd.h>

int setgroups(int size, const gid_t *list)
{
	/* FIXME: Implement this function properly. Currently it only returns a
	 * success. */

	(void) size;
	(void) list;

	return 0;
}
