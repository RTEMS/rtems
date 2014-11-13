/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <crypt.h>
#include <string.h>

static char *
cf_default_func(const char *pw, const char *salt, struct crypt_data *data)
{
	(void)salt;

	strncpy(&data->buffer[0], pw, sizeof(data->buffer) - 1);
	data->buffer[sizeof(data->buffer) - 1] = '\0';

	return (&data->buffer[0]);
}

static struct crypt_format cf_default =
  CRYPT_FORMAT_INITIALIZER(cf_default_func, NULL);

static SLIST_HEAD(, crypt_format) cf_head = {
	&cf_default
};

void crypt_add_format(struct crypt_format *cf)
{
	if (cf->link.sle_next == NULL)
		SLIST_INSERT_HEAD(&cf_head, cf, link);
}

char *
crypt_r(const char *passwd, const char *salt, struct crypt_data *data)
{
	const struct crypt_format *cf;

	SLIST_FOREACH(cf, &cf_head, link)
		if (cf->magic != NULL && strstr(salt, cf->magic) == salt)
			return (cf->func(passwd, salt, data));

	cf = SLIST_FIRST(&cf_head);

	return (cf->func(passwd, salt, data));
}
