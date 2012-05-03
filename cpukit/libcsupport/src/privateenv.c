/**
 * @file
 *
 * @ingroup LibIOEnv
 */

/*
 *  Instantiate a private user environment for the calling thread.
 *
 *  Submitted by: fernando.ruiz@ctv.es (correo@fernando-ruiz.com)
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#include <stdlib.h>

#include <rtems/libio_.h>
#include <rtems/score/thread.h>

static void free_user_env(void *arg)
{
  rtems_user_env_t *env = arg;
  bool uses_global_env = env == &rtems_global_user_env;

  if (!uses_global_env) {
    if (env->reference_count == 1) {
      rtems_filesystem_global_location_release(env->current_directory);
      rtems_filesystem_global_location_release(env->root_directory);
      free(env);
    } else {
      --env->reference_count;
    }
  }
}

static void free_user_env_protected(rtems_user_env_t *env)
{
  _Thread_Disable_dispatch();
  free_user_env(env);
  _Thread_Enable_dispatch();
}

rtems_status_code rtems_libio_set_private_env(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_id self_task_id = rtems_task_self();
  rtems_user_env_t *old_env = rtems_current_user_env;
  bool uses_global_env = old_env == &rtems_global_user_env;
  bool uses_shared_env = old_env->task_id != self_task_id;

  if (uses_global_env || uses_shared_env) {
    rtems_user_env_t *new_env = calloc(1, sizeof(*new_env));

    if (new_env != NULL) {
      *new_env = *old_env;
      new_env->reference_count = 1;
      new_env->task_id = self_task_id;
      new_env->root_directory =
        rtems_filesystem_global_location_obtain(&old_env->root_directory);
      new_env->current_directory =
        rtems_filesystem_global_location_obtain(&old_env->current_directory);

      if (
        !rtems_filesystem_global_location_is_null(new_env->root_directory)
          && !rtems_filesystem_global_location_is_null(new_env->current_directory)
      ) {
        sc = rtems_task_variable_add(
          RTEMS_SELF,
          (void **) &rtems_current_user_env,
          free_user_env
        );
        if (sc == RTEMS_SUCCESSFUL) {
          free_user_env_protected(old_env);
          rtems_current_user_env = new_env;
        } else {
          sc = RTEMS_TOO_MANY;
        }
      } else {
        sc = RTEMS_UNSATISFIED;
      }

      if (sc != RTEMS_SUCCESSFUL) {
        free_user_env(new_env);
      }
    } else {
      sc = RTEMS_NO_MEMORY;
    }
  }

  return sc;
}

rtems_status_code rtems_libio_share_private_env(rtems_id task_id)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_id self_task_id = rtems_task_self();

  if (task_id != RTEMS_SELF && self_task_id != task_id) {
    rtems_user_env_t *env;

    /*
     * We have to disable the thread dispatching to prevent deletion of the
     * environment in the meantime.
     */
    _Thread_Disable_dispatch();
    sc = rtems_task_variable_get(
      task_id,
      (void *) &rtems_current_user_env, 
      (void *) &env
    );
    if (sc == RTEMS_SUCCESSFUL) {
      ++env->reference_count;
    } else {
      sc = RTEMS_UNSATISFIED;
    }
    _Thread_Enable_dispatch();

    if (sc == RTEMS_SUCCESSFUL) {
      sc = rtems_task_variable_add(
        RTEMS_SELF,
        (void **) &rtems_current_user_env,
        free_user_env
      );
      if (sc == RTEMS_SUCCESSFUL) {
        free_user_env_protected(rtems_current_user_env);
        rtems_current_user_env = env;
      } else {
        free_user_env_protected(env);
        sc = RTEMS_TOO_MANY;
      }
    }
  }

  return sc;
}

void rtems_libio_use_global_env(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_user_env_t *env = rtems_current_user_env;
  bool uses_private_env = env != &rtems_global_user_env;

  if (uses_private_env) {
    sc = rtems_task_variable_delete(
      RTEMS_SELF,
      (void **) &rtems_current_user_env
    );
    if (sc != RTEMS_SUCCESSFUL) {
      rtems_fatal_error_occurred(0xdeadbeef);
    }

    rtems_current_user_env = &rtems_global_user_env;
  }
}
