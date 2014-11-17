/**
 * @file
 *
 * @ingroup LibIOEnv
 * @brief User Environment Support
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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_USERENV_H
#define _RTEMS_USERENV_H

/*
 * According to IEEE Std 1003.1-2001,
 * limits.h is supposed to provide _POSIX_LOGIN_NAME_MAX
 * XXX: We do not rely on this.
 */
#include <sys/param.h>
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
 */
/**@{**/

#ifndef LOGIN_NAME_MAX
  #ifdef _POSIX_LOGIN_NAME_MAX
    #define LOGIN_NAME_MAX _POSIX_LOGIN_NAME_MAX
  #else
    /* Fallback */
    #define LOGIN_NAME_MAX 9
  #endif
#endif

/**
 * @brief User environment.
 */
typedef struct {
  /**
   * @brief The anchor directory for relative paths.
   */
  rtems_filesystem_global_location_t *current_directory;

  /**
   * @brief The anchor directory for absolute paths.
   */
  rtems_filesystem_global_location_t *root_directory;

  /**
   * @brief The file mode creation mask.
   */
  mode_t umask;

  /**
   * @brief The real user ID.
   */
  uid_t uid;

  /**
   * @brief The real group ID.
   */
  gid_t gid;

  /**
   * @brief The effective user ID.
   */
  uid_t euid;

  /**
   * @brief The effective group ID.
   */
  gid_t egid;

  /**
   * @brief The login buffer.
   */
  char login_buffer[LOGIN_NAME_MAX];

  /**
   * @brief The process group ID.
   */
  pid_t pgrp;

  /**
   * @brief The count of supplementary group IDs.
   */
  size_t ngroups;

  /**
   * @brief The list of supplementary group IDs.
   */
  gid_t groups[NGROUPS];
} rtems_user_env_t;

extern rtems_user_env_t rtems_global_user_env;

/**
 * @brief Fetch the pointer to the current user environment.
 *
 * If the task has a private user environment the pointer to it will be
 * returned. Otherwise the pointer to rtems_global_user_env will be returned.
 */
rtems_user_env_t * rtems_current_user_env_get(void);

#define rtems_current_user_env rtems_current_user_env_get()

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
 * The private environment internally uses a POSIX key. The key is added to the
 * configuration implicitly. But for each thread that uses a private environment
 * a key value pair has to be configured by the application. If only the global
 * environment is used there is no need to configure a key value pair.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_NO_MEMORY Not enough memory.
 * @retval RTEMS_UNSATISFIED Cloning of the current environment failed.
 * @retval RTEMS_TOO_MANY Cannot register the private environment.
 */
rtems_status_code rtems_libio_set_private_env(void);

/**
 * @brief Use the global environment.
 *
 * A private environment will be released.  This function may be called from
 * every thread context.  Thread dispatching is disabled to protect the
 * critical sections.
 */
void rtems_libio_use_global_env(void);

/**
 * @brief Gets the supplementary group IDs using the current user ID and
 * updates the table of supplementary group IDs in the current user
 * environment.
 *
 * In case of an error, the count of supplementary group IDs is set to zero.
 */
void rtems_current_user_env_getgroups(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
