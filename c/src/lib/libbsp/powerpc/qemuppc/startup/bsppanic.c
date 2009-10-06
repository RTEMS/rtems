#include <rtems.h>
#include <rtems/bspIo.h>

static void
__outb(int port, unsigned char v)
{
  *((volatile unsigned char *)(0x80000000 + port)) = v;
}

void BSP_panic(char *s)
{
  printk("%s PANIC %s\n",_RTEMS_version, s);
  __outb (0x92, 0x01);
}

void _BSP_Fatal_error(unsigned int v)
{
  printk("%s PANIC ERROR %x\n",_RTEMS_version, v);
  __outb (0x92, 0x01);
}
