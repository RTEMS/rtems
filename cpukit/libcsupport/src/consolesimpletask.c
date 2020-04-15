/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2018, 2020 embedded brains GmbH (http://www.embedded-brains.de)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/console.h>
#include <rtems/bspIo.h>
#include <rtems/imfs.h>
#include <rtems/thread.h>

#include "consolesimple.h"

#define CONSOLE_SIMPLE_TASK_BUFFER_SIZE 2048

#define CONSOLE_SIMPLE_TASK_MAX_JUNK_SIZE 80

typedef struct {
  IMFS_jnode_t Node;
  RTEMS_INTERRUPT_LOCK_MEMBER( buf_lock )
  rtems_mutex output_mutex;
  rtems_id task;
  size_t head;
  size_t tail;
  char buf[ CONSOLE_SIMPLE_TASK_BUFFER_SIZE ];
} Console_simple_task_Control;

static Console_simple_task_Control _Console_simple_task_Instance;

static size_t _Console_simple_task_Capacity(
  const Console_simple_task_Control *cons
)
{
  return ( cons->tail - cons->head - 1 ) % CONSOLE_SIMPLE_TASK_BUFFER_SIZE;
}

static size_t _Console_simple_task_Available(
  const Console_simple_task_Control *cons
)
{
  return ( cons->head - cons->tail ) % CONSOLE_SIMPLE_TASK_BUFFER_SIZE;
}

static Console_simple_task_Control *_Console_simple_task_Iop_to_control(
  const rtems_libio_t *iop
)
{
  return (Console_simple_task_Control *) IMFS_iop_to_node( iop );
}

static ssize_t _Console_simple_task_Write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
)
{
  Console_simple_task_Control *cons;
  const char                  *buf;
  size_t                       todo;

  cons = _Console_simple_task_Iop_to_control( iop );
  buf = buffer;
  todo = count;

  while ( todo > 0 ) {
    rtems_interrupt_lock_context lock_context;
    size_t                       junk;
    size_t                       head;
    size_t                       i;

    rtems_interrupt_lock_acquire( &cons->buf_lock, &lock_context );

    junk = _Console_simple_task_Capacity( cons );

    if ( junk > todo ) {
      junk = todo;
    }

    if ( junk > CONSOLE_SIMPLE_TASK_MAX_JUNK_SIZE ) {
      junk = CONSOLE_SIMPLE_TASK_MAX_JUNK_SIZE;
    }

    head = cons->head;

    for ( i = 0; i < junk; ++i ) {
      cons->buf[ head ] = *buf;
      head = ( head + 1 ) % CONSOLE_SIMPLE_TASK_BUFFER_SIZE;
      --todo;
      ++buf;
    }

    cons->head = head;

    rtems_interrupt_lock_release( &cons->buf_lock, &lock_context );

    if ( junk == 0 ) {
      break;
    }
  }

  rtems_event_system_send( cons->task, RTEMS_EVENT_SYSTEM_SERVER );

  return (ssize_t) ( count - todo );
}

static void _Console_simple_task_Put_chars( Console_simple_task_Control *cons )
{
  rtems_interrupt_lock_context lock_context;
  size_t                       available;
  size_t                       tail;
  size_t                       i;

  rtems_mutex_lock( &cons->output_mutex );

  rtems_interrupt_lock_acquire( &cons->buf_lock, &lock_context );

  available = _Console_simple_task_Available( cons );
  tail = cons->tail;

  rtems_interrupt_lock_release( &cons->buf_lock, &lock_context );

  for ( i = 0; i < available; ++i) {
    rtems_putc( cons->buf[ tail ] );
    tail = ( tail + 1 ) % CONSOLE_SIMPLE_TASK_BUFFER_SIZE;
  }

  rtems_interrupt_lock_acquire( &cons->buf_lock, &lock_context );

  cons->tail = tail;

  rtems_interrupt_lock_release( &cons->buf_lock, &lock_context );

  rtems_mutex_unlock( &cons->output_mutex );
}

static int _Console_simple_task_Fsync( rtems_libio_t *iop )
{
  Console_simple_task_Control *cons;

  cons = _Console_simple_task_Iop_to_control( iop );
  _Console_simple_task_Put_chars( cons );

  return 0;
}

static const rtems_filesystem_file_handlers_r _Console_simple_task_Handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = _Console_simple_Read,
  .write_h = _Console_simple_task_Write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = _Console_simple_task_Fsync,
  .fdatasync_h = _Console_simple_task_Fsync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev,
  .mmap_h = rtems_filesystem_default_mmap
};

static const IMFS_node_control
_Console_simple_task_Node_control = IMFS_NODE_CONTROL_INITIALIZER(
  &_Console_simple_task_Handlers,
  IMFS_node_initialize_default,
  IMFS_do_nothing_destroy
);

static void _Console_simple_task_Task( rtems_task_argument arg )
{
  Console_simple_task_Control *cons;

  cons = (Console_simple_task_Control *) arg;

  while ( true ) {
    rtems_event_set events;

    rtems_event_system_receive(
      RTEMS_EVENT_SYSTEM_SERVER,
      RTEMS_WAIT | RTEMS_EVENT_ALL,
      RTEMS_NO_TIMEOUT,
      &events
    );

    _Console_simple_task_Put_chars( cons );
  }
}

static const char _Console_simple_task_Name[] = "console";

void _Console_simple_task_Initialize( void )
{
  Console_simple_task_Control *cons;

  cons = &_Console_simple_task_Instance;

  IMFS_node_preinitialize(
    &cons->Node,
    &_Console_simple_task_Node_control,
    _Console_simple_task_Name,
    sizeof( _Console_simple_task_Name ) - 1,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO
  );

  rtems_interrupt_lock_initialize( &cons->buf_lock, "Console" );
  rtems_mutex_init( &cons->output_mutex, "Console" );

  IMFS_add_node( "/dev", &cons->Node, NULL );

  rtems_task_create(
    rtems_build_name('C', 'O', 'N', 'S'),
    RTEMS_MAXIMUM_PRIORITY - 1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_DEFAULT_MODES,
    &cons->task
  );

  rtems_task_start(
    cons->task,
    _Console_simple_task_Task,
    (rtems_task_argument) cons
  );
}
