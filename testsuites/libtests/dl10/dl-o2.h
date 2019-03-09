/*
 * Copyright (c) 2018 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if !defined(DL02_H)
#define DL02_H

/*
 * A set of variables in dl-o2 reference by dl-03.
 */

extern int   dl02_bss1;
extern float dl02_bss2[7];
extern char  dl02_bss3[21];
extern int   dl02_data1;
extern float dl02_data2;

int rtems_main_o2 (void);

#endif
