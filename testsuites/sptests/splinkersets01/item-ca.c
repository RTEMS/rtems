/*
 * Copyright (C) 2015, 2016 embedded brains GmbH & Co. KG
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "splinkersets01.h"

RTEMS_LINKER_ROSET_ITEM_ORDERED(test_ro, const int *, ca2, OC) = &ca[2];
