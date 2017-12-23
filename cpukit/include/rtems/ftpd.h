/*
 *  FTP Server Information
 */

#ifndef _RTEMS_FTPD_H
#define _RTEMS_FTPD_H

#include <rtems/rtems/tasks.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FTPD_CONTROL_PORT   21

/* Various buffer sizes */
enum {
  FTPD_BUFSIZE  = 256,       /* Size for temporary buffers */
  FTPD_DATASIZE = 4 * 1024,      /* Size for file transfer buffers */
  FTPD_STACKSIZE = RTEMS_MINIMUM_STACK_SIZE + FTPD_DATASIZE /* Tasks stack size */
};

/* FTPD access control flags */
enum
{
  FTPD_NO_WRITE = 0x1,
  FTPD_NO_READ  = 0x2,
  FTPD_NO_RW    = FTPD_NO_WRITE | FTPD_NO_READ
};

typedef int (*rtems_ftpd_hookfunction)(char *, size_t);

#include <rtems/shell.h>

struct rtems_ftpd_hook
{
   char                    *filename;
   rtems_ftpd_hookfunction hook_function;
};

struct rtems_ftpd_configuration
{
   rtems_task_priority     priority;           /* FTPD task priority  */
   unsigned long           max_hook_filesize;  /* Maximum buffersize  */
                                               /*    for hooks        */
   int                     port;               /* Well-known port     */
   struct rtems_ftpd_hook  *hooks;             /* List of hooks       */
   char const              *root;              /* Root for FTPD or 0 for / */
   int                     tasks_count;        /* Max. connections    */
   int                     idle;               /* Idle timeout in seoconds
                                                  or 0 for no (inf) timeout */
   int                     access;             /* 0 - r/w, 1 - read-only,
                                                  2 - write-only,
                                                  3 - browse-only */
   rtems_shell_login_check_t login;            /* Login check or 0 to ignore
                                                  user/passwd. */
};

/*
 * Reply codes.
 */
#define PRELIM          1       /* positive preliminary */
#define COMPLETE        2       /* positive completion */
#define CONTINUE        3       /* positive intermediate */
#define TRANSIENT       4       /* transient negative completion */
#define ERROR           5       /* permanent negative completion */

int rtems_initialize_ftpd(void);

#ifdef __cplusplus
}
#endif

#endif  /* _RTEMS_FTPD_H */
