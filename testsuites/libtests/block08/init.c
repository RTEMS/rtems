/*  Init
 *
 *  This routine is the initialization task for this test program.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 * Copyright (C) 2010 OKTET Labs, St.-Petersburg, Russia
 * Author: Oleg Kravtsov <Oleg.Kravtsov@oktetlabs.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

#include "bdbuf_tests.h"

const char rtems_test_name[] = "BLOCK 8";

rtems_task Init(rtems_task_argument argument)
{
  rtems_test_begin();
  run_bdbuf_tests();
  rtems_test_end();

  exit(0);
}

