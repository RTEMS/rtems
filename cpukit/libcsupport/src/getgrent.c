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

#include <stdlib.h>
#include <pthread.h>

typedef struct {
  FILE *fp;
  char buf[256];
  struct group grp;
} grp_context;

static pthread_once_t grp_once = PTHREAD_ONCE_INIT;

static pthread_key_t grp_key;

static void grp_init(void)
{
  pthread_key_create(&grp_key, free);
}

static grp_context *grp_get_context(void)
{
  pthread_once(&grp_once, grp_init);

  return pthread_getspecific(grp_key);
}

struct group *getgrent(void)
{
  grp_context *ctx = grp_get_context();

  if (ctx == NULL)
    return NULL;

  if (ctx->fp == NULL)
    return NULL;

  if (!_libcsupport_scangr(ctx->fp, &ctx->grp, ctx->buf, sizeof(ctx->buf)))
    return NULL;

  return &ctx->grp;
}

void setgrent(void)
{
  grp_context *ctx = grp_get_context();

  if (ctx == NULL) {
    int eno;

    ctx = calloc(1, sizeof(*ctx));
    if (ctx == NULL)
      return;

    eno = pthread_setspecific(grp_key, ctx);
    if (eno != 0) {
      free(ctx);

      return;
    }
  }

  _libcsupport_pwdgrp_init();

  if (ctx->fp != NULL)
    fclose(ctx->fp);

  ctx->fp = fopen("/etc/group", "r");
}

void endgrent(void)
{
  grp_context *ctx = grp_get_context();

  if (ctx == NULL)
    return;

  if (ctx->fp != NULL) {
    fclose(ctx->fp);
  }

  free(ctx);
  pthread_setspecific(grp_key, NULL);
}
