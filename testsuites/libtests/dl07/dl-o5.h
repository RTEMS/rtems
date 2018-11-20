/*
 * Copyright (c) 2018 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdint.h>

#if !defined(DL05_H)
#define DL05_H

/*
 * A set of variables in dl-o5 referenced by dl-03 and unresolved when dl-o3 is
 * loaded. They are all uninitialised variables with various sizes in a mixed
 * order to get various alignments. These and dl-o4 variables are designed to
 * force the dependent tables to grow.
 */

extern uint64_t dl05_unresolv_1;
extern uint16_t dl05_unresolv_2;
extern uint32_t dl05_unresolv_3;
extern uint8_t  dl05_unresolv_4;
extern int64_t  dl05_unresolv_5;

#endif
