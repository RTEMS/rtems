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

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

rtems_status_code rtems_libio_set_private_env(void) {
  rtems_status_code sc;
  rtems_id          task_id;

  sc=rtems_task_ident(RTEMS_SELF,0,&task_id);
  if (sc != RTEMS_SUCCESSFUL) return sc;

  /* Only for the first time a malloc is necesary */
  if (rtems_current_user_env==&rtems_global_user_env) { 
   sc = rtems_task_variable_add(RTEMS_SELF,(void*)&rtems_current_user_env,free);
   if (sc != RTEMS_SUCCESSFUL) return sc;
   rtems_current_user_env = malloc(sizeof(rtems_user_env_t));
   if (!rtems_current_user_env)
     return RTEMS_NO_MEMORY;
  }; 

  /* the side effect desired . chroot("/") */
  *rtems_current_user_env = rtems_global_user_env; /* get the global values*/
  rtems_current_user_env->task_id=task_id;         /* mark the local values*/

  return RTEMS_SUCCESSFUL;
}

/*
 *  Share a same private environment beetween two task:
 *   Task_id (remote) and RTEMS_SELF(current).
 */

rtems_status_code rtems_libio_share_private_env(rtems_id task_id) {
  rtems_status_code  sc;
  rtems_user_env_t * shared_user_env;
  rtems_id           current_task_id;

  sc=rtems_task_ident(RTEMS_SELF,0,&current_task_id);
  if (sc != RTEMS_SUCCESSFUL) return sc;

  if (rtems_current_user_env->task_id==current_task_id) {
   /* kill the current user env & task_var*/	  
   free(rtems_current_user_env);	  
   sc = rtems_task_variable_delete(RTEMS_SELF,(void*)&rtems_current_user_env);
   if (sc != RTEMS_SUCCESSFUL) return sc;
  };

  sc = rtems_task_variable_get(task_id,(void*)&rtems_current_user_env,
		                       (void*)&shared_user_env       );
  if (sc != RTEMS_SUCCESSFUL) return sc;

  /* don't free(NULL'ed) at the task_delete. It is a shared var... */	   
  sc = rtems_task_variable_add(RTEMS_SELF,(void*)&rtems_current_user_env,NULL);
  if (sc != RTEMS_SUCCESSFUL) return sc;
  
  /* the current_user_env is the same pointer that remote env */
  rtems_current_user_env = shared_user_env;

  return RTEMS_SUCCESSFUL;
}
