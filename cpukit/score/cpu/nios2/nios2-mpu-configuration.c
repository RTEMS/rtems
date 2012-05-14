/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/nios2-utility.h>

static const Nios2_MPU_Configuration *_Nios2_MPU_Configuration;

void _Nios2_MPU_Set_configuration( const Nios2_MPU_Configuration *config )
{
  _Nios2_MPU_Configuration = config;
}

const Nios2_MPU_Configuration *_Nios2_MPU_Get_configuration( void )
{
  return _Nios2_MPU_Configuration;
}
