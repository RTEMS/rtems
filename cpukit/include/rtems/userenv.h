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

#ifndef __rtems_userenv_h
#define __rtems_userenv_h

#include <rtems.h>
#include <rtems/fs.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  External structures
 */

/*
 * According to IEEE Std 1003.1-2001, 
 * limits.h is supposed to provide _POSIX_LOGIN_NAME_MAX
 * FIXME: We should not rely on this.
 */
#include <limits.h>

#ifdef _POSIX_LOGIN_NAME_MAX
#define LOGIN_NAME_MAX _POSIX_LOGIN_NAME_MAX
#else
/* Fallback */
#define LOGIN_NAME_MAX 9
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
	
#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
