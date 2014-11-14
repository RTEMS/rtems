/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_LIBCSUPPORT_PWDGRP_H
#define _RTEMS_LIBCSUPPORT_PWDGRP_H

#include <grp.h>
#include <pwd.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void _libcsupport_pwdgrp_init(void);

int _libcsupport_scanpw(
  FILE *fp,
  struct passwd *pwd,
  char *buffer,
  size_t bufsize
);

int _libcsupport_scangr(
  FILE *fp,
  struct group *grp,
  char *buffer,
  size_t bufsize
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_LIBCSUPPORT_PWDGRP_H */
