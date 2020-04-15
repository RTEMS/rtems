/**
 *  @file
 *
 *  @brief Instantiate a Private User Environment
 *  @ingroup LibIOEnv
 */

/*
 *  Submitted by: fernando.ruiz@ctv.es (correo@fernando-ruiz.com)
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include <rtems/libio_.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/sysinit.h>

/**
 *  Instantiate a private user environment for the calling thread.
 */

static void rtems_libio_free_user_env(rtems_user_env_t *env)
{
  bool uses_global_env = env == &rtems_global_user_env;

  if (!uses_global_env) {
    rtems_filesystem_global_location_release(env->current_directory, false);
    rtems_filesystem_global_location_release(env->root_directory, false);
    free(env);
  }
}

rtems_status_code rtems_libio_set_private_env(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_user_env_t *old_env = rtems_current_user_env;
  bool uses_global_env = old_env == &rtems_global_user_env;

  if (uses_global_env) {
    Thread_Life_state life_state =
      _Thread_Set_life_protection(THREAD_LIFE_PROTECTED);
    rtems_user_env_t *new_env = calloc(1, sizeof(*new_env));

    if (new_env != NULL) {
      *new_env = *old_env;
      new_env->root_directory =
        rtems_filesystem_global_location_obtain(&old_env->root_directory);
      new_env->current_directory =
        rtems_filesystem_global_location_obtain(&old_env->current_directory);

      if (
        !rtems_filesystem_global_location_is_null(new_env->root_directory)
          && !rtems_filesystem_global_location_is_null(new_env->current_directory)
      ) {
        Thread_Control *executing = _Thread_Get_executing();

        executing->user_environment = new_env;
      } else {
        sc = RTEMS_UNSATISFIED;
      }

      if (sc != RTEMS_SUCCESSFUL) {
        rtems_libio_free_user_env(new_env);
      }
    } else {
      sc = RTEMS_NO_MEMORY;
    }

    _Thread_Set_life_protection(life_state);
  }

  return sc;
}

void rtems_libio_use_global_env(void)
{
  rtems_user_env_t *env = rtems_current_user_env;
  bool uses_private_env = env != &rtems_global_user_env;

  if (uses_private_env) {
    Thread_Life_state life_state =
      _Thread_Set_life_protection(THREAD_LIFE_PROTECTED);
    Thread_Control *executing;

    rtems_libio_free_user_env(env);
    executing = _Thread_Get_executing();
    executing->user_environment = NULL;

    _Thread_Set_life_protection(life_state);
  }
}

static void rtems_libio_env_thread_terminate(Thread_Control *the_thread)
{
  rtems_user_env_t *env = the_thread->user_environment;

  if (env != NULL) {
    rtems_libio_free_user_env(env);
  }
}

static void rtems_libio_env_thread_restart(
  Thread_Control *executing,
  Thread_Control *the_thread
)
{
  (void) executing;
  rtems_libio_env_thread_terminate( the_thread );
}

static User_extensions_Control rtems_libio_env_extensions = {
  .Callouts = {
    .thread_restart = rtems_libio_env_thread_restart,
    .thread_terminate = rtems_libio_env_thread_terminate
  }
};

static void rtems_libio_env_init(void)
{
  _User_extensions_Add_API_set(&rtems_libio_env_extensions);
}

RTEMS_SYSINIT_ITEM(
  rtems_libio_env_init,
  RTEMS_SYSINIT_USER_ENVIRONMENT,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
