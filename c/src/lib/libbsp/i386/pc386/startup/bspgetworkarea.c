/*
 *  This routine is an implementation of the bsp_get_work_area()
 *  that can be used by all m68k BSPs following linkcmds conventions
 *  regarding heap, stack, and workspace allocation.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <bsp/bootcard.h>

/*
 *  These are provided by the linkcmds.
 */
extern char   WorkAreaBase[];
extern char   HeapSize[];
extern char   RamSize[];

/* rudimentary multiboot info */
struct multiboot_info {
  uint32_t  flags;       /* start.S only raises flags for items actually */
                         /* saved; this allows us to check for the size  */
                         /* of the data structure.                       */
  uint32_t  mem_lower;  /* avail kB in lower memory */
  uint32_t  mem_upper;  /* avail kB in lower memory */
  /* ... (unimplemented) */
};

extern struct multiboot_info _boot_multiboot_info;

/*
 * Board's memory size easily be overridden by application.
 */
uint32_t bsp_mem_size = 0;

/* Size of stack used during initialization. Defined in 'start.s'.  */
extern uint32_t _stack_size;

/* Address of start of free memory. */
uintptr_t rtemsFreeMemStart;


void bsp_size_memory(void)
{
  uintptr_t topAddr;
  uintptr_t lowest;
  uint32_t  val;
  int       i;

  /* set the value of start of free memory. */
  rtemsFreeMemStart = (uint32_t)WorkAreaBase + _stack_size;

  /* Place RTEMS workspace at beginning of free memory. */

  if (rtemsFreeMemStart & (CPU_ALIGNMENT - 1))  /* not aligned => align it */
    rtemsFreeMemStart = (rtemsFreeMemStart+CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  /* find the lowest 1M boundary to probe */
  lowest = ((rtemsFreeMemStart + (1<<20)) >> 20) + 1;
  if ( lowest  < 2 )
      lowest = 2;

  /* The memory detection algorithm is very crude; try
   * to use multiboot info, if possible (set from start.S)
   */
  if ( ((uintptr_t)RamSize == (uintptr_t) 0xFFFFFFFF)  &&
       (_boot_multiboot_info.flags & 1) &&
       _boot_multiboot_info.mem_upper ) {
    bsp_mem_size = _boot_multiboot_info.mem_upper * 1024;
  }

  if ( (uintptr_t) RamSize == (uintptr_t) 0xFFFFFFFF ) {
    /*
     * We have to dynamically size memory. Memory size can be anything
     * between no less than 2M and 2048M.
     * let us first write
     */
    for (i=2048; i>=lowest; i--) {
      topAddr = i*1024*1024 - 4;
      *(volatile uint32_t*)topAddr = topAddr;
    }

    for(i=lowest; i<=2048; i++) {
      topAddr = i*1024*1024 - 4;
      val =  *(uint32_t*)topAddr;
      if (val != topAddr) {
        break;
      }
    }
      
    topAddr = (i-1)*1024*1024 - 4;
  } else {
    topAddr = (uintptr_t) RamSize;
  }

  bsp_mem_size = topAddr;
}
/*
 *  This method returns the base address and size of the area which
 *  is to be allocated between the RTEMS Workspace and the C Program
 *  Heap.
 */
void bsp_get_work_area(
  void   **work_area_start,
  size_t  *work_area_size,
  void   **heap_start,
  size_t  *heap_size
)
{
  *work_area_start = (void *) rtemsFreeMemStart;
  *work_area_size  = (uintptr_t) bsp_mem_size - (uintptr_t) rtemsFreeMemStart;
  *heap_start      = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size       = (size_t) HeapSize;

  #if 0
    printk( "WorkArea Base = %p\n", *work_area_start );
    printk( "WorkArea Size = 0x%08x\n", *work_area_size );
    printk( "C Program Heap Base = %p\n", *heap_start );
    printk( "C Program Heap Size = 0x%08x\n", *heap_size );
  #endif
}
