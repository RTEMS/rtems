
#include <rtems.h>
#include <sys/types.h>
#include <rtems/rtems/status.h>
/*#include <libcpu/pagetable.h>
 #include "pagetable_.h"*/


/*this is a mid level API used to operate page table 
 *mainly used by memory protect manager
 */
rtems_status_code rtems_pagetable_attribute_check( uint32_t attr)
{
  return  _CPU_Pagetable_attr_Check(attr );
}

void rtems_pagetable_initialize( void )
{
  _CPU_Pagetable_Initialize();
}

rtems_status_code rtems_pagetable_update_attribute(
  uint32_t ea, 
  int cache_attr, 
  int mprot)
{
  return _CPU_Pte_Change_Attributes( ea, cache_attr, mprot);
}
