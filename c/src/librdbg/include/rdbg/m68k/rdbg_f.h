/*
 **************************************************************************
 *
 * Component =   RDBG
 * Module =   rdbg_f.h
 *
 * Synopsis = Machine-dependent header file
 *
 * $Id$
 *
 **************************************************************************
 */

#ifndef RDBG_F_H
#define RDBG_F_H

#include <rtems.h>
#include <rdbg/remdeb.h>

static inline  int isRdbgException(Exception_context *ctx)
{
  if (
      ctx->ctx->vecnum != 9 /* trace - singlestep */
      && ctx->ctx->vecnum != 47 /* trap #15 - breakpoint */
      && ctx->ctx->vecnum != 36 /* trap #4 - enter RDBG */
      ) return 0;
  else return 1;
}
static inline int getExcNum(Exception_context *ctx)
{
  return ctx->ctx->vecnum;
}

extern void connect_rdbg_exception();
extern void disconnect_rdbg_exception();

#endif



