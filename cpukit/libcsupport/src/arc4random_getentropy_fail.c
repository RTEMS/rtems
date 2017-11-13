/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#include <machine/_arc4random.h>
#include <rtems/score/interr.h>

void _arc4random_getentropy_fail( void )
{
  _Internal_error( INTERNAL_ERROR_ARC4RANDOM_GETENTROPY_FAIL );
}
