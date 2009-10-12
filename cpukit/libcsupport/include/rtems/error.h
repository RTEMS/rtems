/**
 * @file rtems/error.h
 */

/*
 *  Defines and externs for rtems error reporting
 *
 *  $Id$
 */

#ifndef _RTEMS_RTEMS_ERROR_H
#define _RTEMS_RTEMS_ERROR_H

#include <rtems/score/interr.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef Internal_errors_t rtems_error_code_t;

/*
 * rtems_error() and rtems_panic() support
 */

#define RTEMS_ERROR_ERRNO  (1<<((sizeof(rtems_error_code_t) * CHAR_BIT) - 2)) /* hi bit; use 'errno' */
#define RTEMS_ERROR_PANIC  (RTEMS_ERROR_ERRNO / 2)       /* err fatal; no return */
#define RTEMS_ERROR_ABORT  (RTEMS_ERROR_ERRNO / 4)       /* err is fatal; panic */

#define RTEMS_ERROR_MASK  (RTEMS_ERROR_ERRNO | RTEMS_ERROR_ABORT | \
                             RTEMS_ERROR_PANIC) /* all */

const char *rtems_status_text(rtems_status_code);
int   rtems_error(rtems_error_code_t error_code, const char *printf_format, ...);
#ifdef __GNUC__
void  rtems_panic(const char *printf_format, ...);
/*
 *  We should be able to use this attribute but gcc complains that
 *  rtems_panic does in fact return. :(
 *
 *      __attribute__ ((__noreturn__));
 */
#else
void  rtems_panic(const char *printf_format, ...);
#endif

extern int rtems_panic_in_progress;

#ifdef __cplusplus
}
#endif


#endif
/* end of include file */
