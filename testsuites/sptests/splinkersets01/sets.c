/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "splinkersets01.h"

RTEMS_LINKER_RWSET(test_rw, const int *);

RTEMS_LINKER_ROSET(test_ro, const int *);

RTEMS_LINKER_RWSET(test_content_rw, char);

RTEMS_LINKER_ROSET(test_content_ro, char);
