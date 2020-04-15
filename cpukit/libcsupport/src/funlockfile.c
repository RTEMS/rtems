/**
 *  @file
 *
 *  @brief Unlock File for Stdio
 *  @ingroup libcsupport
 */

/*
 * Copyright (c) 2009 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE_FUNLOCKFILE)

#include <stdio.h>

/**
 * This is a non-functional stub 
 */
void funlockfile(FILE* file)
{
  (void) file;
}

#endif
