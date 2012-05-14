/*
 *  This file was submitted by Eric Vaitl <vaitl@viasat.com>.
 *  The manipulation of the page table has a very positive impact on
 *  the performance of the MVME162.
 *
 *  The following history is included verbatim from the submitter.
 *
 * Revision 1.8  1995/11/18  00:07:25  vaitl
 * Modified asm-statements to get rid of the register hard-codes.
 *
 * Revision 1.7  1995/10/27  21:00:32  vaitl
 * Modified page table routines so application code can map
 * VME space.
 *
 * Revision 1.6  1995/10/26  17:40:01  vaitl
 * Two cache changes after reading the mvme162 users manual.
 *
 * 1) The users manual says that the MPU can act as a source for the
 *    VME2 chip, so I made the VME accessable memory copy-back instead
 *    of write through.  I have't changed the comments yet. If this
 *    causes problems, I'll change it back.
 *
 * 2) The 162 book also says that IO space should be serialized as well as
 *    non-cacheable. I flipped the appropriate dttr0 and ittr0 registers. I
 *    don't think this is really necessary because we don't recover from any
 *    exceptions. If it slows down IO addresses too much, I'll change it back
 *    and see what happens.
 *
 * Revision 1.5  1995/10/25  19:32:38  vaitl
 * Got it. Three problems:
 *   1) Must cpusha instead of cinva.
 *   2) On page descriptors the PDT field of 1 or 3 is resident. On pointer
 *      descriptors resident is 2 or 3. I was using 2 for everything.
 *      Changed it to 3 for everything.
 *   3) Forgot to do a pflusha.
 *
 * Revision 1.4  1995/10/25  17:47:11  vaitl
 * Still working on it.
 *
 * Revision 1.3  1995/10/25  17:16:05  vaitl
 * Working on page table. Caching partially set up, but can't currently
 * set tc register.
 *
 */

#include <string.h>
#include <page_table.h>

/* All page table must fit between BASE_TABLE_ADDR and
   MAX_TABLE_ADDR. */

#define BASE_TABLE_ADDR 0x10000
#define MAX_TABLE_ADDR 0x20000
#define ROOT_TABLE_SIZE 512
#define POINTER_TABLE_SIZE 512
#define PAGE_TABLE_SIZE 256

static unsigned long *root_table;
static unsigned long *next_avail;

/* Returns a zeroed out table. */
static unsigned long *table_alloc(int size){
    unsigned long *addr=next_avail;
    if(((unsigned long)next_avail + size) > MAX_TABLE_ADDR){
	return 0;
    }
    memset((void *)addr,0, size);
    next_avail =(unsigned long *)((unsigned long)next_avail + size);
    return addr;
}

/*
   void page_table_init();

   This should transparently map the first 4 Meg of ram.  Caching is
   turned off from 0x00000000 to 0x00020000 (this region is used by
   162Bug and contains the page tables). From 0x00020000 to 0x00400000
   we are using copy back caching. DTTR0 and ITTR0 are set up to
   directly translate from 0x80000000-0xffffffff with caching turned
   off and serialized. Addresses between 0x400000 and 0x80000000 are
   illegal.
*/
void page_table_init(){

    /* put everything in a known state */
    page_table_teardown();

    root_table=table_alloc(ROOT_TABLE_SIZE);

    /* First set up TTR.
       base address = 0x80000000
       address mask = 0x7f
       Ignore FC2 for match.
       Noncachable.
       Not write protected.*/
    __asm__ volatile ("movec %0,%%dtt0\n\
                   movec %0,%%itt0"
		  :: "d" (0x807fc040));

    /* Point urp and srp at root page table. */
    __asm__ volatile ("movec %0,%%urp\n\
                   movec %0,%%srp"
		  :: "d" (BASE_TABLE_ADDR));

    page_table_map((void *)0,0x20000, CACHE_NONE);
    page_table_map((void *)0x20000,0x400000-0x20000,CACHE_COPYBACK);

    /* Turn on paging with a 4 k page size.*/
    __asm__ volatile ("movec %0,%%tc"
		  :: "d" (0x8000));

    /* Turn on the cache. */
    __asm__ volatile ("movec %0,%%cacr"
		  :: "d" (0x80008000));
}

void page_table_teardown(){
    next_avail=(unsigned long *)BASE_TABLE_ADDR;
    /* Turn off paging.  Turn off the cache. Flush the cache. Tear down
       the transparent translations. */
    __asm__ volatile ("movec %0,%%tc\n\
                   movec %0,%%cacr\n\
                   cpusha %%bc\n\
                   movec %0,%%dtt0\n\
                   movec %0,%%itt0\n\
                   movec %0,%%dtt1\n\
                   movec %0,%%itt1"
		  :: "d" (0) );
}

/* Identity maps addr to addr+size with caching cache_type. */
int page_table_map(void *addr, unsigned long size, int cache_type){
    unsigned long *pointer_table;
    unsigned long *page_table;
    unsigned long root_index, pointer_index, page_index;
    /* addr must be a multiple of 4k */
    if((unsigned long)addr & 0xfff){
	return  PTM_BAD_ADDR;
    }
    /* size must also be a multiple of 4k */
    if(size & 0xfff){
	return PTM_BAD_SIZE;
    }
    /* check for valid cache type */
    if( (cache_type>CACHE_NONE) || (cache_type<CACHE_WRITE_THROUGH)){
	return PTM_BAD_CACHE;
    }

    while(size){
	root_index=(unsigned long)addr;
	root_index >>= 25;
	root_index &= 0x7f;

	if(root_table[root_index]){
	    pointer_table =
		(unsigned long *) (root_table[root_index] & 0xfffffe00);
	}else{
	    if(!(pointer_table=table_alloc(POINTER_TABLE_SIZE))){
		return  PTM_NO_TABLE_SPACE;
	    }
	    root_table[root_index]=((unsigned long)pointer_table) + 0x03;
	}

	pointer_index=(unsigned long)addr;
	pointer_index >>=18;
	pointer_index &= 0x7f;

	if(pointer_table[pointer_index]){
	    page_table =
		(unsigned long *) (pointer_table[pointer_index] &
				   0xffffff00);
	}else{
	    if(!(page_table=table_alloc(PAGE_TABLE_SIZE))){
		return  PTM_NO_TABLE_SPACE;
	    }
	    pointer_table[pointer_index]=
		((unsigned long)page_table) + 0x03;
	}

	page_index=(unsigned long)addr;
	page_index >>=12;
	page_index &= 0x3f;

	page_table[page_index] =
	    ((unsigned long) addr & 0xfffff000) + 0x03 + (cache_type << 5);

	size -= 4096;
	addr = (void *) ((unsigned long)addr + 4096);
    }

    /* Flush the ATC. Push and invalidate the cache. */
    __asm__ volatile ("pflusha\n\
                   cpusha %bc");

    return  PTM_SUCCESS;
}
