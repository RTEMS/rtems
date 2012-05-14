/*
 *  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __MEMORY_H
#define __MEMORY_H 1

#include "devices.h"

typedef struct memdsc
{
  unsigned long base;
  unsigned long size;
  device_desc *dev;
  struct memdsc *next;
}
memory_desc;

memory_desc *find_memory(device_desc *devices);

#endif


