/*  sys/utsname.h 
 *
 *  $Id$
 */

#ifndef __POSIX_SYS_UTSNAME_h
#define __POSIX_SYS_UTSNAME_h

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/times.h>
#include <sys/types.h>

/*
 *  4.4.1 Get System Name (Table 4-1), P1003.1b-1993, p. 90
 *
 *  NOTE:  The lengths of the strings in this structure are 
 *         just long enough to reliably contain the RTEMS information.
 *         For example, the fields are not long enough to support 
 *         Internet hostnames.
 */

struct utsname {
  char sysname[ 32 ];  /* Name of this implementation of the operating system */
  char nodename[ 32 ]; /* Name of this node within an implementation */
                       /*   specified communication network */
  char release[ 32 ];  /* Current release level of this implementation */
  char version[ 32 ];  /* Current version level of this release */
  char machine[ 32 ];  /* Name of the hardware type on which the system */
                       /*   is running */
};

/*
 *  4.4.1 Get System Name, P1003.1b-1993, p. 90
 */

int uname(
  struct utsname *name
);

/*
 *  4.5.2 Get Process Times, P1003.1b-1993, p. 92
 */

clock_t times(
  struct tms   *buffer
);

#ifdef __cplusplus
}
#endif
#endif
/* end of include file */

