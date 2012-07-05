#include <rtems.h>
#include <sys/types.h>
#include <rtems/rtems/status.h>
//#include "pagetable_.h"



/*attribute check,  to make sure the attribute in ALUT is available 
*/
rtems_status_code _CPU_Pagetable_attr_Check(uint32_t attr )
{
  return RTEMS_SUCCESSFUL;
}


void  _CPU_Pagetable_Initialize( void )
{

}

rtems_status_code _CPU_Pte_Change_Attributes( uint32_t  ea,  int wimg, int pp)
{
  return RTEMS_SUCCESSFUL;
}
