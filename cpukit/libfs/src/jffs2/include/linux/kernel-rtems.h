/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Copyright 2016 Chris Johns <chrisj@rtems.org>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __LINUX_RTEMS_IMPL_H__
#define __LINUX_RTEMS_IMPL_H__

#include <rtems.h>
#include <rtems/bspIo.h>

static inline char *do_kmemdup(const char *s, size_t n)
{
	char *dup = malloc(n + 1);

	if (dup != 0) {
		dup[n] = '\0';
		dup = memcpy(dup, s, n);
	}

	return dup;
}

/*
 * Provide a private printk to avoid all the formatting warnings in the JFFS2 code.
 */
static inline int jffs2_printk(const char* fmt, ...)
{
	va_list ap;
	int r;
	va_start(ap, fmt);
	r = vprintk(fmt, ap);
	va_end(ap);
	return r;
}

#undef printk
#define printk jffs2_printk

#endif /* __LINUX_RTEMS_IMPL_H__ */
