/*
 *  RTEMS Malloc Family Implementation
 *
 *
 *  COPYRIGHT (c) 1989-1999.
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

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>
#include <rtems/libcsupport.h>
#include <rtems/score/protectedheap.h>
#ifdef RTEMS_NEWLIB
#include <sys/reent.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>    /* sbrk(2) */
#include <inttypes.h>

#include <rtems/chain.h>

#ifndef HAVE_UINTMAX_T
/* Fall back to unsigned long if uintmax_t is not available */
#define unsigned long uintmax_t

#ifndef PRIuMAX
#define PRIuMAX		"lu"
#endif
#endif

#ifdef MALLOC_ARENA_CHECK
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
static struct mallocNode mallocNodeHead = { &mallocNodeHead, &mallocNodeHead };
void reportMallocError(const char *msg, struct mallocNode *mp)
{
    unsigned char *sp = (unsigned char *)mp->memory + mp->size;
    int i, ind = 0;
    static char cbuf[500];
    ind += sprintf(cbuf+ind, "Malloc Error: %s\n", msg);
    if ((mp->forw->back != mp) || (mp->back->forw != mp))
        ind += sprintf(cbuf+ind, "mp:0x%x  mp->forw:0x%x  mp->forw->back:0x%x  mp->back:0x%x  mp->back->forw:0x%x\n",
                        mp, mp->forw, mp->forw->back, mp->back, mp->back->forw);
    if (mp->memory != (mp + 1))
        ind += sprintf(cbuf+ind, "mp+1:0x%x  ", mp + 1);
    ind += sprintf(cbuf+ind, "mp->memory:0x%x  mp->size:%d\n", mp->memory, mp->size);
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
#endif

Heap_Control  RTEMS_Malloc_Heap;
Chain_Control RTEMS_Malloc_GC_list;

/* rtems_id RTEMS_Malloc_Heap; */
size_t RTEMS_Malloc_Sbrk_amount;

#ifdef RTEMS_DEBUG
#define MALLOC_STATS
#define MALLOC_DIRTY
#endif

#ifdef MALLOC_STATS
#define MSBUMP(f,n)    rtems_malloc_stats.f += (n)

struct {
    uint32_t    space_available;             /* current size of malloc area */
    uint32_t    malloc_calls;                /* # calls to malloc */
    uint32_t    free_calls;
    uint32_t    realloc_calls;
    uint32_t    calloc_calls;
    uint32_t    max_depth;		     /* most ever malloc'd at 1 time */
    uintmax_t    lifetime_allocated;
    uintmax_t    lifetime_freed;
} rtems_malloc_stats;

#else			/* No rtems_malloc_stats */
#define MSBUMP(f,n)
#endif

void RTEMS_Malloc_Initialize(
  void   *start,
  size_t  length,
  size_t  sbrk_amount
)
{
  uint32_t      status;
  void         *starting_address;
  uintptr_t     old_address;
  uintptr_t     uaddress;

  /*
   *  Initialize the garbage collection list to start with nothing on it.
   */
  Chain_Initialize_empty(&RTEMS_Malloc_GC_list);

  /*
   * If the starting address is 0 then we are to attempt to
   * get length worth of memory using sbrk. Make sure we
   * align the address that we get back.
   */

  starting_address = start;
  RTEMS_Malloc_Sbrk_amount = sbrk_amount;

  if (!starting_address) {
    uaddress = (uintptr_t)sbrk(length);

    if (uaddress == (uintptr_t) -1) {
      rtems_fatal_error_occurred( RTEMS_NO_MEMORY );
      /* DOES NOT RETURN!!! */
    }

    if (uaddress & (CPU_HEAP_ALIGNMENT-1)) {
      old_address = uaddress;
      uaddress = (uaddress + CPU_HEAP_ALIGNMENT) & ~(CPU_HEAP_ALIGNMENT-1);

       /*
	* adjust the length by whatever we aligned by
	*/

      length -= uaddress - old_address;
    }

    starting_address = (void *)uaddress;
  }

  /*
   *  If the BSP is not clearing out the workspace, then it is most likely
   *  not clearing out the initial memory for the heap.  There is no
   *  standard supporting zeroing out the heap memory.  But much code
   *  with UNIX history seems to assume that memory malloc'ed during
   *  initialization (before any free's) is zero'ed.  This is true most
   *  of the time under UNIX because zero'ing memory when it is first
   *  given to a process eliminates the chance of a process seeing data
   *  left over from another process.  This would be a security violation.
   */

  if ( rtems_cpu_configuration_get_do_zero_of_workspace() )
     memset( starting_address, 0, length );

  /*
   *  Unfortunately we cannot use assert if this fails because if this
   *  has failed we do not have a heap and if we do not have a heap
   *  STDIO cannot work because there will be no buffers.
   */

  status = _Protected_heap_Initialize( 
    &RTEMS_Malloc_Heap,
    starting_address,
    length,
    CPU_HEAP_ALIGNMENT
  );
  if ( !status )
    rtems_fatal_error_occurred( status );

#ifdef MALLOC_STATS
  /* zero all the stats */
  (void) memset( &rtems_malloc_stats, 0, sizeof(rtems_malloc_stats) );
#endif

  MSBUMP(space_available, length);
}

#ifdef RTEMS_NEWLIB
void *malloc(
  size_t  size
)
{
  void        *return_this;
  void        *starting_address;
  uint32_t     the_size;
  uint32_t     sbrk_amount;
  Chain_Node  *to_be_freed;

  MSBUMP(malloc_calls, 1);

  if ( !size )
    return (void *) 0;

  /*
   *  Do not attempt to allocate memory if in a critical section or ISR.
   */

  if (_System_state_Is_up(_System_state_Get())) {
    if (_Thread_Dispatch_disable_level > 0)
      return (void *) 0;

    if (_ISR_Nest_level > 0)
      return (void *) 0;
  }

  /*
   *  If some free's have been deferred, then do them now.
   */
  while ((to_be_freed = Chain_Get(&RTEMS_Malloc_GC_list)) != NULL)
    free(to_be_freed);

  /*
   * Try to give a segment in the current heap if there is not
   * enough space then try to grow the heap.
   * If this fails then return a NULL pointer.
   */

#ifdef MALLOC_ARENA_CHECK
  size += sizeof(struct mallocNode) + SENTINELSIZE;
#endif
  return_this = _Protected_heap_Allocate( &RTEMS_Malloc_Heap, size );

  if ( !return_this ) {
    /*
     *  Round to the "requested sbrk amount" so hopefully we won't have
     *  to grow again for a while.  This effectively does sbrk() calls
     *  in "page" amounts.
     */

    sbrk_amount = RTEMS_Malloc_Sbrk_amount;

    if ( sbrk_amount == 0 )
      return (void *) 0;

    the_size = ((size + sbrk_amount) / sbrk_amount * sbrk_amount);

    if ((starting_address = (void *)sbrk(the_size))
            == (void*) -1)
      return (void *) 0;

    if ( !_Protected_heap_Extend(
            &RTEMS_Malloc_Heap, starting_address, the_size) ) {
      sbrk(-the_size);
      errno = ENOMEM;
      return (void *) 0;
    }

    MSBUMP(space_available, the_size);

    return_this = _Protected_heap_Allocate( &RTEMS_Malloc_Heap, size );
    if ( !return_this ) {
      errno = ENOMEM;
      return (void *) 0;
    }
  }

#ifdef MALLOC_STATS
  if (return_this)
  {
      size_t     actual_size = 0;
      uint32_t   current_depth;
      Protected_heap_Get_block_size(&RTEMS_Malloc_Heap, ptr, &actual_size);
      MSBUMP(lifetime_allocated, actual_size);
      current_depth = rtems_malloc_stats.lifetime_allocated -
                   rtems_malloc_stats.lifetime_freed;
      if (current_depth > rtems_malloc_stats.max_depth)
          rtems_malloc_stats.max_depth = current_depth;
  }
#endif

#ifdef MALLOC_DIRTY
  (void) memset(return_this, 0xCF, size);
#endif

#ifdef MALLOC_ARENA_CHECK
  {
  struct mallocNode *mp = (struct mallocNode *)return_this;
  int key, *fp, *nfp, i;
  rtems_interrupt_disable(key);
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
  rtems_interrupt_enable(key);
  }
#endif
  return return_this;
}

void *calloc(
  size_t nelem,
  size_t elsize
)
{
  register char *cptr;
  int length;

  MSBUMP(calloc_calls, 1);

  length = nelem * elsize;
  cptr = malloc( length );
  if ( cptr )
    memset( cptr, '\0', length );

  MSBUMP(malloc_calls, -1);   /* subtract off the malloc */

  return cptr;
}

void *realloc(
  void *ptr,
  size_t size
)
{
  size_t old_size;
  char *new_area;

  MSBUMP(realloc_calls, 1);

  /*
   *  Do not attempt to allocate memory if in a critical section or ISR.
   */

  if (_System_state_Is_up(_System_state_Get())) {
    if (_Thread_Dispatch_disable_level > 0)
      return (void *) 0;

    if (_ISR_Nest_level > 0)
      return (void *) 0;
  }

  /*
   * Continue with realloc().
   */
  if ( !ptr )
    return malloc( size );

  if ( !size ) {
    free( ptr );
    return (void *) 0;
  }

#ifdef MALLOC_ARENA_CHECK
  {
    void *np;
    np = malloc(size);
    if (!np) return np;
    memcpy(np,ptr,size);
    free(ptr);
    return np;
  }
#endif
  if ( _Protected_heap_Resize_block( &RTEMS_Malloc_Heap, ptr, size ) ) {
    return ptr;
  }

  new_area = malloc( size );

  MSBUMP(malloc_calls, -1);   /* subtract off the malloc */

  /*
   *  There used to be a free on this error case but it is wrong to
   *  free the memory per OpenGroup Single UNIX Specification V2
   *  and the C Standard.
   */

  if ( !new_area ) {
    return (void *) 0;
  }

  if ( !_Protected_heap_Get_block_size(&RTEMS_Malloc_Heap, ptr, &old_size) ) {
    errno = EINVAL;
    return (void *) 0;
  }

  memcpy( new_area, ptr, (size < old_size) ? size : old_size );
  free( ptr );

  return new_area;

}

void free(
  void *ptr
)
{
  MSBUMP(free_calls, 1);

  if ( !ptr )
    return;

  /*
   *  Do not attempt to free memory if in a critical section or ISR.
   */

  if (_System_state_Is_up(_System_state_Get())) {
    if ((_Thread_Dispatch_disable_level > 0) || (_ISR_Nest_level > 0)) {
      Chain_Append(&RTEMS_Malloc_GC_list, (Chain_Node *)ptr);
      return;
    }
  }

#ifdef MALLOC_ARENA_CHECK
  {
  struct mallocNode *mp = (struct mallocNode *)ptr - 1;
  struct mallocNode *mp1;
  int key;
  rtems_interrupt_disable(key);
  if ((mp->memory != (mp + 1))
   || (memcmp((char *)mp->memory + mp->size, SENTINEL, SENTINELSIZE) != 0))
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
  ptr = mp;
  rtems_interrupt_enable(key);
  }
#endif
#ifdef MALLOC_STATS
  {
    size_t size;
    if (Protected_heap_Get_block_size(&RTEMS_Malloc_Heap, ptr, &size) ) {
      MSBUMP(lifetime_freed, size);
    }
  }
#endif

  if ( !_Protected_heap_Free( &RTEMS_Malloc_Heap, ptr ) ) {
    errno = EINVAL;
    assert( 0 );
  }
}

#ifdef MALLOC_ARENA_CHECK
void checkMallocArena(void)
{
    struct mallocNode *mp = mallocNodeHead.forw;
    int key;
    rtems_interrupt_disable(key);
    while (mp != &mallocNodeHead) {
        if ((mp->forw->back != mp)
         || (mp->back->forw != mp))
            reportMallocError("Pointers mangled", mp);
        if((mp->memory != (mp + 1))
         || (memcmp((char *)mp->memory + mp->size, SENTINEL, SENTINELSIZE) != 0))
            reportMallocError("Inconsistent pointer/sentinel", mp);
        mp = mp->forw;
    }
    rtems_interrupt_enable(key);
}
#endif

/* end if RTEMS_NEWLIB */
#endif

#ifdef MALLOC_STATS
/*
 * Dump the malloc statistics
 * May be called via atexit()  (installable by our bsp) or
 * at any time by user
 */

void malloc_dump(void)
{
    uint32_t   allocated = rtems_malloc_stats.lifetime_allocated -
                     rtems_malloc_stats.lifetime_freed;

    printf("Malloc stats\n");
    printf("  avail:%"PRIu32"k  allocated:%"PRIu32"k (%"PRId32"%%) "
              "max:%"PRIu32"k (%"PRIu32"%%)"
              " lifetime:%"PRIuMAX"k freed:%"PRIuMAX"k\n",
           rtems_malloc_stats.space_available / 1024,
           allocated / 1024,
           /* avoid float! */
           (allocated * 100) / rtems_malloc_stats.space_available,
           rtems_malloc_stats.max_depth / 1024,
           (rtems_malloc_stats.max_depth * 100) / rtems_malloc_stats.space_available,
           rtems_malloc_stats.lifetime_allocated / 1024,
           rtems_malloc_stats.lifetime_freed / 1024
           );
    printf("  Call counts:   malloc:%"PRIu32"   free:%"PRIu32"   realloc:%"PRIu32"   calloc:%"PRIu32"\n",
           rtems_malloc_stats.malloc_calls,
           rtems_malloc_stats.free_calls,
           rtems_malloc_stats.realloc_calls,
           rtems_malloc_stats.calloc_calls);
}


void malloc_walk(size_t source, size_t printf_enabled)
{
  _Protected_heap_Walk( &RTEMS_Malloc_Heap, source, printf_enabled );
}

#else

void malloc_dump(void)
{
   return;
}

void malloc_walk(size_t source, size_t printf_enabled)
{
   return;
}

#endif

/*
 *  "Reentrant" versions of the above routines implemented above.
 */

#ifdef RTEMS_NEWLIB
void *_malloc_r(
  struct _reent *ignored,
  size_t  size
)
{
  return malloc( size );
}

void *_calloc_r(
  struct _reent *ignored,
  size_t nelem,
  size_t elsize
)
{
  return calloc( nelem, elsize );
}

void *_realloc_r(
  struct _reent *ignored,
  void *ptr,
  size_t size
)
{
  return realloc( ptr, size );
}

void _free_r(
  struct _reent *ignored,
  void *ptr
)
{
  free( ptr );
}

#endif
