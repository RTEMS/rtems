/*
 * Copyright (c) 2018 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if !defined(DL01_H)
#define DL01_H

extern int         dl01_bss1;
extern float       dl01_bss2[30];
extern char        dl01_bss3[10];
extern int         dl01_data1;
extern float       dl01_data2;
extern const int   dl01_const1;
extern const float dl01_const2;

int dl01_func1(void);

#endif
