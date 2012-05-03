/*
 * RTEMS for Nintendo DS interprocessor communication extensions.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#ifndef _MYIPC_H_
#define _MYIPC_H_

#include "types.h"

typedef struct
{
  vuint8_t *record_buffer;
  vuint32_t record_length_max;
  vuint32_t recorded_length;
  vuint8_t record;
} my_TransferRegion;

static inline my_TransferRegion volatile *
getmy_IPC (void)
{
  return (my_TransferRegion volatile *) (0x027FF000 +
                                         sizeof (TransferRegion));
}

#define my_IPC getmy_IPC()

#endif
