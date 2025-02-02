/*
 * Copyright (C) 2013 embedded brains GmbH & Co. KG
 * Copyright (C) 2016 Chris Johns <chrisj@rtems.org>
 *
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
