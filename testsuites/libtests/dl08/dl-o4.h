/*
 * Copyright (c) 2018 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if !defined(DL04_H)
#define DL04_H

/*
 * A set of variables in dl-o4 referenced by dl-03 and unresolved when dl-o3 is
 * loaded. They are all uninitialised variables with various sizes in a mixed
 * order to get various alignments. These and dl-o5 variables are designed to
 * force the dependent tables to grow.
 */

extern int         dl04_unresolv_1;
extern float       dl04_unresolv_2;
extern char        dl04_unresolv_3;
extern char*       dl04_unresolv_4;
extern const int   dl04_unresolv_5;
extern const char* dl04_unresolv_6;

int rtems_main_o4 (void);

#endif
