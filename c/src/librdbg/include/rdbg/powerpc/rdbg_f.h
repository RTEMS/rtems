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
      ctx->ctx->_EXC_number != ASM_SYS_VECTOR &&
      ctx->ctx->_EXC_number != ASM_PROG_VECTOR &&
      ctx->ctx->_EXC_number != ASM_TRACE_VECTOR
      ) return 0;
  else return 1;
}
static inline int getExcNum(Exception_context *ctx)
{
  return ctx->ctx->_EXC_number;
}

extern void connect_rdbg_exception();
extern void disconnect_rdbg_exception();

#endif



