/**
 * @file rtems/error.h
 *
 * Defines and externs for rtems error reporting
 */


#ifndef _RTEMS_RTEMS_ERROR_H
#define _RTEMS_RTEMS_ERROR_H

#include <rtems/rtems/status.h>
#include <rtems/score/interr.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef Internal_errors_t rtems_error_code_t;

/*
 * rtems_error() and rtems_panic() support
 */

#if 0
/* not 16bit-int host clean */
#define RTEMS_ERROR_ERRNO  (1<<((sizeof(rtems_error_code_t) * CHAR_BIT) - 2)) /* hi bit; use 'errno' */
#define RTEMS_ERROR_PANIC  (RTEMS_ERROR_ERRNO / 2)       /* err fatal; no return */
#define RTEMS_ERROR_ABORT  (RTEMS_ERROR_ERRNO / 4)       /* err is fatal; panic */
#else
#define RTEMS_ERROR_ERRNO  (0x40000000) /* hi bit; use 'errno' */
#define RTEMS_ERROR_PANIC  (0x20000000) /* err fatal; no return */
#define RTEMS_ERROR_ABORT  (0x10000000) /* err is fatal; panic */
#endif

#define RTEMS_ERROR_MASK \
  (RTEMS_ERROR_ERRNO | RTEMS_ERROR_ABORT | RTEMS_ERROR_PANIC) /* all */

const char *rtems_status_text(rtems_status_code sc);
int   rtems_error(
  rtems_error_code_t error_code,
  const char *printf_format,
  ...
);
void rtems_panic(
  const char *printf_format,
  ...
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

extern int rtems_panic_in_progress;

#ifdef __cplusplus
}
#endif


#endif
/* end of include file */
