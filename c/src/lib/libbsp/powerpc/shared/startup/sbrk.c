/*
 *  sbrk.c
 *
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2002,
 *        Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 *        under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

/*
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

static void *           remaining_start=(void*)-1LL;
static uintptr_t        remaining_size=0;

/* App. may provide a value by defining the BSP_sbrk_policy
 * variable.
 *
 *  (-1) -> give all memory to the heap at initialization time
 *  > 0  -> value used as sbrk amount; initially give 32M
 *    0  -> limit memory effectively to 32M.
 *
 */
extern uintptr_t        BSP_sbrk_policy __attribute__((weak));

#define LIMIT_32M  ((void*)0x02000000)

uintptr_t bsp_sbrk_init(
  void              *heap_start,
  uintptr_t         *heap_size_p
)
{
  uintptr_t         rval=0;
  uintptr_t         policy;

  remaining_start =  heap_start;
  remaining_size  = *heap_size_p;

  if (remaining_start < LIMIT_32M &&
      remaining_start + remaining_size > LIMIT_32M) {
    /* clip at LIMIT_32M */
    rval = remaining_start + remaining_size - LIMIT_32M;
    *heap_size_p = LIMIT_32M - remaining_start;
    remaining_start = LIMIT_32M;
    remaining_size  = rval;
  }

  policy = (0 == &BSP_sbrk_policy ? (uintptr_t)(-1) : BSP_sbrk_policy);
  switch ( policy ) {
      case (uintptr_t)(-1):
        *heap_size_p    += rval;
        remaining_start  = heap_start + *heap_size_p;
        remaining_size   = 0;
      break;

      case 0:
        remaining_size = 0;
      break;

      default:
        if ( rval > policy )
          rval = policy;
      break;
  }

  return rval;
}

/*
 * This is just so the sbrk test can force its magic. All normal applications
 * should just use the default implementation in this file.
 */
void *sbrk(ptrdiff_t incr) __attribute__ (( weak, alias("bsp_sbrk") ));
void *bsp_sbrk(ptrdiff_t incr)
{
  void *rval=(void*)-1;

  /* FIXME: BEWARE if size >2G */
  if ( remaining_start != (void*)-1LL && incr <= remaining_size) {
    remaining_size-=incr;
    rval = remaining_start;
    remaining_start += incr;
  } else {
    errno = ENOMEM;
  }
  #ifdef DEBUG
    printk("************* SBRK 0x%08x (ret 0x%08x) **********\n", incr, rval);
  #endif
  return rval;
}
