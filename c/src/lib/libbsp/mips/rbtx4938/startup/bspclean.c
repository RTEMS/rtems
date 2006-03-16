/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  bspclean.c,v 1.2.2.1 2003/09/04 18:44:49 joel Exp
 */

void bsp_cleanup( void )
{
  extern void _sys_exit(int);
  _sys_exit(0);
}
