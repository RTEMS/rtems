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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pwdgrp.h"

/*
 * Static, thread-unsafe, buffers
 */
static FILE *passwd_fp;
static char pwbuf[200];
static struct passwd pwent;

struct passwd *getpwnam(
  const char *name
)
{
  struct passwd *p;

  if(getpwnam_r(name, &pwent, pwbuf, sizeof pwbuf, &p))
    return NULL;
  return p;
}

struct passwd *getpwuid(
  uid_t uid
)
{
  struct passwd *p;

  if(getpwuid_r(uid, &pwent, pwbuf, sizeof pwbuf, &p))
    return NULL;
  return p;
}

struct passwd *getpwent(void)
{
  if (passwd_fp == NULL)
    return NULL;
  if (!_libcsupport_scanpw(passwd_fp, &pwent, pwbuf, sizeof pwbuf))
    return NULL;
  return &pwent;
}

void setpwent(void)
{
  _libcsupport_pwdgrp_init();

  if (passwd_fp != NULL)
    fclose(passwd_fp);
  passwd_fp = fopen("/etc/passwd", "r");
}

void endpwent(void)
{
  if (passwd_fp != NULL)
    fclose(passwd_fp);
}
