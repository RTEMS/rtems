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

/* FIXME: Why is this defined? */
#define NO_LONG_DOUBLE

#ifdef NO_LONG_DOUBLE
  #define PROVIDE_EMPTY_FUNC
#endif

#define SUFFIX l
#define FTYPE long double
#define PRI "%Lf"

#include "docomplex.h"
