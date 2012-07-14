

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

rtems_status_code _CPU_Memory_protection_Verify_permission(
    uint32_t permissions
);
#ifdef __cplusplus
  }
#endif
#endif
