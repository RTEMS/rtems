/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if !defined(_DLFCN_SHELL_H_)
#define _DLFCN_SHELL_H_

int shell_dlopen (int argc, char* argv[]);
int shell_dlclose (int argc, char* argv[]);
int shell_dlsym (int argc, char* argv[]);
int shell_dlcall (int argc, char* argv[]);

#endif
