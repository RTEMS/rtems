/*
 *  RTEMS Malloc Block Boundary Integrity Checker
 *
 *  WARNING!!!  WARNING!!!  WARNING!!!  WARNING!!!
 *  WARNING!!!  WARNING!!!  WARNING!!!  WARNING!!!
 *
 *  This file is built but never called.  It is a first
 *  step in reintegrating this functionality.
 *  This code was disabled for a LONG time in malloc.c.
 *  This is a restructured and slightly modified version
 *  that should be able to be configured as a plugin BUT
 *  it has not been tested recently.  When it has been
 *  tested again, please remove this comment.
 *
 *  JOEL: I have not analyzed this code in terms of
 *        the heap changes post 4.6.  It is possible
 *        that that way the boundary area is carved
 *        off breaks the alignment.
 *
 *  WARNING!!!  WARNING!!!  WARNING!!!  WARNING!!!
 *  WARNING!!!  WARNING!!!  WARNING!!!  WARNING!!!
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "malloc_p.h"

#include <stdio.h>

#ifdef RTEMS_NEWLIB

#define SENTINELSIZE    12
#define SENTINEL       "\xD1\xAC\xB2\xF1" "BITE ME"
#define CALLCHAINSIZE 5

struct mallocNode {
  struct mallocNode *back;
  struct mallocNode *forw;
  int                callChain[CALLCHAINSIZE];
  size_t             size;
  void              *memory;
};

struct mallocNode mallocNodeHead;

void rtems_malloc_boundary_initialize(void)
{
  mallocNodeHead.back = &mallocNodeHead;
  mallocNodeHead.forw = &mallocNodeHead;
}

uint32_t rtems_malloc_boundary_overhead(void)
{
  return sizeof(struct mallocNode) + SENTINELSIZE;
}

void rtems_malloc_boundary_at_malloc(
  void     *pointer,
  size_t    size
)
{
  void *return_this;
  struct mallocNode *mp = (struct mallocNode *)pointer;
  int *fp, *nfp, i;

  _RTEMS_Lock_allocator();
    mp->memory = mp + 1;
    return_this = mp->memory;
    mp->size = size - (sizeof(struct mallocNode) + SENTINELSIZE);
    fp = (int *)&size - 2;
    for (i = 0 ; i < CALLCHAINSIZE ; i++) {
      mp->callChain[i] = fp[1];
      nfp = (int *)(fp[0]);
      if((nfp <= fp) || (nfp > (int *)(1 << 24)))
       break;
      fp = nfp;
    }
    while (i < CALLCHAINSIZE)
      mp->callChain[i++] = 0;
    memcpy((char *)mp->memory + mp->size, SENTINEL, SENTINELSIZE);
    mp->forw = mallocNodeHead.forw;
    mp->back = &mallocNodeHead;
    mallocNodeHead.forw->back = mp;
    mallocNodeHead.forw = mp;
  _RTEMS_Unlock_allocator();
}

void reportMallocError(const char *msg, struct mallocNode *mp);

void rtems_malloc_boundary_at_free(
  void     *pointer
)
{
  struct mallocNode *mp = (struct mallocNode *)pointer - 1;
  struct mallocNode *mp1;

  _RTEMS_Lock_allocator();
    if ((mp->memory != (mp + 1)) ||
        (memcmp((char *)mp->memory + mp->size, SENTINEL, SENTINELSIZE) != 0))
      reportMallocError("Freeing with inconsistent pointer/sentinel", mp);
    mp1 = mallocNodeHead.forw;
    while (mp1 != &mallocNodeHead) {
      if (mp1 == mp)
        break;
      mp1 = mp1->forw;
    }
    if (mp1 != mp)
      reportMallocError("Freeing, but not on allocated list", mp);
    mp->forw->back = mp->back;
    mp->back->forw = mp->forw;
    mp->back = mp->forw = NULL;
    pointer = mp;
  _RTEMS_Unlock_allocator();
}

void rtems_malloc_boundary_at_realloc(
  void     *pointer,
  size_t    size
)
{
  /* this needs to be implemented */
}

/*
 *  Malloc boundary support plugin
 */
rtems_malloc_boundary_functions_t rtems_malloc_boundary_functions_table = {
  rtems_malloc_boundary_initialize,
  rtems_malloc_boundary_overhead,
  rtems_malloc_boundary_at_malloc,
  rtems_malloc_boundary_at_free,
  rtems_malloc_boundary_at_realloc,
};

rtems_malloc_boundary_functions_t *rtems_malloc_boundary_helpers = NULL;
/*   &rtems_malloc_boundary_functions_table; */

void reportMallocError(const char *msg, struct mallocNode *mp)
{
    unsigned char *sp = (unsigned char *)mp->memory + mp->size;
    int i, ind = 0;
    static char cbuf[500];
    ind += sprintf(cbuf+ind, "Malloc Error: %s\n", msg);
    if ((mp->forw->back != mp) || (mp->back->forw != mp))
        ind += sprintf(cbuf+ind,
            "mp:%p  mp->forw:%p  mp->forw->back:%p  "
            "mp->back:%p  mp->back->forw:%p\n",
            mp, mp->forw, mp->forw->back, mp->back, mp->back->forw);
    if (mp->memory != (mp + 1))
        ind += sprintf(cbuf+ind, "mp+1:%p  ", mp + 1);
    ind += sprintf(cbuf+ind, "mp->memory:%p  mp->size:%li\n", mp->memory, mp->size);
    if (memcmp((char *)mp->memory + mp->size, SENTINEL, SENTINELSIZE) != 0) {
        ind += sprintf(cbuf+ind, "mp->sentinel: ");
        for (i = 0 ; i < SENTINELSIZE ; i++)
            ind += sprintf(cbuf+ind, " 0x%x", sp[i]);
        ind += sprintf(cbuf+ind, "\n");
    }
    ind += sprintf(cbuf+ind, "Call chain:");
    for (i = 0 ; i < CALLCHAINSIZE ; i++) {
        if (mp->callChain[i] == 0)
            break;
        ind += sprintf(cbuf+ind, " 0x%x", mp->callChain[i]);
    }
    printk("\n\n%s\n\n", cbuf);
}

void checkMallocArena(void)
{
  struct mallocNode *mp;

  _RTEMS_Lock_allocator();
    for ( mp = mallocNodeHead.forw; mp != &mallocNodeHead ; mp = mp->forw ) {
      if ((mp->forw->back != mp) || (mp->back->forw != mp))
        reportMallocError("Pointers mangled", mp);
      if ((mp->memory != (mp + 1)) ||
          (memcmp((char *)mp->memory + mp->size, SENTINEL, SENTINELSIZE) != 0))
        reportMallocError("Inconsistent pointer/sentinel", mp);
    }
  _RTEMS_Unlock_allocator();
}

#endif

