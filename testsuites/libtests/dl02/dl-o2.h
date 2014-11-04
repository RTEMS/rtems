/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if !defined(DL02_H)
#define DL02_H

typedef int (*dl_o2_call_t)(const char* message, int count);

int dl_o2_func1 (int argc, char* argv[]);
double dl_o2_func2 (double d1, double d2);
int dl_o2_func3 (dl_o2_call_t callback, int count);

#endif
