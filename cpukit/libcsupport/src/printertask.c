/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/printer.h>

#include <rtems.h>
#include <rtems/seterr.h>

#include <unistd.h>

#define PRINT_TASK_WAKE_UP RTEMS_EVENT_0

typedef struct {
  rtems_chain_node node;

  enum {
    ACTION_WRITE,
    ACTION_DRAIN
  } action_kind;

  union {
    size_t   size;
    rtems_id task;
  } action_data;

  char data[ RTEMS_ZERO_LENGTH_ARRAY ];
} printer_task_buffer;

static void printer_task_acquire(
  rtems_printer_task_context   *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  rtems_interrupt_lock_acquire( &ctx->lock, lock_context );
}

static void printer_task_release(
  rtems_printer_task_context   *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  rtems_interrupt_lock_release( &ctx->lock, lock_context );
}

static printer_task_buffer *printer_task_get_buffer(
  rtems_printer_task_context *ctx,
  rtems_chain_control        *chain
)
{
  rtems_interrupt_lock_context  lock_context;
  printer_task_buffer          *buffer;

  printer_task_acquire( ctx, &lock_context );
  buffer = (printer_task_buffer *) rtems_chain_get_unprotected( chain );
  printer_task_release( ctx, &lock_context );

  return buffer;
}

static void printer_task_append_buffer(
  rtems_printer_task_context *ctx,
  rtems_chain_control        *chain,
  printer_task_buffer        *buffer
)
{
  rtems_interrupt_lock_context lock_context;

  printer_task_acquire( ctx, &lock_context );
  rtems_chain_append_unprotected( chain, &buffer->node );
  printer_task_release( ctx, &lock_context );
}

static int printer_task_printer( void *context, const char *fmt, va_list ap )
{
  rtems_printer_task_context *ctx;
  printer_task_buffer        *buffer;
  int                         n;

  ctx = context;
  buffer = printer_task_get_buffer( ctx, &ctx->free_buffers );

  if ( buffer == NULL ) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  n = vsnprintf( &buffer->data[ 0 ], ctx->buffer_size, fmt, ap );

  if ( n >= (int) ctx->buffer_size ) {
    printer_task_append_buffer( ctx, &ctx->free_buffers, buffer );
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  buffer->action_kind = ACTION_WRITE;
  buffer->action_data.size = (size_t) n;
  printer_task_append_buffer( ctx, &ctx->todo_buffers, buffer );
  rtems_event_send( ctx->task, PRINT_TASK_WAKE_UP );

  return n;
}

static void printer_task( rtems_task_argument arg )
{
  rtems_printer_task_context *ctx;
  int                         fd;

  ctx = (rtems_printer_task_context *) arg;
  fd = ctx->fd;

  while ( true ) {
    rtems_event_set      unused;
    printer_task_buffer *buffer;

    rtems_event_receive(
      PRINT_TASK_WAKE_UP,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &unused
    );

    while (
      ( buffer = printer_task_get_buffer( ctx, &ctx->todo_buffers ) ) != NULL
    ) {
      switch ( buffer->action_kind ) {
        case ACTION_WRITE:
          write( fd, &buffer->data[ 0 ], buffer->action_data.size );
          printer_task_append_buffer( ctx, &ctx->free_buffers, buffer );
          break;
        case ACTION_DRAIN:
          fsync(fd);
          rtems_event_transient_send( buffer->action_data.task );
          break;
      }
    }
  }
}

int rtems_print_printer_task(
  rtems_printer              *printer,
  rtems_printer_task_context *ctx
)
{
  rtems_status_code sc;

  if ( ctx->buffer_size < sizeof( printer_task_buffer ) ) {
    return EINVAL;
  }

  sc = rtems_task_create(
    rtems_build_name( 'P', 'R', 'N', 'T'),
    ctx->task_priority,
    ctx->task_stack_size,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->task
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    return ENOMEM;
  }

  rtems_chain_initialize_empty( &ctx->todo_buffers );
  rtems_chain_initialize(
    &ctx->free_buffers,
    ctx->buffer_table,
    ctx->buffer_count,
    ctx->buffer_size
  );
  ctx->buffer_size -= sizeof( printer_task_buffer );

  printer->context = ctx;
  printer->printer = printer_task_printer;

  rtems_task_start( ctx->task, printer_task, (rtems_task_argument) ctx );

  return 0;
}

void rtems_printer_task_drain( rtems_printer_task_context *ctx )
{
  printer_task_buffer buffer;

  rtems_chain_initialize_node( &buffer.node );
  buffer.action_kind = ACTION_DRAIN;
  buffer.action_data.task = rtems_task_self();

  printer_task_append_buffer( ctx, &ctx->todo_buffers, &buffer );
  rtems_event_send( ctx->task, PRINT_TASK_WAKE_UP );
  rtems_event_transient_receive( RTEMS_WAIT, RTEMS_NO_TIMEOUT );
}
