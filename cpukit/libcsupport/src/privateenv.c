/*
 *  Instantatiate a private user environment for the calling thread.
 *
 *  Submitted by: fernando.ruiz@ctv.es (correo@fernando-ruiz.com)
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>	/* free */

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

extern Chain_Control rtems_filesystem_mount_table_control;

#define THE_ROOT_FS_LOC \
	(((rtems_filesystem_mount_table_entry_t*)\
	   rtems_filesystem_mount_table_control.first)->mt_fs_root)

/* cleanup a user environment
 * NOTE: this must be called with
 *       thread dispatching disabled!
 */
static void
free_user_env(void *venv)
{
  rtems_user_env_t *env = (rtems_user_env_t*) venv ;

	if (env != &rtems_global_user_env 
#ifdef HAVE_USERENV_REFCNT
		&& --env->refcnt <= 0
#endif
		) {
		rtems_filesystem_freenode( &env->current_directory);
		rtems_filesystem_freenode( &env->root_directory);
		free(env);
	}
}

rtems_status_code rtems_libio_set_private_env(void) {
  rtems_status_code 					sc;
  rtems_id          					task_id;
  rtems_filesystem_location_info_t		loc;

  sc=rtems_task_ident(RTEMS_SELF,0,&task_id);
  if (sc != RTEMS_SUCCESSFUL) return sc;

  /* Only for the first time a malloc is necesary */
  if (rtems_current_user_env==&rtems_global_user_env) {
   rtems_user_env_t	*tmp = malloc(sizeof(rtems_user_env_t));
   if (!tmp)
     return RTEMS_NO_MEMORY;

#ifdef HAVE_USERENV_REFCNT
   tmp->refcnt = 1;
#endif

   sc = rtems_task_variable_add(RTEMS_SELF,(void*)&rtems_current_user_env,(void(*)(void *))free_user_env);
   if (sc != RTEMS_SUCCESSFUL) {
	 /* don't use free_user_env because the pathlocs are
	  * not initialized yet
	  */
     free(tmp);
     return sc;
   }
   rtems_current_user_env = tmp;
  }; 

  *rtems_current_user_env = rtems_global_user_env; /* get the global values*/
  rtems_current_user_env->task_id=task_id;         /* mark the local values*/
  
  /* get a clean root */
  rtems_filesystem_root    = THE_ROOT_FS_LOC;

  /* Clone the pathlocs. In contrast to most other
   * code we must _not_ free the original locs because
   * what we are trying to do here is forking off
   * clones.
   */

  rtems_filesystem_evaluate_path("/", 0, &loc, 0);
  rtems_filesystem_root    = loc;
  rtems_filesystem_evaluate_path("/", 0, &loc, 0);
  rtems_filesystem_current = loc;

  return RTEMS_SUCCESSFUL;
}

/*
 *  Share a same private environment beetween two task:
 *   Task_id (remote) and RTEMS_SELF(current).
 */

/* NOTE:
 *
 * THIS CODE HAS NO PROTECTION IMPLEMENTED
 *
 * Tasks who wish to share their environments must
 *
 *  a) assert that no participants are concurrently
 *     executing
 *     libio_share_private_env() and/or libio_set_private_env()
 *
 *  b) mutex access to rtems_filesystem_current, rtems_filesytem_root
 *     while changing any of those (chdir(), chroot()).
 */

#ifndef HAVE_USERENV_REFCNT
rtems_status_code rtems_libio_share_private_env(rtems_id task_id) {
  rtems_status_code  sc;
  rtems_user_env_t * shared_user_env;
  rtems_id           current_task_id;

  sc=rtems_task_ident(RTEMS_SELF,0,&current_task_id);
  if (sc != RTEMS_SUCCESSFUL) return sc;

  if (rtems_current_user_env->task_id==current_task_id) {
   /* kill the current user env & task_var*/	  
	rtems_user_env_t 	*tmp = rtems_current_user_env;
   sc = rtems_task_variable_delete(RTEMS_SELF,(void*)&rtems_current_user_env);
   if (sc != RTEMS_SUCCESSFUL) return sc;
   free_user_env(tmp);	  
  };

  /* AT THIS POINT, rtems_current_user_env is DANGLING */

  sc = rtems_task_variable_get(task_id,(void*)&rtems_current_user_env,
		                       (void*)&shared_user_env       );
  if (sc != RTEMS_SUCCESSFUL)
    goto bailout;

  sc = rtems_task_variable_add(RTEMS_SELF,(void*)&rtems_current_user_env,free_user_env);
  if (sc != RTEMS_SUCCESSFUL)
    goto bailout;
  
  /* the current_user_env is the same pointer that remote env */
  rtems_current_user_env = shared_user_env;

  /* increase the reference count */
#ifdef HAVE_USERENV_REFCNT
  rtems_current_user_env->refcnt++;
#endif

  return RTEMS_SUCCESSFUL;

bailout:
  /* fallback to the global env */
  rtems_current_user_env = &rtems_global_user_env;
  return sc;
}
#endif
