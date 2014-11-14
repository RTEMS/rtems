/**
 *  @file
 *
 *  @brief User Database Access Routines
 *  @ingroup libcsupport
 */

/*
 *  Copyright (c) 1999-2009 Ralf Corsepius <corsepiu@faw.uni-ulm.de>
 *  Copyright (c) 1999-2013 Joel Sherrill <joel.sherrill@OARcorp.com>
 *  Copyright (c) 2000-2001 Fernando Ruiz Casas <fernando.ruiz@ctv.es>
 *  Copyright (c) 2002 Eric Norum <eric.norum@usask.ca>
 *  Copyright (c) 2003 Till Straumann <strauman@slac.stanford.edu>
 *  Copyright (c) 2012 Alex Ivanov <alexivanov97@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "pwdgrp.h"

/*
 * Static, thread-unsafe, buffers
 */
static char grbuf[200];
static struct group grent;

struct group *getgrnam(
  const char *name
)
{
  struct group *p;

  if(getgrnam_r(name, &grent, grbuf, sizeof grbuf, &p))
    return NULL;
  return p;
}

struct group *getgrgid(
  gid_t gid
)
{
  struct group *p;

  if(getgrgid_r(gid, &grent, grbuf, sizeof grbuf, &p))
    return NULL;
  return p;
}
