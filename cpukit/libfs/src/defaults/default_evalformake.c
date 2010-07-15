/**
 * @file
 *
 * @ingroup LibIO
 *
 * @brief rtems_filesystem_default_evalformake() implementation.
 */

/*
 * Copyright (c) 2010
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

#include <rtems/libio.h>
#include <rtems/seterr.h>

int rtems_filesystem_default_evalformake(
   const char *path,
   rtems_filesystem_location_info_t *pathloc,
   const char **name
)
{
  rtems_set_errno_and_return_minus_one( ENOTSUP );
}
