/*
 *  $Id$
 */

#include <rtems.h>

#if defined(RTEMS_UNIXLIB)

void libc_init(int reentrant)
{
}

#else
 
/* remove ANSI errors.
 *  A program must contain at least one external-declaration
 *  (X3.159-1989 p.82,L3).
 */
void unixlibc_dummy_function( void )
{
}

#endif
