

#ifndef __LIBCPU_PAGETABLE_h
#define __LIBCPU_PAGETABLE_h

#include <sys/types.h>
#include <rtems/rtems/status.h>
#include <rtems/libmmu.h>
#ifdef __cplusplus
  extern "C" {
#endif


int translate_access_attr( uint32_t attr, int * wimg, int * pp);

void rtems_pagetable_initialize( void );

/*
 *  * Initialize the hardware to prepare for memory protection directives.
 *   */
rtems_status_code _CPU_Memory_management_Initialize(void);

/*
 *  * Make sure memory protection @a permission is valid for this CPU
 *   */

rtems_status_code _CPU_Memory_management_Verify_permission(
    uint32_t permissions
);


/*
 *  * Check if memory protection region @a size is valid for this CPU
 *   */
rtems_status_code _CPU_Memory_management_Verify_size(
    size_t size
);

/*
 *  * Install (enforce) the memory protection entry @a mpe
 *   */
rtems_status_code _CPU_Memory_management_Install_MPE(
    rtems_memory_management_entry *mpe
);

rtems_status_code _CPU_Pte_Change_Attributes( uint32_t  ea,  int wimg, int pp);

#ifdef __cplusplus
  }
#endif
#endif
