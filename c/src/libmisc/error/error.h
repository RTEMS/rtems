/*
 *	@(#)error.h	1.1 - 95/08/02
 *	
 *
 *  Defines and externs for rtems error reporting
 *
 *  $Id$
 */

/*
 * rtems_error() and rtems_panic() support
 */

#define RTEMS_ERROR_ERRNO  (1<<((sizeof(int) * 8) - 2)) /* hi bit; use 'errno' */
#define RTEMS_ERROR_PANIC  (RTEMS_ERROR_ERRNO / 2)       /* err fatal; no return */
#define RTEMS_ERROR_ABORT  (RTEMS_ERROR_ERRNO / 4)       /* err is fatal; panic */

#define RTEMS_ERROR_MASK  (RTEMS_ERROR_ERRNO | RTEMS_ERROR_ABORT | \
                             RTEMS_ERROR_PANIC) /* all */

char *rtems_status_text(rtems_status_code);
int   rtems_error(int error_code, char *printf_format, ...);
void  rtems_panic(char *printf_format, ...);

extern int rtems_panic_in_progress;

