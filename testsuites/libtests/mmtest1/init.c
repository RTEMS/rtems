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
 *
 */

#define CONFIGURE_INIT
#include "system.h"
#include <stdio.h>
#include <stdlib.h>
#include <rtems/libmm.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code status;
  uint32_t access;
  size_t  blocksize;
  int i;
  unsigned char* a1;
  unsigned char* a2;
  void * alut_search_addr1;
  void * alut_search_addr2;
  rtems_memory_management_entry *mpe;
  
  alut_search_addr1 = (char*) 0x00180000;
  alut_search_addr2 = (char*) 0x01000000;
  
  rtems_memory_management_region_descriptor r1 = { 
    .name = "Valid Entry-1",
    .base = 0x00100000,
    .size = 0x200000
  };
  rtems_memory_management_region_descriptor r2 = {
    .name = "Valid Entry-2",
    .base = 0x00400000,
    .size = 0x100000
  };

  rtems_memory_management_region_descriptor r5 = {
    .name = "Valid entry",
    .base = 0x00600000,
    .size = 0x100000
  };
  rtems_memory_management_region_descriptor r3 = { 
    .name = "faulty - size",
    .base = 0x00500000,
    .size = 0x00001001
  };

  rtems_memory_management_region_descriptor r4 = { 
    .name = "faulty - overlapping",
    .base = 0x00100000,
    .size = 0x00100000
  };



  puts( "\n\n*** LIBMM HIGH/MID LEVEL TEST BEGINS ***\n" );
  puts( "initialize the memory protect manager\n");

  rtems_memory_management_install_alut();

  status = rtems_memory_management_initialize ( );

  if(status != RTEMS_SUCCESSFUL)
  {
    printf("Failed:initialize the memory protect manager failed; status = %d\n",status);
    exit(0);
  }
  printf("ALUT created\n");


  printf("Test 1 : Adding entry with invalid block size\n");
  status = rtems_memory_management_create_entry(r3, &mpe);
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Invalid block size and still entry added\n");
  }
  else{
    printf("Passed : Entry addition failed, status = %d\n",status);
  }
      
  printf("Test 2 : Adding entry with block size not a multiple of 2\n");  
  status = rtems_memory_management_create_entry(r3, &mpe);
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Invalid block size and still entry successfully added\n");
  }
  else{
    printf("Passed : Entry adding failed, status = %d\n",status);
  }
    
  printf("Test 3 : Adding valid Entry-1 into ALUT\n");
  status = rtems_memory_management_create_entry(r1, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
    printf("Passed : Entry Added\n");
  }
  else{
    printf("Failed : Entry addition failed, status = %d\n",status);
  }
  
  printf("Test 4 : Installing Entry-1 into HW\n");
  status = rtems_memory_management_install_entry(mpe);

  if(status == RTEMS_SUCCESSFUL){
     printf("Passed : Installing Entry-1 into HW = %d\n",status);
   }   
   else{
     printf("Failed : to Install Entry-1 into HW = %d\n",status);
   }

  printf("Test 5 : Set Read only for installed Entry-1 permissions \n");
  status = rtems_memory_management_set_read_only(mpe); 

  if(status == RTEMS_SUCCESSFUL){
     printf("Passed : Set Read only permissions = %d\n",status);
   }
   else{
     printf("Failed : to Set Read only permissions = %d\n",status);
   }

  printf("Test 6 : Adding overlapping  address value\n");
  status = rtems_memory_management_create_entry(r4, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Addition passed inspite of address overlap\n");
  }
  else{
    printf("Passed : Successful detection of address overlap and ignored, status = %d\n",status);
  }

  printf("Test 7 : Adding another valid Entry-2\n");

  status = rtems_memory_management_create_entry(r2, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
    printf("Passed : Entry-2 successfully added, status = %d\n",status);
  }
  else{
    printf("Failed : Entry adding failed, status = %d\n",status);
  }
  printf("Test 8 : Installing Entry-2 into HW\n");
  status = rtems_memory_management_install_entry(mpe);

  if(status == RTEMS_SUCCESSFUL){
     printf("Passed : Installing Entry into HW = %d\n",status);
   }
   else{
     printf("Failed : to Install Entry into HW = %d\n",status);
   }
  status = rtems_memory_management_install_entry(mpe);

  printf("Test 9 : Set Write permission for installed Entry-2 \n");
  status = rtems_memory_management_set_write(mpe);

  if(status == RTEMS_SUCCESSFUL){
     printf("Passed : Set Write permissions = %d\n",status);
   }
   else{
     printf("Failed : to Set Write permissions = %d\n",status);
   }

   /* Adding a new entry to alut when it's full */

  printf("Test 10: Adding an entry to a full alut \n");
  status = rtems_memory_management_create_entry(r5, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Entry-3 successfully added despite exceeding alut \
 size, status = %d\n",status);
  }
  else{
    printf("Passed : Entry adding failed because alut is full \
, status = %d\n",status);
  }
  /* Now that the ALUT is created and populated, start testing for 
   *  search operations over particular address values 
   */
  printf("Test 11: Get access attributes for address 0x%x\n", alut_search_addr1);
  status = rtems_memory_management_find_entry(alut_search_addr1, &mpe); 
  if(status != RTEMS_SUCCESSFUL){
    printf("Failed : Cannot find the entry including this address in ALUT, status = %d\n",status);
  }
  status = rtems_memory_management_get_size(mpe,&blocksize);
  if(status != RTEMS_SUCCESSFUL){
    printf("Failed : Access Attribute not found, status = %d\n",status);
  }
  else {
    printf("Passed : the size of the entry including the request address is 0x%x\n", blocksize);
  }


  printf("Test 12: Get attribute for unmapped address 0x%x\n", alut_search_addr2);
  status = rtems_memory_management_find_entry(alut_search_addr2, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Find the entry including this address in ALUT, status = %d\n",status);
  }
  else printf("Passed : Failed to find unmapped address in ALUT, status = %d\n",status);
  printf(  "\n\n*** LIBMM HIGH/MID LEVEL TEST ENDS ***\n" );
  
  exit( 0 ); 
}

