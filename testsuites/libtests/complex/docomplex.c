/*
 * Copyright (c) 2010, 2011 by
 * Ralf Corsepius, Ulm/Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

/*
 * Try to compile and link against POSIX complex math routines.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#ifdef NO_DOUBLE
  #define PROVIDE_EMPTY_FUNC
#endif

#define FTYPE double
#define PRI "%f"

#include "docomplex.h"
