/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef __LINUX_RTEMS_IMPL_H__
#define __LINUX_RTEMS_IMPL_H__

static inline char *do_kmemdup(const char *s, size_t n)
{
	char *dup = malloc(n + 1);

	if (dup != 0) {
		dup[n] = '\0';
		dup = memcpy(dup, s, n);
	}

	return dup;
}

#endif /* __LINUX_RTEMS_IMPL_H__ */
