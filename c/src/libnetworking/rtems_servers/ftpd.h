/*
 *  FTP Server Information
 *
 *  $Id$
 */

#ifndef __FTPD_H__
#define __FTPD_H__


#define FTPD_CONTROL_PORT   21

/*
 * Reply codes.
 */

#define PRELIM          1       /* positive preliminary */
#define COMPLETE        2       /* positive completion */
#define CONTINUE        3       /* positive intermediate */
#define TRANSIENT       4       /* transient negative completion */
#define ERROR           5       /* permanent negative completion */

void rtems_ftpd_sTart(rtems_task_priority priority);

#endif  /* __FTPD_H__ */

