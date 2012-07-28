

#ifndef __LIBCPU_PAGETABLE_h
#define __LIBCPU_PAGETABLE_h

#include <sys/types.h>
#include <rtems/rtems/status.h>
#include <rtems/libmmu.h>
#ifdef __cplusplus
  extern "C" {
#endif


//int translate_access_attr( uint32_t attr, int * wimg, int * pp);

void rtems_pagetable_initialize( void );

/*
 *  * Initialize the hardware to prepare for memory protection directives.
 *   */
rtems_status_code _CPU_Memory_management_Initialize(void);

/*
 *  * Install (enforce) the memory protection entry @a mpe
 *   */
rtems_status_code _CPU_Memory_management_Install_MPE(
    rtems_memory_management_entry *mpe
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

rtems_status_code _CPU_Memory_management_Set_read_only(
    rtems_memory_management_entry *mpe
);

rtems_status_code _CPU_Memory_management_Set_write(
    rtems_memory_management_entry *mpe
);

rtems_status_code _CPU_Pte_Change_Attributes(
    rtems_memory_management_entry *mpe,
    uint32_t wimg,
    uint32_t pp
);


#ifdef __cplusplus
  }
#endif
#endif
