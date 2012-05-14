/**
 * @file
 *
 * @ingroup LibIOEnv
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_USERENV_H
#define _RTEMS_USERENV_H

/*
 * According to IEEE Std 1003.1-2001,
 * limits.h is supposed to provide _POSIX_LOGIN_NAME_MAX
 * XXX: We do not rely on this.
 */
#include <limits.h>

#include <rtems.h>
#include <rtems/fs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LibIOEnv User Environment
 *
 * @ingroup LibIO
 *
 * @brief Provides a POSIX like user environment for tasks.
 *
 * @{
 */

#ifndef LOGIN_NAME_MAX
  #ifdef _POSIX_LOGIN_NAME_MAX
    #define LOGIN_NAME_MAX _POSIX_LOGIN_NAME_MAX
  #else
    /* Fallback */
    #define LOGIN_NAME_MAX 9
  #endif
#endif

typedef struct {
  rtems_filesystem_global_location_t *current_directory;
  rtems_filesystem_global_location_t *root_directory;
  /* Default mode for all files. */
  mode_t                           umask;
  /* _POSIX_types */
  uid_t                            uid;
  gid_t                            gid;
  uid_t                            euid;
  gid_t                            egid;
  char      login_buffer[LOGIN_NAME_MAX];
  pid_t                            pgrp; /* process group id */
  /* User environment maintenance */
  rtems_id                         task_id;
  int                              reference_count;
} rtems_user_env_t;

extern rtems_user_env_t * rtems_current_user_env;
extern rtems_user_env_t   rtems_global_user_env;

#define rtems_filesystem_current     (rtems_current_user_env->current_directory)
#define rtems_filesystem_root        (rtems_current_user_env->root_directory)
#define rtems_filesystem_umask       (rtems_current_user_env->umask)

#define _POSIX_types_Uid             (rtems_current_user_env->uid)
#define _POSIX_types_Gid             (rtems_current_user_env->gid)
#define _POSIX_types_Euid            (rtems_current_user_env->euid)
#define _POSIX_types_Egid            (rtems_current_user_env->egid)
#define _POSIX_types_Getlogin_buffer (rtems_current_user_env->login_buffer)

/**
 * @brief Creates a private environment.
 *
 * If the task has already a private environment nothing will be changed.  This
 * function must be called from normal thread context and may block on a mutex.
 * Thread dispatching is disabled to protect some critical sections.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_NO_MEMORY Not enough memory.
 * @retval RTEMS_UNSATISFIED Cloning of the current environment failed.
 * @retval RTEMS_TOO_MANY Cannot register the private environment.
 */
rtems_status_code rtems_libio_set_private_env(void);

/**
 * @brief Creates a private environment shared with another task.
 *
 * An attempt to share the environment with itself has no effect.  This
 * function must be called from normal thread context and may block on a mutex.
 * Thread dispatching is disabled to protect some critical sections.
 *
 * @param[in] task_id The private environment is shared with the task specified
 * by this identifier.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_UNSATISFIED No shared environment is available for this task
 * @retval RTEMS_TOO_MANY Cannot register the shared environment.
 * identifier.
 */
rtems_status_code rtems_libio_share_private_env(rtems_id task_id) ;

/**
 * @brief Use the global environment.
 *
 * A private environment will be released.  This function may be called from
 * every thread context.  Thread dispatching is disabled to protect the
 * critical sections.
 */
void rtems_libio_use_global_env(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
