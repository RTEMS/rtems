#include <rtems.h>
#include <rtems/bspIo.h>

void BSP_panic(char *s)
{
  printk("%s PANIC %s\n",_RTEMS_version, s);
  while(1){};
}

void _BSP_Fatal_error(unsigned int v)
{
  printk("%s PANIC ERROR %x\n",_RTEMS_version, v);
  while(1){};
}
