/*
 * Copyright (c) 2010, 2011 by
 * Ralf Corsepius, Ulm/Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

/*
 * Try to compile and link against POSIX math routines.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#ifdef NO_FLOAT
  #define PROVIDE_EMPTY_FUNC
#endif

#define SUFFIX f
#define FTYPE float
#define PRI "%f"

#include <domath.h>
