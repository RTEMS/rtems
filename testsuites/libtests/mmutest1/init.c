/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define CONFIGURE_INIT
#include "system.h"
#include <stdio.h>
#include <stdlib.h>
//#include <rtems/libmmu.h>
//#include <libcpu/mmu_support.h>
#include <rtems/libmmu.h>
/* FIXME: Get some useful memory regions that are well-defined in RTEMS */

rtems_task Init(
  rtems_task_argument ignored
 )
{
  rtems_status_code status;
  size_t  blocksize;
  int i;
  volatile unsigned char a;
  unsigned char* a1;
  unsigned char* a2;
  void * good_address;
  void * bad_address;
  rtems_memory_management_entry *mp_entry;
  rtems_memory_management_permission permission;
  rtems_memory_management_domain *protection_domain, *another_domain;
  void *blob;
  rtems_memory_management_domain p1, p2;
  
  protection_domain = &p1;
  another_domain = &p2;

  rtems_memory_management_region_descriptor r1 = {
    .name = "faulty",
    .base = 0x00,
    .bounds = 2096
  };
  rtems_memory_management_region_descriptor r2 = {
    .name = "read-only",
    .base = 0x00,
    .bounds = 0x2000
  };
  rtems_memory_management_region_descriptor r3 = {
    .name = "stringy",
    .base = 0x00,
    .bounds = 0x2000
  };
   rtems_memory_management_region_descriptor r4 = {
    .name = "big",
    .base = 0x00,
    .bounds = 0x2000
  };

  blob = malloc(2*r2.bounds + r3.bounds + r4.bounds);
  if (!blob) {
    printf("Failed allocation!\n");
  }
#define ALIGN_UP(s, a)  (((s) + ((a) - 1)) & ~((a) - 1))
  r2.base = ALIGN_UP((uintptr_t)blob+r2.bounds,0x2000);
  r3.base = ALIGN_UP((uintptr_t)blob+2*r2.bounds,0x2000);
  r4.base = ALIGN_UP((uintptr_t)blob+2*r2.bounds+r3.bounds,0x2000);

  good_address = r4.base + 0x1000;
  bad_address = r4.base + 0x4000;

  puts( "\n\n*** MMU ALUT TEST 1 BEGINS ***\n" );
  puts( "initialize the memory protect manager\n");
  status = rtems_memory_management_initialize();
  if ( status != RTEMS_SUCCESSFUL )
  {
    puts("Failed: initialize the memory protect manager; status "
        );
    exit(0);
  }

  status = rtems_memory_management_initialize_domain( protection_domain, 64 );
  if ( status != RTEMS_SUCCESSFUL )
  {
    printf("Failed: create protection domain; status = %d\n",
        status);
    exit(0);
  }
  printf("Protection domain created\n");

  status = rtems_memory_management_initialize_domain( another_domain, 8 );
  if ( status != RTEMS_SUCCESSFUL )
  {
    printf("Failed: create protection domain; status = %d\n",
        status);
    exit(0);
  }
  printf("Another protection domain created\n");

  /* FIXME: 4K magic */
  printf("Test 1: Adding entry with block size less than 4K\n");
  status = rtems_memory_management_create_entry(
      protection_domain,
      &r1,
      RTEMS_MEMORY_MANAGEMENT_READ_PERMISSION,
      &mp_entry
  );
  if ( status == RTEMS_SUCCESSFUL ) {
    printf("Failed: Invalid block size and still entry added\n");
  }
  else {
    printf("Passed: Entry addition failed\n");
  }

  r1.bounds = 0x00008FFF;
  /* FIXME: 4K magic */
  printf("Test 2: Adding entry with block size not a multiple of 4K\n");
  status = rtems_memory_management_create_entry(
      protection_domain,
      &r1,
      RTEMS_MEMORY_MANAGEMENT_READ_PERMISSION,
      &mp_entry
  );
  if ( status == RTEMS_SUCCESSFUL ) {
    printf("Failed: Invalid block size and still entry successfully added\n");
  }
  else {
    printf("Passed: Entry adding failed\n");
  }
  
  printf("Test 3: Adding valid entry to first domain\n");
  status = rtems_memory_management_create_entry(
      protection_domain,
      &r2,
      RTEMS_MEMORY_MANAGEMENT_READ_PERMISSION,
      &mp_entry
  ); 
  if ( status == RTEMS_SUCCESSFUL ) {
    printf("Passed: Entry Added\n");
  }
  else {
    printf("Failed: Entry addition failed, status = %d\n",status);
  }

  printf("Test 4: Adding overlapping address value\n");
  status = rtems_memory_management_create_entry(
      protection_domain,
      &r2,
      RTEMS_MEMORY_MANAGEMENT_READ_PERMISSION|RTEMS_MEMORY_MANAGEMENT_WRITE_PERMISSION,
      &mp_entry); 
  if ( status == RTEMS_SUCCESSFUL ) {
    printf("Failed: Addition passed inspite of address overlap\n");
  }
  else {
    printf("Passed: Successful detection of address overlap\n");
  }

  printf("Test 5: Adding valid entry to first domain\n");
  status = rtems_memory_management_create_entry(
      protection_domain,
      &r3,
      RTEMS_MEMORY_MANAGEMENT_READ_PERMISSION,
      &mp_entry);
  if ( status == RTEMS_SUCCESSFUL ) {
    printf("Passed: Entry successfully added\n");
  }
  else {
    printf("Failed: Entry adding failed\n");
  }

  printf("Test 6: Adding valid entry to second domain\n");
  status = rtems_memory_management_create_entry(
      another_domain,
      &r4,
      RTEMS_MEMORY_MANAGEMENT_READ_PERMISSION|RTEMS_MEMORY_MANAGEMENT_WRITE_PERMISSION,
      &mp_entry
  ); 
  if ( status == RTEMS_SUCCESSFUL ) {
      printf("Passed: Entry successfully added, status = %d\n",status);
  }
  else {
    printf("Failed: Entry adding failed\n");
  }

  printf("Test 7: Find valid address in second domain\n");
  status = rtems_memory_management_find_entry(
      another_domain,
      good_address
     // &mp_entry
  ); 
  if ( status != RTEMS_SUCCESSFUL ) {
    printf("Failed: Cannot find address, status = %d\n",status);
  }

  printf("Test 8: Install second domain\n");
  status = rtems_memory_management_install_domain(another_domain);
  if ( status != RTEMS_SUCCESSFUL ) {
    printf("Failed: unable to install domain, status = %d\n",status);
  } else {
    printf("Passed: installed domain\n");
  }

  printf("Test 9: Switch from second to first domain\n");
  status = rtems_memory_management_uninstall_domain(another_domain);
  if ( status != RTEMS_SUCCESSFUL ) {
    printf("Failed: unable to install domain, status = %d\n",status);
  }
  status = rtems_memory_management_install_domain(protection_domain);
  if ( status != RTEMS_SUCCESSFUL ) {
    printf("Failed: unable to install domain, status = %d\n",status);
  } else {
    printf("Passed: installed domain\n");
  }

  printf("Test 10: delete entry from second domain\n");
  status = rtems_memory_management_delete_entry(another_domain, mp_entry);
  if ( status != RTEMS_SUCCESSFUL ) {
    printf("Failed: unable to delete, status = %d\n",status);
  } else {
    printf("Passed: deleted entry\n");
  }

  printf("Test 11: release second domain\n");
  status = rtems_memory_management_finalize_domain(another_domain);
  if ( status != RTEMS_SUCCESSFUL ) {
    printf("Failed: unable to release domain, status = %d\n",status);
  } else {
    printf("Passed: released domain\n");
  }

  printf("Test 12: Find invalid address in first domain\n");
  status = rtems_memory_management_find_entry(
      protection_domain,
      bad_address 
     // &mp_entry
  ); 
  if ( status == RTEMS_SUCCESSFUL ) {
    printf("Failed: Found invalid address, status = %d\n",status);
  }
  else printf("Passed: Failed to find invalid address\n");

  // TODO: make these fatal tests
  a1 = bad_address;
  printf("Checking MMU exception 1:Read from Unmapped block\n");
  for ( i=0;i<16;i++ ) {
   a = *a1++; 
  }
  
  a1 = bad_address + 0x2000;
  printf("Checking MMU exception 2: Write to Unmapped block\n"); 
  for ( i=0;i<16;i++ ) {
   *a1++ = 0xCC;
  }

  // this one isn't an exception.
  a2 = r2.base;
  printf("Checking MMU exception 3: Read from readonly block\n");
  for ( i=0;i<16;i++ ) {
    a = *a2++;
  }
  
  printf("Checking MMU exception 4: Write to readonly block  \n");
  for ( i=0;i<16;i++ ) {
   *a2++ = 0xCC;
  }

  /* TODO: test execution permissions */

  printf("Failed: this line should never be printed!!\n");
  rtems_task_delete(RTEMS_SELF);
}

