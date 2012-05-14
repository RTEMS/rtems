/*
 * (C) Copyright 1997 -
 * - NavIST Group - Real-Time Distributed Systems and Industrial Automation
 *
 * http://pandora.ist.utl.pt
 *
 * Instituto Superior Tecnico * Lisboa * PORTUGAL
 *
 * Disclaimer:
 *
 * This file is provided "AS IS" without warranty of any kind, either
 * expressed or implied.
 *
 * This code is based on code by: Jose Rufino - IST
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <rtems/bspIo.h>

/*
 * printk
 *
 * Kernel printf function requiring minimal infrastructure.
 */
void printk(const char *fmt, ...)
{
  va_list  ap;       /* points to each unnamed argument in turn */

  va_start(ap, fmt); /* make ap point to 1st unnamed arg */
  vprintk(fmt, ap);
  va_end(ap);        /* clean up when done */
}
