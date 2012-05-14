/**
 * @file sys/utsname.h
 *
 * This include file defines the interface to the POSIX utsname() service.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __POSIX_SYS_UTSNAME_h
#define __POSIX_SYS_UTSNAME_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  4.4.1 Get System Name (Table 4-1), P1003.1b-1993, p. 90
 *
 *  NOTE:  The lengths of the strings in this structure are
 *         just long enough to reliably contain the RTEMS information.
 *         For example, the fields are not long enough to support
 *         Internet hostnames.
 */

#ifdef _KERNEL
#define SYS_NMLN        32              /* uname(2) for the FreeBSD 1.1 ABI. */
#endif

#ifndef SYS_NMLN
#define SYS_NMLN        32		/* User can override. */
#endif

struct utsname {
  char sysname[SYS_NMLN];  /* Name of this implementation of the operating system */
  char nodename[SYS_NMLN]; /* Name of this node within an implementation */
                           /*   specified communication network */
  char release[SYS_NMLN];  /* Current release level of this implementation */
  char version[SYS_NMLN];  /* Current version level of this release */
  char machine[SYS_NMLN];  /* Name of the hardware type on which the system */
                           /*   is running */
};

/*
 *  4.4.1 Get System Name, P1003.1b-1993, p. 90
 */

int uname(
  struct utsname *name
);

#ifdef __cplusplus
}
#endif
#endif
/* end of include file */
