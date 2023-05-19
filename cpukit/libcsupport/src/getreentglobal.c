/*
 * Copyright (c) 2018 embedded brains GmbH & Co. KG
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#if defined(RTEMS_NEWLIB)
#include <sys/reent.h>

#ifndef _REENT_THREAD_LOCAL
struct _reent *__getreent(void)
{
  return _GLOBAL_REENT;
}
#endif
#endif
