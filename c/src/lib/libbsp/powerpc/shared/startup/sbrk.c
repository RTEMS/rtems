/* $Id$ */

/*
 *  sbrk.c
 *
 *  Author: Till Straumann <strauman@slac.stanford.edu>, 2002
 *
 *  Hack around the 32bit powerpc 32M problem:
 *
 *  GCC by default uses relative branches which can not jump
 *  farther than 32M. Hence all program text is confined to
 *  a single 32M segment.
 *  This hack gives the RTEMS malloc region all memory below
 *  32M at startup. Only when this region is exhausted will sbrk
 *  add more memory. Loading modules may fail at that point, hence
 *  the user is expected to load all modules at startup _prior_
 *  to malloc()ing lots of memory...
 *
 *  NOTE: it would probably be better to have a separate region
 *        for module code.
 */

#include <rtems.h>

#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

static rtems_unsigned32 remaining_start=0;
static rtems_unsigned32 remaining_size=0;

#define LIMIT_32M  0x02000000

rtems_unsigned32
_bsp_sbrk_init(rtems_unsigned32 heap_start, rtems_unsigned32 *heap_size_p)
{
  rtems_unsigned32 rval=0;

  remaining_start =  heap_start;
  remaining_size  =*  heap_size_p;
  if (remaining_start < LIMIT_32M &&
      remaining_start + remaining_size > LIMIT_32M) {
    /* clip at LIMIT_32M */
    rval = remaining_start + remaining_size - LIMIT_32M;
    *heap_size_p = LIMIT_32M - remaining_start;
  }
  return rval;
}

void * sbrk(ptrdiff_t incr)
{
  void *rval=(void*)-1;

  if (incr <= remaining_size) {
    remaining_size-=incr;
    rval = (void*)remaining_start;
    remaining_start += incr;
  } else {
    errno = ENOMEM;
  }
  return rval;
}

