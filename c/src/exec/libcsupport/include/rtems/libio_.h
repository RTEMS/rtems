/*
 *  Libio Internal Information
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __RTEMS_LIBIO_INTERNAL__h
#define __RTEMS_LIBIO_INTERNAL__h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/libio.h>                /* include before standard IO */
#include <rtems/assoc.h>                /* assoc.h not included by rtems.h */

#include <sys/types.h>                  
	
#include <stdio.h>                      /* O_RDONLY, et.al. */
#include <fcntl.h>                      /* O_RDONLY, et.al. */
#include <assert.h>
#include <stdarg.h>
#include <limits.h>
#include <errno.h>

#if ! defined(O_NDELAY)
# if defined(solaris2)
#  define O_NDELAY O_NONBLOCK
# elif defined(RTEMS_NEWLIB)
#  define O_NDELAY _FNBIO
# endif
#endif

#if !defined(ENOTSUP)
#define ENOTSUP EOPNOTSUPP
#endif

#include <errno.h>
#include <string.h>                     /* strcmp */
#include <unistd.h>
#include <stdlib.h>                     /* calloc() */

/*
 *  Semaphore to protect the io table
 */

#define RTEMS_LIBIO_SEM         rtems_build_name('L', 'B', 'I', 'O')
#define RTEMS_LIBIO_IOP_SEM(n)  rtems_build_name('L', 'B', 'I', n)

extern rtems_id                          rtems_libio_semaphore;
extern rtems_filesystem_file_handlers_r  rtems_filesystem_null_handlers;

/*
 *  File descriptor Table Information
 */

extern unsigned32      rtems_libio_number_iops;
extern rtems_libio_t  *rtems_libio_iops;
extern rtems_libio_t  *rtems_libio_last_iop;
extern rtems_libio_t *rtems_libio_iop_freelist;

/*
 *  set_errno_and_return_minus_one
 *
 *  Macro to ease common way to return an error.
 */

#ifndef set_errno_and_return_minus_one
#define set_errno_and_return_minus_one( _error ) \
  do { errno = (_error); return -1; } while(0)
#endif

/*
 *  rtems_libio_iop
 *
 *  Macro to return the file descriptor pointer.
 */

#define rtems_libio_iop(_fd) \
  ((((unsigned32)(_fd)) < rtems_libio_number_iops) ? \
         &rtems_libio_iops[_fd] : 0)

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
      if ((unsigned32) (_fd) >= rtems_libio_number_iops) { \
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
 *  rtems_libio_check_permissions
 *
 *  Macro to check if a file descriptor is open for this operation.
 */

#define rtems_libio_check_permissions(_iop, _flag)    \
  do {                                                \
      if (((_iop)->flags & (_flag)) == 0) {           \
            set_errno_and_return_minus_one( EINVAL ); \
            return -1;                                \
      }                                               \
  } while (0)

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
 *  rtems_filesystem_is_separator
 *
 *  Macro to determine if a character is a path name separator.
 *
 *  NOTE:  This macro handles MS-DOS and UNIX style names.
 */

#define rtems_filesystem_is_separator( _ch ) \
   ( ((_ch) == '/') || ((_ch) == '\\') || ((_ch) == '\0'))

/*
 *  rtems_filesystem_get_start_loc
 *
 *  Macro to determine if path is absolute or relative.
 */

#define rtems_filesystem_get_start_loc( _path, _index, _loc )  \
  do {                                                         \
    if ( rtems_filesystem_is_separator( (_path)[ 0 ] ) ) {     \
      *(_loc) = rtems_filesystem_root;                         \
      *(_index) = 1;                                           \
    } else {                                                   \
      *(_loc) = rtems_filesystem_current;                      \
      *(_index) = 0;                                           \
    }                                                          \
  } while (0)

#define rtems_filesystem_get_sym_start_loc( _path, _index, _loc )  \
  do {                                                         \
    if ( rtems_filesystem_is_separator( (_path)[ 0 ] ) ) {     \
      *(_loc) = rtems_filesystem_root;                         \
      *(_index) = 1;                                           \
    } else {                                                   \
      *(_index) = 0;                                           \
    }                                                          \
  } while (0)


/*
 *  External structures
 */
#if !defined(LOGIN_NAME_MAX)
#if defined(__linux__)
#define LOGIN_NAME_MAX _POSIX_LOGIN_NAME_MAX
#else
#error "don't know how to set LOGIN_NAME_MAX"
#endif
#endif

typedef struct {
 rtems_id                         task_id;	
 rtems_filesystem_location_info_t current_directory;
 rtems_filesystem_location_info_t root_directory;
 /* Default mode for all files. */
 mode_t                           umask;
 nlink_t                          link_counts;
 /* _POSIX_types */
 uid_t                            uid;
 gid_t                            gid;
 uid_t                            euid;
 gid_t                            egid;
 char      login_buffer[LOGIN_NAME_MAX];

 pid_t                            pgrp; /* process group id */
} rtems_user_env_t;

extern rtems_user_env_t * rtems_current_user_env; 
extern rtems_user_env_t   rtems_global_user_env; 

#define rtems_filesystem_current     (rtems_current_user_env->current_directory)
#define rtems_filesystem_root        (rtems_current_user_env->root_directory)
#define rtems_filesystem_link_counts (rtems_current_user_env->link_counts)
#define rtems_filesystem_umask       (rtems_current_user_env->umask)

#define _POSIX_types_Uid             (rtems_current_user_env->uid)
#define _POSIX_types_Gid             (rtems_current_user_env->gid)
#define _POSIX_types_Euid            (rtems_current_user_env->euid)
#define _POSIX_types_Egid            (rtems_current_user_env->egid)
#define _POSIX_types_Getlogin_buffer (rtems_current_user_env->login_buffer)


/*
 *  Instantiate a private copy of the per user information for the calling task.
 */

rtems_status_code rtems_libio_set_private_env(void);
rtems_status_code rtems_libio_share_private_env(rtems_id task_id) ;
	


/*
 *  File Descriptor Routine Prototypes
 */

rtems_libio_t *rtems_libio_allocate(void);

unsigned32 rtems_libio_fcntl_flags(
  unsigned32 fcntl_flags
);

unsigned32 rtems_libio_to_fcntl_flags(
  unsigned32 flags
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

int rtems_filesystem_evaluate_path(
  const char                        *pathname,
  int                                flags,
  rtems_filesystem_location_info_t  *pathloc,
  int                                follow_link
);

void rtems_filesystem_initialize();

int init_fs_mount_table();

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */



