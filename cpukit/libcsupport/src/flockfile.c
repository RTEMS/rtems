/**
 *  @file
 *
 *  @brief Lock File for Stdio
 *  @ingroup SET
 */
/*
 * Copyright (c) 2009 by
 * Ralf Corsepius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE_FLOCKFILE)

#include <stdio.h>

/**
 *  This is a non-functional stub 
 */
void flockfile(FILE* file)
{
  (void) file;
}

#endif
