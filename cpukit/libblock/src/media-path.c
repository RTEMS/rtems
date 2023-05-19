/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSMedia
 *
 * @brief Media implementation.
 */

/*
 * Copyright (C) 2009, 2010 embedded brains GmbH & Co. KG
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#include <rtems/media.h>

char *rtems_media_create_path(
  const char *prefix,
  const char *name,
  rtems_device_major_number major
)
{
  size_t const size = strlen(prefix) + 1 + strlen(name) + 1 + 10 + 1;
  char *const s = malloc(size);

  if (s != NULL) {
#ifndef NDEBUG
    int rv = 
#endif
    snprintf(s, size, "%s/%s-%" PRIu32, prefix, name, major);
    assert(rv < (int) size);
  }

  return s;
}

char *rtems_media_replace_prefix(const char *new_prefix, const char *path)
{
  const char *const name_try = strrchr(path, '/');
  const char *const name = (name_try == NULL) ? path : name_try + 1;
  size_t const new_prefix_len = strlen(new_prefix);
  size_t const name_size = strlen(name) + 1;
  size_t const size = new_prefix_len + 1 + name_size;
  char *const s = malloc(size);

  if (s != NULL) {
    memcpy(s, new_prefix, new_prefix_len);
    s [new_prefix_len] = '/';
    memcpy(s + new_prefix_len + 1, name, name_size);
  }

  return s;
}

char *rtems_media_append_minor(
  const char *path,
  rtems_device_minor_number minor
)
{
  size_t const size = strlen(path) + 1 + 10 + 1;
  char *const s = malloc(size);

  if (s != NULL) {
#ifndef NDEBUG
    int rv = 
#endif
    snprintf(s, size, "%s-%" PRIu32, path, minor);
    assert(rv < (int) size);
  }

  return s;
}
