/**
 * @file
 *
 * @ingroup RTEMSMedia
 *
 * @brief Media implementation.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <rtems/media.h>

char *rtems_media_create_path(const char *prefix, const char *name, rtems_device_major_number major)
{
  size_t const len = strlen(prefix) + 1 + strlen(name) + 1 + 10 + 1;
  char *const s = malloc(len);

  if (s != NULL) {
    int rv = snprintf(s, len, "%s/%s-%" PRIu32, prefix, name, major);

    if (rv >= (int) len) {
      free(s);

      return NULL;
    }
  }

  return s;
}

char *rtems_media_replace_prefix(const char *new_prefix, const char *path)
{
  const char *const name_try = strrchr(path, '/');
  const char *const name = (name_try == NULL) ? path : name_try + 1;
  size_t const new_prefix_len = strlen(new_prefix);
  size_t const name_len = strlen(name);
  size_t const len = new_prefix_len + 1 + name_len + 1;
  char *const s =  malloc(len);

  if (s != NULL) {
    strcpy(s, new_prefix);
    strcpy(s + new_prefix_len, "/");
    strcpy(s + new_prefix_len + 1, name);
  }

  return s;
}

char *rtems_media_append_minor(const char *path, rtems_device_minor_number minor)
{
  size_t const len = strlen(path) + 1 + 10 + 1;
  char *const s = malloc(len);

  if (s != NULL) {
    int rv = snprintf(s, len, "%s-%" PRIu32, path, minor);

    if (rv >= (int) len) {
      free(s);

      return NULL;
    }
  }

  return s;
}
