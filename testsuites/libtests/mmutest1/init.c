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
 *  $Id: init.c,v 1.15 2008/01/11 22:50:10 joel Exp $
 */

#define CONFIGURE_INIT
#include "system.h"
#include <stdio.h>
#include <stdlib.h>
//#include <rtems/libmmu.h>
#include <rtems/libmmu.h>

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
  unsigned char* ppteg_addr;
  unsigned char* spteg_addr;
  void * alut_search_addr1;
  void * alut_search_addr2;
  rtems_memory_management_entry *mpe;
  a1 = (unsigned char *)0x00A10008;
  a2 = (unsigned char *)0x000f0008; 
  
  alut_search_addr1 = (char*) 0x00008111;
  alut_search_addr2 = (char*) 0x00708111;
  
  ppteg_addr = (unsigned char *) 0x00FF8000;
  spteg_addr = (unsigned char *) 0x00FF7FC0;
  
  rtems_memory_management_region_descriptor r1 = {
    .name = "test",
    .base = 0x00,
    .bounds = 2096
  };


  puts( "\n\n*** MMU ALUT TEST 1 BEGINS ***\n" );
  puts( "initialize the memory protect manager\n");

  rtems_memory_management_install_alut();

  status = rtems_memory_management_initialize ( );

  if(status != RTEMS_SUCCESSFUL)
  {
    printf("Failed:initialize the memory protect manager failed; status = %d\n",status);
    exit(0);
  }
  printf("ALUT created\n");


  printf("Test 1 : Adding entry with block size less than 4K\n");
  status = rtems_memory_management_create_entry(r1, &mpe);
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Invalid block size and still entry added\n");
  }
  else{
    printf("Passed : Entry addition failed, status = %d\n",status);
  }
      
  printf("Test 2 : Adding entry with block size not a multiple of 4K\n");  
  r1.bounds = 0x00008FFF;
  status = rtems_memory_management_create_entry(r1, &mpe);
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Invalid block size and still entry successfully added\n");
  }
  else{
    printf("Passed : Entry adding failed, status = %d\n",status);
  }
    
  printf("Test 3 : Adding valid entry into ALUT with Read only attr\n");
  r1.base = (void*)0x000f0000;
  r1.bounds = 0x8000; 
  status = rtems_memory_management_create_entry(r1, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
    printf("Passed : Entry Added\n");
  }
  else{
    printf("Failed : Entry addition failed, status = %d\n",status);
  }
  
  rtems_memory_management_install_entry(mpe);

  status = rtems_memory_management_set_read_only(mpe);
  if(status == RTEMS_SUCCESSFUL){
     printf("Passed: Setting Read only permissions = %d\n",status);
   }   
   else{
     printf("Failed : to set Read only permissions = %d\n",status);
   } 
  printf("Test 4 : Adding overlapping  address value\n");
  r1.base = (void*)0x01A07000;
  r1.bounds = 0x4000;
  status = rtems_memory_management_create_entry(r1, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Addition passed inspite of address overlap\n");
  }
  else{
    printf("Passed : Successful detection of address overlap and ignored, status = %d\n",status);
  }

  printf("Test 5 : Adding valid entry\n");
  r1.base = (void*)0x001f0000;
  r1.bounds = 0x8000;
  status = rtems_memory_management_create_entry(r1, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
    printf("Passed: Entry successfully added, status = %d\n",status);
  }
  else{
    printf("Failed : Entry adding failed, status = %d\n",status);
  }

  rtems_memory_management_install_entry(mpe);

  printf("Test 6 : Adding valid entry\n");
  r1.base = (void*)0x00008000;
  r1.bounds = 0x8000;
  status = rtems_memory_management_create_entry(r1, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
      printf("Passed : Entry successfully added, status = %d\n",status);
  }
  else{
    printf("Failed : Entry adding failed, status = %d\n",status);
  }

rtems_memory_management_install_entry(mpe);
  /* Now that the ALUT is created and populated, start testing for 
   *  search operations over particular address values 
   */
  printf("Test 7 : Get access attrbute for address 0x%x\n", alut_search_addr1);
  status = rtems_memory_management_find_entry(alut_search_addr1, &mpe); 
  if(status != RTEMS_SUCCESSFUL){
    printf("Failed : Cannot find the entry including this address in ALUT, status = %d\n",status);
  }
  status = rtems_memory_management_get_size(mpe,&blocksize);
  if(status != RTEMS_SUCCESSFUL){
    printf("Failed : Access Attribute not found, status = %d\n",status);
  }
  else {
    printf("Passed :the size of the entry including the request address is 0x%x\n", blocksize);
  }


  printf("Test 8 : Get attrbute for unmapped address 0x%x\n", alut_search_addr2);
  status = rtems_memory_management_find_entry(alut_search_addr2, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Find the entry including this address in ALUT, status = %d\n",status);
  }
  else printf("Passed : Failed to find unmapped address in ALUT, status = %d\n",status);

  printf("Checking MMU exception 1:Read from Unmapped block \n");
  for(i=0;i<16;i++){
   printf("0x%x,  ",*a1++);
   if(i%8 == 7)
        printf("\n");
  }
  
  printf("Checking MMU exception 2: Write to Unmapped block  \n"); 
  for(i=0;i<16;i++){
   *a1++ = 0xCC;
  }
  
  printf("Checking MMU exception 3: Read from readonly block  \n");
  for(i=0;i<16;i++){
   printf("0x%x,  ",*a2++);
   if(i%8 == 7)
        printf("\n");
  }

  printf("Checking MMU exception 4: Write to readonly block  \n");
  for(i=0;i<16;i++){
   *a2++ = 0xCC;
  }

  printf("Failed: this line should never be printed!!");
  rtems_task_delete(RTEMS_SELF);
}
