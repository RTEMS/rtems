/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/imfs.h>
#include <rtems/libio_.h>
#include <rtems/ramdisk.h>
#include <rtems/rtems-rfs-format.h>
#include <rtems/test-info.h>
#include <rtems/test.h>

const char rtems_test_name[] = "FSCLOSE 2";

#define PATH "generic"

#define RAMDISK_PATH "/dev/rda"

#define RFS_PATH "/rfs"

#define RFS_FILE_PATH RFS_PATH "/file"

#define WORKER_PRIORITY 1

#define RUNNER_PRIORITY 2

#define STAT_DONE RTEMS_EVENT_0

#define STAT_TIMEOUT 10

typedef struct {
  rtems_id runner_id;
  rtems_id worker_id;
  rtems_id stat_worker_id;
  bool     wait_in_open;
  bool     wait_in_fstat;
  bool     wait_in_fcntl;
  bool     close_busy;
  bool     fail_open;
  bool     publish_in_open;
  int      fd_in_open;
  int      fd;
  int      status;
  int      stale_fd;
  int      phase;
  int      open_fd;
  int      close_status;
} test_context;

static test_context test_instance;

static size_t free_iops( void )
{
  size_t         count;
  rtems_libio_t *iop;

  count = 0;
  iop = rtems_libio_iop_free_head;

  /* A cyclic free list yields a count above the count of iops */
  while ( iop != NULL && count <= rtems_libio_number_iops ) {
    ++count;
    iop = iop->data1;
  }

  return count;
}

static bool is_on_free_list( const rtems_libio_t *iop )
{
  const rtems_libio_t *other;
  size_t               count;

  count = 0;
  other = rtems_libio_iop_free_head;

  while ( other != NULL && count <= rtems_libio_number_iops ) {
    if ( other == iop ) {
      return true;
    }

    ++count;
    other = other->data1;
  }

  return false;
}

static void wait( void )
{
  rtems_status_code sc;

  sc = rtems_event_transient_receive( RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  T_quiet_rsc_success( sc );
}

static void wake_up( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_event_transient_send( id );
  T_quiet_rsc_success( sc );
}

static void start_worker( test_context *ctx, rtems_task_entry entry )
{
  rtems_status_code sc;

  sc = rtems_task_start( ctx->worker_id, entry, (rtems_task_argument) ctx );
  T_assert_rsc_success( sc );
}

static int handler_open(
  rtems_libio_t *iop,
  const char    *path,
  int            oflag,
  mode_t         mode
)
{
  test_context *ctx;

  (void) path;
  (void) oflag;
  (void) mode;

  ctx = IMFS_generic_get_context_by_iop( iop );

  if ( ctx->close_busy ) {
    rtems_libio_iop_flags_set( iop, LIBIO_FLAGS_CLOSE_BUSY );
  }

  if ( ctx->publish_in_open ) {
    rtems_libio_iop_flags_set( iop, LIBIO_FLAGS_OPEN );
  }

  if ( ctx->wait_in_open ) {
    ctx->wait_in_open = false;
    ctx->fd_in_open = (int) rtems_libio_iop_to_descriptor( iop );
    wake_up( ctx->runner_id );
    wait();
  }

  if ( ctx->fail_open ) {
    errno = EIO;
    return -1;
  }

  return 0;
}

static int handler_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat                            *buf
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_location( loc );

  if ( ctx->wait_in_fstat ) {
    ctx->wait_in_fstat = false;
    wake_up( ctx->runner_id );
    wait();
  }

  return IMFS_stat( loc, buf );
}

static int handler_fcntl( rtems_libio_t *iop, int cmd )
{
  test_context *ctx;

  (void) cmd;

  ctx = IMFS_generic_get_context_by_iop( iop );

  if ( ctx->wait_in_fcntl ) {
    ctx->wait_in_fcntl = false;
    wake_up( ctx->runner_id );
    wait();
  }

  return 0;
}

static const rtems_filesystem_file_handlers_r node_handlers = {
  .open_h = handler_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = rtems_filesystem_default_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = handler_fstat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = handler_fcntl,
  .poll_h = rtems_filesystem_default_poll,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev,
  .mmap_h = rtems_filesystem_default_mmap
};

static const IMFS_node_control node_control = {
  .handlers = &node_handlers,
  .node_initialize = IMFS_node_initialize_generic,
  .node_remove = IMFS_node_remove_default,
  .node_destroy = IMFS_node_destroy_default
};

static void setup( void *arg )
{
  test_context     *ctx;
  rtems_status_code sc;
  int               rv;

  ctx = arg;
  ctx->runner_id = rtems_task_self();
  ctx->wait_in_open = false;
  ctx->wait_in_fstat = false;
  ctx->wait_in_fcntl = false;
  ctx->close_busy = false;
  ctx->fail_open = false;
  ctx->publish_in_open = false;
  ctx->fd_in_open = -1;
  ctx->fd = -1;
  ctx->status = -1;

  rv = IMFS_make_generic_node(
    PATH,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    ctx
  );
  T_assert_eq_int( rv, 0 );

  sc = rtems_task_create(
    rtems_build_name( 'W', 'O', 'R', 'K' ),
    WORKER_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_id
  );
  T_assert_rsc_success( sc );
}

static void teardown( void *arg )
{
  test_context     *ctx;
  rtems_status_code sc;
  int               rv;

  ctx = arg;

  sc = rtems_task_delete( ctx->worker_id );
  T_rsc_success( sc );

  rv = unlink( PATH );
  T_eq_int( rv, 0 );
}

static const T_fixture fixture =
  { .setup = setup, .teardown = teardown, .initial_context = &test_instance };

static void worker_open( rtems_task_argument arg )
{
  test_context *ctx;

  ctx = (test_context *) arg;
  ctx->wait_in_open = true;
  ctx->status = open( PATH, O_RDWR );
  wake_up( ctx->runner_id );
  (void) rtems_task_suspend( RTEMS_SELF );
}

/*
 * A task calls read() with the descriptor which an open() of another task
 * constructs.  The open() owns the iop until it returns.
 */
T_TEST_CASE_FIXTURE( StaleDescriptorDuringOpen, &fixture )
{
  test_context *ctx;
  size_t        free_before;
  char          buf[ 1 ];
  ssize_t       n;
  int           rv;

  ctx = T_fixture_context();
  free_before = free_iops();

  /* The worker blocks in the open handler */
  start_worker( ctx, worker_open );
  wait();
  T_assert_ge_int( ctx->fd_in_open, 0 );
  T_assert_eq_sz( free_iops(), free_before - 1 );

  errno = 0;
  n = read( ctx->fd_in_open, buf, sizeof( buf ) );
  T_eq_ssz( n, -1 );
  T_eq_int( errno, EBADF );

  /*
   * An iop on the free list has a cleared location.  The open() would
   * return a descriptor without handlers.
   */
  T_assert_eq_sz( free_iops(), free_before - 1 );

  wake_up( ctx->worker_id );
  wait();
  T_eq_int( ctx->status, ctx->fd_in_open );

  rv = close( ctx->status );
  T_eq_int( rv, 0 );
  T_eq_sz( free_iops(), free_before );
}

/*
 * The open() of the worker fails while the runner holds a reference through
 * the descriptor which the open() constructs.  The drop of the runner is the
 * last one.
 */
T_TEST_CASE_FIXTURE( StaleReferenceDuringFailedOpen, &fixture )
{
  test_context  *ctx;
  rtems_libio_t *iop;
  size_t         free_before;
  unsigned int   flags;

  ctx = T_fixture_context();
  free_before = free_iops();
  ctx->fail_open = true;

  /* The worker blocks in the open handler */
  start_worker( ctx, worker_open );
  wait();
  T_assert_ge_int( ctx->fd_in_open, 0 );
  iop = rtems_libio_iop( ctx->fd_in_open );

  flags = rtems_libio_iop_hold( iop );
  T_eq_uint( flags & LIBIO_FLAGS_OPEN, 0 );

  wake_up( ctx->worker_id );
  wait();
  T_eq_int( ctx->status, -1 );
  T_eq_sz( free_iops(), free_before - 1 );

  rtems_libio_iop_drop( iop );
  T_eq_sz( free_iops(), free_before );
}

static void worker_close( rtems_task_argument arg )
{
  test_context *ctx;

  ctx = (test_context *) arg;
  ctx->status = close( ctx->fd );
  wake_up( ctx->runner_id );
  (void) rtems_task_suspend( RTEMS_SELF );
}

static const rtems_rfs_format_config rfs_config = { .block_size = 512 };

static void rfs_setup( void *arg )
{
  test_context     *ctx;
  rtems_status_code sc;
  int               rv;

  ctx = arg;
  ctx->runner_id = rtems_task_self();
  ctx->fd = -1;
  ctx->status = -1;

  rv = rtems_rfs_format( RAMDISK_PATH, &rfs_config );
  T_assert_eq_int( rv, 0 );

  rv = mkdir( RFS_PATH, S_IRWXU | S_IRWXG | S_IRWXO );
  T_assert_eq_int( rv, 0 );

  rv = mount(
    RAMDISK_PATH,
    RFS_PATH,
    RTEMS_FILESYSTEM_TYPE_RFS,
    RTEMS_FILESYSTEM_READ_WRITE,
    NULL
  );
  T_assert_eq_int( rv, 0 );

  sc = rtems_task_create(
    rtems_build_name( 'W', 'O', 'R', 'K' ),
    WORKER_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_id
  );
  T_assert_rsc_success( sc );

  sc = rtems_task_create(
    rtems_build_name( 'S', 'T', 'A', 'T' ),
    WORKER_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->stat_worker_id
  );
  T_assert_rsc_success( sc );
}

static void rfs_teardown( void *arg )
{
  test_context     *ctx;
  rtems_status_code sc;
  int               rv;

  ctx = arg;

  sc = rtems_task_delete( ctx->stat_worker_id );
  T_rsc_success( sc );

  sc = rtems_task_delete( ctx->worker_id );
  T_rsc_success( sc );

  rv = unmount( RFS_PATH );
  T_eq_int( rv, 0 );

  rv = rmdir( RFS_PATH );
  T_eq_int( rv, 0 );
}

static const T_fixture rfs_fixture = {
  .setup = rfs_setup,
  .teardown = rfs_teardown,
  .initial_context = &test_instance
};

static void worker_stat( rtems_task_argument arg )
{
  test_context     *ctx;
  struct stat       st;
  rtems_status_code sc;

  ctx = (test_context *) arg;
  (void) stat( "/", &st );
  sc = rtems_event_send( ctx->runner_id, STAT_DONE );
  T_quiet_rsc_success( sc );
  (void) rtems_task_suspend( RTEMS_SELF );
}

/*
 * The worker frees the iop of an RFS file while an RFS operation holds the
 * instance lock of RFS.  The runner emulates this operation.  A stat() in
 * the IMFS root does not depend on the RFS operation.
 */
T_TEST_CASE_FIXTURE( FreeDuringFileSystemOperation, &rfs_fixture )
{
  test_context                     *ctx;
  rtems_filesystem_location_info_t *rfs_loc;
  rtems_event_set                   events;
  rtems_status_code                 sc;
  int                               dir_fd;
  int                               rv;

  ctx = T_fixture_context();

  dir_fd = open( RFS_PATH, O_RDONLY );
  T_assert_ge_int( dir_fd, 0 );
  rfs_loc = &rtems_libio_iop( dir_fd )->pathinfo;

  ctx->fd = open( RFS_FILE_PATH, O_RDWR | O_CREAT, S_IRWXU );
  T_assert_ge_int( ctx->fd, 0 );

  /* The worker blocks on the libio lock while it frees the iop */
  rtems_libio_lock();
  start_worker( ctx, worker_close );
  sc = rtems_event_transient_receive( RTEMS_NO_WAIT, 0 );
  T_rsc( sc, RTEMS_UNSATISFIED );

  rtems_filesystem_instance_lock( rfs_loc );
  rtems_libio_unlock();

  sc = rtems_task_start(
    ctx->stat_worker_id,
    worker_stat,
    (rtems_task_argument) ctx
  );
  T_rsc_success( sc );

  events = 0;
  sc = rtems_event_receive(
    STAT_DONE,
    RTEMS_EVENT_ALL | RTEMS_WAIT,
    STAT_TIMEOUT,
    &events
  );
  T_rsc_success( sc );

  rtems_filesystem_instance_unlock( rfs_loc );

  if ( events == 0 ) {
    sc = rtems_event_receive(
      STAT_DONE,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    T_rsc_success( sc );
  }

  wait();
  T_eq_int( ctx->status, 0 );

  rv = close( dir_fd );
  T_eq_int( rv, 0 );

  rv = unlink( RFS_FILE_PATH );
  T_eq_int( rv, 0 );
}

/*
 * The runner takes a reference while a close() of the worker frees the iop.
 * The hold and the drop emulate rtems_libio_get_iop() of a task which another
 * processor or a preemption interrupts between the two operations.
 */
T_TEST_CASE_FIXTURE( ReferenceDuringFree, &fixture )
{
  test_context     *ctx;
  rtems_libio_t    *iop;
  size_t            free_before;
  unsigned int      flags;
  rtems_status_code sc;

  ctx = T_fixture_context();
  free_before = free_iops();

  ctx->fd = open( PATH, O_RDWR );
  T_assert_ge_int( ctx->fd, 0 );
  iop = rtems_libio_iop( ctx->fd );

  /* The worker blocks on the libio lock while it frees the iop */
  rtems_libio_lock();
  start_worker( ctx, worker_close );
  sc = rtems_event_transient_receive( RTEMS_NO_WAIT, 0 );
  T_rsc( sc, RTEMS_UNSATISFIED );

  flags = rtems_libio_iop_hold( iop );
  T_eq_uint( flags & LIBIO_FLAGS_OPEN, 0 );

  rtems_libio_unlock();
  wait();
  T_eq_int( ctx->status, 0 );

  rtems_libio_iop_drop( iop );

  /* The reference count is zero */
  flags = rtems_libio_iop_flags( iop );
  T_lt_uint( flags, LIBIO_FLAGS_REFERENCE_INC );
  T_eq_sz( free_iops(), free_before );

  /* The other test cases need a zero reference count of the iop */
  while ( rtems_libio_iop_flags( iop ) >= LIBIO_FLAGS_REFERENCE_INC ) {
    (void) rtems_libio_iop_hold( iop );
  }
}

static void worker_fstat( rtems_task_argument arg )
{
  test_context *ctx;
  struct stat   st;

  ctx = (test_context *) arg;
  ctx->wait_in_fstat = true;
  ctx->status = fstat( ctx->fd, &st );
  wake_up( ctx->runner_id );
  (void) rtems_task_suspend( RTEMS_SELF );
}

/*
 * The close() of a file which allows a close with references succeeds while
 * the worker blocks in fstat().  The runner takes a reference through the
 * closed descriptor and drops it after the worker.
 */
T_TEST_CASE_FIXTURE( StaleReferenceAfterBusyClose, &fixture )
{
  test_context  *ctx;
  rtems_libio_t *iop;
  size_t         free_before;
  unsigned int   flags;
  int            rv;

  ctx = T_fixture_context();
  free_before = free_iops();

  ctx->close_busy = true;
  ctx->fd = open( PATH, O_RDWR );
  T_assert_ge_int( ctx->fd, 0 );
  iop = rtems_libio_iop( ctx->fd );

  /* The worker blocks in the fstat handler */
  start_worker( ctx, worker_fstat );
  wait();

  rv = close( ctx->fd );
  T_eq_int( rv, 0 );

  flags = rtems_libio_iop_hold( iop );
  T_eq_uint( flags & LIBIO_FLAGS_OPEN, 0 );

  wake_up( ctx->worker_id );
  wait();
  T_eq_sz( free_iops(), free_before - 1 );

  rtems_libio_iop_drop( iop );
  T_eq_sz( free_iops(), free_before );
}

/*
 * A task calls read() with the descriptor of the last iop, which is on the
 * free list since system initialization.
 */
T_TEST_CASE( StaleDescriptorOfLastIop )
{
  rtems_libio_t *iop;
  size_t         free_before;
  char           buf[ 1 ];
  ssize_t        n;
  int            fd;

  fd = (int) rtems_libio_number_iops - 1;
  iop = rtems_libio_iop( fd );
  T_assert_true( is_on_free_list( iop ) );
  free_before = free_iops();

  T_eq_uint(
    rtems_libio_iop_flags( iop ) & LIBIO_FLAGS_FREE,
    LIBIO_FLAGS_FREE
  );

  errno = 0;
  n = read( fd, buf, sizeof( buf ) );
  T_eq_ssz( n, -1 );
  T_eq_int( errno, EBADF );
  T_eq_sz( free_iops(), free_before );

  /* Break a cycle of the free list for the other test cases */
  if ( iop->data1 == iop ) {
    iop->data1 = NULL;
  }
}

/*
 * A task calls fcntl( F_DUP2FD ) with a target descriptor while the worker
 * blocks in fstat() through the target.  The file of the target does not
 * allow a close with references.
 */
T_TEST_CASE_FIXTURE( DuplicateOntoDescriptorInUse, &fixture )
{
  test_context *ctx;
  size_t        free_before;
  int           fd1;
  int           rv;

  ctx = T_fixture_context();
  free_before = free_iops();

  fd1 = open( PATH, O_RDWR );
  T_assert_ge_int( fd1, 0 );
  ctx->fd = open( PATH, O_RDWR );
  T_assert_ge_int( ctx->fd, 0 );

  /* The worker blocks in the fstat handler */
  start_worker( ctx, worker_fstat );
  wait();

  errno = 0;
  rv = fcntl( fd1, F_DUP2FD, ctx->fd );
  T_eq_int( rv, -1 );
  T_eq_int( errno, EBUSY );

  wake_up( ctx->worker_id );
  wait();

  rv = close( ctx->fd );
  T_eq_int( rv, 0 );
  rv = close( fd1 );
  T_eq_int( rv, 0 );
  T_eq_sz( free_iops(), free_before );
}

/*
 * The close() of a file which allows a close with references succeeds while
 * the worker blocks in fstat().  The fstat handler succeeds.
 */
T_TEST_CASE_FIXTURE( FstatDuringBusyClose, &fixture )
{
  test_context *ctx;
  size_t        free_before;
  int           rv;

  ctx = T_fixture_context();
  free_before = free_iops();

  ctx->close_busy = true;
  ctx->fd = open( PATH, O_RDWR );
  T_assert_ge_int( ctx->fd, 0 );

  /* The worker blocks in the fstat handler */
  start_worker( ctx, worker_fstat );
  wait();

  rv = close( ctx->fd );
  T_eq_int( rv, 0 );

  wake_up( ctx->worker_id );
  wait();
  T_eq_int( ctx->status, 0 );
  T_eq_sz( free_iops(), free_before );
}

static void worker_fcntl( rtems_task_argument arg )
{
  test_context *ctx;

  ctx = (test_context *) arg;
  ctx->wait_in_fcntl = true;
  ctx->status = fcntl( ctx->fd, F_DUPFD, 0 );
  wake_up( ctx->runner_id );
  (void) rtems_task_suspend( RTEMS_SELF );
}

/*
 * The close() of a file which allows a close with references succeeds while
 * the worker blocks in the fcntl handler of an F_DUPFD.  The new descriptor
 * exists when the fcntl handler returns.
 */
T_TEST_CASE_FIXTURE( FcntlDuringBusyClose, &fixture )
{
  test_context *ctx;
  size_t        free_before;
  int           rv;

  ctx = T_fixture_context();
  free_before = free_iops();

  ctx->close_busy = true;
  ctx->fd = open( PATH, O_RDWR );
  T_assert_ge_int( ctx->fd, 0 );

  /* The worker blocks in the fcntl handler */
  start_worker( ctx, worker_fcntl );
  wait();

  rv = close( ctx->fd );
  T_eq_int( rv, 0 );

  wake_up( ctx->worker_id );
  wait();
  T_assert_ge_int( ctx->status, 0 );

  rv = close( ctx->status );
  T_eq_int( rv, 0 );
  T_eq_sz( free_iops(), free_before );
}

/*
 * The open handler sets LIBIO_FLAGS_OPEN, then the open() fails because of
 * O_TRUNC without write access.
 */
T_TEST_CASE_FIXTURE( FailedOpenAfterPublish, &fixture )
{
  test_context *ctx;
  size_t        free_before;
  int           fd;

  ctx = T_fixture_context();
  free_before = free_iops();
  ctx->publish_in_open = true;

  errno = 0;
  fd = open( PATH, O_RDONLY | O_TRUNC );
  T_eq_int( fd, -1 );
  T_eq_int( errno, EINVAL );
  T_eq_sz( free_iops(), free_before );
}

static int check_is_open( rtems_libio_t *iop )
{
  rtems_libio_check_is_open( iop );
  return 0;
}

/*
 * The rtems_libio_check_is_open() macro accepts an open iop and returns EBADF
 * for an iop which is not open.
 */
T_TEST_CASE_FIXTURE( CheckIsOpen, &fixture )
{
  rtems_libio_t *iop;
  int            fd;
  int            rv;

  fd = open( PATH, O_RDWR );
  T_assert_ge_int( fd, 0 );
  iop = rtems_libio_iop( fd );

  errno = 0;
  rv = check_is_open( iop );
  T_eq_int( rv, 0 );
  T_eq_int( errno, 0 );

  rv = close( fd );
  T_eq_int( rv, 0 );

  errno = 0;
  rv = check_is_open( iop );
  T_eq_int( rv, -1 );
  T_eq_int( errno, EBADF );
}

static void worker_open_close( rtems_task_argument arg )
{
  test_context *ctx;

  ctx = (test_context *) arg;

  while ( true ) {
    wait();
    ctx->open_fd = open( PATH, O_RDWR );

    if ( ctx->open_fd >= 0 ) {
      ctx->close_status = close( ctx->open_fd );
    }
  }
}

static void stale_access_prepare( void *arg )
{
  test_context *ctx;

  ctx = arg;
  ctx->phase = 0;
  ctx->stale_fd = (int) rtems_libio_iop_to_descriptor(
    (rtems_libio_t *) rtems_libio_iop_free_head
  );
}

static void stale_access_action( void *arg )
{
  test_context *ctx;
  char          buf[ 1 ];

  ctx = arg;
  ctx->phase = 1;
  (void) read( ctx->stale_fd, buf, sizeof( buf ) );
  ctx->phase = 2;
}

static T_interrupt_test_state stale_access_interrupt( void *arg )
{
  test_context *ctx;
  unsigned int  flags;

  ctx = arg;

  if ( T_interrupt_test_get_state() != T_INTERRUPT_TEST_ACTION ) {
    return T_INTERRUPT_TEST_CONTINUE;
  }

  if ( ctx->phase == 0 ) {
    return T_INTERRUPT_TEST_EARLY;
  }

  if ( ctx->phase == 2 ) {
    return T_INTERRUPT_TEST_LATE;
  }

  flags = rtems_libio_iop_flags( rtems_libio_iop( ctx->stale_fd ) );

  if (
    ( flags & LIBIO_FLAGS_REFERENCE_MASK ) == 0 ||
    ( flags & LIBIO_FLAGS_OPEN ) != 0
  ) {
    return T_INTERRUPT_TEST_CONTINUE;
  }

  /* The worker preempts the action between its hold and its drop */
  wake_up( ctx->worker_id );
  return T_INTERRUPT_TEST_DONE;
}

static const T_interrupt_test_config stale_access_config = {
  .prepare = stale_access_prepare,
  .action = stale_access_action,
  .interrupt = stale_access_interrupt,
  .max_iteration_count = 1000
};

/*
 * A read() through a stale descriptor holds the iop while the worker opens
 * and closes the file through this iop.  The file does not allow a close with
 * references.
 */
T_TEST_CASE_FIXTURE( CloseDuringStaleAccess, &fixture )
{
  test_context          *ctx;
  T_interrupt_test_state state;
  size_t                 free_before;
  int                    rv;

  ctx = T_fixture_context();
  free_before = free_iops();
  ctx->open_fd = -1;
  ctx->close_status = -1;
  start_worker( ctx, worker_open_close );

  state = T_interrupt_test( &stale_access_config, ctx );

  if ( state == T_INTERRUPT_TEST_DONE ) {
    T_eq_int( ctx->open_fd, ctx->stale_fd );
    T_eq_int( ctx->close_status, 0 );

    if ( ctx->close_status != 0 && ctx->open_fd >= 0 ) {
      rv = close( ctx->open_fd );
      T_eq_int( rv, 0 );
    }
  } else {
    T_eq_int( state, T_INTERRUPT_TEST_TIMEOUT );
  }

  T_eq_sz( free_iops(), free_before );
}

static void Init( rtems_task_argument arg )
{
  rtems_status_code sc;

  /* The first block device starts the swapout task of the block buffer */
  sc = ramdisk_register( 512, 1024, false, RAMDISK_PATH );

  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_fatal( RTEMS_FATAL_SOURCE_APPLICATION, sc );
  }

  rtems_test_run( arg, TEST_STATE );
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_FILESYSTEM_RFS

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_MAXIMUM_TASKS 4

#define CONFIGURE_INIT_TASK_STACK_SIZE ( 32 * 1024 )

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_PRIORITY RUNNER_PRIORITY

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
