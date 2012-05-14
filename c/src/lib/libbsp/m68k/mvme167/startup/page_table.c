/*  page_table.c
 *
 *  The code submitted by Eric Vaitl <vaitl@viasat.com> for the MVME162 appears
 *  to be for a uniprocessor implementation. The function that sets up the
 *  page tables, page_table_init(), is not data driven. For all processors, it
 *  sets up page tables to map virtual addresses from 0x20000 to 0x3FFFFF to
 *  physical addresses 0x20000 to 0x3FFFFF. This presumably maps a subset of
 *  a local 4 MB space, which is probably the amount of RAM on Eric Vailt's
 *  MVME162.
 *
 *  It is possible to set up the various bus bridges in the MVME167s to create
 *  a flat physical address space across multiple boards, i.e., it is possible
 *  for each MVME167 in a multiprocessor system to access a given memory
 *  location using the same physical address, whether that location is in local
 *  or VME space. Addres translation can be set up so that each virtual address
 *  maps to its corresponding physical address, e.g. virtual address 0x12345678
 *  is mapped to physical address 0x12345678. With this mapping, the MMU is
 *  only used to control the caching modes for the various regions of memory.
 *  Mapping the virtual addresses to their corresponding physical address makes
 *  it unnecessary to map addresses under software control during the
 *  initialization of RTEMS, before address translation is turned on.
 *
 *  With the above approach, address translation may be set up either with the
 *  transparent address translation registers, or with page tables. If page
 *  tables are used, a more efficient use of page table space can be achieved
 *  by sharing the page tables between processors. The entire page table tree
 *  can be shared, or each processor can hold a private copy of the top nodes
 *  which point to leaf nodes stored on individual processors.
 *
 *  In this port, only the transparent address translation registers are used.
 *  We map the entire virtual range from 0x0 to 0x7FFFFFFF to the identical
 *  physical range 0x0 to 0x7FFFFFFF. We rely on the hardware to signal bus
 *  errors if we address non-existent memory within this range. Our two
 *  MVME167s are configured to exist at physical addresses 0x00800000 to
 *  0x00BFFFFF and 0x00C00000 to 0x00FFFFFF respectively. If jumper J1-4 is
 *  installed, memory and cache control can be done by providing parameters
 *  in NVRAM and jumpers J1-[5-7] are ignored. See the README for details.
 *  If J1-4 is removed, behaviour defaults to the following. We map the space
 *  from 0x0 to 0x7FFFFFFF as copyback, unless jumper J1-5 is removed, in which
 *  case we map as writethrough. If jumper J1-7 is removed, the data cache is
 *  NOT enabled. If jumper J1-6 is removed, the instruction cache is not enabled.
 *
 *  Copyright (c) 1998, National Research Council of Canada
 */

#include <bsp.h>
#include <page_table.h>                 /* Nothing in here for us */

/*
 *  page_table_init
 *
 *  Map the virtual range 0x00000000--0x7FFFFFFF to the physical range
 *  0x00000000--0x7FFFFFFF. Rely on the hardware to raise exceptions when
 *  addressing non-existent memory. Use only the transparent translation
 *  registers (for now).
 *
 *  On all processors, the local virtual address range 0xFF000000--0xFFFFFFFF
 *  is mapped to the physical address range 0xFF000000--0xFFFFFFFF as
 *  caching disabled, serialized access.
 *
 *  Input parameters:
 *    config_table - ignored for now
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
void page_table_init(
  rtems_configuration_table *config_table
)
{
  unsigned char j1;               /* State of J1 jumpers */
  register unsigned long dtt0;    /* Content of dtt0 */
  register unsigned long cacr;    /* Content of cacr */

  /*
   *  Logical base addr = 0x00    map starting at 0x00000000
   *  Logical address mask = 0x7F map up to 0x7FFFFFFF
   *  E = 0b1                     enable address translation
   *  S-Field = 0b1X              ignore FC2 when matching
   *  U1, U0 = 0b00               user page attributes not used
   *  CM = 0b01                   cachable, copyback
   *  W = 0b0                     read/write access allowed
   */
  dtt0 = 0x007FC020;

  cacr = 0x00000000;              /* Data and instruction cache off */

  /* Read the J1 header */
  j1 = (unsigned char)(lcsr->vector_base & 0xFF);

  if ( !(j1 & 0x10) ) {
  	/* Jumper J1-4 is on, configure from NVRAM */

  	if ( nvram->cache_mode & 0x01 )
  		cacr |= 0x80000000;

  	if ( nvram->cache_mode & 0x02 )
  		cacr |= 0x00008000;

  	if ( nvram->cache_mode )
  		dtt0 = ((nvram->cache_mode & 0x0C) << 3) | (dtt0 & 0xFFFFFF9F);
  }
	else {
		/* Configure according to other jumper settings */

	  if ( !(j1 & 0x80) )
  	  /* Jumper J1-7 if on, enable data caching */
   		cacr |= 0x80000000;

	  if ( !(j1 & 0x40) )
	    /* Jumper J1-6 if on, enable instruction caching */
	    cacr |= 0x00008000;

	  if ( j1 & 0x20 )
	    /* Jumper J1-5 is off, enable writethrough caching */
	    dtt0 &= 0xFFFFFF9F;
	}

  /* do it ! */
  __asm__ volatile("movec %0, %%tc\n\t"    /* turn off paged address translation */
               "movec %0, %%cacr\n\t"  /* disable both caches */
               "cinva %%bc\n\t"        /* clear both caches */
               "movec %1,%%dtt0\n\t"   /* block address translation on */
               "movec %1,%%itt0\n\t"
               "movec %2,%%dtt1\n\t"
               "movec %2,%%itt1\n\t"
               "movec %3,%%cacr"       /* data cache on */
	  :: "d" (0), "d" (dtt0), "d" (0xFF00C040), "d" (cacr));
}

/*
 *  page_table_teardown
 *
 *  Turn off paging. Turn off the cache. Flush the cache. Tear down
 *  the transparent translations.
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
void page_table_teardown( void )
{
  __asm__ volatile ("movec %0,%%tc\n\t"
                "movec %0,%%cacr\n\t"
                "cpusha %%bc\n\t"
                "movec %0,%%dtt0\n\t"
                "movec %0,%%itt0\n\t"
                "movec %0,%%dtt1\n\t"
                "movec %0,%%itt1"
    :: "d" (0) );
}
