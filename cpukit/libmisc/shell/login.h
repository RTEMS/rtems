/**
 * @file
 *
 * @author Sebastian Huber <sebastian.huber@embedded-brains.de>
 *
 * @brief Login types and functions.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * Based on work from Chris Johns, Fernando Ruiz and Till Straumann.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef RTEMS_LOGIN_H
#define RTEMS_LOGIN_H

#include <stdio.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef bool (*rtems_login_check)(
  const char * /* user */,
  const char * /* passphrase */
);

bool rtems_shell_login_prompt(
  FILE *in,
  FILE *out,
  const char *device,
  rtems_login_check check
);

bool rtems_shell_login_check(
  const char *user,
  const char *passphrase
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_LOGIN_H */
