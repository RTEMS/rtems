/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/console.h>
#include <rtems/bspIo.h>
#include <rtems/imfs.h>
#include <rtems/thread.h>

#include "consolesimple.h"

#define CONSOLE_SIMPLE_TASK_BUFFER_SIZE 2048

#define CONSOLE_SIMPLE_TASK_MAX_JUNK_SIZE 80

typedef struct {
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

static ssize_t _Console_simple_task_Write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
)
{
  Console_simple_task_Control *cons;
  const char                  *buf;
  size_t                       todo;

  cons = IMFS_generic_get_context_by_iop( iop );
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

  cons = IMFS_generic_get_context_by_iop( iop );
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
_Console_simple_task_Node_control = IMFS_GENERIC_INITIALIZER(
  &_Console_simple_task_Handlers,
  IMFS_node_initialize_generic,
  IMFS_node_destroy_default
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

void _Console_simple_task_Initialize( void )
{
  Console_simple_task_Control *cons;

  cons = &_Console_simple_task_Instance;

  rtems_interrupt_lock_initialize( &cons->buf_lock, "Console" );
  rtems_mutex_init( &cons->output_mutex, "Console" );

  IMFS_make_generic_node(
    CONSOLE_DEVICE_NAME,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &_Console_simple_task_Node_control,
    cons
  );

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
