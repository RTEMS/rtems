/*
 * Copyright (c) 2009 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE_FUNLOCKFILE) && defined(HAVE_DECL_FUNLOCKFILE)

#include <stdio.h>

/* This is a non-functional stub */
void funlockfile(FILE* file __attribute__((unused)))
{
}

#endif
