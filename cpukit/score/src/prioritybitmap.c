/*
 * @file
 *
 * @brief Priority Bit Map Implementation
 *
 * @ingroup ScorePriorityBitmap
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/prioritybitmapimpl.h>

Priority_bit_map_Word          _Priority_Bit_map[16] CPU_STRUCTURE_ALIGNMENT;

/* Instantiate any global variables needed by the priority scheduler */
volatile Priority_bit_map_Word _Priority_Major_bit_map;
