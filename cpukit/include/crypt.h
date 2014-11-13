/* LINTLIBRARY */
/*
 * Copyright (c) 1999
 *      Mark Murray.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY MARK MURRAY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL MARK MURRAY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 *
 */

#ifndef _CRYPT_H
#define _CRYPT_H

#include <sys/types.h>
#include <sys/queue.h>

__BEGIN_DECLS

struct crypt_data {
	char buffer[256];
};

struct crypt_format {
	SLIST_ENTRY(crypt_format) link;
	char *(*func)(const char *, const char *, struct crypt_data *);
	const char *magic;
};

#define CRYPT_FORMAT_INITIALIZER(func, magic) { { NULL }, (func), (magic) }

extern struct crypt_format crypt_md5_format;

extern struct crypt_format crypt_sha256_format;

extern struct crypt_format crypt_sha512_format;

void crypt_add_format(struct crypt_format *);

char *crypt_r(const char *, const char *, struct crypt_data *);

char *crypt_md5_r(const char *, const char *, struct crypt_data *);

char *crypt_sha256_r(const char *, const char *, struct crypt_data *);

char *crypt_sha512_r(const char *, const char *, struct crypt_data *);

void _crypt_to64(char *s, u_long v, int n);

#define b64_from_24bit _crypt_b64_from_24bit
void _crypt_b64_from_24bit(uint8_t, uint8_t, uint8_t, int, int *, char **);

__END_DECLS

#endif /* _CRYPT_H */
