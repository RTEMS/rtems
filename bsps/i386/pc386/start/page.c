/*
 * page.c :- This file contains implementation of C function to
 *           Instanciate paging. More detailled information
 *	     can be found on Intel site and more precisely in
 *           the following book :
 *
 *		Pentium Processor familly
 *		Developper's Manual
 *
 *		Volume 3 : Architecture and Programming Manual
 *
 * Copyright (C) 1999  Emmanuel Raguet (raguet@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtems.h>
#include <rtems/score/cpu.h>
#include <libcpu/page.h>

#define MEMORY_SIZE 0x4000000 		/* 64Mo */

static int directoryEntry=0;
static int tableEntry=0;
static page_directory *pageDirectory;

extern uint32_t   bsp_mem_size;

/*************************************************************************/
/************** IT IS A ONE-TO-ONE TRANSLATION ***************************/
/*************************************************************************/


/*
 * Disable the paging
 */
void _CPU_disable_paging(void)
{
  unsigned int regCr0;

  rtems_cache_flush_entire_data();
  regCr0 = i386_get_cr0();
  regCr0 &= ~(CR0_PAGING);
  i386_set_cr0( regCr0 );
}

/*
 * Enable the paging
 */
void _CPU_enable_paging(void)
{
  unsigned int regCr0;

  regCr0 = i386_get_cr0();
  regCr0 |= CR0_PAGING;
  i386_set_cr0( regCr0 );
  rtems_cache_flush_entire_data();
}


/*
 * Initialize the paging with 1-to-1 mapping
 */

int init_paging(void)
{
  int nbPages;
  int nbInitPages;
  char *Tables;
  unsigned int regCr3;
  page_table *pageTable;
  unsigned int physPage;
  int nbTables=0;

  nbPages = ( (bsp_mem_size - 1) / PG_SIZE ) + 1;
  nbTables = ( (bsp_mem_size - 1) / FOUR_MB ) + 2;

  /* allocate 1 page more to page alignement */
  Tables = (char *)malloc( (nbTables + 1)*sizeof(page_table) );
  if ( Tables == NULL ){
    return -1; /*unable to allocate memory */
  }

  /* 4K-page alignement */
  Tables += (PG_SIZE - (int)Tables) & 0xFFF;

  /* Reset Tables */
  memset( Tables, 0, nbTables*sizeof(page_table) );
  pageDirectory = (page_directory *) Tables;
  pageTable     = (page_table *)((int)Tables + PG_SIZE);

  nbInitPages = 0;
  directoryEntry = 0;
  tableEntry = 0;
  physPage = 0;

  while ( nbInitPages != nbPages ){
    if ( tableEntry == 0 ){
      pageDirectory->pageDirEntry[directoryEntry].bits.page_frame_address = (unsigned int)pageTable >> 12;
      pageDirectory->pageDirEntry[directoryEntry].bits.available      = 0;
      pageDirectory->pageDirEntry[directoryEntry].bits.page_size      = 0;
      pageDirectory->pageDirEntry[directoryEntry].bits.accessed       = 0;
      pageDirectory->pageDirEntry[directoryEntry].bits.cache_disable  = 0;
      pageDirectory->pageDirEntry[directoryEntry].bits.write_through  = 0;
      pageDirectory->pageDirEntry[directoryEntry].bits.user           = 1;
      pageDirectory->pageDirEntry[directoryEntry].bits.writable       = 1;
      pageDirectory->pageDirEntry[directoryEntry].bits.present        = 1;
    }
    pageTable->pageTableEntry[tableEntry].bits.page_frame_address = physPage;
    pageTable->pageTableEntry[tableEntry].bits.available      = 0;
    pageTable->pageTableEntry[tableEntry].bits.dirty          = 0;
    pageTable->pageTableEntry[tableEntry].bits.accessed       = 0;
    pageTable->pageTableEntry[tableEntry].bits.cache_disable  = 0;
    pageTable->pageTableEntry[tableEntry].bits.write_through  = 0;
    pageTable->pageTableEntry[tableEntry].bits.user           = 1;
    pageTable->pageTableEntry[tableEntry].bits.writable       = 1;
    pageTable->pageTableEntry[tableEntry].bits.present        = 1;

    physPage ++;
    tableEntry ++;

    if (tableEntry >= MAX_ENTRY){
      tableEntry = 0;
      directoryEntry ++;
      pageTable ++;
    }

    nbInitPages++;
  }

  regCr3 &= ~(CR3_PAGE_WRITE_THROUGH);
  regCr3 &= ~(CR3_PAGE_CACHE_DISABLE);
  /*regCr3.cr3.page_directory_base    = (unsigned int)pageDirectory >> 12;*/
  regCr3 = (unsigned int)pageDirectory & CR3_PAGE_DIRECTORY_MASK;

  i386_set_cr3( regCr3 );

  _CPU_enable_cache();
  _CPU_enable_paging();

  return 0;
}

/*
 * Is cache enable
 */
int  _CPU_is_cache_enabled(void)
{
  unsigned int regCr0;

  regCr0 = i386_get_cr0();
  return( ~(regCr0 & CR0_PAGE_LEVEL_CACHE_DISABLE) );
}

/*
 * Is paging enable
 */
int  _CPU_is_paging_enabled(void)
{
  unsigned int regCr0;

  regCr0 = i386_get_cr0();
  return(regCr0 & CR0_PAGING);
}


/*
 * Translate the physical address in the virtual space and return
 * the translated address in mappedAddress
 */

int _CPU_map_phys_address(
  void **mappedAddress,
  void  *physAddress,
  int    size,
  int    flag
)
{
  page_table *localPageTable;
  unsigned int lastAddress, countAddress;
  char *Tables;
  linear_address virtualAddress;
  unsigned char pagingWasEnabled;

  pagingWasEnabled = 0;

  if (_CPU_is_paging_enabled()){
    pagingWasEnabled = 1;
    _CPU_disable_paging();
  }

  countAddress = (unsigned int)physAddress;
  lastAddress = (unsigned int)physAddress + (size - 1);
  virtualAddress.address = 0;

  while (1) {

    if ((countAddress & ~MASK_OFFSET) > (lastAddress & ~MASK_OFFSET))
      break;

    /* Need to allocate a new page table */
    if (pageDirectory->pageDirEntry[directoryEntry].bits.page_frame_address == 0){
      /* We allocate 2 pages to perform 4k-page alignement */
      Tables = (char *)malloc(2*sizeof(page_table));
      if ( Tables == NULL ){
	if (pagingWasEnabled)
	  _CPU_enable_paging();
	return -1; /* unable to allocate memory */
      }
      /* 4K-page alignement */
      Tables += (PG_SIZE - (int)Tables) & 0xFFF;

      /* Reset Table */
      memset( Tables, 0, sizeof(page_table) );
      pageDirectory->pageDirEntry[directoryEntry].bits.page_frame_address =
	(unsigned int)Tables >> 12;
      pageDirectory->pageDirEntry[directoryEntry].bits.available      = 0;
      pageDirectory->pageDirEntry[directoryEntry].bits.page_size      = 0;
      pageDirectory->pageDirEntry[directoryEntry].bits.accessed       = 0;
      pageDirectory->pageDirEntry[directoryEntry].bits.cache_disable  = 0;
      pageDirectory->pageDirEntry[directoryEntry].bits.write_through  = 0;
      pageDirectory->pageDirEntry[directoryEntry].bits.user           = 1;
      pageDirectory->pageDirEntry[directoryEntry].bits.writable       = 1;
      pageDirectory->pageDirEntry[directoryEntry].bits.present        = 1;
    }


    localPageTable = (page_table *)(pageDirectory->
				    pageDirEntry[directoryEntry].bits.
				    page_frame_address << 12);

    if (virtualAddress.address == 0){
      virtualAddress.bits.directory = directoryEntry;
      virtualAddress.bits.page      = tableEntry;
      virtualAddress.bits.offset    = (unsigned int)physAddress & MASK_OFFSET;
    }

    localPageTable->pageTableEntry[tableEntry].bits.page_frame_address =
      ((unsigned int)countAddress & ~MASK_OFFSET) >> 12;
    localPageTable->pageTableEntry[tableEntry].bits.available      = 0;
    localPageTable->pageTableEntry[tableEntry].bits.dirty          = 0;
    localPageTable->pageTableEntry[tableEntry].bits.accessed       = 0;
    localPageTable->pageTableEntry[tableEntry].bits.cache_disable  = 0;
    localPageTable->pageTableEntry[tableEntry].bits.write_through  = 0;
    localPageTable->pageTableEntry[tableEntry].bits.user           = 1;
    localPageTable->pageTableEntry[tableEntry].bits.writable       = 0;
    localPageTable->pageTableEntry[tableEntry].bits.present        = 1;

    localPageTable->pageTableEntry[tableEntry].table_entry |= flag ;

    countAddress += PG_SIZE;
    tableEntry++;
    if (tableEntry >= MAX_ENTRY){
      tableEntry = 0;
      directoryEntry++;
    }
  }

  if (mappedAddress != 0)
    *mappedAddress = (void *)(virtualAddress.address);
  if (pagingWasEnabled)
    _CPU_enable_paging();
  return 0;
}

/*
 * "Compress" the Directory and Page tables to avoid
 * important loss of address range
 */
static void Paging_Table_Compress(void)
{
  unsigned int dirCount, pageCount;
  page_table *localPageTable;

  if (tableEntry == 0){
    dirCount  = directoryEntry - 1;
    pageCount = MAX_ENTRY - 1;
  }
  else {
    dirCount  = directoryEntry;
    pageCount = tableEntry - 1;
  }

  while (1){

    localPageTable = (page_table *)(pageDirectory->
				    pageDirEntry[dirCount].bits.
				    page_frame_address << 12);

    if (localPageTable->pageTableEntry[pageCount].bits.present == 1){
      pageCount++;
      if (pageCount >= MAX_ENTRY){
	pageCount = 0;
	dirCount++;
      }
      break;
    }


    if (pageCount == 0) {
      if (dirCount == 0){
	break;
      }
      else {
	pageCount = MAX_ENTRY - 1;
	dirCount-- ;
      }
    }
    else
      pageCount-- ;
  }

  directoryEntry = dirCount;
  tableEntry = pageCount;
}


/*
 * Unmap the virtual address from the tables
 * (we do not deallocate the table already allocated)
 */

int _CPU_unmap_virt_address(
  void *mappedAddress,
  int   size
)
{

  linear_address linearAddr;
  page_table *localPageTable;
  unsigned int lastAddr ;
  unsigned char pagingWasEnabled;

  pagingWasEnabled = 0;

  if (_CPU_is_paging_enabled()){
    pagingWasEnabled = 1;
    _CPU_disable_paging();
  }

  linearAddr.address = (unsigned int)mappedAddress;
  lastAddr = (unsigned int)mappedAddress + (size - 1);

  while (1){

    if ((linearAddr.address & ~MASK_OFFSET) > (lastAddr & ~MASK_OFFSET))
      break;

    if (pageDirectory->pageDirEntry[linearAddr.bits.directory].bits.present == 0){
      if (pagingWasEnabled)
	_CPU_enable_paging();
      return -1;
    }

    localPageTable = (page_table *)(pageDirectory->
				    pageDirEntry[linearAddr.bits.directory].bits.
				    page_frame_address << 12);

    if (localPageTable->pageTableEntry[linearAddr.bits.page].bits.present == 0){
      if (pagingWasEnabled)
	_CPU_enable_paging();
      return -1;
    }

    localPageTable->pageTableEntry[linearAddr.bits.page].bits.present = 0;

    linearAddr.address += PG_SIZE ;
  }
  Paging_Table_Compress();
  if (pagingWasEnabled)
    _CPU_enable_paging();

  return 0;
}

/*
 * Modify the flags PRESENT, WRITABLE, USER, WRITE_TROUGH, CACHE_DISABLE
 * of the page's descriptor.
 */

int _CPU_change_memory_mapping_attribute(
  void         **newAddress,
  void          *mappedAddress,
  unsigned int   size,
  unsigned int   flag
)
{

  linear_address linearAddr;
  page_table *localPageTable;
  unsigned int lastAddr ;
  unsigned char pagingWasEnabled;

  pagingWasEnabled = 0;

  if (_CPU_is_paging_enabled()){
    pagingWasEnabled = 1;
    _CPU_disable_paging();
  }

  linearAddr.address  = (unsigned int)mappedAddress;
  lastAddr = (unsigned int)mappedAddress + (size - 1);

  while (1){

    if ((linearAddr.address & ~MASK_OFFSET) > (lastAddr & ~MASK_OFFSET))
      break;

    if (pageDirectory->pageDirEntry[linearAddr.bits.directory].bits.present == 0){
      if (pagingWasEnabled)
	_CPU_enable_paging();
      return -1;
    }
    localPageTable = (page_table *)(pageDirectory->
				    pageDirEntry[linearAddr.bits.directory].bits.
				    page_frame_address << 12);

    if (localPageTable->pageTableEntry[linearAddr.bits.page].bits.present == 0){
      if (pagingWasEnabled)
	_CPU_enable_paging();
      return -1;
    }

    localPageTable->pageTableEntry[linearAddr.bits.page].table_entry &= ~MASK_FLAGS ;
    localPageTable->pageTableEntry[linearAddr.bits.page].table_entry |= flag ;

    linearAddr.address += PG_SIZE ;
  }

  if (newAddress != NULL)
    *newAddress = mappedAddress ;

  if (pagingWasEnabled)
    _CPU_enable_paging();

  return 0;
}

/*
 * Display the page descriptor flags
 * CACHE_DISABLE of the whole memory
 */

#include <rtems/bspIo.h>

int  _CPU_display_memory_attribute(void)
{
  unsigned int dirCount, pageCount;
  unsigned int regCr0;
  page_table *localPageTable;
  unsigned int prevCache;
  unsigned int prevPresent;
  unsigned int maxPage;
  unsigned char pagingWasEnabled;

  regCr0 = i386_get_cr0();

  printk("\n\n********* MEMORY CACHE CONFIGURATION *****\n");

  printk("CR0 -> paging           : %s\n",((regCr0 & CR0_PAGING) ? "ENABLE ":"DISABLE"));
  printk("       page-level cache : %s\n\n",((regCr0 & CR0_PAGE_LEVEL_CACHE_DISABLE) ? "DISABLE":"ENABLE"));

  if ((regCr0 & CR0_PAGING) == 0)
    return 0;

  prevPresent = 0;
  prevCache   = 1;

  pagingWasEnabled = 0;

  if (_CPU_is_paging_enabled()){
    pagingWasEnabled = 1;
    _CPU_disable_paging();
  }

  for (dirCount = 0; dirCount < directoryEntry+1; dirCount++) {

    localPageTable = (page_table *)(pageDirectory->
				    pageDirEntry[dirCount].bits.
				    page_frame_address << 12);

    maxPage = MAX_ENTRY;
    /*if ( dirCount == (directoryEntry-1))
      maxPage = tableEntry;*/
    for (pageCount = 0; pageCount < maxPage; pageCount++) {

      if (localPageTable->pageTableEntry[pageCount].bits.present != 0){
	if (prevPresent == 0){
	  prevPresent = 1;
	  printk ("present page from address %x \n", ((dirCount << 22)|(pageCount << 12)));
	}
	if (prevCache != localPageTable->pageTableEntry[pageCount].bits.cache_disable ) {
	  prevCache = localPageTable->pageTableEntry[pageCount].
	    bits.cache_disable;
	  printk ("    cache %s from %x <phy %x>\n",
		  (prevCache ? "DISABLE" : "ENABLE "),
		  ((dirCount << 22)|(pageCount << 12)),
		  localPageTable->pageTableEntry[pageCount].bits.page_frame_address << 12);
	}
      }
      else {
	if (prevPresent == 1){
	  prevPresent = 0;
	  printk ("Absent from %x \n", ((dirCount << 22)|(pageCount << 12)));
	}
      }
    }
  }
  if (pagingWasEnabled)
    _CPU_enable_paging();

  return 0;
}
