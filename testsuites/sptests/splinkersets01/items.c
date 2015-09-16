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

RTEMS_LINKER_RWSET_ITEM_ORDERED(test_rw, const int *, a1, 1) = &a[1];

RTEMS_LINKER_ROSET_ITEM_ORDERED(test_ro, const int *, ca2, OC) = &ca[2];
