/*
 *  Extracted from smp-imps.c
 *
 *  <Insert copyright here : it must be BSD-like so anyone can use it>
 *
 *  Author:  Erich Boleyn  <erich@uruk.org>   http://www.uruk.org/~erich/
 *
 *  $Id$
 */

int bsp_smp_processor_id(void)
{
  unsigned int id;
  __asm__ __volatile__( "rd     %%asr17,%0\n\t" : "=r" (id) : );

  return ((id >> 28) & 0xff);
}

