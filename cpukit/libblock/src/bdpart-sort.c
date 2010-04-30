/**
 * @file
 *
 * @ingroup rtems_bdpart
 *
 * Block device partition management.
 */

/*
 * Copyright (c) 2009, 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include <rtems.h>
#include <rtems/bdpart.h>

static int rtems_bdpart_partition_compare( const void *aa, const void *bb)
{
  const rtems_bdpart_partition *a = aa;
  const rtems_bdpart_partition *b = bb;

  if (a->begin < b->begin) {
    return -1;
  } else if (a->begin == b->begin) {
    return 0;
  } else {
    return 1;
  }
}

void rtems_bdpart_sort( rtems_bdpart_partition *pt, size_t count)
{
  qsort( pt, count, sizeof( *pt), rtems_bdpart_partition_compare);
}
