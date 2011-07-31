/**
 * @file rtems/libio_.h
 */

/*
 *  Libio Internal Information
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_RTEMS_LIBIO__H
#define _RTEMS_RTEMS_LIBIO__H

#include <rtems.h>
#include <rtems/libio.h>                /* include before standard IO */

#include <sys/types.h>

#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Semaphore to protect the io table
 */

#define RTEMS_LIBIO_SEM         rtems_build_name('L', 'B', 'I', 'O')
#define RTEMS_LIBIO_IOP_SEM(n)  rtems_build_name('L', 'B', 'I', n)

extern rtems_id                          rtems_libio_semaphore;
extern const rtems_filesystem_file_handlers_r rtems_filesystem_null_handlers;

/*
 *  File descriptor Table Information
 */

extern uint32_t        rtems_libio_number_iops;
extern rtems_libio_t  *rtems_libio_iops;
extern rtems_libio_t  *rtems_libio_last_iop;
extern rtems_libio_t *rtems_libio_iop_freelist;

/*
 *  rtems_libio_iop
 *
 *  Macro to return the file descriptor pointer.
 */

#define rtems_libio_iop(_fd) \
  ((((uint32_t)(_fd)) < rtems_libio_number_iops) ? \
         &rtems_libio_iops[_fd] : 0)

/*
 *  rtems_libio_iop_to_descriptor
 *
 *  Macro to convert an internal file descriptor pointer (iop) into
 *  the integer file descriptor used by the "section 2" system calls.
 */

#define rtems_libio_iop_to_descriptor(_iop) \
   ((!(_iop)) ? -1 : (_iop - rtems_libio_iops))

/*
 *  rtems_libio_check_is_open
 *
 *  Macro to check if a file descriptor is actually open.
 */

#define rtems_libio_check_is_open(_iop) \
  do {                                               \
      if (((_iop)->flags & LIBIO_FLAGS_OPEN) == 0) { \
          errno = EBADF;                             \
          return -1;                                 \
      }                                              \
  } while (0)

/*
 *  rtems_libio_check_fd
 *
 *  Macro to check if a file descriptor number is valid.
 */

#define rtems_libio_check_fd(_fd) \
  do {                                                     \
      if ((uint32_t) (_fd) >= rtems_libio_number_iops) {   \
          errno = EBADF;                                   \
          return -1;                                       \
      }                                                    \
  } while (0)

/*
 *  rtems_libio_check_buffer
 *
 *  Macro to check if a buffer pointer is valid.
 */

#define rtems_libio_check_buffer(_buffer) \
  do {                                    \
      if ((_buffer) == 0) {               \
          errno = EINVAL;                 \
          return -1;                      \
      }                                   \
  } while (0)

/*
 *  rtems_libio_check_count
 *
 *  Macro to check if a count or length is valid.
 */

#define rtems_libio_check_count(_count) \
  do {                                  \
      if ((_count) == 0) {              \
          return 0;                     \
      }                                 \
  } while (0)

/*
 *  rtems_libio_check_permissions_with_error
 *
 *  Macro to check if a file descriptor is open for this operation.
 *  On failure, return the user specified error.
 */

#define rtems_libio_check_permissions_with_error(_iop, _flag, _errno) \
  do {                                                      \
      if (((_iop)->flags & (_flag)) == 0) {                 \
            rtems_set_errno_and_return_minus_one( _errno ); \
            return -1;                                      \
      }                                                     \
  } while (0)

/*
 *  rtems_libio_check_permissions
 *
 *  Macro to check if a file descriptor is open for this operation.
 *  On failure, return EINVAL
 */

#define rtems_libio_check_permissions(_iop, _flag) \
   rtems_libio_check_permissions_with_error(_iop, _flag, EINVAL )

/*
 *  rtems_filesystem_freenode
 *
 *  Macro to free a node.
 */

#define rtems_filesystem_freenode( _node ) \
  do { \
    if ( (_node)->ops )\
      if ( (_node)->ops->freenod_h ) \
        (*(_node)->ops->freenod_h)( (_node) ); \
  } while (0)


/*
 *  External structures
 */
#include <rtems/userenv.h>

extern rtems_user_env_t * rtems_current_user_env;
extern rtems_user_env_t   rtems_global_user_env;

/*
 *  Instantiate a private copy of the per user information for the calling task.
 */

rtems_status_code rtems_libio_set_private_env(void);
rtems_status_code rtems_libio_share_private_env(rtems_id task_id) ;

static inline void rtems_libio_lock( void )
{
  rtems_semaphore_obtain( rtems_libio_semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
}

static inline void rtems_libio_unlock( void )
{
  rtems_semaphore_release( rtems_libio_semaphore );
}

/*
 *  File Descriptor Routine Prototypes
 */

rtems_libio_t *rtems_libio_allocate(void);

uint32_t   rtems_libio_fcntl_flags(
  uint32_t   fcntl_flags
);

uint32_t   rtems_libio_to_fcntl_flags(
  uint32_t   flags
);

void rtems_libio_free(
  rtems_libio_t *iop
);

int rtems_libio_is_open_files_in_fs(
  rtems_filesystem_mount_table_entry_t *mt_entry
);

int rtems_libio_is_file_open(
  void  *node_access
);

/*
 *  File System Routine Prototypes
 */

int rtems_filesystem_evaluate_relative_path(
  const char                        *pathname,
  size_t                             pathnamelen,
  int                                flags,
  rtems_filesystem_location_info_t  *pathloc,
  int                                follow_link
);

int rtems_filesystem_evaluate_path(
  const char                        *pathname,
  size_t                             pathnamelen,
  int                                flags,
  rtems_filesystem_location_info_t  *pathloc,
  int                                follow_link
);

int rtems_filesystem_dirname(
  const char  *pathname
);

int rtems_filesystem_prefix_separators(
  const char  *pathname,
  int          pathnamelen
);

void rtems_filesystem_initialize(void);

int init_fs_mount_table(void);

int rtems_filesystem_is_separator(char ch);

void rtems_filesystem_get_start_loc(const char *path,
				    int *index,
				    rtems_filesystem_location_info_t *loc);

void rtems_filesystem_get_sym_start_loc(const char *path,
					int *index,
					rtems_filesystem_location_info_t *loc);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
